/*
 * Copyright (c) 2019 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

# include  "TimerNetworkService.h"
# include  "TimerControlBox.h"
# include  "version_tag.h"
# include  <QTcpSocket>
# include  <cstdio>
# include  <cassert>

/*
 * The TimerNetworkService object implements the network protocol for
 * the timer control box. The server creates a port, and listens for a
 * single connection. (Only one connection is allowed.) Once the
 * connection is made, the client may send commands, and this service
 * sends responses. The general format of the commands is:
 *
 * ** Commands: <command> <options>...\n
 * Commands are a set of words, terminated by a newline
 * character. Each token of the command line is separated by one or
 * more space characters. Leading and trailing spaces are ignored, and
 * empty lines are ignored. The first word of the command line is the
 * command code. The remaining words are arguments to th command.
 *
 * ** Responses: OK:<code>:<text>\n
 *               ERROR:<code>:<text>\n
 *
 * The response to a command is an ERROR record, or an OK record. In
 * both cases, the <code> is a signed decimal value, and the <text> is
 * arbitrary tex. The meaning of the text is defined by the command
 * being responded to. For ERROR records, the <text> is a human
 * readable error message.
 *
 * ======
 * The supported commands are:
 *
 *** version
 *   -> OK:0:<version string>
 *
 * Retrieve the Archery Timer application version string. This is
 * mostly intended for display. Also, this can be used to check the
 * connection.
 *
 *** close-detach
 *   -> (no response)
 *
 * This is the only command that returns no response. The service
 * closes the port, which detaches from the client. New connections
 * can be made.
 *
 *** next-end
 *   -> OK:<N>
 *
 * Tell the timer to set up for the next end. The end counter is
 * incremented, the starting line is queued up, and the timer is
 * prepared for the call-up phase. The timer is not started yet. The
 * <N> in the response is the number of the end that is set up to
 * start.
 *
 *** start-timer
 *   -> OK:0
 *   -> OK:-1
 *
 * If the timer is not running, start the timer and return OK:0. If
 * the timer is currently running, then return OK:-1 and do nothing.
 *
 *** pause-timer
 *   -> OK:0
 *   -> OK:-1
 *
 * Opposite of start-timer. If the timer is running, pause it and
 * return OK:0. A later start-timer will resume the timer. If the
 * timer is not running, return OK:-1 and do nothing else.
 *
 *** fast-forward
 *   -> OK:0
 *   -> OK:-1
 *
 * If the timer is running, then skip to the end of the current
 * phase. For example, if the timer is counting down the end, skip to
 * the end of the end, blow 3 whistles, and go to the next
 * line. Return OK:0.
 *
 * If the timer is not running, return OK:-1 and do nothing.
 *
 *** toggle-fullscreen
 *   -> OK:0
 *   -> OK:1
 *
 * If the timer window is not in fullscreen mode, then put it into
 * fullscreen mode and hide the control box. If the screen is aready
 * in fullscreen mode, then remove from fullscreen mode and show the
 * control box.
 *
 * Return OK:1 if the timer ends up in fullscreen mode, and OK:0 if
 * the timer ends up NOT in fullscreen mode.
 *
 *** query-settings
 *   -> OK:0:<key-value-pairs>...
 *
 * Query the timer settings. The response is all the current settings
 * of the timer, formatted as <key>=<value> pairs. The keys are
 * described below.
 *
 *** set <key-value-pairs>...
 *   -> OK:0
 *
 * Set settings. Any number of <key>=<value> pairs may be in the
 * command line. Other settings will be left unchanged.
 *
 * ======
 * The set and query-settings commands operate on some key-value
 * pairs. These are the settings supported so far:
 *
 *** callup-time=<N>
 * This is is time (in seconds) to call up a line.
 *
 *** end-time=<N>
 * This is the time (in seconds) for an end.
 *
 *** warn-time=<N>
 * This is the time (in seconds) remaining in an end when the display
 * shows a warning color. For example, if end-time=120 and
 * warn-time=30, then for the last 30 seconds of the end, the timer
 * will display the warning color while still counting down.
 */

using namespace std;

TimerNetworkService::TimerNetworkService(TimerControlBox*parent)
: QTcpServer(parent)
{
      controls_ = parent;
      connection_ = 0;

      connect(this, SIGNAL(newConnection()), SLOT(new_connection_signal()));

      listen();
      fprintf(stdout, "XXXX TimerNetworkService: server port = %u\n", serverPort());
      QString port_text;
      port_text.setNum(serverPort());
      controls_->network_service_port(port_text);
}

TimerNetworkService::~TimerNetworkService()
{
}

void TimerNetworkService::new_connection_signal(void)
{
      while (QTcpSocket*sock = nextPendingConnection()) {
	    if (connection_ != 0) {
		  fprintf(stdout, "XXXX TimerNetworkService: connection busy.\n");
		  fflush(stdout);
		  delete sock;
		  continue;
	    }

	    fprintf(stdout, "XXXX TimerNetworkService: Accepted a connection.\n");
	    fflush(stdout);
	    pauseAccepting();
	    connection_ = sock;
	    controls_->network_service_port("connected");
	    connect(connection_, SIGNAL(readyRead()),   SLOT(ready_read()));
	    connect(connection_, SIGNAL(disconnected()),SLOT(port_disconnected()));
      }
}

/*
 * This slot receives the signal that there is data to read. Read as
 * much data as possible, processing lines when there are line breaks.
 */
void TimerNetworkService::ready_read(void)
{
      assert(connection_);
      char buf[1024];
      qint64 rc;
      while ( connection_ && ((rc = connection_->read(buf, sizeof buf)) > 0)) {
	    for (int idx = 0 ; idx < rc ; idx += 1) {
		  if (buf[idx] != '\n') {
			line_buffer_.append(buf[idx]);
			continue;
		  }

		  process_command_(line_buffer_);
		  line_buffer_ = "";
	    }
      }
}

void TimerNetworkService::port_disconnected(void)
{
      fprintf(stdout, "XXXX TimerNetworkService: dropped connection\n");
      fflush(stdout);
      QTcpSocket*port = connection_;
      connection_ = 0;
      resumeAccepting();
      port->deleteLater();
	// Show the port in the control panel.
      QString port_text;
      port_text.setNum(serverPort());
      controls_->network_service_port(port_text);
}

static inline QString ok_with_error_code(int rc)
{
      QString res = "OK:";
      QString num;
      num.setNum(rc, 10);
      res.append(num);
      res.append("\n");
      return res;
}

static inline QString ok_with_error_code(int rc, const QString&text)
{
      QString res = "OK:";
      QString num;
      num.setNum(rc, 10);
      res.append(num);
      res.append(":");
      res.append(text);
      res.append("\n");
      return res;
}

void TimerNetworkService::process_command_(const QString&cmd)
{
      QStringList args = cmd.simplified().split(' ', QString::SkipEmptyParts);
      if (args.size() == 0) return;

      if (args[0] == "version") {
	    connection_->write(ok_with_error_code(0, VERSION_TAG).toLatin1().data());
	    return;
      }

      if (args[0] == "close-detach") {
	    disconnect(connection_, SIGNAL(readyRead()), this, SLOT(ready_read()));
	    QTcpSocket*tmp = connection_;
	    connection_ = 0;
	    resumeAccepting();
	    tmp->close();
	    return;
      }
      
      if (args[0] == "next-end") {
	    int rc = controls_->next_end_command();
	    connection_->write(ok_with_error_code(rc).toLatin1().data());
	    return;
      }

      if (args[0] == "start-timer") {
	    int rc = controls_->start_timer_command();
	    connection_->write(ok_with_error_code(rc).toLatin1().data());
	    return;
      }

      if (args[0] == "fast-forward") {
	    int rc = controls_->fast_forward_command();
	    connection_->write(ok_with_error_code(rc).toLatin1().data());
	    return;
      }

      if (args[0] == "pause-timer") {
	    int rc = controls_->pause_timer_command();
	    connection_->write(ok_with_error_code(rc).toLatin1().data());
	    return;
      }

      if (args[0] == "toggle-fullscreen") {
	    int rc = controls_->toggle_fullscreen_command();
	    connection_->write(ok_with_error_code(rc).toLatin1().data());
	    return;
      }

      if (args[0] == "set") {
	    int rc = 0;
	    for (int idx = 1 ; idx < args.size() ; idx += 1)
		  controls_->set_command(args[idx]);
	    connection_->write(ok_with_error_code(rc).toLatin1().data());
	    return;
      }

      if (args[0] == "query-settings") {
	    QString text;
	    int rc = controls_->query_settings_command(text);
	    connection_->write(ok_with_error_code(rc, text).toLatin1().data());
	    return;
      }

      connection_->write("ERROR:-1:No such command\n");
}
