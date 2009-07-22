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

#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
//
class QLabel;
class QComboBox;
class QAction;
class QFrame;
class QStandardItemModel;
class QPushButton;
class QSplitter;
class QToolBar;
//
class DriveBar;
class QFilePanel;
class QFileOperationsDialog;
class QFileOperationsDialog;
class PantherViewer;
//
#include "qfileoperationsthread.h"
#include "pctoolbar.h"
//
class MainWindowImpl : public QMainWindow
{
	Q_OBJECT
	Q_PROPERTY(QStringList commandHistory READ commandHistory WRITE setCommandHistory)

private:
	DriveBar*									qdbDriveBarLeft;
	DriveBar*									qdbDriveBarRight;
	QFilePanel*									qflvLeftPanel;
	QFilePanel*									qflvRightPanel;
	QFilePanel*									qfpFocusedFilePanel;
	QSplitter*									qsplitSplitter;
	QLabel*										qlConsolePath;
	QComboBox*								qcbConsoleCommand;
	QFrame*										qfCommandButtons;
	QPointer<PantherViewer>			pvViewer;
	QStandardItemModel*				qsimQeueuModel;
	QList<QFileOperationsDialog*> qlQueueList;
	QList<PCToolBar*> qlpcToolBars;

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
	QAction*										actionRename;
	QAction*										actionMkDir;
	QAction*										actionRemove;
	QAction*										actionExit;
	QAction*										actionFindFiles;
	QAction*										actionPreferences;
public:
	explicit MainWindowImpl(QWidget* parent = 0, Qt::WFlags f = 0);
	virtual ~MainWindowImpl();

	QStringList commandHistory() const;
	void setCommandHistory(const QStringList& commandHistory);

private:
	void createWidgets();
	void createActions();
	void createMenus();
	void createCommandButtons();
	void saveSettings();
	void loadSettings();
	inline void connectToolBar(PCToolBar *toolBar);

protected:
	void resizeEvent(QResizeEvent* event);
	void dropEvent(QDropEvent* event);
	void dragMoveEvent(QDragMoveEvent* event);
	void dragEnterEvent(QDragEnterEvent* event);

	QFileOperationsDialog* addJob(QFileOperationsDialog* queue, QFileOperationsThread::FileOperation operation, const QStringList& parameters);

private Q_SLOTS:
	void showSplitterContextMenu(const QPoint& pos);
	void slotResizeSplitter();
	void slotChangedFocus();
	void slotRunCommand();
	void slotSetDisc(const QString& path);
	void slotView(const QString& fileName = QString());
	void slotCopy(const QString& destDir = QString(), const QStringList& fileList = QStringList());
	void slotMove(const QString& destDir = QString(), const QStringList& fileList = QStringList());
	void slotRemove(const QStringList& fileList = QStringList());
	void slotMkDir();
	void slotPathChanged(const QString& path);
	void slotCpCurFileName2Cmd();
	void slotCpCurFileNameWhithPath2Cmd();

	void slotRunConsole();
	void slotRename();
	void slotQueueFinished();
	void slotQueueChanged();
	void slotAddToolBar();
	void slotRemoveToolBar();
	void slotRenameToolBar();
	void slotFindFiles();
	void slotPreferences();
	void toolBarActionExecute(const SToolBarButton& action);
	void cdExecute(const QString& path);

	/* ** TESTING PURPOSES ONLY ** */
	void slotTestingFileDialog();
#ifdef Q_WS_WIN
	void slotTestingEnableNTFSPermissionLookup(bool enable);
#endif
};

#endif // MAINWINDOWIMPL_H
