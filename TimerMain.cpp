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

# include  "TimerMain.h"
# include  "ui_TimerMain.h"

TimerMain::TimerMain(QWidget*parent)
: QMainWindow(parent)
{
      ui = new Ui::TimerMain;
      ui->setupUi(this);
}

TimerMain::~TimerMain()
{
      delete ui;
}

void TimerMain::set_end_number(int val, bool practice_flag)
{
      QString txt;
      txt.setNum(val);
      if (practice_flag) txt.prepend("P ");
      ui->end_text->setText(txt);
}

void TimerMain::set_line_text(const class QString&txt)
{
      ui->line_text->setText(txt);
}

void TimerMain::set_time_value(int val, timer_mode_t mode)
{
      QString val_text;
      val_text.setNum(val);
      ui->timer_text->setText(val_text);
}
