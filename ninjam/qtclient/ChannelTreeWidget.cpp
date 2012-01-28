/*
    Copyright (C) 2012 Stefan Hajnoczi <stefanha@gmail.com>

    Wahjam is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Wahjam is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wahjam; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <QPushButton>
#include "ChannelTreeWidget.h"

ChannelTreeWidget::ChannelTreeWidget(QWidget *parent)
  : QTreeWidget(parent)
{
  setColumnCount(5);
  setHeaderHidden(true);
  setRootIsDecorated(false);
  setItemsExpandable(false);
  setSelectionMode(QAbstractItemView::NoSelection);

  QTreeWidgetItem *local = addRootItem("Local");
  QTreeWidgetItem *metronome = addChannelItem(local, "Metronome", CF_BOOST);
  connect(itemWidget(metronome, 1), SIGNAL(toggled(bool)),
          this, SIGNAL(MetronomeMuteChanged(bool)));
  connect(itemWidget(metronome, 2), SIGNAL(toggled(bool)),
          this, SIGNAL(MetronomeBoostChanged(bool)));
}

QTreeWidgetItem *ChannelTreeWidget::addRootItem(const QString &text)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(this);
  item->setText(0, text);
  item->setExpanded(true);
  return item;
}

QTreeWidgetItem *ChannelTreeWidget::addChannelItem(QTreeWidgetItem *parent, const QString &text, int flags)
{
  QTreeWidgetItem *channel = new QTreeWidgetItem(parent);
  channel->setText(0, text);

  QPushButton *pushbutton = new QPushButton("Mute");
  pushbutton->setCheckable(true);
  int idx = 1;
  setItemWidget(channel, idx++, pushbutton);

  if (flags & CF_BROADCAST) {
    pushbutton = new QPushButton("Broadcast");
    pushbutton->setCheckable(true);
    setItemWidget(channel, idx++, pushbutton);
  }

  if (flags & CF_BOOST) {
    pushbutton = new QPushButton("Boost (+3dB)");
    pushbutton->setCheckable(true);
    setItemWidget(channel, idx++, pushbutton);
  }

  return channel;
}

void ChannelTreeWidget::addLocalChannel(int ch, const QString &name, bool mute, bool broadcast)
{
  QTreeWidgetItem *local = topLevelItem(0);
  QTreeWidgetItem *channel = addChannelItem(local, name, CF_BROADCAST | CF_BOOST);

  QPushButton *button = static_cast<QPushButton*>(itemWidget(channel, 1));
  button->setChecked(mute);
  localChannelHash.insert(button, ch);
  connect(button, SIGNAL(toggled(bool)),
          this, SLOT(mapLocalChannelMuteChanged(bool)));

  button = static_cast<QPushButton*>(itemWidget(channel, 2));
  button->setChecked(broadcast);
  localChannelHash.insert(button, ch);
  connect(button, SIGNAL(toggled(bool)),
          this, SLOT(mapLocalChannelBroadcastChanged(bool)));

  button = static_cast<QPushButton*>(itemWidget(channel, 3));
  localChannelHash.insert(button, ch);
  connect(button, SIGNAL(toggled(bool)),
          this, SLOT(mapLocalChannelBoostChanged(bool)));
}

void ChannelTreeWidget::mapLocalChannelMuteChanged(bool mute)
{
  QWidget *button = static_cast<QWidget*>(sender());
  int ch = localChannelHash.value(button);

  emit LocalChannelMuteChanged(ch, mute);
}

void ChannelTreeWidget::mapLocalChannelBoostChanged(bool boost)
{
  QWidget *button = static_cast<QWidget*>(sender());
  int ch = localChannelHash.value(button);

  emit LocalChannelBoostChanged(ch, boost);
}

void ChannelTreeWidget::mapLocalChannelBroadcastChanged(bool broadcast)
{
  QWidget *button = static_cast<QWidget*>(sender());
  int ch = localChannelHash.value(button);

  emit LocalChannelBroadcastChanged(ch, broadcast);
}
