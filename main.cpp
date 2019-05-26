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

# include  <QApplication>
# include  "TimerMain.h"
# include  "TimerControlBox.h"
# include  "TimerNetworkService.h"

int main(int argc, char*argv[])
{
    QApplication app(argc, argv);

    TimerMain timer_main;
    timer_main.show();

    TimerControlBox timer_box;
    timer_main.set_control_box(&timer_box);
    timer_box.show();

      // Bind the timer window with the control box.
    timer_box.set_timer_window(&timer_main);

      // Create a network service and bind it to the control box. By
      // setting the timer_box as the parent, the allocated object
      // becomes "owned", so won't leak.
    new TimerNetworkService(&timer_box);

    return app.exec();
}
