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
#include "qfilepanel.h"
#ifdef Q_WS_WIN
	#include <windows.h>
#endif
#define TIMER_INTERVAL 1000
//
QFilePanel::QFilePanel(const QString& name,QWidget* parent)
		:QWidget(parent)
{
	panelName=name;
	createControls();
	createActions();
	setLayouts();
	setConnects();
	loadSettings();
	timerID=this->startTimer(TIMER_INTERVAL);
}
QFilePanel::~QFilePanel()
{
	saveSettings();
	for (int i=0; i<qlflwFileLists.count(); i++)
		delete qlflwFileLists.at(i);
}
//
void QFilePanel::createControls()
{
	this->setContextMenuPolicy(Qt::PreventContextMenu);
	qlePath=new QLineEdit(this);
	qlePath->setEnabled(false);

	qpbGotoRoot=new QPushButton("/",this);
	qpbGotoRoot->setFlat(true);
	qpbGotoRoot->setFocusPolicy(Qt::NoFocus);
	qpbGotoRoot->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	qpbGotoRoot->setMaximumSize(qpbGotoRoot->fontMetrics().width("WW"),
								qpbGotoRoot->height());

	qpbGotoUp=new QPushButton("..",this);
	qpbGotoUp->setFlat(true);
	qpbGotoUp->setFocusPolicy(Qt::NoFocus);
	qpbGotoUp->setMaximumSize(qpbGotoUp->fontMetrics().width("WW"),
								qpbGotoUp->height());

	qpbGotoHome=new QPushButton("~",this);
	qpbGotoHome->setFlat(true);
	qpbGotoHome->setFocusPolicy(Qt::NoFocus);
	qpbGotoHome->setMaximumSize(qpbGotoHome->fontMetrics().width("WW"),
								qpbGotoHome->height());

	qtabwTabs=new QTabWidget(this);
	qtabwTabs->setContextMenuPolicy(Qt::ActionsContextMenu);
	qtabwTabs->setTabsClosable(true);
	qtabwTabs->setFocusPolicy(Qt::NoFocus);

	qlDirInformation=new QLabel(this);

	qlDiscInformation=new QLabel(this);

	qtbDrives=new QToolBar(this);
	qtbDrives->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	//qtbDrives->setFloatable(false);
	qtbDrives->setMovable(false);
	qtbDrives->setFocusPolicy(Qt::NoFocus);
	qtbDrives->setIconSize(QSize(16,16));
	qtbDrives->setContextMenuPolicy(Qt::CustomContextMenu);

	qagDrives=new QActionGroup(this);

	qfswFileSystemWatcher=new QFileSystemWatcher(this);
}
//
void QFilePanel::setLayouts()
{
	QHBoxLayout* qhblEditLayout=new QHBoxLayout();
	qhblEditLayout->setSpacing(0);
	qhblEditLayout->addWidget(qlePath);
	qhblEditLayout->addWidget(qpbGotoRoot);
	QFrame* splitter=new QFrame(this);
	splitter->setFrameShape(QFrame::VLine);
	splitter->setFrameShadow(QFrame::Sunken);
	qhblEditLayout->addWidget(splitter);
	qhblEditLayout->addWidget(qpbGotoUp);
	splitter=new QFrame(this);
	splitter->setFrameShape(QFrame::VLine);
	splitter->setFrameShadow(QFrame::Sunken);
	qhblEditLayout->addWidget(splitter);
	qhblEditLayout->addWidget(qpbGotoHome);

	QVBoxLayout* layout=new QVBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(qtbDrives);
	layout->addLayout(qhblEditLayout);
	layout->addWidget(qlDiscInformation);
	layout->addWidget(qtabwTabs);
	layout->addWidget(qlDirInformation);
	this->setLayout(layout);
}
//
void QFilePanel::setConnects()
{
	connect(qtabwTabs,
				SIGNAL(tabCloseRequested(int)),
				this,
				SLOT(slotCloseTab(int)));
	connect(qtabwTabs,
				SIGNAL(currentChanged(int)),
				this,
				SLOT(slotCurrentTabChange(int)));
	connect(qtbDrives,
		SIGNAL(customContextMenuRequested(QPoint)),
		this,
		SLOT(slotDriveToolbarMenu(QPoint)));
	connect(this,
		SIGNAL(changedIgnoringDrive()),
		this,
		SLOT(slotRereadDriveToolBar()));

	connect(actionAddTab,
			SIGNAL(triggered()),
			this,
			SLOT(slotAddTab()));

	connect(qpbGotoRoot,
			SIGNAL(clicked()),
			this,
			SLOT(slotGotoRoot()));
	connect(qpbGotoUp,
			SIGNAL(clicked()),
			this,
			SLOT(slotGotoUp()));
	connect(qpbGotoHome,
			SIGNAL(clicked()),
			this,
			SLOT(slotGotoHome()));

	connect(actionAddDriveToIgnorList,
			SIGNAL(triggered()),
			this,
			SLOT(slotAddDriveToIgnorList()));

	connect(qfswFileSystemWatcher,
			SIGNAL(directoryChanged(QString)),
			this,
			SLOT(slotRefresh()));
}
//
void QFilePanel::createActions()
{
	actionAddTab=new QAction(tr("Add copy of this tab"),this);
	qtabwTabs->addAction(actionAddTab);

	actionAddDriveToIgnorList=new QAction(tr("Ignore drive"),this);
	actionAddDriveToIgnorList->setToolTip(tr("Add drive to ignor list"));
}
//
void QFilePanel::slotChangedPath(const QString& path)
{
	qlePath->setText(path);
	updateDirInformation();
	updateDiscInformation();
	selectCurrentDrive();
	if (!qfswFileSystemWatcher->directories().isEmpty())
		qfswFileSystemWatcher->removePaths(qfswFileSystemWatcher->directories());
	qfswFileSystemWatcher->addPath(path);
#ifdef Q_WS_WIN
	qtabwTabs->setTabText(qtabwTabs->currentIndex(),
						QDir(path).absolutePath().mid(0,2)+QDir(path).dirName());
#else
	if (!QDir(path).isRoot())
		qtabwTabs->setTabText(qtabwTabs->currentIndex(),QDir(path).dirName());
	else
		qtabwTabs->setTabText(qtabwTabs->currentIndex(),"/");
#endif
}
//
void QFilePanel::addTab(const QString& path)
{
	QString qsPath=path;
	int sortColumn=0;
	Qt::SortOrder sortOrder=Qt::AscendingOrder;
	QFileListView* fileList;
	if (qsPath.isEmpty())
	{
		if (fileList=qobject_cast<QFileListView*> (qtabwTabs->widget(qtabwTabs->currentIndex())))
		{
			qsPath=fileList->path();
			sortColumn=fileList->sortColumn();
			sortOrder=fileList->sortOrder();
		}
		else
			qsPath=QApplication::applicationDirPath();
	}
	fileList=new QFileListView(this,qsPath);
	connect(fileList,
			SIGNAL(dirInformationChanged()),
			this,
			SLOT(slotInformationChanged()));
	fileList->setContextMenuPolicy(Qt::PreventContextMenu);
	fileList->sort(sortColumn,sortOrder);
	connect(fileList,
			SIGNAL(changedPath(const QString&)),
			this,
			SLOT(slotChangedPath(QString)));
	connect(fileList,
			SIGNAL(changedPath(const QString&)),
			this,
			SIGNAL(changedPath(QString)));
	connect(fileList,
			SIGNAL(headerSectionResized(int,int,int)),
			this,
			SLOT(slotFileListHeaderSectionResize(int,int,int)));
	connect(fileList,
			SIGNAL(headerSectionResized(int,int,int)),
			this,
			SIGNAL(headerSectionResized(int,int,int)));
	connect(fileList,
			SIGNAL(focusIn()),
			this,
			SIGNAL(focusIn()));
	connect(fileList,
			SIGNAL(focusOut()),
			this,
			SIGNAL(focusOut()));
	qtabwTabs->addTab(fileList,"");
	qtabwTabs->setCurrentWidget(fileList);
	slotChangedPath(fileList->path());
	qlflwFileLists << fileList;
}
//
void QFilePanel::slotCloseTab(int index)
{
	if (qtabwTabs->count()>1)
		qtabwTabs->removeTab(index);
}
//
void QFilePanel::slotGotoRoot()
{
	qflvCurrentFileList->gotoRoot();
}
//
void QFilePanel::slotGotoUp()
{
	qflvCurrentFileList->cdUP();
}
//
void QFilePanel::slotGotoHome()
{
	qflvCurrentFileList->gotoHome();
}
//
void QFilePanel::slotCurrentTabChange(int index)
{
	qflvCurrentFileList=qobject_cast<QFileListView*> (qtabwTabs->widget(index));
	if (qflvCurrentFileList)
	{
		qlePath->setText(qflvCurrentFileList->path());
		updateDirInformation();
		updateDiscInformation();
		selectCurrentDrive();
		if (!qfswFileSystemWatcher->directories().isEmpty())
			qfswFileSystemWatcher->removePaths(qfswFileSystemWatcher->directories());
		qfswFileSystemWatcher->addPath(qflvCurrentFileList->path());
		qflvCurrentFileList->setFocus();
	}
}
//
void QFilePanel::setDisc(const QString& name)
{
	qflvCurrentFileList->setPath(name);
}
//
void QFilePanel::slotFileListHeaderSectionResize(int index, int /*oldSize*/, int newSize)
{
	QFileListView* fileList;
	for (int i=0; i<qtabwTabs->count(); i++)
	{
		fileList=qobject_cast<QFileListView*> (qtabwTabs->widget(i));
		if (fileList && fileList!=qobject_cast<QFileListView*>(sender()))
			fileList->headerSectionResize(index,newSize);
	}
}
//
void QFilePanel::loadSettings()
{
	QSettings* settings=new QSettings(this);
	int count=settings->value(panelName+"/TabsCount",1).toInt();
	for (int i=0; i<count; i++)
	{
		addTab(settings->value(panelName+"/Path_"+QString::number(i),"").toString());
		QFileListView* fileList=qobject_cast<QFileListView*> (qtabwTabs->widget(i));
		if (settings->value(panelName+"/SortOrder_"+QString::number(i),0).toInt()==0)
			fileList->sort(settings->value(panelName+"/SortColumn_"+QString::number(i),0).toInt(),
				Qt::AscendingOrder);
		else
			fileList->sort(settings->value(panelName+"/SortColumn_"+QString::number(i),0).toInt(),
				Qt::DescendingOrder);
	}
	qtabwTabs->setCurrentIndex(settings->value(panelName+"/CurrentTab",0).toInt());
	qflvCurrentFileList=qobject_cast<QFileListView*>(qtabwTabs->currentWidget());
	delete settings;
}
//
void QFilePanel::saveSettings()
{
	QSettings* settings=new QSettings(this);
	settings->remove(panelName);
	settings->setValue(panelName+"/TabsCount",qtabwTabs->count());
	settings->setValue(panelName+"/CurrentTab",qtabwTabs->currentIndex());
	settings->sync();
	QFileListView* fileList;
	for (int i=0; i<qtabwTabs->count(); i++)
	{
		fileList=qobject_cast<QFileListView*> (qtabwTabs->widget(i));
		if (fileList)
		{
			settings->setValue(panelName+"/Path_"+QString::number(i),fileList->path());
			settings->setValue(panelName+"/SortColumn_"+QString::number(i),fileList->sortColumn());
			if (fileList->sortOrder()==Qt::AscendingOrder)
				settings->setValue(panelName+"/SortOrder_"+QString::number(i),0);
			else
				settings->setValue(panelName+"/SortOrder_"+QString::number(i),1);
		}
	}
	settings->sync();
	delete settings;
}
//
void QFilePanel::slotSetDisc()
{
	this->killTimer(timerID);
	QAction* action=qobject_cast<QAction*>(sender());
	if (!action)
		return;
#ifdef Q_WS_WIN
	if (SetCurrentDirectoryA(action->data().toString().toLocal8Bit()))
		setDisc(action->data().toString());
	else
	{
		selectCurrentDrive();
		QMessageBox::critical(this,"",tr("Drive is not ready!!!"));
	}
#endif

#ifdef Q_WS_X11
	setDisc(action->data().toString());
#endif
	timerID=this->startTimer(TIMER_INTERVAL);
}
//
void QFilePanel::setFocus()
{
	qflvCurrentFileList->setFocus();
}
//
void QFilePanel::createDrivesToolbar()
{
	this->blockSignals(true);
	QSettings* settings=new QSettings(this);
	QStringList qslIgnoreList=settings->value("Global/IgnoredDrives",QStringList()).toStringList();
	delete settings;
#ifdef Q_WS_WIN
	QList<QFileInfo> fileInfoList=QDir::drives();
	for (int i=0; i<fileInfoList.count(); i++)
	{
		if (qslIgnoreList.contains(fileInfoList.at(i).absolutePath()))
			continue;
		QString name=fileInfoList.at(i).absolutePath();
		name=name.remove(name.length()-2,2);
		if (qtbDrives->actions().count()>i && qtbDrives->actions().at(i)->text()==name)
			continue;
		QAction* action=new QAction(name,this);
		if (qmDrivesIcons.contains(fileInfoList.at(i).absolutePath()))
			action->setIcon(qmDrivesIcons.value(fileInfoList.at(i).absolutePath()));
		else
		{
			action->setIcon(qfipIconProvider.icon(fileInfoList.at(i)));
			qmDrivesIcons.insert(fileInfoList.at(i).absolutePath(),qfipIconProvider.icon(fileInfoList.at(i)));
		}
		action->setData(fileInfoList.at(i).absolutePath());
		action->setCheckable(true);
		connect(action,
				SIGNAL(triggered()),
				this,
				SLOT(slotSetDisc()));
		if (qtbDrives->actions().count()>i )
			qtbDrives->insertAction(qtbDrives->actions().at(i),action);
		else
			qtbDrives->addAction(action);
		qagDrives->addAction(action);
	}
	while(qtbDrives->actions().count()!=fileInfoList.count())
	{
		delete qtbDrives->actions().at(fileInfoList.count());
	}
	if (!qagDrives->checkedAction())
		selectCurrentDrive();
//		qflvCurrentFileList->slotRefresh();
#endif
#ifdef Q_WS_X11
	QFile file;
	file.setFileName("/etc/mtab");
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);
		QString buffer;
		int i=-1;
		while(!stream.atEnd())
		{
			buffer=stream.readLine();
			QFileInfo fileInfo(buffer.split(" ").at(1));
			if (!fileInfo.isDir() || fileInfo.isRoot() || qslIgnoreList.contains(fileInfo.absoluteFilePath()))
				continue;
			i++;
			if (qtbDrives->actions().count()>i && qtbDrives->actions().at(i)->text()==fileInfo.fileName())
				continue;
			QAction* action=new QAction(fileInfo.fileName(),this);
			action->setCheckable(true);
			action->setToolTip(fileInfo.absoluteFilePath());
			action->setIcon(qfipIconProvider.icon(QFileIconProvider::Drive));
			action->setData(fileInfo.absoluteFilePath());
			connect(action,
					SIGNAL(triggered()),
					this,
					SLOT(slotSetDisc()));
			qtbDrives->addAction(action);
			qagDrives->addAction(action);
		}
		file.close();
		while(qtbDrives->actions().count()>(i+1))
		{
			delete qtbDrives->actions().at(i);
		}
	}
#endif
	this->blockSignals(false);
}
//
void QFilePanel::slotDriveToolbarMenu(const QPoint & pos)
{
	qmToolBarMenu=new QMenu(qtbDrives);
	qmToolBarMenu->setAttribute(Qt::WA_DeleteOnClose);
	QAction* action=qtbDrives->actionAt(pos);
	if (action)
	{
		qmToolBarMenu->addAction(action->data().toString());
		actionAddDriveToIgnorList->setData(action->data());
		qmToolBarMenu->addAction(actionAddDriveToIgnorList);

		qmToolBarMenu->addSeparator();

		qmToolBarMenu->popup(this->mapToGlobal(pos));
	}
}
//
void QFilePanel::slotAddDriveToIgnorList()
{
	QAction* action=qobject_cast<QAction*>(sender());
	if (!action)
		return;
	QSettings* settings=new QSettings(this);
	QStringList list=settings->value("Global/IgnoredDrives",QStringList()).toStringList();
	list << action->data().toString();
	settings->setValue("Global/IgnoredDrives",list);
	list.clear();
	settings->sync();
	delete settings;
	emit changedIgnoringDrive();//Reread toolbar
}
//
QString QFilePanel::path()
{
	return qflvCurrentFileList->path();
}
//
QString QFilePanel::currentFileName()
{
	return qflvCurrentFileList->currentFileName();
}
//
void QFilePanel::setPath(const QString& path)
{
	qflvCurrentFileList->setPath(path);
}
//
void QFilePanel::selectCurrentDrive()
{
#ifdef Q_WS_WIN
		QList<QAction*> actionList=qagDrives->actions();
		for (int i=0; i<actionList.count(); i++)
		{
			if (qflvCurrentFileList->path().toUpper().contains(QDir::toNativeSeparators(actionList.at(i)->data().toString()).toUpper()))
			{
				actionList.at(i)->setChecked(true);
				break;
			}
		}
#endif
}
//
void QFilePanel::slotRefresh()
{
	qfswFileSystemWatcher->blockSignals(true);
	qflvCurrentFileList->slotRefresh();
	qApp->processEvents();
	qfswFileSystemWatcher->blockSignals(false);
}
//
QStringList QFilePanel::selectedFileNames()
{
	return qflvCurrentFileList->selectedFileNames();
}
//
void QFilePanel::updateDirInformation()
{
	QString information=tr("Selected <b>%1</b> from <b>%2</b> in <b>%3</b>/<b>%4</b> files and <b>%5</b>/<b>%6</b> dirs");
	information=information.arg(getSizeStr(qflvCurrentFileList->filesSelectedSize())).arg(getSizeStr(qflvCurrentFileList->filesSize())).arg(qflvCurrentFileList->filesSelectedCount()).arg(qflvCurrentFileList->filesCount()).arg(qflvCurrentFileList->dirsSelectedCount()).arg(qflvCurrentFileList->dirsCount());
	qlDirInformation->setText(information);
}
//
void QFilePanel::slotInformationChanged()
{
	if (sender()==qflvCurrentFileList)
		updateDirInformation();
	updateDiscInformation();
}
//
QString QFilePanel::getSizeStr(double size) const
{
	if (size<1024.0) return QString(tr("%1 b ")).arg(size);
	if ((size>=1024.0) && (size<1048576.0)) return QString(tr("%1 Kb")).arg(size/1024.0,0,'f',2);
	if ((size>=1048576.0) && (size<1073741824.0))
		return QString(tr("%1 Mb")).arg(size/1048576.0,0,'f',2);
	if (size>=1073741824.0) return QString(tr("%1 Gb")).arg(size/1073741824.0,0,'f',2);
	return "";
}
//
void QFilePanel::updateDiscInformation()
{
	QString information="[<i>%1</i>] <b>%2</b> of <b>%3</b> free";
	qint64 availableBytes,totalBytes,freeBytes;
	if (!QFileOperationsThread::getDiskSpace(qflvCurrentFileList->path(),
						availableBytes,
						totalBytes,
						freeBytes))
		return;
	QString label=QFileOperationsThread::getDiskLabel(qflvCurrentFileList->path());
	if (label.isEmpty())
		label="_NO_LABEL_";
	information=information.arg(label).arg(getSizeStr(availableBytes)).arg(getSizeStr(totalBytes));
	qlDiscInformation->setText(information);
}
//
void QFilePanel::timerEvent(QTimerEvent */*event*/)
{
	updateDiscInformation();
	createDrivesToolbar();
}
//
void QFilePanel::showEvent(QShowEvent */* event*/)
{
	createDrivesToolbar();
}
//
