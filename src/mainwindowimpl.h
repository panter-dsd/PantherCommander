#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
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
class QComboBox;
class QAction;
class QFrame;
class QMenuBar;
class QStandardItemModel;
//
#include <QMainWindow>
#include <QSplitter>
#include <QToolBar>
#include <QPointer>
#include "qfilepanel.h"
#include "./PantherViewer/pantherviewer.h"
#include "qfileoperationsdialog.h"
#include "Preferences/qtoolbuttonpreference.h"
//
class MainWindowImpl : public QMainWindow
{
Q_OBJECT
private:
	QWidget*										qwCentralWidget;
	QFilePanel*									qflvLeftPanel;
	QFilePanel*									qflvRightPanel;
	QFilePanel*									qfpFocusedFilePanel;
	QSplitter*									qsplitSplitter;
	QMenuBar*									qmbMainMenu;
	QLabel*										qlConsolePath;
	QComboBox*								qcbConsoleCommand;
	QFrame*										qfCommandButtons;
	QPointer<PantherViewer>			pvViewer;
	QStandardItemModel*				qsimQeueuModel;
	QList<QFileOperationsDialog*> qlQueueList;
	QMap<QString,SToolBarButton>		qmToolBurButtons;
	QList<QToolBar*>						qltbToolBarList;

	QPushButton*								qpbRunConsole;
	QPushButton*								qpbView;
	QPushButton*								qpbEdit;
	QPushButton*								qpbCopy;
	QPushButton*								qpbMove;
	QPushButton*								qpbMkDir;
	QPushButton*								qpbRemove;
	QPushButton*								qpbExit;

	QAction*										actionCpCurFileName2Cmd;
	QAction*										actionCpCurFileNameWhithPath2Cmd;
	QAction*										actionClearCmd;

	QAction*										actionRunConsole;
	QAction*										actionView;
	QAction*										actionEdit;
	QAction*										actionCopy;
	QAction*										actionMove;
	QAction*										actionMkDir;
	QAction*										actionRemove;
	QAction*										actionExit;
	QAction*										actionRename;


	bool												isLeftFocused;
public:
	MainWindowImpl(QWidget* parent=0,Qt::WFlags f=0);
	~MainWindowImpl();
private:
	void createControls();
	void setLayouts();
	void setConnects();
	void loadSettings();
	void saveSettings();
	void createSplitterMenu();
	void createActions();
	void createMenu();
	void createCommandButtons();
	void loadToolBar(const QString& toolBarName);
	void saveToolBars();
	QFileOperationsDialog* addJob(QFileOperationsDialog* queue,QFileOperationsThread::FileOperation operation,QStringList parameters);
protected:
	void paintEvent ( QPaintEvent * event );
	void dropEvent (QDropEvent* event);
	void dragMoveEvent ( QDragMoveEvent * event );
	void dragEnterEvent(QDragEnterEvent* event);
private slots:
	void slotResizeSplitter();
	void slotChangedFocus();
	void slotRunCommand();
	void slotView(const QString& fileName="");
	void slotCopy(const QString& destDir="",const QStringList& fileList=QStringList());
	void slotMove(const QString& destDir="",const QStringList& fileList=QStringList());
	void slotRemove(const QStringList& fileList=QStringList());
	void slotMkDir();
	void slotChangedPath(const QString& path);
	void slotCpCurFileName2Cmd();
	void slotCpCurFileNameWhithPath2Cmd();

	void slotRunConsole();
	void slotRename();
	void slotQueueComplite();
	void slotQueueChanged();
	void slotToolButtonPress();
	void slotToolBarContextMenu(const QPoint& pos);
	void slotToolButtonChange();
	void slotToolButtonDelete();
};
#endif




