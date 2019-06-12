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
    bool timer_fullscreen_flag = false;
    bool control_box_show_flag = true;

      // Look for arguments that are interesting to us.
    QStringList args = app.arguments();
    for (int idx = 1 ; idx < args.size() ; idx += 1) {
	  if (args[idx] == "--timer-fullscreen") {
		timer_fullscreen_flag = true;
		continue;
	  }
	  if (args[idx] == "--no-timer-fullscreen") {
		timer_fullscreen_flag = false;
		continue;
	  }
	  if (args[idx] == "--control-box") {
		control_box_show_flag = true;
		continue;
	  }
	  if (args[idx] == "--no-control-box") {
		control_box_show_flag = false;
		continue;
	  }
	  if (args[idx] == "--fulscreen") {
		timer_fullscreen_flag = true;
		control_box_show_flag = false;
		continue;
	  }
	  if (args[idx] == "--no-fullscreen") {
		timer_fullscreen_flag = false;
		control_box_show_flag = true;
	  }
    }
    
    TimerMain timer_main;
    timer_main.show();
    timer_main.set_fullscreen(timer_fullscreen_flag);

    TimerControlBox timer_box;
    timer_main.set_control_box(&timer_box);

      // Bind the timer window with the control box.
    timer_box.set_timer_window(&timer_main);

      // Show or hide the control box based on fullscreen mode.
    if (control_box_show_flag)
	  timer_box.show();
    else
	  timer_box.hide();

      // Create a network service and bind it to the control box. By
      // setting the timer_box as the parent, the allocated object
      // becomes "owned", so won't leak.
    new TimerNetworkService(&timer_box);

    return app.exec();
}
