#ifndef __TimerServiceItem_H
#define __TimerServiceItem_H
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

# include <QListWidgetItem>
# include <QString>
# include <qzeroconf.h>

/*
 * This is a service item that is found by QZeroConf.  We save the
 * QZeroConfService in this item, and set the text to be a string
 * representation that is displayed in the QListWidget.
 */
class TimerServiceItem  : public QListWidgetItem
{
  public:
      TimerServiceItem(QZeroConfService srv);
      ~TimerServiceItem() = default;

      QHostAddress ip() const { return srv_->ip(); }
      quint16 port() const { return srv_->port(); }

      static inline QString key(QZeroConfService srv) {
	    return srv->name() + " [" + srv->ip().toString() + ":"
		  + QString::number(srv->port(),10) + "]";
      }

  private:
      QZeroConfService srv_;
};


#endif
