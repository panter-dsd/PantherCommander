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
#include <QtGui>
#include "mainwindowimpl.h"
//
MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f)
	: QMainWindow(parent, f)
{
	this->resize(640,480);
	setAcceptDrops(true);
	QTime time;
	time.start();
	createControls();
	qDebug(QString::number(time.elapsed()).toLocal8Bit());
	setLayouts();

	loadSettings();
	createSplitterMenu();
	createActions();
	createMenu();
	setConnects();
	createCommandButtons();

	qflvLeftPanel->setFocus();
	qfpFocusedFilePanel=qflvLeftPanel;
}
//
void MainWindowImpl::createControls()
{
	qwCentralWidget=new QWidget(this);
	this->setCentralWidget(qwCentralWidget);

	qflvLeftPanel=new QFilePanel("LeftPanel",this);
	qflvRightPanel=new QFilePanel("RightPanel",this);

	qsplitSplitter=new QSplitter(this);
	qsplitSplitter->setContextMenuPolicy(Qt::ActionsContextMenu);
	qsplitSplitter->addWidget(qflvLeftPanel);
	qsplitSplitter->addWidget(qflvRightPanel);

	qlConsolePath=new QLabel(this);
	qlConsolePath->setAlignment(Qt::AlignRight);

	qcbConsoleCommand=new QComboBox(this);
	qcbConsoleCommand->setEditable(true);
	qcbConsoleCommand->setInsertPolicy(QComboBox::InsertAtTop);
	qcbConsoleCommand->setFocusPolicy(Qt::ClickFocus);
	qcbConsoleCommand->setDuplicatesEnabled(false);

	qfCommandButtons=new QFrame(this);
	qfCommandButtons->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Maximum);

	qsimQeueuModel=new QStandardItemModel(this);
}
//
void MainWindowImpl::setLayouts()
{
	QHBoxLayout* qhblConsoleCommandLayout=new QHBoxLayout();
	qhblConsoleCommandLayout->addWidget(qlConsolePath);
	qhblConsoleCommandLayout->addWidget(qcbConsoleCommand);

	QVBoxLayout* layout=new QVBoxLayout();
	layout->addWidget(qsplitSplitter);
	layout->addLayout(qhblConsoleCommandLayout);
	layout->addWidget(qfCommandButtons);
	qwCentralWidget->setLayout(layout);
}
//
MainWindowImpl::~MainWindowImpl()
{
	saveSettings();
}
//
void MainWindowImpl::loadSettings()
{
	QSettings* settings=new QSettings(this);
	this->move(settings->value("MainWindow/pos",QPoint(0,0)).toPoint());
	this->resize(settings->value("MainWindow/size",QSize(640,480)).toSize());
	bool bIsMaximized=settings->value("MainWindow/IsMaximized",false).toBool();
	if (bIsMaximized)
		this->setWindowState(Qt::WindowMaximized);

	qsplitSplitter->restoreState(settings->value("MainWindow/Splitter",QByteArray()).toByteArray());

	qcbConsoleCommand->addItems(settings->value("Global/CommandHistory",QStringList()).toStringList());
	qcbConsoleCommand->setCurrentIndex(-1);

	QStringList qslToolBars=settings->value("Global/ToolBars",QStringList()).toStringList();
	if (qslToolBars.isEmpty())
		loadToolBar("Main toolbar");
	else
		for (int i=0; i<qslToolBars.count(); i++)
			loadToolBar(qslToolBars.at(i));
	delete settings;
}
//
void MainWindowImpl::saveSettings()
{
	QSettings* settings=new QSettings(this);
	if (this->windowState()!=Qt::WindowMaximized)
	{
		settings->setValue("MainWindow/pos",this->pos());
		settings->setValue("MainWindow/size",this->size());
		settings->setValue("MainWindow/IsMaximized",false);
	}
	else
		settings->setValue("MainWindow/IsMaximized",true);
	settings->setValue("MainWindow/Splitter",qsplitSplitter->saveState());
#ifndef Q_CC_MSVC
	#warning "Maby have property, which return QStringList???"
#endif
	QStringList qslCommandHistory;
	for (int i=0; i<qcbConsoleCommand->count(); i++)
		qslCommandHistory << qcbConsoleCommand->itemText(i);
	settings->setValue("Global/CommandHistory",qslCommandHistory);
	saveToolBars();
	settings->sync();
	delete settings;
}
//
void MainWindowImpl::createSplitterMenu()
{
	QAction* action;
	action=new QAction("&20/80",qsplitSplitter);
	action->setData(20.0/100.0);
	qsplitSplitter->addAction(action);
	connect(action,
			SIGNAL(triggered()),
			this,
			SLOT(slotResizeSplitter()));

	action=new QAction("&30/70",qsplitSplitter);
	action->setData(30.0/100.0);
	qsplitSplitter->addAction(action);
	connect(action,
			SIGNAL(triggered()),
			this,
			SLOT(slotResizeSplitter()));

	action=new QAction("&40/60",qsplitSplitter);
	action->setData(40.0/100.0);
	qsplitSplitter->addAction(action);
	connect(action,
			SIGNAL(triggered()),
			this,
			SLOT(slotResizeSplitter()));

	action=new QAction("&50/50",qsplitSplitter);
	action->setData(50.0/100.0);
	qsplitSplitter->addAction(action);
	connect(action,
			SIGNAL(triggered()),
			this,
			SLOT(slotResizeSplitter()));

	action=new QAction("&60/40",qsplitSplitter);
	action->setData(60.0/100.0);
	qsplitSplitter->addAction(action);
	connect(action,
			SIGNAL(triggered()),
			this,
			SLOT(slotResizeSplitter()));

	action=new QAction("&70/30",qsplitSplitter);
	action->setData(70.0/100.0);
	qsplitSplitter->addAction(action);
	connect(action,
			SIGNAL(triggered()),
			this,
			SLOT(slotResizeSplitter()));

	action=new QAction("&80/20",qsplitSplitter);
	action->setData(80.0/100.0);
	qsplitSplitter->addAction(action);
	connect(action,
			SIGNAL(triggered()),
			this,
			SLOT(slotResizeSplitter()));
}
//
void MainWindowImpl::slotResizeSplitter()
{
	QAction* action=qobject_cast<QAction*> (sender());
	if (!action)
		return;
	QList<int> sizes;
	sizes << int(this->width()*action->data().toDouble());
	sizes << this->width()-int(this->width()*action->data().toDouble());
	qsplitSplitter->setSizes(sizes);
}
//
void MainWindowImpl::slotChangedFocus()
{
	qfpFocusedFilePanel=qobject_cast<QFilePanel*>(sender());
	if (qfpFocusedFilePanel)
		slotChangedPath(qfpFocusedFilePanel->path());
}
//
void MainWindowImpl::slotChangedPath(const QString& path)
{
	qlConsolePath->setText(path);
	qlConsolePath->setToolTip(path);
}
//
void MainWindowImpl::paintEvent ( QPaintEvent * event )
{
	qlConsolePath->setMaximumWidth(int(this->width()*0.3));
	event->accept();
}
//
void MainWindowImpl::slotRunCommand()
{
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
	}
	myProcess->start(qsCommand);

//		QProcess::startDetached("cmd.exe /C "+qsCommand);
//	if (!rez)
//		myProcess->start("cmd.exe /C "+qsCommand);
#endif
}
//
void MainWindowImpl::createActions()
{
	actionCpCurFileName2Cmd=new QAction(tr("Copy current file name to command string"),
											this);
	actionCpCurFileName2Cmd->setShortcut(Qt::CTRL+Qt::Key_Return);
	this->addAction(actionCpCurFileName2Cmd);

	actionCpCurFileNameWhithPath2Cmd=new QAction(tr("Copy current file name whith path to command string"),
											this);
	actionCpCurFileNameWhithPath2Cmd->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_Return);
	this->addAction(actionCpCurFileNameWhithPath2Cmd);

	actionClearCmd=new QAction(tr("Clear command string"),this);
	actionClearCmd->setShortcut(Qt::Key_Escape);
	this->addAction(actionClearCmd);

	actionRunConsole=new QAction(tr("Run console"),this);
	actionRunConsole->setShortcut(Qt::Key_F2);
	this->addAction(actionRunConsole);

	actionView=new QAction(tr("View"),this);
	actionView->setShortcut(Qt::Key_F3);
	this->addAction(actionView);

	actionEdit=new QAction(tr("Edit"),this);
	actionEdit->setShortcut(Qt::Key_F4);
	this->addAction(actionEdit);

	actionCopy=new QAction(tr("Copy"),this);
	actionCopy->setShortcut(Qt::Key_F5);
	this->addAction(actionCopy);

	actionMove=new QAction(tr("Move"),this);
	actionMove->setShortcut(Qt::Key_F6);
	this->addAction(actionMove);

	actionMkDir=new QAction(tr("Create dir"),this);
	actionMkDir->setShortcut(Qt::Key_F7);
	this->addAction(actionMkDir);

	actionRemove=new QAction(tr("Remove"),this);
	actionRemove->setShortcut(Qt::Key_F8);
	this->addAction(actionRemove);

	actionExit=new QAction(tr("Exit"),this);
	actionExit->setShortcut(Qt::ALT+Qt::Key_X);
	this->addAction(actionExit);

	actionRename=new QAction(tr("Rename"),this);
	actionRename->setShortcut(Qt::SHIFT+Qt::Key_F6);
	this->addAction(actionRename);
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
void MainWindowImpl::setConnects()
{
	connect(qflvLeftPanel,
			SIGNAL(headerSectionResized(int,int,int)),
			qflvRightPanel,
			SLOT(slotFileListHeaderSectionResize(int,int,int)));
	connect(qflvLeftPanel,
			SIGNAL(focusIn()),
			this,
			SLOT(slotChangedFocus()));
	connect(qflvRightPanel,
			SIGNAL(headerSectionResized(int,int,int)),
			qflvLeftPanel,
			SLOT(slotFileListHeaderSectionResize(int,int,int)));
	connect(qflvRightPanel,
			SIGNAL(focusIn()),
			this,
			SLOT(slotChangedFocus()));
	connect(qflvLeftPanel,
			SIGNAL(changedIgnoringDrive()),
			qflvRightPanel,
			SLOT(slotRereadDriveToolBar()));
	connect(qflvRightPanel,
			SIGNAL(changedIgnoringDrive()),
			qflvLeftPanel,
			SLOT(slotRereadDriveToolBar()));
	connect(qflvLeftPanel,
			SIGNAL(changedPath(QString)),
			this,
			SLOT(slotChangedPath(QString)));
	connect(qflvRightPanel,
			SIGNAL(changedPath(QString)),
			this,
			SLOT(slotChangedPath(QString)));

	connect(qcbConsoleCommand->lineEdit(),
			SIGNAL(returnPressed()),
			this,
			SLOT(slotRunCommand()));

	connect(actionCpCurFileName2Cmd,
			SIGNAL(triggered()),
			this,
			SLOT(slotCpCurFileName2Cmd()));
	connect(actionCpCurFileNameWhithPath2Cmd,
			SIGNAL(triggered()),
			this,
			SLOT(slotCpCurFileNameWhithPath2Cmd()));
	connect(actionClearCmd,
			SIGNAL(triggered()),
			qcbConsoleCommand->lineEdit(),
			SLOT(clear()));

	connect(actionRunConsole,
			SIGNAL(triggered()),
			this,
			SLOT(slotRunConsole()));
	connect(actionView,
			SIGNAL(triggered()),
			this,
			SLOT(slotView()));
	connect(actionCopy,
			SIGNAL(triggered()),
			this,
			SLOT(slotCopy()));
	connect(actionMove,
			SIGNAL(triggered()),
			this,
			SLOT(slotMove()));
	connect(actionMkDir,
			SIGNAL(triggered()),
			this,
			SLOT(slotMkDir()));
	connect(actionRemove,
			SIGNAL(triggered()),
			this,
			SLOT(slotRemove()));
	connect(actionExit,
			SIGNAL(triggered()),
			this,
			SLOT(close()));
	connect(actionRename,
			SIGNAL(triggered()),
			this,
			SLOT(slotRename()));
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
	if (!pvViewer)
	{
		pvViewer=new PantherViewer();
		pvViewer->setAttribute(Qt::WA_DeleteOnClose);
	}
	QString qsName=fileName;
	if (qsName.isEmpty())
	{
		qsName=qfpFocusedFilePanel->path();
		if (qsName.at(qsName.length()-1)!=QDir::separator())
			qsName+=QDir::separator();
		qsName+=qfpFocusedFilePanel->currentFileName();
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
void MainWindowImpl::createMenu()
{
	qmbMainMenu=new QMenuBar(this);

	QMenu* qmFile=new QMenu(tr("File"));
	qmFile->addAction(actionRename);
	qmFile->addAction(actionExit);

	qmbMainMenu->addMenu(qmFile);

	this->setMenuBar(qmbMainMenu);
}
//
void MainWindowImpl::slotCopy(const QString& destDir,const QStringList& fileList)
{
	QStringList qslFileNames=fileList.isEmpty()
							? qfpFocusedFilePanel->selectedFileNames()
							: fileList;
	QString destPath,sourcePath;
	QFilePanel* sourcePanel;
	QFilePanel* destPanel;
	if (qfpFocusedFilePanel==qflvLeftPanel)
	{
		sourcePanel=qflvLeftPanel;
		destPanel=qflvRightPanel;
	}
	else
	{
		sourcePanel=qflvRightPanel;
		destPanel=qflvLeftPanel;
	}
	sourcePath=destDir.isEmpty() ? sourcePanel->path() : "";
	destPath=destDir.isEmpty() ? destPanel->path() : destDir;

	QDialog* qdCopyMoveDialog=new QDialog(this);
	qdCopyMoveDialog->setWindowTitle(tr("Copy"));

	QLabel* qlSources=new QLabel(qdCopyMoveDialog);
	if (qslFileNames.count()==0)
		qlSources->setText(tr("Copy \"%1\" to").arg(qslFileNames.at(0)));
	else
		qlSources->setText(tr("Copy \"%1\" file(s) to").arg(qslFileNames.count()));

	QLineEdit* qleDest=new QLineEdit(qdCopyMoveDialog);
	qleDest->setText(destPath);
	if (qslFileNames.count()==1 && QFileInfo(sourcePath+qslFileNames.at(0)).isFile())
	{
		//qleDest->setText(qleDest->text()+QFileInfo(qslFileNames.at(0)).fileName());
	}

	QLabel* qlQueue=new QLabel(tr("Queue"),qdCopyMoveDialog);

	QComboBox* qcbQueue=new QComboBox(qdCopyMoveDialog);
	qcbQueue->setModel(qsimQeueuModel);
	qcbQueue->setModelColumn(0);
	qcbQueue->setCurrentIndex(-1);

	QDialogButtonBox* qdbbButtons=new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
			Qt::Horizontal,
			qdCopyMoveDialog);
	connect(qdbbButtons,
			SIGNAL(accepted()),
			qdCopyMoveDialog,
			SLOT(accept()));
	connect(qdbbButtons,
			SIGNAL(rejected()),
			qdCopyMoveDialog,
			SLOT(reject()));

	QVBoxLayout* qvblMainLayout=new QVBoxLayout();
	qvblMainLayout->addWidget(qlSources);
	qvblMainLayout->addWidget(qleDest);
	qvblMainLayout->addWidget(qlQueue);
	qvblMainLayout->addWidget(qcbQueue);
	qvblMainLayout->addWidget(qdbbButtons);

	qdCopyMoveDialog->setLayout(qvblMainLayout);

	if (qdCopyMoveDialog->exec())
	{
		QFileOperationsDialog* queue=0;
		int queueIndex=qcbQueue->currentIndex();
		if (queueIndex>=0 && qlQueueList.at(queueIndex))
		{
			queue=qlQueueList.at(queueIndex);
		}
		for (int i=0; i<qslFileNames.count(); i++)
		{
			if (QFileInfo(sourcePath+qslFileNames.at(i)).isDir())
				queue=addJob(queue,
					QFileOperationsThread::CopyDirOperation,
					QStringList() << sourcePath+qslFileNames.at(i)+QDir::separator()
										<< qleDest->text()+QDir::separator());
			else
				queue=addJob(queue,
					QFileOperationsThread::CopyFileOperation,
					QStringList() << sourcePath+qslFileNames.at(i)
										<< qleDest->text()+QDir::separator()+QFileInfo(qslFileNames.at(i)).fileName());
		}
		queue->setBlocked(false);
		sourcePanel->clearSelection();
	}
	delete qdCopyMoveDialog;
}
//
void MainWindowImpl::slotRemove(const QStringList& fileList)
{
	QFilePanel* sourcePanel=qfpFocusedFilePanel;
	QString path=fileList.isEmpty() ? sourcePanel->path() : "";

	QDialog* qdRemoveDialog=new QDialog(this);
	qdRemoveDialog->setWindowTitle(tr("Remove"));

	QLabel* qlQuestion=new QLabel(tr("Do you really want to delete this file(s)?"),qdRemoveDialog);

	QLabel* qlPath=new QLabel(path,qdRemoveDialog);
	qlPath->setWordWrap(true);

	QStringListModel* qslmStrings;
	if (fileList.isEmpty())
		qslmStrings=new QStringListModel(sourcePanel->selectedFileNames(),
														qdRemoveDialog);
	else
		qslmStrings=new QStringListModel(fileList,
														qdRemoveDialog);

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
	qvblMainLayout->addWidget(qlPath);
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
			if (QFileInfo(path+qslmStrings->stringList().at(i)).isDir())
				queue=addJob(queue,
					QFileOperationsThread::RemoveDirOperation,
					QStringList() << path+qslmStrings->stringList().at(i)+QDir::separator());
			else
				queue=addJob(queue,
					QFileOperationsThread::RemoveFileOperation,
					QStringList() << path+qslmStrings->stringList().at(i));
		}
		queue->setBlocked(false);
		sourcePanel->clearSelection();
	}
	delete qdRemoveDialog;
}
//
QFileOperationsDialog* MainWindowImpl::addJob(QFileOperationsDialog* queue,QFileOperationsThread::FileOperation operation,QStringList parameters)
{
	if (queue)
	{
		queue->setBlocked(true);
		queue->addJob(operation,parameters);
		return queue;
	}
	else
	{
		QFileOperationsDialog* qfodCopyDialog=new QFileOperationsDialog();
		connect(qfodCopyDialog,
				SIGNAL(finished(int)),
				this,
				SLOT(slotQueueComplite()));
		connect(qfodCopyDialog,
				SIGNAL(jobChanged()),
				this,
				SLOT(slotQueueChanged()));
		qlQueueList << qfodCopyDialog;
		qfodCopyDialog->show();
		qfodCopyDialog->setBlocked(true);
		qfodCopyDialog->addJob(operation,parameters);
		return qfodCopyDialog;
	}
}
//
void MainWindowImpl::slotQueueComplite()
{
	QFileOperationsDialog* dialog=qobject_cast<QFileOperationsDialog*>(sender());
	if (!dialog)
		return;
	int index=qlQueueList.indexOf(dialog);
	qlQueueList.removeAt(index);
	dialog->deleteLater();
	slotQueueChanged();
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
	QDialog* qdMkDirDialog=new QDialog(this);
	qdMkDirDialog->setWindowTitle(tr("Creating directory"));

	QComboBox* qcbDirName=new QComboBox(qdMkDirDialog);
	qcbDirName->setEditable(true);
//Load history
	QSettings* settings=new QSettings(this);
	qcbDirName->addItems(settings->value("Global/MkDirHistory",QStringList()).toStringList());
	qcbDirName->setCurrentIndex(-1);
//
	QString fileName=qfpFocusedFilePanel->currentFileName();
	if (QFileInfo(qfpFocusedFilePanel->path()+fileName).isFile())
		qcbDirName->setEditText(QFileInfo(qfpFocusedFilePanel->path()+fileName).baseName());
	else
		qcbDirName->setEditText(fileName);
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
		if (!QDir().mkdir(qfpFocusedFilePanel->path()+qcbDirName->currentText()))
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
				#warning "Maby have property, which return QStringList???"
			#endif
			QStringList qslMkDirHistory;
			for (int i=0; i<qcbDirName->count(); i++)
				qslMkDirHistory << qcbDirName->itemText(i);
			settings->setValue("Global/MkDirHistory",qslMkDirHistory);
			settings->sync();
			//
		}
	}
	delete settings;
	delete qdMkDirDialog;
}
//
void MainWindowImpl::loadToolBar(const QString& toolBarName)
{
	QToolBar* qtbToolBar=new QToolBar(toolBarName,this);
	qtbToolBar->setObjectName(toolBarName);
	qtbToolBar->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(qtbToolBar,
			SIGNAL(customContextMenuRequested(QPoint)),
			this,
			SLOT(slotToolBarContextMenu(QPoint)));
	this->addToolBar(qtbToolBar);
	qltbToolBarList << qtbToolBar;
	if (toolBarName.isEmpty())
		return;
	QSettings* settings=new QSettings(this);
	int buttonsCount=settings->value("ToolBar_"+toolBarName+"/ButtonsCount",0).toInt();
	QAction* action;
	for (int i=0; i<buttonsCount; i++)
	{
		SToolBarButton button;
		button.qsCommand=settings->value("ToolBar_"+toolBarName+"/Command_"+QString::number(i),"").toString();
		button.qsParams=settings->value("ToolBar_"+toolBarName+"/Params_"+QString::number(i),"").toString();
		button.qsWorkDir=settings->value("ToolBar_"+toolBarName+"/WorkDir_"+QString::number(i),"").toString();
		button.qsIconFile=settings->value("ToolBar_"+toolBarName+"/IconFile_"+QString::number(i),"").toString();
		button.qiIcon=settings->value("ToolBar_"+toolBarName+"/Icon_"+QString::number(i),"").value<QIcon>();
		button.iconNumber=settings->value("ToolBar_"+toolBarName+"/IconNumber_"+QString::number(i),-1).toInt();
		button.qsCaption=settings->value("ToolBar_"+toolBarName+"/Caption_"+QString::number(i),"").toString();

		action=new QAction(button.qiIcon,button.qsCaption,this);
		button.qaAction=action;
		connect(action,
				SIGNAL(triggered()),
				this,
				SLOT(slotToolButtonPress()));
		action->setData(toolBarName+QString::number(i));
		qmToolBurButtons.insert(toolBarName+QString::number(i),button);
		qtbToolBar->addAction(action);
	}
	delete settings;
}
//
void MainWindowImpl::saveToolBars()
{
	QSettings* settings=new QSettings(this);
	QStringList qslToolBars;
	for (int i=0; i<qltbToolBarList.count(); i++)
	{
		QString toolBarName=qltbToolBarList.at(0)->objectName();
		qslToolBars << toolBarName;
		int actionsCount=qltbToolBarList.at(0)->actions().count();
		settings->remove("ToolBar_"+toolBarName);
		settings->setValue("ToolBar_"+
						toolBarName+
						"/ButtonsCount",actionsCount);
		for (int i=0; i<actionsCount; i++)
		{
			SToolBarButton button=qmToolBurButtons.value(toolBarName+QString::number(i));
			settings->setValue("ToolBar_"+toolBarName+"/Command_"+QString::number(i),button.qsCommand);
			settings->setValue("ToolBar_"+toolBarName+"/Params_"+QString::number(i),button.qsParams);
			settings->setValue("ToolBar_"+toolBarName+"/WorkDir_"+QString::number(i),button.qsWorkDir);
			settings->setValue("ToolBar_"+toolBarName+"/IconFile_"+QString::number(i),button.qsIconFile);
			settings->setValue("ToolBar_"+toolBarName+"/IconNumber_"+QString::number(i),button.iconNumber);
			settings->setValue("ToolBar_"+toolBarName+"/Icon_"+QString::number(i),button.qiIcon);
			settings->setValue("ToolBar_"+toolBarName+"/Caption_"+QString::number(i),button.qsCaption);
		}
	}
	settings->setValue("Global/ToolBars",qslToolBars);
	settings->sync();
	delete settings;
}
//
void MainWindowImpl::slotToolButtonPress()
{
	QAction* action=qobject_cast<QAction*>(sender());
	if (!action)
		return;
	QString key=action->data().toString();
	QProcess::startDetached(qmToolBurButtons.value(key).qsCommand,
							#ifndef Q_CC_MSVC
								#warning "Need parce params"
#endif
							qmToolBurButtons.value(key).qsParams.split(" "),
							qmToolBurButtons.value(key).qsWorkDir);
}
//
void MainWindowImpl::slotMove(const QString& destDir,const QStringList& fileList)
{
	QStringList qslFileNames=fileList.isEmpty()
							? qfpFocusedFilePanel->selectedFileNames()
							: fileList;
	QString destPath,sourcePath;
	QFilePanel* sourcePanel;
	QFilePanel* destPanel;
	if (qfpFocusedFilePanel==qflvLeftPanel)
	{
		sourcePanel=qflvLeftPanel;
		destPanel=qflvRightPanel;
	}
	else
	{
		sourcePanel=qflvRightPanel;
		destPanel=qflvLeftPanel;
	}
	sourcePath=destDir.isEmpty() ? sourcePanel->path() : "";
	destPath=destDir.isEmpty() ? destPanel->path() : destDir;

	QDialog* qdCopyMoveDialog=new QDialog(this);
	qdCopyMoveDialog->setWindowTitle(tr("Move"));

	QLabel* qlSources=new QLabel(qdCopyMoveDialog);
	if (qslFileNames.count()==0)
		qlSources->setText(tr("Move \"%1\" to").arg(qslFileNames.at(0)));
	else
		qlSources->setText(tr("Move \"%1\" file(s) to").arg(qslFileNames.count()));

	QLineEdit* qleDest=new QLineEdit(qdCopyMoveDialog);
	qleDest->setText(destPath);
	if (qslFileNames.count()==1 && QFileInfo(sourcePath+qslFileNames.at(0)).isFile())
	{
		//qleDest->setText(qleDest->text()+QFileInfo(qslFileNames.at(0)).fileName());
	}

	QLabel* qlQueue=new QLabel(tr("Queue"),qdCopyMoveDialog);

	QComboBox* qcbQueue=new QComboBox(qdCopyMoveDialog);
	qcbQueue->setModel(qsimQeueuModel);
	qcbQueue->setModelColumn(0);
	qcbQueue->setCurrentIndex(-1);

	QDialogButtonBox* qdbbButtons=new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
			Qt::Horizontal,
			qdCopyMoveDialog);
	connect(qdbbButtons,
			SIGNAL(accepted()),
			qdCopyMoveDialog,
			SLOT(accept()));
	connect(qdbbButtons,
			SIGNAL(rejected()),
			qdCopyMoveDialog,
			SLOT(reject()));

	QVBoxLayout* qvblMainLayout=new QVBoxLayout();
	qvblMainLayout->addWidget(qlSources);
	qvblMainLayout->addWidget(qleDest);
	qvblMainLayout->addWidget(qlQueue);
	qvblMainLayout->addWidget(qcbQueue);
	qvblMainLayout->addWidget(qdbbButtons);

	qdCopyMoveDialog->setLayout(qvblMainLayout);

	if (qdCopyMoveDialog->exec())
	{
		QFileOperationsDialog* queue=0;
		int queueIndex=qcbQueue->currentIndex();
		if (queueIndex>=0 && qlQueueList.at(queueIndex))
		{
			queue=qlQueueList.at(queueIndex);
		}
		for (int i=0; i<qslFileNames.count(); i++)
		{
			if (QFileInfo(sourcePath+qslFileNames.at(i)).isDir())
				queue=addJob(queue,
					QFileOperationsThread::MoveDirOperation,
					QStringList() << sourcePath+qslFileNames.at(i)+QDir::separator()
										<< qleDest->text()+QDir::separator());
			else
				queue=addJob(queue,
					QFileOperationsThread::MoveFileOperation,
					QStringList() << sourcePath+qslFileNames.at(i)
										<< qleDest->text()+QDir::separator()+QFileInfo(qslFileNames.at(i)).fileName());
		}
		queue->setBlocked(false);
		sourcePanel->clearSelection();
	}
	delete qdCopyMoveDialog;
}
//
void MainWindowImpl::dropEvent (QDropEvent* event)
{
	if (qpbRunConsole==this->childAt(event->pos()))
	{

	}
	if (qpbView==this->childAt(event->pos()))
	{
		slotView(event->mimeData()->urls().at(0).toLocalFile());
	}
	if (qpbEdit==this->childAt(event->pos()))
	{

	}
	if (qpbRemove==this->childAt(event->pos()))
	{
		QStringList list;
		foreach(QUrl url,event->mimeData()->urls())
			list << QDir::toNativeSeparators(url.toLocalFile());
		slotRemove(list);
	}
	//this->childAt(event->pos()) на QTreeView возвращяет QAbstractScrollArea, из
	// которой через parent получаем QTreeView
	QTreeView* view=qobject_cast<QTreeView*>(this->childAt(event->pos())->parent());
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
	}
	QToolBar* toolBar=0;
	for (int i=0; i<qltbToolBarList.count(); i++)
		if (qltbToolBarList.at(i)==this->childAt(event->pos()))
		{
			toolBar=qltbToolBarList.at(i);
			break;
		}
//ToolButton
	QToolButton* button=qobject_cast<QToolButton*>(this->childAt(event->pos()));
	if (button)
	{
		QAction* action=button->defaultAction();
		if (action)
		{
			QString key=action->data().toString();
			QString params=qmToolBurButtons.value(key).qsParams;
			if (!params.isEmpty())
				params+=" ";
			params+=QDir::toNativeSeparators(event->mimeData()->urls().at(0).toLocalFile());
			QProcess::startDetached(qmToolBurButtons.value(key).qsCommand,
									#ifndef Q_CC_MSVC
										#warning "Need parce params"
									#endif
									(params).split(" "),
									qmToolBurButtons.value(key).qsWorkDir);
		}
	}
//ToolBar
	if (toolBar)
	{
		SToolBarButton button=QToolButtonPreference::getButton(event->mimeData()->urls().at(0).toLocalFile());

		QAction* action=new QAction(button.qiIcon,button.qsCaption,this);
		button.qaAction=action;
		connect(action,
				SIGNAL(triggered()),
				this,
				SLOT(slotToolButtonPress()));
		action->setData(toolBar->objectName()+QString::number(toolBar->actions().count()));
		qmToolBurButtons.insert(toolBar->objectName()+QString::number(toolBar->actions().count()),button);
		toolBar->addAction(action);
	}
}
//
void MainWindowImpl::dragMoveEvent ( QDragMoveEvent * event )
{
	if (!(event->mouseButtons() & Qt::LeftButton))
	{
		this->dropEvent(event);
		event->ignore();
		return;
	}
	bool isAccepted=false;
//ToolBars
	for (int i=0; i<qltbToolBarList.count(); i++)
		if (qltbToolBarList.at(i)==this->childAt(event->pos()))
		{
			isAccepted=true;
			break;
		}
//ToolButtons
	QToolButton* button=qobject_cast<QToolButton*>(this->childAt(event->pos()));
	if (button)
		isAccepted=true;
//
//Command Buttons
	isAccepted=isAccepted || (qpbRunConsole==this->childAt(event->pos())) ||
				(qpbView==this->childAt(event->pos())) ||
				(qpbEdit==this->childAt(event->pos())) ||
				(qpbRemove==this->childAt(event->pos()));
//TreeView
	//this->childAt(event->pos()) на QTreeView возвращает QAbstractScrollArea, из
	// которой через parent получаем QTreeView
	QTreeView* view=qobject_cast<QTreeView*>(this->childAt(event->pos())->parent());
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
	}
//Drives toolbar

	if (isAccepted)
		event->accept();
	else
		event->ignore();
	//QWidget::dragMoveEvent(event);
}
//
void MainWindowImpl::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
		event->acceptProposedAction();
	//QWidget::dragEnterEvent(event);
}
//
void MainWindowImpl::slotToolBarContextMenu(const QPoint& pos)
{
	QToolBar* toolBar=qobject_cast<QToolBar*>(sender());
	if (!toolBar)
		return;
	QMenu* qmToolBarMenu=new QMenu(toolBar);
	qmToolBarMenu->setAttribute(Qt::WA_DeleteOnClose);
	QAction* action=toolBar->actionAt(pos);
	QAction* menuAction;
	if (action)
	{
		SToolBarButton button=qmToolBurButtons.value(action->data().toString());
		menuAction=new QAction(button.qsCaption,qmToolBarMenu);
		connect(menuAction,
				SIGNAL(triggered()),
				action,
				SLOT(trigger()));
		qmToolBarMenu->addAction(menuAction);
		qmToolBarMenu->addSeparator();

		menuAction=new QAction(tr("Change..."),qmToolBarMenu);
		menuAction->setData(action->data());
		connect(menuAction,
				SIGNAL(triggered()),
				this,
				SLOT(slotToolButtonChange()));
		qmToolBarMenu->addAction(menuAction);

		menuAction=new QAction(tr("Delete"),qmToolBarMenu);
		menuAction->setData(action->data());
		connect(menuAction,
				SIGNAL(triggered()),
				this,
				SLOT(slotToolButtonDelete()));
		connect(menuAction,
						SIGNAL(triggered()),
						action,
						SLOT(deleteLater()));
		qmToolBarMenu->addAction(menuAction);

		qmToolBarMenu->popup(this->mapToGlobal(pos));
	}
}
//
void MainWindowImpl::slotToolButtonChange()
{
	QAction* action=qobject_cast<QAction*>(sender());
	if (!action)
		return;
	SToolBarButton button=qmToolBurButtons.value(action->data().toString());
	QDialog* toolButtonChangeDialog=new QDialog(this);

	QToolButtonPreference* qtbpToolButtonPreference=new QToolButtonPreference(toolButtonChangeDialog);
	qtbpToolButtonPreference->setButton(button);

	QDialogButtonBox* qdbbButtonBox=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
									Qt::Horizontal,
									toolButtonChangeDialog);
	connect(qdbbButtonBox,
			SIGNAL(accepted()),
			toolButtonChangeDialog,
			SLOT(accept()));
	connect(qdbbButtonBox,
			SIGNAL(rejected()),
			toolButtonChangeDialog,
			SLOT(reject()));

	QVBoxLayout* layout=new QVBoxLayout();
	layout->addWidget(qtbpToolButtonPreference);
	layout->addWidget(qdbbButtonBox);
	toolButtonChangeDialog->setLayout(layout);

	if (toolButtonChangeDialog->exec())
	{
		button=qtbpToolButtonPreference->getButton();
		qmToolBurButtons.remove(action->data().toString());
		qmToolBurButtons.insert(action->data().toString(),button);
		button.qaAction->setText(button.qsCaption);
		button.qaAction->setIcon(button.qiIcon);
	}
	delete toolButtonChangeDialog;
}
//
void MainWindowImpl::slotToolButtonDelete()
{
	QAction* action=qobject_cast<QAction*>(sender());
	if (!action)
		return;
	qmToolBurButtons.remove(action->data().toString());

}
//
