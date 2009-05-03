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
#include "qfilelistview.h"
#ifdef Q_WS_WIN
	#include "qt_windows.h"
#endif
//
QFileListView::QFileListView(QWidget* parent,const QString& path)
		:QWidget(parent)
{
	qsPath=path;
	if (qsPath.isEmpty())
		qsPath=QApplication::applicationDirPath();
	createControls();
	setLayouts();
	setConnects();
	loadSettings();
	qleRenameEditor=0;
	slotUpdateDirInformation();
	dirInformation.dirsSelectedCount=0;
	dirInformation.filesSelectedCount=0;
	dirInformation.filesSelectedSize=0;
}
//
QFileListView::~QFileListView()
{
	saveSettings();
}
//
void QFileListView::createControls()
{
	flmModel=new QFileListModel(qsPath,this);
	qflsfpmProxyModel=new QFileListSortFilterProxyModel(this);
	qflsfpmProxyModel->setDynamicSortFilter(true);
	qflsfpmProxyModel->setSourceModel(flmModel);
	qtrvView=new QFullView(this);

	//Set the inactive pallete
	QPalette palette=qtrvView->palette();
	palette.setColor(QPalette::Active,
					QPalette::Highlight,
					qApp->palette().color(QPalette::Inactive,QPalette::Highlight));
	qtrvView->setPalette(palette);
	//
	qtrvView->setModel(qflsfpmProxyModel);
	qtrvView->setRootIsDecorated(false);
	qtrvView->setSortingEnabled(true);
	qtrvView->setAllColumnsShowFocus(true);
	qtrvView->setSelectionMode(QAbstractItemView::MultiSelection);
	//qtrvView->setEditTriggers(QAbstractItemView::SelectedClicked);
	//qtrvView->setDragEnabled(true);
	selectItem();
}
//
void QFileListView::setLayouts()
{
	QVBoxLayout* layout=new QVBoxLayout();
	layout->addWidget(qtrvView);
	layout->setMargin(0);
	this->setLayout(layout);
}
//
void QFileListView::setConnects()
{
	connect(qtrvView,
			SIGNAL(activated(const QModelIndex&)),
			this,
			SLOT(slotDoubleClick( const QModelIndex&)));
	connect(qtrvView->header(),
			SIGNAL(sectionResized(int,int,int)),
			this,
			SIGNAL(headerSectionResized(int,int,int)));
	connect(qtrvView,
			SIGNAL(focusIn()),
			this,
			SLOT(slotFocusIn()));
	connect(qtrvView,
			SIGNAL(focusIn()),
			this,
			SIGNAL(focusIn()));
	connect(qtrvView->selectionModel(),
			SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
			this,
			SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));
}
//
void QFileListView::slotDoubleClick( const QModelIndex& index)
{
	qtrvView->blockSignals(true);
	qtrvView->setCursor(Qt::WaitCursor);
	QString qsFileName = qflsfpmProxyModel->data(qflsfpmProxyModel->index(index.row(),0),Qt::EditRole).toString();
	if (qsFileName=="..")
	{
		this->cdUP();
		qtrvView->setCursor(Qt::ArrowCursor);
		qApp->processEvents();
		qtrvView->blockSignals(false);
		return;
	}
	QFileInfo qfiFileInfo(flmModel->path()+qsFileName);
	if (qfiFileInfo.isDir())
	{
		setPath(qfiFileInfo.filePath());
		qtrvView->setCursor(Qt::ArrowCursor);
		qApp->processEvents();
		qtrvView->blockSignals(false);
		return;
	}
#ifdef Q_WS_X11
	if (qfiFileInfo.isExecutable())
		QProcess::startDetached(qfiFileInfo.absoluteFilePath(),
								QStringList(),
								qfiFileInfo.absolutePath());
	else
		QDesktopServices::openUrl(QUrl::fromLocalFile(qfiFileInfo.absoluteFilePath()));
#endif
#ifdef Q_WS_WIN
	if (qfiFileInfo.suffix().toLower()=="bat")
	{
		QProcess::startDetached("cmd.exe",
								QStringList() << "/C" << QDir::toNativeSeparators(qfiFileInfo.absoluteFilePath()),
								qfiFileInfo.absolutePath());
	}
	else
	{
		HINSTANCE__* instance=ShellExecuteA(0,
						"Open",
						QDir::toNativeSeparators(qfiFileInfo.absoluteFilePath()).toLocal8Bit(),
						NULL,
						QDir::toNativeSeparators(qfiFileInfo.absolutePath()).toLocal8Bit(),
						SW_NORMAL);
		if (int(instance)<32)
		{
			if (int(instance)==SE_ERR_NOASSOC)
			{
				ShellExecuteA(0,
							"open",
							"rundll32.exe",
							"shell32.dll,OpenAs_RunDLL "+
							QDir::toNativeSeparators(qfiFileInfo.absoluteFilePath()).toLocal8Bit(),
							NULL,
							SW_SHOWNORMAL);
			}
			else
				QMessageBox::information(0, this->windowTitle(),tr("Program is not running"));
		}
	}
#endif
	qtrvView->setCursor(Qt::ArrowCursor);
	qtrvView->blockSignals(false);
}
//
QString QFileListView::path()
{
	return flmModel->path();
}
//
void QFileListView::gotoRoot()
{
#ifdef Q_WS_WIN
	setPath(QDir(flmModel->path().mid(0,3)).absolutePath());
#else
	setPath(QDir(flmModel->path()).rootPath());
#endif
}
//
void QFileListView::cdUP()
{
	QString qsOldDirName=QDir(flmModel->path()).dirName();
	qtrvView->setUpdatesEnabled(false);
	flmModel->cdUP();
	emit changedPath(flmModel->path());
	QDir::setCurrent(flmModel->path());
	qtrvView->setUpdatesEnabled(true);
	qtrvView->setFocus();
	selectItem(qsOldDirName);
	slotUpdateDirInformation();
}
//
void QFileListView::gotoHome()
{
	setPath(QDir::homePath());
}
//
void QFileListView::setPath(const QString& path)
{
	if (path==flmModel->path())
	{
		qtrvView->setUpdatesEnabled(false);
		flmModel->slotRefresh();
		qtrvView->setUpdatesEnabled(true);
	}
	QString lastPath=flmModel->path();
	QString lastName=qflsfpmProxyModel->data(qtrvView->currentIndex(),Qt::EditRole).toString();
	qtrvView->setUpdatesEnabled(false);
	flmModel->setPath(path);
	selectItem();
	switch(flmModel->getLastError())
	{
		case FLM_PRIVILEGY_ERROR:
			QMessageBox::critical(this,"",tr("You have not privilegy"));
			flmModel->setPath(lastPath);
			selectItem(lastName);
			break;
	}
	emit changedPath(flmModel->path());
	QDir::setCurrent(flmModel->path());
	qtrvView->setUpdatesEnabled(true);
	qtrvView->setFocus();
	qtLastRefresh=QTime::currentTime();
	slotUpdateDirInformation();
}
//
void QFileListView::loadSettings()
{
	QSettings* settings=new QSettings(this);
	for (int i=0; i<qtrvView->model()->columnCount()-1; i++)//Without last section
		qtrvView->setColumnWidth(i,
								settings->value("FileList/Column_"+QString::number(i),100).toInt());
	iRefreshInterval=settings->value("Global/RefreshInterval",1).toInt();
	selectOnlyFileName=settings->value("Global/SelectOnlyFileName",true).toBool();
	settings->sync();
	delete settings;
}
//
void QFileListView::saveSettings()
{
	QSettings* settings=new QSettings(this);
	for (int i=0; i<qtrvView->model()->columnCount()-1; i++)//Without last section
		settings->setValue("FileList/Column_"+QString::number(i),qtrvView->columnWidth(i));
	settings->sync();
	delete settings;
}
//
void QFileListView::headerSectionResize ( int logicalIndex, int newSize )
{
	if (logicalIndex!=qflsfpmProxyModel->columnCount()-1)//Without last section
		qtrvView->setColumnWidth(logicalIndex,newSize);
}
//
void QFileListView::sort(int column, Qt::SortOrder order)
{
	qtrvView->sortByColumn(column,order);
}
//
bool QFileListView::eventFilter(QObject *obj, QEvent *event)
{
	if (obj==qleRenameEditor)
	{
		if (event->type()==QEvent::FocusOut)
		{
			qleRenameEditor->close();
		}
		if (event->type()==QEvent::KeyRelease)
		{
			QKeyEvent* keyEvent=static_cast<QKeyEvent*>(event);
			bool b=keyEvent &&
				 (keyEvent->key()==Qt::Key_Escape);
			if (b)
				qleRenameEditor->close();
		}
	}
	return QObject::eventFilter(obj, event);
}
//
void QFileListView::setFocus()
{
	qtrvView->setFocus();
}
//
QString QFileListView::currentFileName()
{
	return qflsfpmProxyModel->data(qflsfpmProxyModel->index(qtrvView->currentIndex().row(),0),Qt::EditRole).toString();
}
//
void QFileListView::selectItem(const QString& name)
{
	qtrvView->selectionModel()->setCurrentIndex(qflsfpmProxyModel->index(0,0),
												QItemSelectionModel::NoUpdate);
	if (!name.isEmpty())
		for (int i=0; i<flmModel->rowCount(); i++)//Select dir
		{
#ifndef Q_CC_MSVC
	#warning "this is only windows, because not registers of symbols"
#endif
			if (QString::compare(flmModel->data(qflsfpmProxyModel->mapToSource(qflsfpmProxyModel->index(i,0)),Qt::EditRole).toString(),name,Qt::CaseInsensitive)==0)
			{
				qtrvView->selectionModel()->setCurrentIndex(qflsfpmProxyModel->index(i,0),
															QItemSelectionModel::NoUpdate);
				break;
			}
		}
}
//
void QFileListView::selectItem(int row)
{
	if (row<0)
		row=0;
	if (row>=qflsfpmProxyModel->rowCount())
		row=qflsfpmProxyModel->rowCount()-1;
	qtrvView->selectionModel()->setCurrentIndex(qflsfpmProxyModel->index(row,0),
															QItemSelectionModel::NoUpdate);
}
//
void QFileListView::slotRefresh()
{
//	if (qtLastRefresh.secsTo(QTime::currentTime())<iRefreshInterval)
//		return;
	int selectedRow=qtrvView->currentIndex().row();
	qtrvView->setUpdatesEnabled(false);
	flmModel->slotRefresh();
	qtrvView->setUpdatesEnabled(true);
	selectItem(selectedRow);
//	qtLastRefresh=QTime::currentTime();
	slotUpdateDirInformation();
}
//
void QFileListView::slotRename()
{
	if (currentFileName()=="..")
		return;
	if (!qleRenameEditor)
	{
		qleRenameEditor=new QLineEdit(currentFileName(),this);
		qleRenameEditor->setAttribute(Qt::WA_DeleteOnClose);
		connect(qleRenameEditor,
				SIGNAL(destroyed()),
				this,
				SLOT(slotCloseEditor()));
		connect(qleRenameEditor,
				SIGNAL(returnPressed()),
				this,
				SLOT(slotRenameFile()));
		qleRenameEditor->installEventFilter(this);
		QModelIndex index=qflsfpmProxyModel->index(qtrvView->currentIndex().row(),0);
		QPoint rect=(QPoint(qtrvView->visualRect(index).left()+
							qtrvView->iconSize().width(),
							qtrvView->visualRect(index).top()+
							qtrvView->header()->height()));
		qleRenameEditor->move(rect);
		qleRenameEditor->resize(qtrvView->header()->sectionSize(0)+
								qtrvView->header()->sectionSize(1),
								qleRenameEditor->fontMetrics().height()+6);
		QPalette palette=qleRenameEditor->palette();
		palette.setColor(QPalette::Active,
						QPalette::Highlight,
						qApp->palette().color(QPalette::Active,QPalette::Highlight));
		qleRenameEditor->setPalette(palette);
		qleRenameEditor->show();
		qleRenameEditor->setFocus();
	}
	QString qsFileName=qleRenameEditor->text();
	int index=qsFileName.lastIndexOf(".");
	if (qleRenameEditor->selectedText()==qsFileName.left(index) || qleRenameEditor->selectedText().isEmpty() || !selectOnlyFileName)
		qleRenameEditor->selectAll();
	else
		qleRenameEditor->setSelection(0,index);
}
//
void QFileListView::slotCloseEditor()
{
	qleRenameEditor=0;
	qtrvView->setFocus();
}
//
QStringList QFileListView::selectedFileNames()
{
	QList<QModelIndex> indexList=qtrvView->selectionModel()->selectedRows(0);
	QStringList fileNamesList;
	for (int i=0; i<indexList.count(); i++)
	{
		fileNamesList << qflsfpmProxyModel->data(qflsfpmProxyModel->index(indexList.at(i).row(),0),Qt::EditRole).toString();
	}
	if (fileNamesList.isEmpty())
		fileNamesList << qflsfpmProxyModel->data(qflsfpmProxyModel->index(qtrvView->currentIndex().row(),0),Qt::EditRole).toString();
	return fileNamesList;
}
//
void QFileListView::clearSelection()
{
	QString curFile=currentFileName();
	qtrvView->clearSelection();
	selectItem(curFile);
}
//
void QFileListView::slotFocusIn()
{
	QDir::setCurrent(flmModel->path());
}
//
void QFileListView::slotUpdateDirInformation()
{
	dirInformation.dirsCount=flmModel->getDirsCount();
	dirInformation.filesCount=flmModel->getFilesCount();
	dirInformation.filesSize=flmModel->getFilesSize();
	if (qtrvView->selectionModel()->selectedIndexes().isEmpty())
	{
		dirInformation.dirsSelectedCount=0;
		dirInformation.filesSelectedCount=0;
		dirInformation.filesSelectedSize=0;
	}
	emit dirInformationChanged();
}
//
void QFileListView::slotSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	QList<QModelIndex> indexList=selected.indexes();
	for (int i=0; i<indexList.count(); i++)
	{
		if (indexList.at(i).column()!=0)
			continue;
		if (flmModel->isDir(qflsfpmProxyModel->mapToSource(indexList.at(i))))
			dirInformation.dirsSelectedCount++;
		else
			dirInformation.filesSelectedCount++;
		dirInformation.filesSelectedSize+=qint64(qflsfpmProxyModel->index(indexList.at(i).row(),2) .data(Qt::UserRole).toDouble());
	}
	indexList=deselected.indexes();
	for (int i=0; i<indexList.count(); i++)
	{
		if (indexList.at(i).column()!=0)
			continue;
		if (flmModel->isDir(qflsfpmProxyModel->mapToSource(indexList.at(i))))
			dirInformation.dirsSelectedCount--;
		else
			dirInformation.filesSelectedCount--;
		dirInformation.filesSelectedSize-=qint64(qflsfpmProxyModel->index(indexList.at(i).row(),2) .data(Qt::UserRole).toDouble());
	}
	emit dirInformationChanged();
}
//
