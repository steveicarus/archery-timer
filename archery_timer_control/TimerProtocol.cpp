/*
 * Copyright (c) 2022 Stephen Williams (steve@icarus.com)
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

# include  "TimerProtocol.h"
# include  <cassert>

bool timer_protocol_debug = true;

TimerProtocol::TimerProtocol(QObject*parent)
: QObject(parent), socket_(parent)
{
      connect_ = nullptr;

      connect(&socket_, SIGNAL(connected()), this, SLOT(slot_connected_()));
      connect(&socket_, SIGNAL(disconnected()), this, SLOT(slot_disconnected_()));
      connect(&socket_, SIGNAL(readyRead()), this, SLOT(slot_ready_read_()));
}

void TimerProtocol::connect_to_timer(TimerServiceItem*item)
{
      assert(connect_ == nullptr);
      connect_ = item;
      socket_.connectToHost(item->ip(), item->port());
}

void TimerProtocol::disconnect_from_timer(void)
{
      static const char DISCONNECT_CMD[] = "close-detach";
      command_queue_.push_back(DISCONNECT_CMD);
      socket_write_(DISCONNECT_CMD);
}

bool TimerProtocol::is_connected() const
{
      return connect_ != nullptr;
}

bool TimerProtocol::is_connected(TimerServiceItem*item) const
{
      return connect_ == item;
}

/*
 * This slot is called when the TcpSocket detects that the link to the remote
 * timer is connected.  Send a "version" command to the remote to verify that
 * the remote is up, and to get the version string.
 */
void TimerProtocol::slot_connected_()
{
      static const char VERSION_CMD[] = "version";
      command_queue_.push_back(VERSION_CMD);
      socket_write_(VERSION_CMD);
}

/*
 * This slot is called when the TcpSocket detects that the link to the remote
 * timer drops. It shouild be intentional, or a network failure.
 */
void TimerProtocol::slot_disconnected_()
{
      connect_ = nullptr;
      socket_.disconnectFromHost();

      command_queue_.clear();
      emit set_version_string("-");
}

/*
 * This slot is called when the TcpSocket detects that there are bytes to be
 * read from the remote. Get lines of data from the socket, parse the lines
 * into protocol responses, and dispatch the response.
 */
void TimerProtocol::slot_ready_read_()
{
      while (socket_.canReadLine()) {
	    // Get the next line.
	    char buffer[512];
	    socket_.readLine(buffer, sizeof(buffer));

	    if (timer_protocol_debug) {
		  printf("TimerProtocol: Recv: %s", buffer);
		  fflush(stdout);
	    }

	    // Parse it into the fields: OK:<num>:<text>
	    char ok_error[32];
	    int code;
	    char text[512];
	    int rc = sscanf(buffer, "%32[^:]:%d:%511[^\n]", ok_error, &code, text);
	    assert(rc == 3 || rc == 2);
	    if (rc == 2)
		  text[0] = 0;

	    QString last_command;
	    if (!command_queue_.empty()) {
		  last_command = command_queue_.front();
		  command_queue_.pop_front();
	    }

	    if (last_command == "version") {
		  // Response to the version command.
		  emit set_version_string(text);
	    } else if (last_command == "set") {
		  // Ignore SET response.
	    } else if (last_command == "start-timer") {
		  // Ignore start-timer response.
	    } else if (last_command == "fast-forward") {
		  // Ignore fast-forward response.
	    } else if (last_command == "pause-timer") {
		  // Ignore pause-timer response.
	    } else if (last_command == "next-end") {
		  // Ignore next-end response.
	    } else if (last_command == "emergency-stop") {
		  // Ignore emergency-stop response.
	    } else if (last_command == "makeup-end") {
		  // Ignore makeup-end response.
	    } else {
		  printf("XXXX Unexpected last_command: %s\n", last_command.toStdString().c_str());
	    }
      }
}

void TimerProtocol::send_settings(const QList<QString>&settings)
{
      if (! is_connected())
	    return;

      char buffer[512];
      char*bptr = buffer;
      size_t len = sizeof buffer;
      int rc = snprintf(bptr, len, "set");
      bptr += rc;
      len -= rc;

      for (auto&&cur : settings) {
	    rc = snprintf(bptr, len, " %s", cur.toStdString().c_str());
	    bptr += rc;
	    len -= rc;
      }
      
      command_queue_.push_back("set");
      socket_write_(buffer);
}

void TimerProtocol::pause_timer(void)
{
      if (! is_connected())
	    return;

      command_queue_.push_back("pause-timer");
      socket_write_("pause-timer");
}

void TimerProtocol::start_timer(void)
{
      if (! is_connected())
	    return;

      command_queue_.push_back("start-timer");
      socket_write_("start-timer");
}

void TimerProtocol::fast_forward(void)
{
      if (! is_connected())
	    return;

      command_queue_.push_back("fast-forward");
      socket_write_("fast-forward");
}

void TimerProtocol::next_end(int end_number, bool practice_flag)
{
      // Do nothing if the remote is unconnected
      if (! is_connected())
	    return;

      char buffer[512];
      snprintf(buffer, sizeof buffer, "next-end %d %s", end_number,
	       practice_flag? "practice" : "no-practice");
      command_queue_.push_back("next-end");
      socket_write_(buffer);
}

void TimerProtocol::emergency_stop(void)
{
      if (! is_connected())
	    return;

      command_queue_.push_back("emergency-stop");
      socket_write_("emergency-stop");
}

void TimerProtocol::makeup_end(void)
{
      if (! is_connected())
	    return;

      command_queue_.push_back("makeup-end");
      socket_write_("makeup-end");
}

void TimerProtocol::socket_write_(const char*txt)
{
      if (timer_protocol_debug) {
	    printf("TimerProtocol: Send: %s\n", txt);
	    fflush(stdout);
      }

      qint64 len = strlen(txt);
      while (len > 0) {
	    qint64 rc = socket_.write(txt, len);
	    assert(rc > 0);
	    txt += rc;
	    len -= rc;
      }
      socket_.write("\n", 1);
}
