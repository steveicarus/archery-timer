#ifndef __ArcheryTimerControl_H
#define __ARcheryTimerControl_H
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

# include  <QMainWindow>
# include  <qzeroconf.h>
# include  "TimerServiceItem.h"
# include  "TimerProtocol.h"

namespace Ui {
    class ArcheryTimerControl;
}

/*
 * This is the main window of the Archery Timer Control panel.
 */
class ArcheryTimerControl : public QMainWindow
{
      Q_OBJECT

  public:
      explicit ArcheryTimerControl(QWidget*parent =0);
      ~ArcheryTimerControl();

  private slots:
      // Slots that receive zero-conf signals.
      void service_added(QZeroConfService srv);
      void service_removed(QZeroConfService srv);
      void service_updated(QZeroConfService srv);

      void callup_time_edited();
      void end_time_edited();
      void warn_time_edited();
      void line_ab_edited();
      void line_cd_edited();
      void line_cd_checked(int);
      void toggle_order_checked(int);

      void connect_button_clicked(bool checked);
      void version_set(const QString&txt);

      void pause_button_clicked(bool flag);
      void run_button_clicked(bool flag);
      void skip_button_clicked(bool flag);
      void next_button_clicked(bool flag);
      void emergency_stop_button_clicked(bool flag);
      void makeup_end_button_clicked(bool flag);

  private:
      Ui::ArcheryTimerControl *ui;
      QZeroConf zero_conf_;

      // Service that I'm connected to.
      TimerProtocol timer_protocol_;
};

#endif
