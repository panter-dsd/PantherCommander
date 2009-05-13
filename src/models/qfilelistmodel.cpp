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
#include "qfilelistmodel.h"
//
QFileListModel::QFileListModel(QObject *parent)
		:QAbstractItemModel(parent)
{
	provider=new QFileIconProvider();
	qiFolderIcon=provider->icon(QFileIconProvider::Folder);
	qiFileIcon=provider->icon(QFileIconProvider::File);
}
//
QFileListModel::~QFileListModel()
{
	future.cancel();
	future.waitForFinished();
}
//
int QFileListModel::getFileList()
{
	qdCurrentDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	qdCurrentDir.setSorting(QDir::Unsorted);
	QDirIterator qdiIterator(qdCurrentDir);
	if (!qdiIterator.hasNext() && !qdCurrentDir.isRoot())
	{
		return FLM_PRIVILEGY_ERROR;
	}
	dirsCount=filesCount=filesSize=0;
	future.cancel();
	future.waitForFinished();
	infoList.clear();
	QTime time;
	time.start();
	int index=0;
	while (qdiIterator.hasNext())
	{
		const QString& qsName=qdiIterator.next();
		if (qsName.endsWith("/.") || qsName.endsWith("/.."))
		{
			continue;
		}
		//infoList.insert(getIndex(qdiIterator.fileInfo()),qdiIterator.fileInfo());
		infoList.append(QPCFileInfo(qdiIterator.fileInfo()));
		if (qdiIterator.fileInfo().isDir())
		{
			dirsCount++;
		}
		else
			filesCount++;
	}
	qDebug(QString::number(time.elapsed()).toLocal8Bit());
	if (!qdCurrentDir.isRoot())
	{
		infoList.insert(0,QPCFileInfo(QFileInfo(qdCurrentDir.absolutePath())));
	}
	future = QtConcurrent::run(getIcons,&infoList,provider);
	return FLM_NO_ERROR;
}
//
QModelIndex QFileListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column>columnCount() || row>infoList.count() || infoList.isEmpty())
		return QModelIndex();
	return createIndex(row,column);
}
//
QModelIndex QFileListModel::parent(const QModelIndex& child) const
{
	if (!child.isValid())
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
	if (!index.isValid())
		return 0;
//	if (index.column()==0)
//		return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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
			case 0://Name
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
			case 1://Ext
				iTmp=infoList.at(index.row()).fileName().lastIndexOf(".");
				if (iTmp==0 || (infoList.at(index.row()).isDir()))
					return "";
				if (iTmp!=-1 && iTmp!=infoList.at(index.row()).fileName().length()-1)
					return infoList.at(index.row()).fileName().mid(iTmp+1);
				else
					return "";
				break;
			case 2://size
				if (!infoList.at(index.row()).isDir())
					return getSizeStr(infoList.at(index.row()).size());
				else
					return "-=DIR=-";
				break;
			case 3://time
				return infoList.at(index.row()).lastModified().toString("dd.MM.yy hh:mm:ss");
			case 4://attr
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
	if (role==Qt::DecorationRole && index.column()==0)
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
			case 2:
				return infoList.at(index.row()).size();
				break;
		}
	}
	if (role==Qt::EditRole && index.column()==0)
	{
		if (infoList.at(index.row())==QFileInfo(qdCurrentDir.absolutePath()))
					return "..";
		return infoList.at(index.row()).fileName();
	}
	if (role==Qt::TextAlignmentRole && index.column()==2)
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
			case 0:return infoList.at(index.row()).fileName(); break;
			case 1:return infoList.at(index.row()).fileName(); break;
			case 2: return infoList.at(index.row()).size(); break;
			case 3: return infoList.at(index.row()).lastModified().toString("dd.MM.yyyy hh:mm:ss"); break;
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
	return (infoList.at(index.row()).isDir());
}
//
void QFileListModel::slotRefresh()
{
	lastError=getFileList();
	this->reset();
	//this->reset();//If this remove and last dirrectory whith no files, sorting is no enableds
}
//
inline int QFileListModel::getIndex(const QFileInfo& info)
{
	if (infoList.isEmpty())
		return 0;
	int first=0;
	int last=infoList.count()-1;
	int index=(first+last)/2;
	while(last!=first)
	{
		if (info.size()==infoList.at(index).size())
			break;
		if (info.size()<infoList.at(index).size())
		{
			last=index;
		}
		else
		{
			first=index+1;
		}
		index=(first+last)/2;
	}
//
	return index;
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
qint64 QFileListModel::size(const QModelIndex& index)
{
	if (!index.isValid())
		return 0;
	return infoList.at(index.row()).size();
}
//
QFile::Permissions QFileListModel::permissions(const QModelIndex& index)
{
	if (!index.isValid())
		return 0;
	return infoList.at(index.row()).permissions();
}
//
QString QFileListModel::filePath(const QModelIndex& index)
{
	if (!index.isValid())
		return "";
	return infoList.at(index.row()).filePath();
}
//
QString QFileListModel::fileName(const QModelIndex& index)
{
	if (!index.isValid())
		return "";
	if (infoList.at(index.row())==QFileInfo(qdCurrentDir.absolutePath()))
			return "..";
	return infoList.at(index.row()).fileName();
}
//
QDir QFileListModel::rootDirectory()
{
	return qdCurrentDir;
}
//
QVariant QFileListModel::myComputer()
{
	return QDir::rootPath();
}
//
QModelIndex QFileListModel::setRootPath(const QString& path)
{
	qdCurrentDir=QDir(path);
	lastError=getFileList();
	this->reset();
	emit rootPathChanged(path);
	return index(0,0);
}
//
QModelIndex QFileListModel::index(const QString& fileName)
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
	lastError=getFileList();
	this->reset();
}
//


