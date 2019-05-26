#ifndef __TimerNetworkService_H
#define __TimerNetworkService_H
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

/*
 * The TimerNetworkService accepts connections on a TCP port, with the
 * connection being a link to a remote controller. Only one controller
 * at a time may be connected.
 *
 * A connected controller sends commands as ASCII text lines. Each
 * line starts with a command, and the remaining words are options for
 * the commands. The controller responds with "OK <words>" with the
 * words being optional responses, and depend on the command being
 * responded to. If there in an error, the response is
 * "ERROR:<N>:<text>" where <N> is an error number, and <text> is a
 * human readable error string.
 *
 * The supported commands are:
 *
 * close-detach
 *     close the link and detach the controller.
 *
 * next-end
 *
 * start-timer
 *
 * fast-forward
 *
 * pause-timer
 *
 */

# include  <QString>
# include  <QTcpServer>

class TimerControlBox;
class QTcpSocket;

class TimerNetworkService : private QTcpServer {

      Q_OBJECT

    public:
      explicit TimerNetworkService(TimerControlBox*parent);
      ~TimerNetworkService();

    private slots:
      void new_connection_signal(void);

      void ready_read(void);

    private:
      void process_command_(const QString&cmd);

    private:
      TimerControlBox*controls_;
      QTcpSocket*     connection_;
      QString         line_buffer_;
};

#endif
