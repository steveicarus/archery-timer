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
# include  <QString>
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

    public: // These methods are commands that remote controllers used
	    // to control the timer.

	// Handle special ends, such as makeup ends, by using this
	// method instead of the next_end_command() method.
      int special_end_command(const QString&label_txt);

	// This method sets up for the next end. If the timer is still
	// running, it returns -1. Otherwise, it sets up the counters
	// for the next end and returns >= 0.
      int next_end_command(int end_number, bool practice_flag);
      int next_end_command(void);

	// Start the timer.
      int start_timer_command(void);

	// While the timer is running, advance immediately to the next
	// phase, which may be the end of the end. Do NOT go to the
	// next end.
      int fast_forward_command(void);

	// Stop the timer, without advancing to the next end.
      int pause_timer_command(void);

	// Emergency Stop
	  int emergency_stop_command(void);

	// Toggle fullscreen displal mode.
      int toggle_fullscreen_command(void);

	// Set a setting.
      int set_command(const QString&text);

	// Query controller settings.
      int query_settings_command(QString&text);

	// Display the network service port in use. This should only
	// be called by the TimerNetworkService instance.
      void network_service_port(const QString&port_text);

    private slots:
      void go_button(void);
      void prev_button(void);
      void next_button(void);
      void reset_ends_button(void);
      void line_cd_check(int state);

      void timer_timeout(void);

    private:
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
	  QSound*sound_emergency_stop_;

	// State machine and timer for running an end.
      QTimer timer_;
      int timer_callup_;
      int timer_end_;
      int timer_end_warn_;
      bool timer_running_flag_;

      bool timer_next_line_;
      QString timer_next_line_txt_;
};

#endif
