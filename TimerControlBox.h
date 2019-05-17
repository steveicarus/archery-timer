#ifndef __TimerControlBox_H
#define __TimerControlBox_H
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
# include  <QTimer>

namespace Ui {
      class TimerControlBox;
}

class TimerMain;
class QSound;

class TimerControlBox : public QMainWindow {

      Q_OBJECT

    public:
      explicit TimerControlBox(QWidget*parent =0);
      ~TimerControlBox();

      void set_timer_window(TimerMain*timer);
					  
    private slots:
      void go_toggle(bool state);
      void prev_button(void);
      void next_button(void);
      void reset_ends_button(void);
      void line_cd_check(int state);

      void timer_timeout(void);

    private:
      void go_button_toggle_(bool state);
      void set_running_state_(void);
      void set_paused_state_(void);

    private:
	// User interface for the control box.
      Ui::TimerControlBox *ui;

	// This is the timer window, that has all the display.
      TimerMain* timer_window_;

	// Sounds that can be played at various times.
      QSound*sound_callup_;
      QSound*sound_start_;
      QSound*sound_clear_;

	// State machine and timer for running an end.
      QTimer timer_;
      int timer_callup_;
      int timer_end_;
      int timer_end_warn_;

      bool timer_next_line_;
      QString timer_next_line_txt_;
};

#endif