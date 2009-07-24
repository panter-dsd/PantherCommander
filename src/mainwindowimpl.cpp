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

#include "mainwindowimpl.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtGui/QAction>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStandardItemModel>
#include <QtGui/QToolBar>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include <QtGui/QStringListModel>
#include <QtGui/QListView>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDropEvent>
#include <QtGui/QToolButton>


#include "appsettings.h"
#include "findfilesdialog.h"
#include "pantherviewer.h"
#include "drivebar.h"
#include "qfilepanel.h"
#include "qfileoperationsdialog.h"
#include "qpreferencesdialog.h"
#include "pccopymovedialog.h"
#include "pctoolbar.h"
#include "pccommands.h"

MainWindowImpl::MainWindowImpl(QWidget* parent, Qt::WFlags f) : QMainWindow(parent, f)
	, qlConsolePath(0)
{
	resize(640, 480);
	setAcceptDrops(true);

//	QTime time;
//	time.start();
	createWidgets();
//	qDebug(QString::number(time.elapsed()).toLocal8Bit());
	createActions();
	createMenus();
	createCommandButtons();

	loadSettings();
	qflvLeftPanel->loadSettings();
	qflvRightPanel->loadSettings();

	qflvLeftPanel->setFocus();
	qfpFocusedFilePanel=qflvRightPanel;
}
//
MainWindowImpl::~MainWindowImpl()
{
	qflvLeftPanel->saveSettings();
	qflvRightPanel->saveSettings();
	saveSettings();
}
//
void MainWindowImpl::createWidgets()
{
	QWidget* widget = new QWidget(this);

	qdbDriveBarLeft = new DriveBar(this);
	connect(qdbDriveBarLeft, SIGNAL(discChanged(const QString&)),
			this, SLOT(slotSetDisc(const QString&)));

	qdbDriveBarRight = new DriveBar(this);
	connect(qdbDriveBarRight, SIGNAL(discChanged(const QString&)),
			this, SLOT(slotSetDisc(const QString&)));

	qflvLeftPanel = new QFilePanel(this);
	qflvLeftPanel->setObjectName("LeftPanel");
	connect(qflvLeftPanel, SIGNAL(pathChanged(const QString&)),
			qdbDriveBarLeft, SLOT(slotSetDisc(const QString&)));
	connect(qflvLeftPanel, SIGNAL(pathChanged(const QString&)),
			this, SLOT(slotPathChanged(const QString&)));
	qdbDriveBarLeft->slotSetDisc(qflvLeftPanel->path());

	qflvRightPanel = new QFilePanel(this);
	qflvRightPanel->setObjectName("RightPanel");
	connect(qflvRightPanel, SIGNAL(pathChanged(const QString&)),
			qdbDriveBarRight, SLOT(slotSetDisc(const QString&)));
	connect(qflvRightPanel, SIGNAL(pathChanged(const QString&)),
			this, SLOT(slotPathChanged(const QString&)));
	qdbDriveBarRight->slotSetDisc(qflvRightPanel->path());

	qsplitSplitter = new QSplitter(widget);
	qsplitSplitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	qsplitSplitter->setContextMenuPolicy(Qt::CustomContextMenu);
	qsplitSplitter->addWidget(qflvLeftPanel);
	qsplitSplitter->addWidget(qflvRightPanel);

	QObject::connect(qsplitSplitter, SIGNAL(customContextMenuRequested(const QPoint&)),
						this, SLOT(showSplitterContextMenu(const QPoint&)));

	qlConsolePath = new QLabel(this);
	qlConsolePath->setAlignment(Qt::AlignRight);

	qcbConsoleCommand = new QComboBox(this);
	qcbConsoleCommand->setEditable(true);
	qcbConsoleCommand->setInsertPolicy(QComboBox::InsertAtTop);
	qcbConsoleCommand->setFocusPolicy(Qt::ClickFocus);
	qcbConsoleCommand->setDuplicatesEnabled(false);

	connect(qcbConsoleCommand->lineEdit(), SIGNAL(returnPressed()),
			this, SLOT(slotRunCommand()));

	qfCommandButtons = new QFrame(this);
	qfCommandButtons->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	qsimQeueuModel = new QStandardItemModel(this);

	QHBoxLayout* qhblDriveBarLayout = new QHBoxLayout;
	qhblDriveBarLayout->addWidget(qdbDriveBarLeft);
	qhblDriveBarLayout->addWidget(qdbDriveBarRight);

	QHBoxLayout* qhblConsoleCommandLayout = new QHBoxLayout;
	qhblConsoleCommandLayout->addWidget(qlConsolePath);
	qhblConsoleCommandLayout->addWidget(qcbConsoleCommand);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(-1, -1, -1, 0);
	layout->addLayout(qhblDriveBarLayout);
	layout->addWidget(qsplitSplitter);
	layout->addLayout(qhblConsoleCommandLayout);
	layout->addWidget(qfCommandButtons);
	widget->setLayout(layout);

	setCentralWidget(widget);
}
//
void MainWindowImpl::createActions()
{
	actionCpCurFileName2Cmd = new QAction(this);
	actionCpCurFileName2Cmd->setObjectName("actionCpCurFileName2Cmd");
	actionCpCurFileName2Cmd->setText(tr("Copy current file name to command string"));
	actionCpCurFileName2Cmd->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return));
	connect(actionCpCurFileName2Cmd, SIGNAL(triggered(bool)),
			this, SLOT(slotCpCurFileName2Cmd()));
	addAction(actionCpCurFileName2Cmd);
	PCCommands::instance()->addAction(tr("Command line"), actionCpCurFileName2Cmd);

	actionCpCurFileNameWhithPath2Cmd = new QAction(this);
	actionCpCurFileNameWhithPath2Cmd->setObjectName("actionCpCurFileNameWhithPath2Cmd");
	actionCpCurFileNameWhithPath2Cmd->setText(tr("Copy current file name whith path to command string"));
	actionCpCurFileNameWhithPath2Cmd->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Return));
	connect(actionCpCurFileNameWhithPath2Cmd, SIGNAL(triggered(bool)),
			this, SLOT(slotCpCurFileNameWhithPath2Cmd()));
	addAction(actionCpCurFileNameWhithPath2Cmd);
	PCCommands::instance()->addAction(tr("Command line"), actionCpCurFileNameWhithPath2Cmd);

	actionClearCmd = new QAction(this);
	actionClearCmd->setObjectName("actionClearCmd");
	actionClearCmd->setText(tr("Clear command string"));
	actionClearCmd->setShortcut(QKeySequence(Qt::Key_Escape));
	connect(actionClearCmd, SIGNAL(triggered(bool)),
			qcbConsoleCommand->lineEdit(), SLOT(clear()));
	addAction(actionClearCmd);
	PCCommands::instance()->addAction(tr("Command line"), actionClearCmd);


	actionRunConsole = new QAction(this);
	actionRunConsole->setObjectName("actionRunConsole");
	actionRunConsole->setText(tr("Run console"));
	actionRunConsole->setShortcut(QKeySequence(Qt::Key_F2));
	connect(actionRunConsole, SIGNAL(triggered(bool)),
			this, SLOT(slotRunConsole()));
	addAction(actionRunConsole);
	PCCommands::instance()->addAction(tr("Misc"), actionRunConsole);

	actionView = new QAction(this);
	actionView->setObjectName("actionView");
	actionView->setText(tr("View"));
	actionView->setShortcut(QKeySequence(Qt::Key_F3));
	connect(actionView, SIGNAL(triggered(bool)),
			this, SLOT(slotView()));
	addAction(actionView);
	PCCommands::instance()->addAction(tr("File operations"), actionView);

	actionEdit = new QAction(this);
	actionEdit->setObjectName("actionEdit");
	actionEdit->setText(tr("Edit"));
	actionEdit->setShortcut(QKeySequence(Qt::Key_F4));
//	connect(actionEdit, SIGNAL(triggered(bool)),
//			this, SLOT(slotView()));
	addAction(actionEdit);
	PCCommands::instance()->addAction(tr("File operations"), actionEdit);

	actionCopy = new QAction(this);
	actionCopy->setObjectName("actionCopy");
	actionCopy->setText(tr("Copy"));
	actionCopy->setShortcut(QKeySequence(Qt::Key_F5));
	connect(actionCopy, SIGNAL(triggered(bool)),
			this, SLOT(slotCopy()));
	addAction(actionCopy);
	PCCommands::instance()->addAction(tr("File operations"), actionCopy);

	actionMove = new QAction(this);
	actionMove->setObjectName("actionMove");
	actionMove->setText(tr("Move"));
	actionMove->setShortcut(QKeySequence(Qt::Key_F6));
	connect(actionMove, SIGNAL(triggered(bool)),
			this, SLOT(slotMove()));
	addAction(actionMove);
	PCCommands::instance()->addAction(tr("File operations"), actionMove);

	actionRename = new QAction(this);
	actionRename->setObjectName("actionRename");
	actionRename->setText(tr("Rename"));
	actionRename->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F6));
	connect(actionRename, SIGNAL(triggered(bool)),
			this, SLOT(slotRename()));
	addAction(actionRename);
	PCCommands::instance()->addAction(tr("File operations"), actionRename);

	actionMkDir = new QAction(this);
	actionMkDir->setObjectName("actionMkDir");
	actionMkDir->setText(tr("Create Dir"));
	actionMkDir->setShortcut(QKeySequence(Qt::Key_F7));
	connect(actionMkDir, SIGNAL(triggered(bool)),
			this, SLOT(slotMkDir()));
	addAction(actionMkDir);
	PCCommands::instance()->addAction(tr("File operations"), actionMkDir);

	actionRemove = new QAction(this);
	actionRemove->setObjectName("actionRemove");
	actionRemove->setText(tr("Remove"));
	actionRemove->setShortcut(QKeySequence(Qt::Key_F8));
	connect(actionRemove, SIGNAL(triggered(bool)),
			this, SLOT(slotRemove()));
	addAction(actionRemove);
	PCCommands::instance()->addAction(tr("File operations"), actionRemove);

	actionExit = new QAction(this);
	actionExit->setObjectName("actionExit");
	actionExit->setText(tr("Exit"));
	actionExit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_X));
	connect(actionExit, SIGNAL(triggered(bool)),
			qApp, SLOT(quit()));
	addAction(actionExit);
	PCCommands::instance()->addAction(tr("Window"), actionExit);


	actionFindFiles = new QAction(this);
	actionFindFiles->setObjectName("actionFindFiles");
	actionFindFiles->setText(tr("Find Files"));
//	actionFindFiles->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
	connect(actionFindFiles, SIGNAL(triggered(bool)),
			this, SLOT(slotFindFiles()));
	addAction(actionFindFiles);
	PCCommands::instance()->addAction(tr("Tools"), actionFindFiles);

	actionPreferences = new QAction(this);
	actionPreferences->setObjectName("actionPreferences");
	actionPreferences->setText(tr("Preferences"));
//	actionPreferences->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
	connect(actionPreferences, SIGNAL(triggered(bool)),
			this, SLOT(slotPreferences()));
	addAction(actionPreferences);
	PCCommands::instance()->addAction(tr("Preferences"), actionFindFiles);
}
//
void MainWindowImpl::createMenus()
{
	QMenu* qmFile = menuBar()->addMenu(tr("File"));
	qmFile->addAction(actionRename);
	qmFile->addAction(actionExit);

	QMenu* qmActions = menuBar()->addMenu(tr("Actions"));
	qmActions->addAction(actionFindFiles);

	QMenu* qmConfiguration = menuBar()->addMenu(tr("Configuration"));
	qmConfiguration->addAction(actionPreferences);

	QMenu* qmTesting = menuBar()->addMenu("testing");
	qmTesting->addAction("filedialog", this, SLOT(slotTestingFileDialog()));
#ifdef Q_WS_WIN
	QAction* action = qmTesting->addAction("ntfs_permission_lookup", this, SLOT(slotTestingEnableNTFSPermissionLookup(bool)));
	action->setCheckable(true);
#endif
}
//
void MainWindowImpl::createCommandButtons()
{
	QFrame* splitter;
	QHBoxLayout* qhblLayout=new QHBoxLayout();
	qhblLayout->setSpacing(0);
	qhblLayout->setMargin(0);

	qpbRunConsole=new QPushButton(actionRunConsole->shortcut().toString()+" "+actionRunConsole->text(),
						qfCommandButtons);
	qpbRunConsole->setFlat(true);
	qpbRunConsole->setFocusPolicy(Qt::NoFocus);
	qpbRunConsole->addAction(actionRunConsole);
	connect(qpbRunConsole,SIGNAL(clicked()),actionRunConsole,SIGNAL(triggered()));
	qhblLayout->addWidget(qpbRunConsole);
	splitter=new QFrame(qfCommandButtons);
	splitter->setFrameShape(QFrame::VLine);
	splitter->setFrameShadow(QFrame::Sunken);
	qhblLayout->addWidget(splitter);

	qpbView=new QPushButton(actionView->shortcut().toString()+" "+actionView->text(),
						qfCommandButtons);
	qpbView->setFlat(true);
	qpbView->setFocusPolicy(Qt::NoFocus);
	qpbView->addAction(actionView);
	connect(qpbView,SIGNAL(clicked()),actionView,SIGNAL(triggered()));
	qhblLayout->addWidget(qpbView);
	splitter=new QFrame(qfCommandButtons);
	splitter->setFrameShape(QFrame::VLine);
	splitter->setFrameShadow(QFrame::Sunken);
	qhblLayout->addWidget(splitter);

	qpbEdit=new QPushButton(actionEdit->shortcut().toString()+" "+actionEdit->text(),
						qfCommandButtons);
	qpbEdit->setFlat(true);
	qpbEdit->setFocusPolicy(Qt::NoFocus);
	qpbEdit->addAction(actionEdit);
	connect(qpbEdit,SIGNAL(clicked()),actionEdit,SIGNAL(triggered()));
	qhblLayout->addWidget(qpbEdit);
	splitter=new QFrame(qfCommandButtons);
	splitter->setFrameShape(QFrame::VLine);
	splitter->setFrameShadow(QFrame::Sunken);
	qhblLayout->addWidget(splitter);

	qpbCopy=new QPushButton(actionCopy->shortcut().toString()+" "+actionCopy->text(),
						qfCommandButtons);
	qpbCopy->setFlat(true);
	qpbCopy->setFocusPolicy(Qt::NoFocus);
	qpbCopy->addAction(actionCopy);
	connect(qpbCopy,SIGNAL(clicked()),actionCopy,SIGNAL(triggered()));
	qhblLayout->addWidget(qpbCopy);
	splitter=new QFrame(qfCommandButtons);
	splitter->setFrameShape(QFrame::VLine);
	splitter->setFrameShadow(QFrame::Sunken);
	qhblLayout->addWidget(splitter);

	qpbMove=new QPushButton(actionMove->shortcut().toString()+" "+actionMove->text(),
						qfCommandButtons);
	qpbMove->setFlat(true);
	qpbMove->setFocusPolicy(Qt::NoFocus);
	qpbMove->addAction(actionMove);
	connect(qpbMove,SIGNAL(clicked()),actionMove,SIGNAL(triggered()));
	qhblLayout->addWidget(qpbMove);
	splitter=new QFrame(qfCommandButtons);
	splitter->setFrameShape(QFrame::VLine);
	splitter->setFrameShadow(QFrame::Sunken);
	qhblLayout->addWidget(splitter);

	qpbMkDir=new QPushButton(actionMkDir->shortcut().toString()+" "+actionMkDir->text(),
						qfCommandButtons);
	qpbMkDir->setFlat(true);
	qpbMkDir->setFocusPolicy(Qt::NoFocus);
	qpbMkDir->addAction(actionMkDir);
	connect(qpbMkDir,SIGNAL(clicked()),actionMkDir,SIGNAL(triggered()));
	qhblLayout->addWidget(qpbMkDir);
	splitter=new QFrame(qfCommandButtons);
	splitter->setFrameShape(QFrame::VLine);
	splitter->setFrameShadow(QFrame::Sunken);
	qhblLayout->addWidget(splitter);

	qpbRemove=new QPushButton(actionRemove->shortcut().toString()+" "+actionRemove->text(),
						qfCommandButtons);
	qpbRemove->setFlat(true);
	qpbRemove->setFocusPolicy(Qt::NoFocus);
	qpbRemove->addAction(actionRemove);
	connect(qpbRemove,SIGNAL(clicked()),actionRemove,SIGNAL(triggered()));
	qhblLayout->addWidget(qpbRemove);
	splitter=new QFrame(qfCommandButtons);
	splitter->setFrameShape(QFrame::VLine);
	splitter->setFrameShadow(QFrame::Sunken);
	qhblLayout->addWidget(splitter);

	qpbExit=new QPushButton(actionExit->shortcut().toString()+" "+actionExit->text(),
						qfCommandButtons);
	qpbExit->setFlat(true);
	qpbExit->setFocusPolicy(Qt::NoFocus);
	qpbExit->addAction(actionExit);
	connect(qpbExit,SIGNAL(clicked()),actionExit,SIGNAL(triggered()));
	qhblLayout->addWidget(qpbExit);

	qfCommandButtons->setLayout(qhblLayout);
}
//
void MainWindowImpl::saveSettings()
{
	QSettings* settings = AppSettings::instance();
	settings->beginGroup("MainWindow");
	settings->setValue("IsMaximized", isMaximized());
	if(!isMaximized())
	{
		settings->setValue("pos", pos());
		settings->setValue("size", size());
	}
	settings->setValue("Splitter", qsplitSplitter->saveState());
	settings->setValue("State", this->saveState());
	settings->endGroup();

	settings->beginGroup("Global");
	settings->setValue("CommandHistory", commandHistory());
	settings->endGroup();

	QStringList qslToolBars;
	foreach(PCToolBar *toolBar, qlpcToolBars) {
		qslToolBars << toolBar->name();
		toolBar->save();
	}

	settings->beginGroup("Global");
	settings->setValue("ToolBars", qslToolBars);
	settings->endGroup();

	settings->sync();
}
//
void MainWindowImpl::loadSettings()
{
	QSettings* settings = AppSettings::instance();
	settings->beginGroup("MainWindow");
	move(settings->value("pos", QPoint(0, 0)).toPoint());
	resize(settings->value("size", QSize(640, 480)).toSize());
	if(settings->value("IsMaximized", false).toBool())
		showMaximized();
	qsplitSplitter->restoreState(settings->value("Splitter").toByteArray());
	settings->endGroup();

	settings->beginGroup("Global");
	setCommandHistory(settings->value("CommandHistory").toStringList());
	QStringList toolBarNames = settings->value("ToolBars").toStringList();
	settings->endGroup();

	if(toolBarNames.isEmpty()) {
		PCToolBar *toolBar = new PCToolBar(tr("Main toolbar"), this);
		connectToolBar(toolBar);
		this->addToolBar(toolBar);
		qlpcToolBars << toolBar;
	}

	foreach(const QString& toolBarName, toolBarNames) {
		PCToolBar *toolBar = new PCToolBar(toolBarName, this);
		connectToolBar(toolBar);
		this->addToolBar(toolBar);
		qlpcToolBars << toolBar;
	}

	qdbDriveBarLeft->setVisible(settings->value("Interface/ShowDriveBar", true).toBool());
	qdbDriveBarRight->setVisible(settings->value("Interface/ShowTwoDriveBar", true).toBool()
				&& qdbDriveBarLeft->isVisible());
	qlConsolePath->setVisible(settings->value("Interface/ShowCommandLine", true).toBool());
	qcbConsoleCommand->setVisible(qlConsolePath->isVisible());
	qfCommandButtons->setVisible(settings->value("Interface/ShowFunctionButtons", true).toBool());

	this->restoreState(settings->value("MainWindow/State", QByteArray()).toByteArray());
}
//
void MainWindowImpl::showSplitterContextMenu(const QPoint& pos)
{
	QMenu* menu = new QMenu(qsplitSplitter);
	for(int size = 20; size <= 80; size += 10)
	{
		int size2 = 100 - size;
		QString text = QString("&%1/%2").arg(size).arg(size2);

		QAction* action = new QAction(qsplitSplitter);
		action->setText(text);
		action->setData((double)size / 100);
		connect(action, SIGNAL(triggered()), this, SLOT(slotResizeSplitter()));
		menu->addAction(action);
	}
	menu->exec(qsplitSplitter->mapToGlobal(pos));
	delete menu;
}
//
void MainWindowImpl::slotResizeSplitter()
{
	const QAction* action = qobject_cast<const QAction*>(sender());
	if(action)
	{
		QList<int> sizes;
		sizes << int(width() * action->data().toDouble());
		sizes << int(width() * (1 - action->data().toDouble()));
		qsplitSplitter->setSizes(sizes);
	}
}
//
#ifndef Q_CC_MSVC
	#warning "TODO: make eventFilter instead"
#endif
void MainWindowImpl::slotChangedFocus()
{
	qfpFocusedFilePanel=qobject_cast<QFilePanel*>(sender());
	if (qfpFocusedFilePanel)
		slotPathChanged(qfpFocusedFilePanel->path());
}
//
void MainWindowImpl::slotPathChanged(const QString& path)
{
	qlConsolePath->setText(QDir::toNativeSeparators(path));
	qlConsolePath->setToolTip(QDir::toNativeSeparators(path));
}
//
void MainWindowImpl::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);

	if(qlConsolePath)
		qlConsolePath->setMaximumWidth(int(width() * 0.3));
}
//
void MainWindowImpl::slotRunCommand()
{
#ifndef Q_CC_MSVC
	#warning "TODO: `_localpath_ _params_' command must not run consle - just execute program"
#endif
	QString qsCommand=qcbConsoleCommand->currentText();
	if (qsCommand.isEmpty())
		return;
	qcbConsoleCommand->removeItem(qcbConsoleCommand->currentIndex());
	qcbConsoleCommand->insertItem(0,qsCommand);
	qcbConsoleCommand->setCurrentIndex(-1);
	QProcess *myProcess = new QProcess();
	myProcess->setWorkingDirectory(qlConsolePath->text());
#ifdef Q_WS_X11
	myProcess->start(qsCommand);
#endif
#ifdef Q_WS_WIN
	if (qsCommand.contains(QRegExp("^cd ")))
	{
		QDir dir(qlConsolePath->text());
		dir.cd(qsCommand.remove(QRegExp("^cd ")));
		qfpFocusedFilePanel->setPath(dir.absolutePath());
		return;
	}
	myProcess->start(qsCommand);

//		QProcess::startDetached("cmd.exe /C "+qsCommand);
//	if (!rez)
//		myProcess->start("cmd.exe /C "+qsCommand);
#endif
}
//
void MainWindowImpl::slotCpCurFileName2Cmd()
{
	if (qfpFocusedFilePanel->currentFileName().contains(" "))
		qcbConsoleCommand->setEditText(qcbConsoleCommand->currentText()+
					"\""+
					qfpFocusedFilePanel->currentFileName()+
					"\" ");
	else
		qcbConsoleCommand->setEditText(qcbConsoleCommand->currentText()+
					qfpFocusedFilePanel->currentFileName()+
					" ");
	qcbConsoleCommand->setFocus();
}
//
void MainWindowImpl::slotCpCurFileNameWhithPath2Cmd()
{
	QString qsName=qfpFocusedFilePanel->path();
	if (qsName.at(qsName.length()-1)!=QDir::separator())
		qsName+=QDir::separator();
	qsName+=qfpFocusedFilePanel->currentFileName();
	if (qsName.contains(" "))
		qcbConsoleCommand->setEditText(qcbConsoleCommand->currentText()+
					"\""+
					qsName+
					"\" ");
	else
		qcbConsoleCommand->setEditText(qcbConsoleCommand->currentText()+
					qsName+
					" ");
	qcbConsoleCommand->setFocus();
}
//
void MainWindowImpl::slotRunConsole()
{
#ifdef Q_WS_X11
//	myProcess->startDetached("/bin/sh");
#endif
#ifdef Q_WS_WIN
	QProcess::startDetached("cmd.exe",QStringList(),qlConsolePath->text());
#endif
}
//
void MainWindowImpl::slotView(const QString& fileName)
{
	if (!qflvRightPanel->hasFocus() && !qflvLeftPanel->hasFocus())
		return;
	QFilePanel* sourcePanel=qflvRightPanel->hasFocus() ? qflvRightPanel : qflvLeftPanel;

	if (!pvViewer)
	{
		pvViewer=new PantherViewer();
		pvViewer->setAttribute(Qt::WA_DeleteOnClose);
	}
	QString qsName=fileName;
	if (qsName.isEmpty())
	{
		qsName=sourcePanel->currentFileName();
	}
	if (!QFileInfo(qsName).isFile())
	{
		QMessageBox::information(this,tr("Error viewing"),tr("Not selected files"));
		return;
	}
	pvViewer->viewFile(qsName);
	pvViewer->show();
	pvViewer->activateWindow();
	pvViewer->setFocus();
}
//
void MainWindowImpl::slotRename()
{
	bool ok;
	QString newName=QInputDialog::getText(this,
											tr("Rename"),
											tr("New name"),
											QLineEdit::Normal,
											qfpFocusedFilePanel->currentFileName(),
											&ok);
	if (ok && !newName.isEmpty())
	{
		QFile::rename(qfpFocusedFilePanel->path()+qfpFocusedFilePanel->currentFileName(),
					qfpFocusedFilePanel->path()+newName);
	}
}
//
void MainWindowImpl::slotCopy(const QString& destDir,const QStringList& fileList)
{
	if (!qflvRightPanel->hasFocus() && !qflvLeftPanel->hasFocus())
		return;
	QFilePanel* sourcePanel=qflvRightPanel->hasFocus() ? qflvRightPanel : qflvLeftPanel;
	QFilePanel* destPanel=qflvRightPanel->hasFocus() ? qflvLeftPanel : qflvRightPanel;

	QStringList qslFileNames=fileList.isEmpty()
							? sourcePanel->selectedFiles()
							: fileList;
	QString destPath,sourcePath;
	sourcePath=destDir.isEmpty() ? sourcePanel->path() : "";
	destPath=destDir.isEmpty() ? destPanel->path() : destDir;

	PCCopyMoveDialog *copyDialog = new PCCopyMoveDialog(this);
	copyDialog->setSource(qslFileNames);
	copyDialog->setDest(destPath);
	copyDialog->setOperation(tr("Copy"));
	copyDialog->setQueueModel(qsimQeueuModel);

	if (copyDialog->exec())
	{
		QFileOperationsDialog* queue=0;
		int queueIndex = copyDialog->queueIndex();
		if (queueIndex>=0 && qlQueueList.at(queueIndex))
		{
			queue=qlQueueList.at(queueIndex);
		}
		for (int i=0; i<qslFileNames.count(); i++)
		{
			if (QFileInfo(qslFileNames.at(i)).isDir())
				queue=addJob(queue,
					QFileOperationsThread::CopyDirOperation,
					QStringList() << qslFileNames.at(i)+QDir::separator()
										<< destPath);
			else
				queue=addJob(queue,
					QFileOperationsThread::CopyFileOperation,
					QStringList() << qslFileNames.at(i)
										<< copyDialog->dest());
		}
		queue->setBlocked(false);
		sourcePanel->clearSelection();
	}
	delete copyDialog;
}
//
void MainWindowImpl::slotRemove(const QStringList& fileList)
{
	if (!qflvRightPanel->hasFocus() && !qflvLeftPanel->hasFocus())
		return;
	QFilePanel* sourcePanel=qflvRightPanel->hasFocus() ? qflvRightPanel : qflvLeftPanel;


	QString path=fileList.isEmpty() ? sourcePanel->path() : "";

	QDialog* qdRemoveDialog=new QDialog(this);
	qdRemoveDialog->setWindowTitle(tr("Remove"));

	QLabel* qlQuestion=new QLabel(tr("Do you really want to delete this file(s)?"),qdRemoveDialog);

	QStringListModel* qslmStrings;
	QStringList qslFiles = fileList.isEmpty() ? sourcePanel->selectedFiles() : fileList;
	for (int i = 0; i < qslFiles.size(); i++)
		qslFiles[i] = QDir::toNativeSeparators(qslFiles[i]);
	qslmStrings=new QStringListModel(qslFiles, qdRemoveDialog);

	QListView* qlvStrings=new QListView(qdRemoveDialog);
	qlvStrings->setModel(qslmStrings);

	QLabel* qlQueue=new QLabel(tr("Queue"),qdRemoveDialog);

	QComboBox* qcbQueue=new QComboBox(qdRemoveDialog);
	qcbQueue->setModel(qsimQeueuModel);
	qcbQueue->setModelColumn(0);
	qcbQueue->setCurrentIndex(-1);

	QDialogButtonBox* qdbbButtons=new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
			Qt::Horizontal,
			qdRemoveDialog);
	connect(qdbbButtons,
			SIGNAL(accepted()),
			qdRemoveDialog,
			SLOT(accept()));
	connect(qdbbButtons,
			SIGNAL(rejected()),
			qdRemoveDialog,
			SLOT(reject()));

	QVBoxLayout* qvblMainLayout=new QVBoxLayout();
	qvblMainLayout->addWidget(qlQuestion);
	qvblMainLayout->addWidget(qlvStrings);
	qvblMainLayout->addWidget(qlQueue);
	qvblMainLayout->addWidget(qcbQueue);
	qvblMainLayout->addWidget(qdbbButtons);

	qdRemoveDialog->setLayout(qvblMainLayout);

	if (qdRemoveDialog->exec())
	{
		QFileOperationsDialog* queue=0;
		int queueIndex=qcbQueue->currentIndex();
		if (queueIndex>=0 && qlQueueList.at(queueIndex))
		{
			queue=qlQueueList.at(queueIndex);
		}
		for (int i=0; i<qslmStrings->rowCount(); i++)
		{
			if (QFileInfo(qslmStrings->stringList().at(i)).isDir())
				queue=addJob(queue,
					QFileOperationsThread::RemoveDirOperation,
					QStringList() << qslmStrings->stringList().at(i)+QDir::separator());
			else
				queue=addJob(queue,
					QFileOperationsThread::RemoveFileOperation,
					QStringList() << qslmStrings->stringList().at(i));
		}
		queue->setBlocked(false);
		sourcePanel->clearSelection();
	}
	delete qdRemoveDialog;
}
//
QFileOperationsDialog* MainWindowImpl::addJob(QFileOperationsDialog* queue, QFileOperationsThread::FileOperation operation, const QStringList& parameters)
{
	if(!queue)
	{
		queue = new QFileOperationsDialog(0);
		connect(queue, SIGNAL(finished(int)),
				this, SLOT(slotQueueFinished()));
		connect(queue, SIGNAL(jobChanged()),
				this, SLOT(slotQueueChanged()));
		qlQueueList << queue;
		queue->show();
	}
	queue->setBlocked(true);
	queue->addJob(operation, parameters);

	return queue;
}
//
void MainWindowImpl::slotQueueFinished()
{
	QFileOperationsDialog* dialog = qobject_cast<QFileOperationsDialog*>(sender());
	if(dialog)
	{
		if(qlQueueList.removeOne(dialog))
		{
			dialog->deleteLater();
			slotQueueChanged();
		}
	}
}
//
void MainWindowImpl::slotQueueChanged()
{
	qsimQeueuModel->clear();
	QStandardItem* item;
	for (int i=0; i<qlQueueList.count(); i++)
	{
		item=new QStandardItem(qlQueueList.at(i)->jobName());
		qsimQeueuModel->appendRow(item);
	}
}
//
void MainWindowImpl::slotMkDir()
{
	if (!qflvRightPanel->hasFocus() && !qflvLeftPanel->hasFocus())
		return;
	QFilePanel* sourcePanel=qflvRightPanel->hasFocus() ? qflvRightPanel : qflvLeftPanel;

	QDialog* qdMkDirDialog=new QDialog(this);
	qdMkDirDialog->setWindowTitle(tr("Creating directory"));

	QComboBox* qcbDirName=new QComboBox(qdMkDirDialog);
	qcbDirName->setEditable(true);
//Load history
	QSettings* settings = AppSettings::instance();
	qcbDirName->addItems(settings->value("Global/MkDirHistory",QStringList()).toStringList());
	qcbDirName->setCurrentIndex(-1);
//
	QString fileName=sourcePanel->currentFileName();
	if (QFileInfo(sourcePanel->path()+fileName).isFile())
		qcbDirName->setEditText(QFileInfo(sourcePanel->path()+fileName).baseName());
	else
		qcbDirName->setEditText(QFileInfo(fileName).fileName());
	qcbDirName->lineEdit()->selectAll();

	QDialogButtonBox* qdbbButtons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
													Qt::Horizontal,
													qdMkDirDialog);
	connect(qdbbButtons,
			SIGNAL(accepted()),
			qdMkDirDialog,
			SLOT(accept()));
	connect(qdbbButtons,
			SIGNAL(rejected()),
			qdMkDirDialog,
			SLOT(reject()));

	QVBoxLayout* qvblMainLayout=new QVBoxLayout();
	qvblMainLayout->addWidget(qcbDirName);
	qvblMainLayout->addWidget(qdbbButtons);
	qdMkDirDialog->setLayout(qvblMainLayout);

	if (qdMkDirDialog->exec())
	{
		if (!QDir().mkdir(sourcePanel->path() + "/" + qcbDirName->currentText()))
		{
			QMessageBox::critical(this,qdMkDirDialog->windowTitle(),tr("Error creating directory"));
		}
		else
		{
			//Move last index to top
			QString qsLastDirName=qcbDirName->currentText();
			if (qsLastDirName.isEmpty())
				return;
			qcbDirName->removeItem(qcbDirName->findText(qsLastDirName));
			qcbDirName->insertItem(0,qsLastDirName);
			//
			//Save history
			#ifndef Q_CC_MSVC
				#warning "Maybe have property, which return QStringList???"
			#endif
			QStringList qslMkDirHistory;
			for (int i=0; i<qcbDirName->count(); i++)
				qslMkDirHistory << qcbDirName->itemText(i);
			settings->setValue("Global/MkDirHistory",qslMkDirHistory);
			settings->sync();
			//
		}
	}
	delete qdMkDirDialog;
}

QStringList MainWindowImpl::commandHistory() const
{
	QStringList history;
	for(int i = 0, count = qcbConsoleCommand->count(); i < count; ++i)
		history.append(qcbConsoleCommand->itemText(i));
	return history;
}

void MainWindowImpl::setCommandHistory(const QStringList& commandHistory)
{
	qcbConsoleCommand->clear();
	qcbConsoleCommand->addItems(commandHistory);
	qcbConsoleCommand->setCurrentIndex(-1);
}
//
void MainWindowImpl::slotMove(const QString& destDir,const QStringList& fileList)
{
	if (!qflvRightPanel->hasFocus() && !qflvLeftPanel->hasFocus())
		return;
	QFilePanel* sourcePanel=qflvRightPanel->hasFocus() ? qflvRightPanel : qflvLeftPanel;
	QFilePanel* destPanel=qflvRightPanel->hasFocus() ? qflvLeftPanel : qflvRightPanel;

	QStringList qslFileNames=fileList.isEmpty()
							? sourcePanel->selectedFiles()
							: fileList;
	QString destPath,sourcePath;
	sourcePath=destDir.isEmpty() ? sourcePanel->path() : "";
	destPath=destDir.isEmpty() ? destPanel->path() : destDir;

	PCCopyMoveDialog *moveDialog = new PCCopyMoveDialog(this);
	moveDialog->setSource(qslFileNames);
	moveDialog->setDest(destPath);
	moveDialog->setOperation(tr("Move"));
	moveDialog->setQueueModel(qsimQeueuModel);

	if (moveDialog->exec())
	{
		QFileOperationsDialog* queue=0;
		int queueIndex = moveDialog->queueIndex();
		if (queueIndex>=0 && qlQueueList.at(queueIndex))
		{
			queue=qlQueueList.at(queueIndex);
		}
		for (int i=0; i<qslFileNames.count(); i++)
		{
			if (QFileInfo(qslFileNames.at(i)).isDir())
				queue=addJob(queue,
					QFileOperationsThread::MoveDirOperation,
					QStringList() << qslFileNames.at(i)+QDir::separator()
										<< moveDialog->dest()+QDir::separator());
			else
				queue=addJob(queue,
					QFileOperationsThread::MoveFileOperation,
					QStringList() << qslFileNames.at(i)
										<< moveDialog->dest()+QDir::separator()+QFileInfo(qslFileNames.at(i)).fileName());
		}
		queue->setBlocked(false);
		sourcePanel->clearSelection();
	}
	delete moveDialog;
}
//
void MainWindowImpl::dropEvent(QDropEvent* event)
{
	QWidget* widget = childAt(event->pos());
	if (widget == qpbRunConsole)
	{
		qWarning() << event->mimeData()->urls();
#ifndef Q_CC_MSVC
	#warning "dnd for F2 not implemented yet"
#endif
	}
	else if (widget == qpbView)
	{
#ifndef Q_CC_MSVC
	#warning "will not work for vfs since `QUrl::toLocalFile()'"
#endif
		slotView(event->mimeData()->urls().at(0).toLocalFile());
	}
	else if (widget == qpbEdit)
	{
#ifndef Q_CC_MSVC
	#warning "dnd for F4 not implemented yet"
#endif
	}
	else if (widget == qpbRemove)
	{
#ifndef Q_CC_MSVC
	#warning "will not work for vfs since `QUrl::toLocalFile()'"
#endif
		QStringList list;
		foreach(QUrl url,event->mimeData()->urls())
			list << QDir::toNativeSeparators(url.toLocalFile());
		slotRemove(list);
	}

/*?	QTreeView* view=qobject_cast<QTreeView*>(widget->parent());
	if (view)
	{
		QString destPath;
		if (qflvLeftPanel->isAncestorOf(view))
			destPath=qflvLeftPanel->path();
		else
			destPath=qflvRightPanel->path();

		QModelIndex index=view->model()->index(view->indexAt(view->viewport()->mapFrom(this,event->pos())).row(),0);
		if (index.isValid())
		{
			QDir dir(destPath);
			dir.cd(index.data(Qt::EditRole).toString());
			destPath=QDir::toNativeSeparators(dir.absolutePath());
			if (destPath.at(destPath.length()-1)!=QDir::separator())
				destPath+=QDir::separator();
		}
		QStringList fileList;
		foreach(QUrl url,event->mimeData()->urls())
			fileList << QDir::toNativeSeparators(url.toLocalFile());
		if (event->mouseButtons() & Qt::RightButton)
			slotMove(destPath,fileList);
		else
			slotCopy(destPath,fileList);
	}*/
/*?*///	QMainWindow::dropEvent(event);
}
//
void MainWindowImpl::dragMoveEvent(QDragMoveEvent* event)
{
	bool isAccepted = false;
	QWidget* widget = childAt(event->pos());
//Command Buttons
	isAccepted = widget == qpbRunConsole || widget == qpbView || widget == qpbEdit || widget == qpbRemove;

	foreach(PCToolBar *toolBar, qlpcToolBars)
		if (toolBar == widget || toolBar == widget->parentWidget()) {
			isAccepted = true;
			break;
		}
//TreeView
/*?	QTreeView* view=qobject_cast<QTreeView*>(widget->parent());
	if (view)
	{
		QModelIndex index=view->model()->index(view->indexAt(view->viewport()->mapFrom(this,event->pos())).row(),0);
		if (!index.isValid())
			isAccepted=true;
		else
		{
			if (!(view==event->source() && view->selectionModel()->isSelected(index)))
				isAccepted=true;
		}
	}*/
//Drives toolbar

	event->setAccepted(isAccepted);

/*?*///	QMainWindow::dragMoveEvent(event);
}
//
void MainWindowImpl::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
		event->acceptProposedAction();

/*?*///	QMainWindow::dragEnterEvent(event);
}
//
void MainWindowImpl::toolBarActionExecute(const SToolBarButton& actionButton)
{
#ifndef Q_CC_MSVC
	#warning "TODO: parse params"
#endif
	QFileInfo fi(actionButton.qsCommand);
	if (!fi.exists()) {
		QAction *action = PCCommands::instance()->action(actionButton.qsCommand);
		if (action)
			action->trigger();
	}
	if (fi.isDir())
		cdExecute(fi.absoluteFilePath());
	else
		QFileOperationsThread::execute(actionButton.qsCommand,
									actionButton.qsParams.split(QLatin1Char(' ')),
									actionButton.qsWorkDir);
}

void MainWindowImpl::slotFindFiles()
{
	FindFilesDialog dialog(this);
	dialog.exec();
}
//
void MainWindowImpl::slotPreferences()
{
	QPreferencesDialog dialog(this);
	dialog.exec();
}
//
void MainWindowImpl::slotSetDisc(const QString& path)
{
	if (sender()==qdbDriveBarLeft)
	{
		if (qdbDriveBarRight->isVisible())
		{
			qflvLeftPanel->setPath(path);
			qflvLeftPanel->setFocus();
		}
		else
		{
			#ifndef Q_CC_MSVC
				#warning "qfpFocusedFilePanel must be automatical"
			#endif
			qfpFocusedFilePanel=qflvLeftPanel->hasFocus() ? qflvLeftPanel : qflvRightPanel;
			qfpFocusedFilePanel->setPath(path);
		}
	}
	if (sender()==qdbDriveBarRight)
	{
		qflvRightPanel->setPath(path);
		qflvRightPanel->setFocus();
	}
}

void MainWindowImpl::slotAddToolBar()
{
	bool ok;
	QString qsToolBarName = QInputDialog::getText(this,
												  tr("Set toolbar name"),
												  tr("Name"),
												  QLineEdit::Normal,
												  QString(),
												  &ok);
	if (!ok)
		return;

	if (qsToolBarName.isEmpty()) {
		QMessageBox::critical(this, "", tr("Toolbar name is empty. Break."));
		return;
	}

	foreach (PCToolBar *toolBar, qlpcToolBars) {
		if (toolBar->name() == qsToolBarName) {
			QMessageBox::critical(this, "", tr("This name is not unique."));
			slotAddToolBar();
			return;
		}
	}

	PCToolBar *toolBar = new PCToolBar(qsToolBarName, this);
	connectToolBar(toolBar);
	this->addToolBar(toolBar);
	qlpcToolBars << toolBar;
}

void MainWindowImpl::slotRemoveToolBar()
{
	QAction *action = qobject_cast<QAction*> (sender());
	if (!action)
		return;
	PCToolBar *toolBar = qlpcToolBars.at(action->data().toInt());
	if (!toolBar)
		return;

	QString qsName = toolBar->name();
	this->removeToolBar(toolBar);
	qlpcToolBars.removeOne(toolBar);
	toolBar->deleteLater();

	QSettings* settings = AppSettings::instance();
	settings->remove("ToolBar_" + qsName);
	settings->sync();

	if (qlpcToolBars.count() == 0) {
		PCToolBar *toolBar = new PCToolBar(tr("Main toolbar"), this);
		toolBar->hide();
		connectToolBar(toolBar);
		this->addToolBar(toolBar);
		qlpcToolBars << toolBar;
	}
}

void MainWindowImpl::slotRenameToolBar()
{
	QAction *action = qobject_cast<QAction*> (sender());
	if (!action)
		return;
	PCToolBar *toolBar = qlpcToolBars.at(action->data().toInt());
	if (!toolBar)
		return;

	QString qsName = toolBar->name();
	bool ok;
	QString qsToolBarNewName = QInputDialog::getText(this,
												  tr("Set toolbar name"),
												  tr("Name"),
												  QLineEdit::Normal,
												  qsName,
												  &ok);
	if (!ok)
		return;

	if (qsToolBarNewName.isEmpty()) {
		QMessageBox::critical(this, "", tr("Toolbar name is empty. Break."));
		return;
	}

	if (qsToolBarNewName == qsName)
		return;

	foreach (PCToolBar *toolBar, qlpcToolBars) {
		if (toolBar->name() == qsToolBarNewName) {
			QMessageBox::critical(this, "", tr("This name is not unique. Break."));
			return;
		}
	}

	toolBar->rename(qsToolBarNewName);

	QSettings* settings = AppSettings::instance();
	settings->remove("ToolBar_" + qsName);

	QStringList qslToolBars;
	foreach(PCToolBar *toolBar, qlpcToolBars) {
		qslToolBars << toolBar->name();
	}

	settings->beginGroup("Global");
	settings->setValue("ToolBars", qslToolBars);
	settings->endGroup();

	toolBar->save();

	settings->sync();
}

void MainWindowImpl::connectToolBar(PCToolBar *toolBar)
{
	connect (toolBar, SIGNAL(toolBarActionExecuted(SToolBarButton)),
			 this, SLOT(toolBarActionExecute(SToolBarButton)));
	connect (toolBar, SIGNAL(cdExecuted(QString)),
			 this, SLOT(cdExecute(QString)));
	connect (toolBar, SIGNAL(toolbarContextMenu(QPoint)),
			 this, SLOT(slotToolBarContextMenu(QPoint)));
}

void MainWindowImpl::cdExecute(const QString& path)
{
	if (!qflvRightPanel->hasFocus() && !qflvLeftPanel->hasFocus())
		return;
	qfpFocusedFilePanel = qflvRightPanel->hasFocus() ? qflvRightPanel : qflvLeftPanel;

	QDir dir(path);
	qfpFocusedFilePanel->setPath(dir.absolutePath());
}

QMenu* MainWindowImpl::createToolBarsMenu(PCToolBar *currentToolBar)
{
	QMenu *qmToolBarMenu = new QMenu(tr("Toolbars"), this);
	QAction *menuAction;

	menuAction = new QAction(tr("&Add toolbar"), qmToolBarMenu);
	connect(menuAction, SIGNAL(triggered()),
			this, SLOT(slotAddToolBar()));
	qmToolBarMenu->addAction(menuAction);

	if (qlpcToolBars.count() <= 0)
		return qmToolBarMenu;

	QMenu *removeMenu = new QMenu(tr("&Remove toolbar"), this);
	int i = 0;
	foreach(PCToolBar *toolBar, qlpcToolBars) {
		menuAction = new QAction(toolBar->name(), removeMenu);
		menuAction->setData(i++);
		connect(menuAction, SIGNAL(triggered()),
				this, SLOT(slotRemoveToolBar()));
		removeMenu->addAction(menuAction);
		if (toolBar == currentToolBar)
			removeMenu->setDefaultAction(menuAction);
	}
	qmToolBarMenu->addMenu(removeMenu);

	QMenu *renameMenu = new QMenu(tr("Re&name toolbar"), this);
	i = 0;
	foreach(PCToolBar *toolBar, qlpcToolBars) {
		menuAction = new QAction(toolBar->name(), renameMenu);
		menuAction->setData(i++);
		connect(menuAction, SIGNAL(triggered()),
				this, SLOT(slotRenameToolBar()));
		renameMenu->addAction(menuAction);
		if (toolBar == currentToolBar)
			renameMenu->setDefaultAction(menuAction);
	}
	qmToolBarMenu->addMenu(renameMenu);

	QMenu *showHideMenu = new QMenu(tr("&Show/Hide toolbar"), this);
	foreach(PCToolBar *toolBar, qlpcToolBars) {
		menuAction = new QAction(toolBar->name(), showHideMenu);
		menuAction->setCheckable(true);
		menuAction->setChecked(toolBar->isVisible());
		connect(menuAction, SIGNAL(triggered(bool)),
				toolBar, SLOT(setVisible(bool)));
		showHideMenu->addAction(menuAction);
		if (toolBar == currentToolBar)
			showHideMenu->setDefaultAction(menuAction);
	}
	qmToolBarMenu->addMenu(showHideMenu);

	if (currentToolBar) {
		qmToolBarMenu->addSeparator();

		menuAction = new QAction(tr("A&dd separator"), qmToolBarMenu);
		connect(menuAction, SIGNAL(triggered()),
				currentToolBar, SLOT(slotAddSeparator()));
		qmToolBarMenu->addAction(menuAction);
	}

	return qmToolBarMenu;
}

void MainWindowImpl::slotToolBarContextMenu(const QPoint& pos)
{
	PCToolBar *pcToolBar = qobject_cast<PCToolBar*> (sender());
	if (!pcToolBar)
		return;

	createToolBarsMenu(pcToolBar)->exec(pos);
}

/* ** TESTING PURPOSES ONLY ** */
#include <QFileDialog>

void MainWindowImpl::slotTestingFileDialog()
{
	QFileDialog dialog(this);
	dialog.exec();
}

#ifdef Q_WS_WIN
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;

void MainWindowImpl::slotTestingEnableNTFSPermissionLookup(bool enable)
{
	if(enable)
		qt_ntfs_permission_lookup = 1;
	else
		qt_ntfs_permission_lookup = 0;
}
#endif
