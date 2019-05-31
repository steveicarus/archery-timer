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
# include  <QTcpSocket>
# include  <cstdio>
# include  <cassert>

using namespace std;

TimerNetworkService::TimerNetworkService(TimerControlBox*parent)
: QTcpServer(parent)
{
      controls_ = parent;
      connection_ = 0;

      connect(this, SIGNAL(newConnection()), SLOT(new_connection_signal()));

      listen();
      fprintf(stdout, "XXXX TimerNetworkService: server port = %u\n", serverPort());
      fflush(stdout);
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

void TimerNetworkService::process_command_(const QString&cmd)
{
      QStringList args = cmd.simplified().split(' ', QString::SkipEmptyParts);
      if (args.size() == 0) return;

      if (args[0] == "version") {
	    connection_->write("OK:version text\n");
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

      connection_->write("ERROR:-1:No such command\n");
}
