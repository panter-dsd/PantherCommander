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
#include <QApplication>
#include <QTextCodec>
#include <QSettings>
#include "mainwindowimpl.h"
//
#define ApplicationVersion "0.0.0.4"
//
int main(int argc, char ** argv)
{
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("System"));
	QApplication app( argc, argv );
	app.setOrganizationDomain("panter.org");
	app.setOrganizationName("PanteR");
	app.setApplicationName("Panther Commander");
	app.setApplicationVersion(ApplicationVersion);
	QSettings::setDefaultFormat(QSettings::IniFormat);

	MainWindowImpl win;
	win.setWindowTitle(app.applicationName()+" "+
									app.applicationVersion());
	win.show();
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
