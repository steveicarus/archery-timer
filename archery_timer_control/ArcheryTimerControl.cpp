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

# include  "ArcheryTimerControl.h"
# include  "ui_ArcheryTimerControl.h"
# include  "TimerServiceItem.h"
# include  <QListWidget>
# include  <cassert>

ArcheryTimerControl::ArcheryTimerControl(QWidget*parent)
: QMainWindow(parent)
{
      ui = new Ui::ArcheryTimerControl;
      ui->setupUi(this);

      // Connect the zero-conf signals.
      connect(&zero_conf_, &QZeroConf::serviceAdded,
	      this, &ArcheryTimerControl::service_added);
      connect(&zero_conf_, &QZeroConf::serviceRemoved,
	      this, &ArcheryTimerControl::service_removed);
      connect(&zero_conf_, &QZeroConf::serviceUpdated,
	      this, &ArcheryTimerControl::service_updated);

      connect(ui->connect_button, SIGNAL(clicked(bool)),
	      this, SLOT(connect_button_clicked(bool)));
      connect(&timer_protocol_, SIGNAL(set_version_string(const QString&)),
	      this, SLOT(version_set(const QString&)));

      connect(ui->callup_time_text, SIGNAL(editingFinished()),
	      this, SLOT(callup_time_edited()));
      connect(ui->end_time_text, SIGNAL(editingFinished()),
	      this, SLOT(end_time_edited()));
      connect(ui->warn_time_text, SIGNAL(editingFinished()),
	      this, SLOT(warn_time_edited()));
      connect(ui->line_ab_text, SIGNAL(editingFinished()),
	      this, SLOT(line_ab_edited()));
      connect(ui->line_cd_text, SIGNAL(editingFinished()),
	      this, SLOT(line_cd_edited()));
      connect(ui->line_cd_check, SIGNAL(stateChanged(int)),
	      this, SLOT(line_cd_checked(int)));
      connect(ui->toggle_order_check, SIGNAL(stateChanged(int)),
	      this, SLOT(toggle_order_checked(int)));

      connect(ui->pause_button, SIGNAL(clicked(bool)),
	      this, SLOT(pause_button_clicked(bool)));
      connect(ui->run_button, SIGNAL(clicked(bool)),
	      this, SLOT(run_button_clicked(bool)));
      connect(ui->skip_button, SIGNAL(clicked(bool)),
	      this, SLOT(skip_button_clicked(bool)));
      connect(ui->next_button, SIGNAL(clicked(bool)),
	      this, SLOT(next_button_clicked(bool)));

      connect(ui->emergency_stop_button, SIGNAL(clicked(bool)),
	      this, SLOT(emergency_stop_button_clicked(bool)));
      connect(ui->makeup_end_button, SIGNAL(clicked(bool)),
	      this, SLOT(makeup_end_button_clicked(bool)));

      ui->connect_button->setEnabled(false);
      ui->timers_list->setEnabled(true);

      zero_conf_.startBrowser("_icarus_archery_timer._tcp");
      
}

ArcheryTimerControl::~ArcheryTimerControl()
{
      delete ui;
}

/*
 * This is a convenience function for gettint a setting value from a QLineEdit
 * widget, formatting it so that a "set" command can understand it, and
 * returning the formatted setting.
 */
static QString format_numeric_set(const QString&name, QLineEdit*wid)
{
      QString txt = wid->text();
      unsigned int val = txt.toUInt();
      txt.setNum(val, 10);
      wid->setText(txt);
      return name + "=" + txt;
}

/*
 * This slot is called when the ZeroConf detects an instance of the service on
 * the network. Add the service to the list of available services (displayed
 * to the user) and make sure the connect button is enabled.
 */
void ArcheryTimerControl::service_added(QZeroConfService srv)
{
      TimerServiceItem *item = new TimerServiceItem(srv);
      ui->timers_list->addItem(item);
      if (! timer_protocol_.is_connected())
	    ui->connect_button->setEnabled(true);
}

/*
 * This slot is called when the ZeroConf detects that an instance of the
 * service disapears from the network. Remove the service from the network,
 * and if that was the service that we were connected to, then disconnect from
 * it. Also set the enable for the connect button as appropriate.
 */
void ArcheryTimerControl::service_removed(QZeroConfService srv)
{
      QString key = TimerServiceItem::key(srv);
      QList<QListWidgetItem*> items = ui->timers_list->findItems(key, Qt::MatchExactly);

      for (auto&&cur : items) {
	    TimerServiceItem*item = dynamic_cast<TimerServiceItem*>(cur);
	    ui->timers_list->removeItemWidget(item);
	    if (timer_protocol_.is_connected(item))
		  timer_protocol_.disconnect_from_timer();
	    delete item;
      }

      // No items left? Then we cannot connect to anything.
      // We are already connected? Then disable the connect button.
      // Otherwise, enable the connect button.
      if (ui->timers_list->count() == 0)
	    ui->connect_button->setEnabled(false);
      else if (timer_protocol_.is_connected())
	    ui->connect_button->setEnabled(false);
      else
	    ui->connect_button->setEnabled(true);
}

void ArcheryTimerControl::service_updated(QZeroConfService srv)
{
      printf("XXXX service_updated: %s\n", TimerServiceItem::key(srv).toStdString().c_str());
}

void ArcheryTimerControl::connect_button_clicked(bool checked)
{
      if (checked) {
	    // This enables the connection.
	    QListWidgetItem*item_raw = ui->timers_list->currentItem();
	    if (item_raw == nullptr) {
		  ui->connect_button->setChecked(false);
		  return;
	    }

	    TimerServiceItem*item = dynamic_cast<TimerServiceItem*> (item_raw);
	    if (item) {
		  timer_protocol_.connect_to_timer(item);
		  ui->timers_list->setEnabled(false);
	    }

      } else {
	    // This disables the connection
	    timer_protocol_.disconnect_from_timer();
	    ui->timers_list->setEnabled(true);
      }
}

void ArcheryTimerControl::callup_time_edited()
{
      QString txt = format_numeric_set("callup-time", ui->callup_time_text);
      QList<QString> settings_list;
      settings_list.push_back(txt);
      timer_protocol_.send_settings(settings_list);
}

void ArcheryTimerControl::end_time_edited()
{
      QString txt = format_numeric_set("end-time", ui->end_time_text);
      QList<QString> settings_list;
      settings_list.push_back(txt);
      timer_protocol_.send_settings(settings_list);
}

void ArcheryTimerControl::warn_time_edited()
{
      QString txt = format_numeric_set("warn-time", ui->warn_time_text);
      QList<QString> settings_list;
      settings_list.push_back(txt);
      timer_protocol_.send_settings(settings_list);
}

void ArcheryTimerControl::line_ab_edited()
{
      QString txt = "line-one=" + ui->line_ab_text->text();
      QList<QString> settings_list;
      settings_list.push_back(txt);
      timer_protocol_.send_settings(settings_list);
}

void ArcheryTimerControl::line_cd_edited()
{
      QString txt = "line-two=" + ui->line_cd_text->text();
      QList<QString> settings_list;
      settings_list.push_back(txt);
      timer_protocol_.send_settings(settings_list);
}

void ArcheryTimerControl::line_cd_checked(int)
{
      QList<QString> settings_list;

      if (ui->line_cd_check->isChecked())
	    settings_list.push_back("line-two-enabled=1");
      else
	    settings_list.push_back("line-two-enabled=0");

      timer_protocol_.send_settings(settings_list);
}

void ArcheryTimerControl::toggle_order_checked(int)
{
      QList<QString> settings_list;

      if (ui->toggle_order_check->isChecked())
	    settings_list.push_back("toggle-lines=1");
      else
	    settings_list.push_back("toggle-lines=0");

      timer_protocol_.send_settings(settings_list);
}

void ArcheryTimerControl::pause_button_clicked(bool)
{
      timer_protocol_.pause_timer();
}

void ArcheryTimerControl::run_button_clicked(bool)
{
      timer_protocol_.start_timer();
}

void ArcheryTimerControl::skip_button_clicked(bool)
{
      timer_protocol_.fast_forward();
}

void ArcheryTimerControl::next_button_clicked(bool)
{
      int next_end = ui->end_count_box->value();
      bool practice_flag = ui->practice_check->isChecked();
      timer_protocol_.next_end(next_end, practice_flag);
      ui->end_count_box->setValue(next_end + 1);
}

void ArcheryTimerControl::emergency_stop_button_clicked(bool)
{
      timer_protocol_.emergency_stop();
}

void ArcheryTimerControl::makeup_end_button_clicked(bool)
{
      timer_protocol_.makeup_end();
}

/*
 * When we get a version string from the remote, we are finished the initial
 * connect to the server. So send a bunch of settings to the server to make
 * sure it matches the behavior we want.
 */
void ArcheryTimerControl::version_set(const QString&version_txt)
{
      ui->server_version_label->setText(version_txt);

      if (!timer_protocol_.is_connected())
	    return;

      QList<QString> settings_list;
      QString txt;

      txt = format_numeric_set("callup-time", ui->callup_time_text);
      settings_list.push_back(txt);

      txt = format_numeric_set("end-time", ui->end_time_text);
      settings_list.push_back(txt);

      txt = format_numeric_set("warn-time", ui->warn_time_text);
      settings_list.push_back(txt);

      txt = "line-one=" + ui->line_ab_text->text();
      settings_list.push_back(txt);

      txt = "line-two=" + ui->line_cd_text->text();
      settings_list.push_back(txt);

      if (ui->line_cd_check->isChecked())
	    settings_list.push_back("line-two-enabled=1");
      else
	    settings_list.push_back("line-two-enabled=0");

      if (ui->toggle_order_check->isChecked())
	    settings_list.push_back("toggle-lines=1");
      else
	    settings_list.push_back("toggle-lines=0");

      timer_protocol_.send_settings(settings_list);

      // When the remote connects, enable the control buttons.
      ui->pause_button->setEnabled(true);
      ui->run_button->setEnabled(true);
      ui->skip_button->setEnabled(true);
      ui->next_button->setEnabled(true);
      ui->emergency_stop_button->setEnabled(true);
      ui->makeup_end_button->setEnabled(true);
}
