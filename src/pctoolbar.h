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
* Author:		PanteR
* Contact:	panter.dsd@gmail.com
*******************************************************************/
#ifndef PCTOOLBAR_H
#define PCTOOLBAR_H

#include <QtWidgets/QToolBar>
#include "qtoolbuttonpreference.h"

class PCToolBar : public QToolBar
{
	Q_OBJECT
private:
	QList<SToolBarButton> qlButtons;
	QString qsName;
public:
	PCToolBar(const QString &name, QWidget *parent = 0);
	virtual ~PCToolBar() {}
	QString name() {return qsName;}
	void restore();
	void save();
	void rename(const QString &name);

private:
	void refreshActions();

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void dropEvent(QDropEvent* event);
	void dragMoveEvent(QDragMoveEvent* event);
	void dragEnterEvent(QDragEnterEvent* event);

private Q_SLOTS:
	void slotToolButtonPress();
	void slotToolButtonChange();
	void slotToolButtonDelete();
	void slotToolButtonCD();

public Q_SLOTS:
	void slotAddSeparator();

Q_SIGNALS:
	void toolBarActionExecuted(const SToolBarButton& action);
	void cdExecuted(const QString& path);
	void toolbarContextMenu(const QPoint& pos);
};

#endif // PCTOOLBAR_H
