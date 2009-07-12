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

#ifndef QDRIVEBAR_H
#define QDRIVEBAR_H

#include <QtGui/QFrame>

class QAction;
class QActionGroup;

class QDriveBar : public QFrame
{
	Q_OBJECT

private:
	QAction*					qaLastChecked;
	QActionGroup*		qagDrives;
	QString					qsCurrentPath;

public:
	explicit QDriveBar(QWidget* parent = 0);
	virtual ~QDriveBar();

private slots:
	void _q_actionTriggered(QAction* action);

public slots:
	void slotRefresh();
	void slotSetDisc(const QString& path);

signals:
	void discChanged(const QString& disc);

private:
	bool m_inRefresh;
};

#endif // QDRIVEBAR_H
