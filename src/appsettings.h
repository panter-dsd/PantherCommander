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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QtCore/QSettings>

class AppSettings : public QSettings
{
	Q_OBJECT

public:
	static AppSettings* instance();
	virtual ~AppSettings();

	void settingsChange();

protected:
	explicit AppSettings(QObject* parent = 0);

Q_SIGNALS:
	void settingsChanged();

private:
	static AppSettings* pInstance;
};

#endif // APPSETTINGS_H
