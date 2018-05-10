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

#ifndef DRIVEBAR_H
#define DRIVEBAR_H

class QAction;
class QActionGroup;
class VolumeInfoProvider;
class QFileIconProvider;

#include <QtWidgets/QFrame>

class DriveBar : public QFrame
{
	Q_OBJECT

private:
	QAction *lastChecked;
	QActionGroup *actionGroup;
	VolumeInfoProvider *provider;
	QFileIconProvider *iconProvider;

public:
	explicit DriveBar(QWidget* parent = 0);
	virtual ~DriveBar();

private:
	void loadDrivesList();

public Q_SLOTS:
	void slotSetDisc(const QString& path);

Q_SIGNALS:
	void discChanged(const QString& disc);

private Q_SLOTS:
	void _q_actionTriggered(QAction* action);
	void _q_showContextMenu(const QPoint& position);
	void volumeAdd(const QString&);
	void volumeRemove(const QString&);
};

#endif // DRIVEBAR_H
