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

#ifndef QINTERFACEPREFERENCE_H
#define QINTERFACEPREFERENCE_H
//
#include "qabstractpreferencespage.h"
//
class QCheckBox;
class QGroupBox;
//
class QInterfacePreference : public QAbstractPreferencesPage
{
	Q_OBJECT

private:
	QGroupBox							*qgbGlobal;
	QCheckBox							*qchbShowDriveBar;
	QCheckBox							*qchbShowTwoDriveBar;
	QCheckBox							*qchbShowDriveButton;
	QCheckBox							*qchbShowDriveComboBox;
	QCheckBox							*qchbShowTabs;
	QCheckBox							*qchbShowHeader;
	QCheckBox							*qchbShowDirInformation;
	QCheckBox							*qchbShowCommandLine;
	QCheckBox							*qchbShowFunctionButtons;

	QGroupBox							*qgbCurrentPathEditor;
	QCheckBox							*qchbShowHistory;
	QCheckBox							*qchbShowGotoRoot;
	QCheckBox							*qchbShowGotoUp;
	QCheckBox							*qchbShowGotoHome;

	QGroupBox							*qgbFlatInterface;
	QCheckBox							*qchbFlatToolBar;
	QCheckBox							*qchbFlatDriveBar;
	QCheckBox							*qchbFlatDriveButtons;
	QCheckBox							*qchbFlatFunctionButtons;
public:
	QInterfacePreference(QWidget *parent = 0);
	~QInterfacePreference();

	void saveSettings();
	void loadSettings();
	void setDefaults();

	static QString preferenceGroup() { return tr("Interface"); }

private:
	void createControls();
	void setLayouts();
	void setConnects();
};

#endif // QINTERFACEPREFERENCE_H
