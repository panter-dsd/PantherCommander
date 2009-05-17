#ifndef PANTHERVIEWER_H
#define PANTHERVIEWER_H
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
//
class QTabWidget;
class QMenuBar;
class QToolBar;
class QAction;
class QActionGroup;
//
#include <QMainWindow>
#include "plainview.h"
#include "abstractview.h"
#include "appsettings.h"
//
class PantherViewer : public QMainWindow
{
Q_OBJECT
private:
	QStringList					qslFiles;
	QTabWidget*				qtabwTabs;
	QMenuBar*					qmbMainMenu;
	QToolBar*					qtbarMainToolBar;
	QActionGroup*			qagCodecs;

	QAction*						actionExit;
	QAction*						actionCloseCurrentTab;
	AppSettings					*appSettings;
public:
	PantherViewer(QWidget* parent=0,Qt::WFlags f=0);
	~PantherViewer();
	void viewFile(const QString& fileName);
private:
	void createControls();
	void setLayouts();
	void setConnects();
	void createActions();
	void createMenu();
	void createToolBar();
	void loadSettings();
	void saveSettings();
	QStringList codecsList();
private slots:
	void slotSetEncoding();
	void slotCurrentTabChanged(int index);
	void slotCloseCurrentTab();
};

#endif // PANTHERVIEWER_H
