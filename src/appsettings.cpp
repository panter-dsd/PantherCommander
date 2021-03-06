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

#include "appsettings.h"

#include <QtCore/QCoreApplication>

AppSettings* AppSettings::pInstance = 0;

AppSettings* AppSettings::instance()
{
	if(!AppSettings::pInstance)
		pInstance = new AppSettings(qApp);
	return pInstance;
}

AppSettings::AppSettings(QObject* parent) : QSettings(parent)
{
}

AppSettings::~AppSettings()
{
	if(pInstance == this)
		pInstance = 0;
}

void AppSettings::settingsChange()
{
}

bool AppSettings::useNativeDialogs()
{
	return value("Global/UseNativeDialogs", true).toBool();
}
