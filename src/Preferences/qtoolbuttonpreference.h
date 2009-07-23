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

#ifndef QTOOLBUTTONPREFERENCE_H
#define QTOOLBUTTONPREFERENCE_H

class QLabel;
class QLineEdit;
class QListWidget;
class QToolButton;
class QAction;

#include <QtGui/QWidget>
#include <QtGui/QIcon>

struct SToolBarButton
{
	QString qsCommand;
	QString qsParams;
	QString qsWorkDir;
	QString qsIconFile;
	int iconNumber;
	QIcon qiIcon;
	QString qsCaption;
};

class QToolButtonPreference : public QWidget
{
	Q_OBJECT
private:
	QLabel *qlCommand;
	QLineEdit *qleCommand;
	QToolButton *qtbCommand;

	QLabel *qlParams;
	QLineEdit *qleParams;

	QLabel *qlWorkDir;
	QLineEdit *qleWorkDir;
	QToolButton *qtbWorkDir;

	QLabel *qlIconFile;
	QLineEdit *qleIconFile;
	QToolButton *qtbIconFile;

	QLabel *qlIcon;
	QListWidget *qlwIcons;

	QLabel *qlCaption;
	QLineEdit *qleCaption;

	SToolBarButton stbbButton;

public:
	QToolButtonPreference(QWidget* parent=0);
	void setButton(const SToolBarButton& button);
	SToolBarButton getButton();
	static SToolBarButton getButton(const QString& command);

private:
	void createControls();
	void setLayouts();
	void setConnects();

private Q_SLOTS:
	void slotChooseCommandFile();
	void slotChooseWorkDir();
	void slotChooseIconFile();
	void slotGetIconList(const QString& iconFileName);
};

#endif // QTOOLBUTTONPREFERENCE_H
