#ifndef __TimerMain_H
#define __TimerMain_H
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

namespace Ui {
      class TimerMain;
}

class TimerControlBox;

class TimerMain : public QMainWindow
{
      Q_OBJECT

    public:
      explicit TimerMain(QWidget*parent =0);
      ~TimerMain();

      void set_control_box(TimerControlBox*box);

    public:
      void set_end_number(int val, bool practice_flag);
      
	// Set the text for the active line display.
      void set_line_text(const class QString&txt);
      
      enum timer_mode_t { TIMER_CALLUP, TIMER_END, TIMER_END_WARN, TIMER_STOP };
      void set_time_value(int val, timer_mode_t mode);

	// Toggle fullscreen mode. Return true if the result leaves
	// the timer in fullscreen mode.
      bool toggle_fullscreen(void);
      void set_fullscreen(bool flag);

    protected:
      void keyPressEvent(QKeyEvent*event);

    private slots:

    private:
      Ui::TimerMain *ui;

      TimerControlBox*control_box_;

      bool fullscreen_flag_;
      QPalette red_;
      QPalette yellow_;
      QPalette green_;

};

#endif
