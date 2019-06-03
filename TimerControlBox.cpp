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

# include  "TimerControlBox.h"
# include  "ui_TimerControlBox.h"
# include  "TimerMain.h"
# include  <QtMultimedia/QSound>
# include  <cstdio>
# include  <cassert>

using namespace std;

TimerControlBox::TimerControlBox(QWidget*parent)
: QMainWindow(parent), timer_window_(0)
{
      ui = new Ui::TimerControlBox;
      ui->setupUi(this);

      sound_callup_ = new QSound("sounds/whistle-2.wav");
      sound_start_  = new QSound("sounds/whistle-1.wav");
      sound_clear_  = new QSound("sounds/whistle-3.wav");

      timer_callup_ = 0;
      timer_end_ = 0;
      timer_end_warn_ = 0;
      timer_running_flag_ = false;
      timer_next_line_ = false;

	// The timer...
      connect(&timer_,
	      SIGNAL(timeout()),
	      SLOT(timer_timeout()));
      
	// The control box buttons...
      connect(ui->go_button,
	      SIGNAL(clicked()),
	      SLOT(go_button()));
      connect(ui->prev_button,
	      SIGNAL(clicked()),
	      SLOT(prev_button()));
      connect(ui->next_button,
	      SIGNAL(clicked()),
	      SLOT(next_button()));
      connect(ui->line_cd_check,
	      SIGNAL(stateChanged(int)),
	      SLOT(line_cd_check(int)));
      connect(ui->reset_ends_button,
	      SIGNAL(clicked()),
	      SLOT(reset_ends_button()));

	// Cannot go until an end is set up.
      ui->go_button->setEnabled(false);
}

TimerControlBox::~TimerControlBox()
{
      delete sound_clear_;
      delete sound_start_;
      delete sound_callup_;
      delete ui;
}

void TimerControlBox::set_timer_window(TimerMain*timer)
{
      assert(timer_window_ == 0);
      timer_window_ = timer;
}

/*
 * Advance to the next end. Advance the end counter, set up the timer
 * counters to the correct sizes, and set the line label to the line
 * that starts the next end.
 *
 * If the tiner is still running for the current end, then return an
 * error.
 */
int TimerControlBox::next_end_command(void)
{
      if (timer_running_flag_) {
	    return -1;
      }

	// Count and display the end number.
      int end_counter = ui->end_count_box->value();
      ui->end_count_box->setValue(end_counter+1);
      bool practice_flag = ui->practice_check->checkState()? true : false;
      bool toggle_order_flag = ui->toggle_order_check->checkState()? true : false;
      timer_window_->set_end_number(end_counter, practice_flag);

	// This is true if there are multiple lines.
      bool dual_line_flag = ui->line_cd_check->checkState()? true : false;

      if (dual_line_flag && toggle_order_flag && (end_counter%2 == 0)) {
	    
	      // If there are 2 lines, and this is an even end, then
	      // the cd line is first up, followed by the ab line.
	    timer_window_->set_line_text(ui->line_cd_text->text());
	    timer_next_line_txt_ = ui->line_ab_text->text();

      } else {
	      // Starting with the ab line, set the line in the display,
	      // and stash the cd line name for later use.
	    timer_window_->set_line_text(ui->line_ab_text->text());
	    timer_next_line_txt_ = ui->line_cd_text->text();
      }

	// Load the state machine with the phase times.
      timer_callup_ = ui->callup_time_text->text().toInt();
      timer_end_ = ui->end_time_text->text().toInt() + 1;
      timer_end_warn_ = ui->warn_time_text->text().toInt();
      timer_next_line_ = dual_line_flag? timer_end_ : 0;

	// State the callup time in the timer display, and enable the
	// GO button.
      timer_window_->set_time_value(timer_callup_, TimerMain::TIMER_CALLUP);
      ui->go_button->setEnabled(true);

      return end_counter;
}

int TimerControlBox::start_timer_command(void)
{
	// If the timer is already running, then do nothing.
      if (timer_running_flag_) return -1;
      
      set_running_state_();
      ui->go_button->setText("PAUSE");
      timer_.start(1000);
      sound_callup_->play();
      return 0;
}

int TimerControlBox::fast_forward_command(void)
{
	// If the timer is not running, then do nothing.
      if (!timer_running_flag_) return -1;
      
      if (timer_callup_ >= 1) {
	    timer_callup_ = 1;
      } else if (timer_end_ >= 1) {
	    timer_end_ = 1;
      } else {
	    return -1;
      }
      timer_timeout();
      return 0;
}

int TimerControlBox::pause_timer_command(void)
{
	// If the timer is not running, then do nothing.
      if (!timer_running_flag_) return -1;
	// Stop the timer, and enable the "RUN" button.
      timer_.stop();
      set_paused_state_();
      ui->go_button->setText("RUN");
      return 0;
}

void TimerControlBox::network_service_port(const QString&port_text)
{
      ui->remote_port_text->setText(port_text);
}

void TimerControlBox::set_running_state_(void)
{
      timer_running_flag_ = true;
      ui->prev_button->setEnabled(false);
      ui->next_button->setEnabled(false);
      ui->reset_ends_button->setEnabled(false);
}

void TimerControlBox::set_paused_state_(void)
{
      ui->prev_button->setEnabled(true);
      ui->next_button->setEnabled(true);
      ui->reset_ends_button->setEnabled(true);
      timer_running_flag_ = false;
}

void TimerControlBox::line_cd_check(int state)
{
      if (state) {
	    ui->line_cd_text->setEnabled(true);
      } else {
	    ui->line_cd_text->setEnabled(false);
      }
}

void TimerControlBox::go_button(void)
{
      if (timer_running_flag_) {
	    pause_timer_command();
      } else {
	    start_timer_command();
      }
}

void TimerControlBox::prev_button(void)
{
      fprintf(stdout, "XXXX NOT IMPLEMENTED\n");
      fflush(stdout);
}

void TimerControlBox::next_button(void)
{
      next_end_command();
}

void TimerControlBox::reset_ends_button(void)
{
      ui->end_count_box->setValue(1);
}

void TimerControlBox::timer_timeout(void)
{
      if (timer_callup_ > 0) {
	      // Still calling archers to the line.
	    timer_callup_ -= 1;
	    timer_window_->set_time_value(timer_callup_, TimerMain::TIMER_CALLUP);
	      // If time's up for calling archers to the line, then
	      // signal line's hot.
	    if (timer_callup_ == 0) {
		  sound_start_->play();
	    }

      } else if (timer_end_ > 0) {
	      // Counting down the end.
	    timer_end_ -= 1;
	    if (timer_end_ > timer_end_warn_)
		  timer_window_->set_time_value(timer_end_, TimerMain::TIMER_END);
	    else
		  timer_window_->set_time_value(timer_end_, TimerMain::TIMER_END_WARN);

	      // If time's up...
	    if (timer_end_ == 0) {
		  if (timer_next_line_) {
			  // If the other line has to shoot, reset the
			  // state machine, set the new line text, and restar.
			timer_next_line_ = false;
			timer_callup_ = ui->callup_time_text->text().toInt();
			timer_end_ = ui->end_time_text->text().toInt() + 1;
			timer_window_->set_line_text(timer_next_line_txt_);
			sound_callup_->play();
		  } else {
			sound_clear_->play();
		  }
	    }

      } else {
	      // All the steps ran out, so stop the timer by clicking
	      // the go_button toggle switch.
	    timer_window_->set_time_value(0, TimerMain::TIMER_STOP);
	    pause_timer_command();
	    ui->go_button->setEnabled(false);
      }
}
