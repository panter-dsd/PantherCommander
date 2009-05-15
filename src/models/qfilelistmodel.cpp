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

#include "qfilelistmodel.h"

#include <QtGui>
#include <QDebug>

#include "dirsorter.h"
//
QFileListModel::QFileListModel(QObject *parent)
		:QAbstractItemModel(parent)
{
	provider=new QFileIconProvider();
	qiFolderIcon=provider->icon(QFileIconProvider::Folder);
	qiFileIcon=provider->icon(QFileIconProvider::File);
//	connect(&fileSystemWatcher,
//			SIGNAL(directoryChanged ( const QString &)),
//			this,
//			SLOT(slotRefresh()));
}
//
QFileListModel::~QFileListModel()
{
	future.cancel();
	future.waitForFinished();
}
//
void QFileListModel::getFileList()
{
	future.cancel();
	future.waitForFinished();
	qdCurrentDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	qdCurrentDir.setSorting(QDir::Unsorted);
//Get file list
	QFileInfoList infos;
	future = QtConcurrent::run(getInfoList, &infos, qdCurrentDir);
	while(future.isRunning())
		qApp->processEvents();
//Sort file list
	QFuture<QFileInfoList> sortFuture = QtConcurrent::run(Dir::sortFileList,infos, QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
	while(sortFuture.isRunning())
		qApp->processEvents();
	infos = sortFuture.result();

	infoList.clear();
	beginInsertRows(createIndex(0,0),0,infos.count());
	foreach(const QFileInfo& info, infos)
		infoList.append(QPCFileInfo(info));
	endInsertRows();
	future = QtConcurrent::run(getIcons, &infoList, provider);
}
//
QModelIndex QFileListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column>columnCount() || row>infoList.count() || infoList.isEmpty())
		return QModelIndex();
	return createIndex(row, column);
}
//
QModelIndex QFileListModel::parent(const QModelIndex& child) const
{
	if (!child.isValid() || (child.row()==0 && child.column()==0))
		return QModelIndex();
	return createIndex(0,0);
}
//
bool QFileListModel::hasChildren(const QModelIndex &parent) const
{
	 if (parent.column() > 0)
		return false;
	return infoList.at(parent.row()).isDir() || (parent.row()==0);
}
//
Qt::ItemFlags QFileListModel::flags ( const QModelIndex & index ) const
{
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);
	if(index.isValid())
	{
		if(isDir(index))
			flags |= Qt::ItemIsDropEnabled;
		if(index.column() == 0)
			flags |= Qt::ItemIsEditable;
	}
	return flags;
}
//
QVariant QFileListModel::data ( const QModelIndex & index, int role) const
{
	if (!index.isValid() || index.row()>infoList.count() || infoList.isEmpty())
		return QVariant();
	if (role==Qt::DisplayRole)
	{
		int iTmp;
		switch (index.column())
		{
			case NAME://Name
				if (infoList.at(index.row())==QFileInfo(qdCurrentDir.absolutePath()))
					return "..";
				iTmp=infoList.at(index.row()).fileName().lastIndexOf(".");
				if (iTmp==0 || (infoList.at(index.row()).isDir()))
					return infoList.at(index.row()).fileName();
				if (iTmp!=-1 && iTmp!=infoList.at(index.row()).fileName().length()-1)
					return infoList.at(index.row()).fileName().left(iTmp);
				else
					return infoList.at(index.row()).fileName();
				break;
			case EXT://Ext
				iTmp=infoList.at(index.row()).fileName().lastIndexOf(".");
				if (iTmp==0 || (infoList.at(index.row()).isDir()))
					return "";
				if (iTmp!=-1 && iTmp!=infoList.at(index.row()).fileName().length()-1)
					return infoList.at(index.row()).fileName().mid(iTmp+1);
				else
					return "";
				break;
			case SIZE://size
				if (!infoList.at(index.row()).isDir())
					return getSizeStr(infoList.at(index.row()).size());
				else
					return "-=DIR=-";
				break;
			case TIME_LAST_UPDATE://time
				return infoList.at(index.row()).lastModified().toString("dd.MM.yy hh:mm:ss");
			case ATTR://attr
				#ifdef Q_WS_WIN
					QString attr;
/*					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_READONLY)
						attr+="r";
					else
						attr+="-";
					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_ARCHIVE)
						attr+="a";
					else
						attr+="-";
					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_HIDDEN)
						attr+="h";
					else
						attr+="-";
					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_SYSTEM)
						attr+="s";
					else
						attr+="-";
					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_COMPRESSED)
						attr+="c";
					else
						attr+="-";*/
					return attr;
				#endif
				break;
		}
	}
	if (role==Qt::DecorationRole && index.column()==NAME)
	{
		if (!infoList.at(index.row()).icon.isNull())
			return infoList.at(index.row()).icon;
		else
			return (infoList.at(index.row()).isDir()) ? qiFolderIcon : qiFileIcon;
	}
	if (role==Qt::UserRole )
	{
		switch (index.column())
		{
			case SIZE:
				return infoList.at(index.row()).size();
				break;
		}
	}
	if (role==Qt::EditRole && index.column()==NAME)
	{
		if (infoList.at(index.row())==QFileInfo(qdCurrentDir.absolutePath()))
					return "..";
		return infoList.at(index.row()).fileName();
	}
	if (role==Qt::TextAlignmentRole && index.column()==SIZE)
	{
		if (!isDir(index))
			return Qt::AlignRight;
		else
			return Qt::AlignHCenter;
	}
	if (role==Qt::ToolTipRole)
	{
		switch(index.column())
		{
			case NAME:return infoList.at(index.row()).fileName(); break;
			case EXT:return infoList.at(index.row()).fileName(); break;
			case SIZE: return infoList.at(index.row()).size(); break;
			case TIME_LAST_UPDATE: return infoList.at(index.row()).lastModified().toString("dd.MM.yyyy hh:mm:ss"); break;
			default: return QVariant();
		}
		;
	}
	if (role==Qt::ForegroundRole)
	{
		if (infoList.at(index.row()).lastModified().date().daysTo(QDate::currentDate())<=3)
			return Qt::blue;
	}
	return QVariant();
}
//
bool QFileListModel::setData(const QModelIndex &/*index*/, const QVariant &/*value*/,
						 int /*role*/)
 {
/* if (role != Qt::EditRole)
		 return false;

	 TreeItem *item = getItem(index);
	 bool result = item->setData(index.column(), value);

	 if (result)
		 emit dataChanged(index, index);

	 return result;*/
	return true;
 }
//
QString QFileListModel::getSizeStr(double size) const
{
	if (size<1024.0) return QString(tr("%1 b ")).arg(size);
	if ((size>=1024.0) && (size<1048576.0)) return QString(tr("%1 Kb")).arg(size/1024.0,0,'f',2);
	if ((size>=1048576.0) && (size<1073741824.0))
		return QString(tr("%1 Mb")).arg(size/1048576.0,0,'f',2);
	if (size>=1073741824.0) return QString(tr("%1 Gb")).arg(size/1073741824.0,0,'f',2);
	return "";
}
//
QVariant QFileListModel::headerData ( int section,
									Qt::Orientation orientation,
									int role) const
{
	if (role == Qt::DisplayRole && orientation==Qt::Horizontal)
		switch (section)
		{
			case 0: return tr("Name"); break;
			case 1: return tr("Type"); break;
			case 2: return tr("Size"); break;
			case 3: return tr("Date"); break;
			case 4: return tr("Attr"); break;
		}
	return QVariant();
}
//
bool QFileListModel::isDir(const QModelIndex& index) const
{
	return index.isValid() ?(infoList.at(index.row()).isDir()) : false;
}
//
void QFileListModel::slotRefresh()
{
	fileSystemWatcher.blockSignals (true);

	future.cancel();
	future.waitForFinished();
	qdCurrentDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	qdCurrentDir.setSorting(QDir::Unsorted);
//Get file list
	QFileInfoList infos;
	future = QtConcurrent::run(getInfoList, &infos, qdCurrentDir);
	while(future.isRunning())
		qApp->processEvents();
//Sort file list
	QFuture<QFileInfoList> sortFuture = QtConcurrent::run(Dir::sortFileList,infos, QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
	while(sortFuture.isRunning())
		qApp->processEvents();
	infos = sortFuture.result();

	for (int i=0; i<infos.count(); i++)
	{
		if (infoList.count()<=i)
		{
			beginInsertRows(QModelIndex(),infoList.count(),infoList.count());
			infoList.append(QPCFileInfo(infos.at(i)));
			endInsertRows();
			continue;
		}
		if (infoList.at(i).fileInfo()!=infos.at(i))
		{
			infoList[i]=QPCFileInfo(infos.at(i));
			//emit dataChanged(createIndex(i,0),createIndex(i,columnCount-1));
		}
	}
	while (infoList.count()>infos.count())
	{
		//emit rowsAboutToBeRemoved(createIndex(-1,0),infoList.count()-1,infoList.count()-1);
		infoList.removeLast();
	}
	future = QtConcurrent::run(getIcons, &infoList, provider);
	fileSystemWatcher.blockSignals (false);
}
//
void QFileListModel::getIcons(QList<QPCFileInfo>* info,QFileIconProvider* prov)
{
	for (int i=0; i<info->count();i++)
	{
		if (!info->at(i).icon.isNull())
			continue;
		(*info)[i].icon=prov->icon(info->at(i).fileInfo());
	}
}
//
qint64 QFileListModel::size(const QModelIndex& index) const
{
	return index.isValid() ? infoList.at(index.row()).size() : 0;
}
//
QFile::Permissions QFileListModel::permissions(const QModelIndex& index) const
{
	if (!index.isValid())
		return 0;
	return infoList.at(index.row()).permissions();
}
//
QString QFileListModel::filePath(const QModelIndex& index) const
{
	return index.isValid() ? infoList.at(index.row()).filePath() : "";
}
//
QString QFileListModel::fileName(const QModelIndex& index) const
{
	return index.isValid() ? infoList.at(index.row()).fileName() : "";
}
//
QDir QFileListModel::rootDirectory() const
{
	return qdCurrentDir;
}
//
QVariant QFileListModel::myComputer() const
{
	return QDir::rootPath();
}
//
QModelIndex QFileListModel::setRootPath(const QString& path)
{
	qdCurrentDir=QDir(path);
	getFileList();
	this->reset();
	//emit rowsInserted(createIndex(-1,0),0,infoList.count()-1);
	emit rootPathChanged(qdCurrentDir.absolutePath());

	if (!fileSystemWatcher.files().isEmpty())
		fileSystemWatcher.removePaths(fileSystemWatcher.files());
	fileSystemWatcher.addPath(path);

	return index(0,0);
}
//
QModelIndex QFileListModel::index(const QString& fileName) const
{
	for (int i=0; i<infoList.count(); i++)
		if (infoList.at(i).fileName()==fileName)
			return index(i,0);
	return QModelIndex();
}
//
void QFileListModel::setFilter(QDir::Filters filters)
{
	qdCurrentDir.setFilter(filters);
	getFileList();
	this->reset();
}
//
void QFileListModel::getInfoList(QFileInfoList *fileInfoList,const QDir& dir)
{
	QDirIterator it(dir);
	if (!it.hasNext() && !dir.isRoot())
		return;
	while (it.hasNext())
	{
		it.next();
		QFileInfo info(it.fileInfo());
		if (info.fileName() == QLatin1String("."))
			continue;

		fileInfoList->append(info);
	}
}
//


