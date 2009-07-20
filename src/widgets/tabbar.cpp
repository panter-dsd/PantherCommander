/********************************************************************
* Copyright (C) PanteR
*-------------------------------------------------------------------
*
* Panther Commander is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* Panther Commander is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Panther Commander; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301 USA
*-------------------------------------------------------------------
* Project:		Panther Commander
* Author:		Ritt K.
* Contact:		ritt.ks@gmail.com
*******************************************************************/

#include "tabbar.h"

#include <QtCore/QEvent>

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>

TabBar::TabBar(QWidget* parent) : QTabBar(parent),
	m_showTabBarWhenOneTab(true)
{
	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(contextMenuRequested(const QPoint&)));

#if QT_VERSION >= 0x040500
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
#endif
}

TabBar::~TabBar()
{
}

bool TabBar::showTabBarWhenOneTab() const
{
	return m_showTabBarWhenOneTab;
}

void TabBar::setShowTabBarWhenOneTab(bool enabled)
{
	m_showTabBarWhenOneTab = enabled;
	updateVisibility();
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	if(!childAt(event->pos())
		// Remove the line below when QTabWidget does not have a one pixel frame
		&& event->pos().y() < y() + height())
	{
		emit newTab();
		return;
	}

	QTabBar::mouseDoubleClickEvent(event);
}

void TabBar::mouseReleaseEvent(QMouseEvent* event)
{
	if(event->button() == Qt::MidButton)
	{
		int index = tabAt(event->pos());
		if(index != -1)
		{
			removeTab(index);
			return;
		}
	}

	QTabBar::mouseReleaseEvent(event);
}

QSize TabBar::tabSizeHint(int index) const
{
	QSize sizeHint = QTabBar::tabSizeHint(index);
	QFontMetrics fm = fontMetrics();
	return sizeHint.boundedTo(QSize(fm.width(QLatin1Char('M')) * 18, sizeHint.height()));
}

void TabBar::tabInserted(int position)
{
	Q_UNUSED(position);
	updateVisibility();
}

void TabBar::tabRemoved(int position)
{
	Q_UNUSED(position);
	updateVisibility();
}

void TabBar::cloneTab()
{
	if(QAction* action = qobject_cast<QAction*>(sender()))
	{
		int index = action->data().toInt();
		if(index != -1)
			emit cloneTab(index);
	}
}

void TabBar::closeTab(int index)
{
	if(index == -1)
	{
		if(QAction* action = qobject_cast<QAction*>(sender()))
			index = action->data().toInt();
	}
	if(index != -1 && count() > 1)
		removeTab(index);
}

void TabBar::closeOtherTabs()
{
	if(QAction* action = qobject_cast<QAction*>(sender()))
	{
		int index = action->data().toInt();
		if(index != -1)
		{
			for(int i = count() - 1; i > index; --i)
				removeTab(i);
			for(int i = index - 1; i >= 0; --i)
				removeTab(i);
		}
	}
}

void TabBar::contextMenuRequested(const QPoint& pos)
{
	QMenu menu;
	QAction* action = menu.addAction(tr("Add Tab"), this, SIGNAL(newTab()));

	int index = tabAt(pos);
	if(index != -1)
	{
		action = menu.addAction(tr("Duplicate Tab"), this, SLOT(cloneTab()));
		action->setData(index);

		menu.addSeparator();

		action = menu.addAction(tr("&Close Tab"), this, SLOT(closeTab()), QKeySequence::Close);
		action->setData(index);

		action = menu.addAction(tr("Close &Other Tabs"), this, SLOT(closeOtherTabs()));
		action->setData(index);
	}
	menu.exec(mapToGlobal(pos));
}

void TabBar::updateVisibility()
{
	setVisible(count() > 1 || m_showTabBarWhenOneTab);
}
