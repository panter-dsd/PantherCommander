#ifndef QFILEPANEL_H
#define QFILEPANEL_H
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
class QLabel;
class QLineEdit;
class QActionGroup;
class QTabWidget;
class QAction;
class QPushButton;
class QToolBar;
class QMenu;
class QFileSystemWatcher;
class QFileIconProvider;
//
#include <QWidget>
#include "qfilelistview.h"
#include "qfileoperationsthread.h"
//
class QFilePanel : public QWidget
{
Q_OBJECT
private:
	QToolBar*							qtbDrives;
	QActionGroup*					qagDrives;
	QLineEdit*							qlePath;
	QTabWidget*						qtabwTabs;
	QList<QFileListView*>		qlflwFileLists;
	QFileListView*					qflvCurrentFileList;
	QFileSystemWatcher*			qfswFileSystemWatcher;
	QPushButton*						qpbGotoRoot;
	QPushButton*						qpbGotoUp;
	QPushButton*						qpbGotoHome;
	QLabel*								qlDirInformation;
	QLabel*								qlDiscInformation;

	QAction*							actionAddTab;
	QAction*							actionAddDriveToIgnorList;

	QMenu*							qmToolBarMenu;

	QFileIconProvider			qfipIconProvider;
	QMap<QString,QIcon>	qmDrivesIcons;

	QString								panelName;
	int									timerID;
public:
	QFilePanel(const QString& name,QWidget* parent=0);
	~QFilePanel();
	QString filePanelName() {return panelName;}
	void setDisc(const QString& name);
	QString path();
	void setPath(const QString& path);
	QString currentFileName();
	QStringList selectedFileNames();
	void setFocus();
	void clearSelection() {qflvCurrentFileList->clearSelection();}
protected:
	void timerEvent(QTimerEvent *event);
	void showEvent(QShowEvent * event);
private:
	void createControls();
	void setLayouts();
	void setConnects();
	void createActions();
	void loadSettings();
	void saveSettings();
	void addTab(const QString& path);
	void createDrivesToolbar();
	void selectCurrentDrive();
	void updateDirInformation();
	void updateDiscInformation();
	QString getSizeStr(double size) const;
private slots:
	void slotChangedPath(const QString& path);
	void slotCloseTab(int);
	void slotAddTab() {addTab(""); saveSettings();}
	void slotGotoRoot();
	void slotGotoUp();
	void slotGotoHome();
	void slotCurrentTabChange(int);
	void slotFileListHeaderSectionResize(int index, int oldSize, int newSize);
	void slotSetDisc();
	void slotDriveToolbarMenu(const QPoint & pos);
	void slotAddDriveToIgnorList();
	void slotInformationChanged();
signals:
	void headerSectionResized ( int logicalIndex, int oldSize, int newSize );
	void focusIn();
	void focusOut();
	void changedPath(const QString&);
	void changedIgnoringDrive();
public slots:
	void slotRereadDriveToolBar() {createDrivesToolbar();}
	void slotRefresh();
};

#endif // QFILEPANEL_H
