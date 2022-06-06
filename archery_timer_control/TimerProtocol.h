#ifndef __TimerProtocol_H
#define __TimerProtocol_H
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

# include  <QList>
# include  <QObject>
# include  <QString>
# include  <QTcpSocket>
# include  "TimerServiceItem.h"

/*
 * This class represents the protocol engine for communicating with the
 * TimerNetworkService, which is the remote timer.
 */
class TimerProtocol : public QObject
{
      Q_OBJECT
      
  public:
      TimerProtocol(QObject*parent =0);

      void connect_to_timer(TimerServiceItem*item);
      void disconnect_from_timer(void);
      
      bool is_connected() const;
      bool is_connected(TimerServiceItem*item) const;

      // Send to the remote a list of settings. Each item in the list should
      // be formatted as <key>=<value>. The method will bundle the settings
      // into a command and set that command to the remote.
      void send_settings(const QList<QString>&settings);

      void pause_timer(void);
      void start_timer(void);
      void fast_forward(void);
      void emergency_stop(void);
      void makeup_end(void);

      // Send to the remote the "next-end" command.
      void next_end(int end_number, bool practice_flag);

  signals:
      // Emit this signal when the protocol receives the version string from
      // the remote timer. This indicates that the connection is established.
      void set_version_string(const QString&);
      
  private slots:
      // Slots form the Tcp connection
      void slot_connected_();
      void slot_disconnected_();
      void slot_ready_read_();

  private:
      void send_initial_settings() const;
      void socket_write_(const char*txt);

  private:
      QTcpSocket socket_;
      TimerServiceItem* connect_;

      QList<QString> command_queue_;
};

#endif
