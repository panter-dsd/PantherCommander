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
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
//
#ifdef Q_WS_WIN
	#define stat64 _stati64
	#include <windows.h>
#endif
//
QFileListModel::QFileListModel(const QString& rootPath,QObject *parent)
		:QAbstractItemModel(parent)
{
	qsCurrentDir=QDir(rootPath).absolutePath();
	if (QDir::toNativeSeparators(qsCurrentDir).at(qsCurrentDir.length()-1)!=QDir::separator())
		qsCurrentDir+=QDir::separator();
	qiFolderIcon=provider.icon(QFileIconProvider::Folder);
	qiFileIcon=provider.icon(QFileIconProvider::File);
	timerID=0;
	getFileList();
}
//
int QFileListModel::getFileList()
{
	if (timerID!=0)
		this->killTimer(timerID);
	QDir dir(qsCurrentDir);
	dir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	QDirIterator qdiIterator(dir);
	if (!qdiIterator.hasNext() && !QFileInfo(qsCurrentDir).isRoot())
	{
		// !QFileInfo(qsCurrentDir).isRoot() используется, чтобы отсечь пустые диски.
		return FLM_PRIVILEGY_ERROR;
	}
	dirsCount=filesCount=filesSize=0;
	foreach(SFileInfo fileInfo,fileList)
		if (!fileInfo.isStandardIcon)
			delete fileInfo.icon;
	fileList.clear();
	int iIndex=-1;
	while (qdiIterator.hasNext())
	{
		qdiIterator.next();
		const QString qsName=qdiIterator.fileName();
		if ((qsName==".") || (qsName==".."))
		{
			continue;
		}
		iIndex++;
//		if ((iIndex/100*100)==iIndex)
//			qApp->processEvents();

		SFileInfo fileInfo;
		fileInfo.name=qsName;

		struct stat64 st;
		if (stat64((qsCurrentDir+QDir::separator()+qsName).toLocal8Bit(),&st)!=0)
		{
			iIndex--;
			continue;
		}
		if ((st.st_mode & S_IFMT)==S_IFREG)
		{
			fileInfo.size=qint64(st.st_size);
			filesSize+=fileInfo.size;
			fileInfo.fileFlags |=ISFILE;
			fileInfo.isStandardIcon=true;
		}
		if ((st.st_mode & S_IFMT)==S_IFDIR)
		{
			fileInfo.fileFlags |=ISDIR;
			fileInfo.size=0;
			dirsCount++;
			fileInfo.isStandardIcon=true;
		}
		else
			filesCount++;
//time
		struct tm* time=localtime(&(st.st_mtime));
		QDateTime dateTime;
		dateTime.setDate(QDate(time->tm_year+1900,time->tm_mon+1,time->tm_mday));
		dateTime.setTime(QTime(time->tm_hour,time->tm_min,time->tm_sec));
		fileInfo.lastUpdateDate=dateTime;
//attr
#ifdef Q_WS_WIN
	fileInfo.attr=GetFileAttributesA((qsCurrentDir+QDir::separator()+qsName).toLocal8Bit());
#endif
//
		fileList << fileInfo;
	}
	QFileInfo qfiFileInfo(qsCurrentDir);
	if (!qfiFileInfo.isRoot())
	{
		SFileInfo fileInfo;
		fileInfo.name="..";
		fileInfo.fileFlags |=ISDIR;
		fileInfo.createDate=qfiFileInfo.created();
		//time
		fileInfo.lastUpdateDate=qfiFileInfo.lastModified();
		//attr
		#ifdef Q_WS_WIN
			fileInfo.attr=GetFileAttributesA(qsCurrentDir.toLocal8Bit());
		#endif
		fileInfo.isStandardIcon=true;
		fileList << fileInfo;
	}
	lastIconID=0;
	timerID=this->startTimer(0);
	return FLM_NO_ERROR;
}
//
QModelIndex QFileListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column>columnCount() || row>fileList.count() || fileList.isEmpty())
		return QModelIndex();
	return createIndex(row,column);
}
//
QModelIndex QFileListModel::parent(const QModelIndex& child) const
{
	return QModelIndex();
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
	if (!index.isValid() || index.row()>fileList.count() || fileList.isEmpty())
		return QVariant();
	if (role==Qt::DisplayRole)
	{
		int iTmp;
		switch (index.column())
		{
			case 0://Name
				iTmp=fileList.at(index.row()).name.lastIndexOf(".");
				if (iTmp==0 || (fileList.at(index.row()).fileFlags & ISDIR))
					return fileList.at(index.row()).name;
				if (iTmp!=-1 && iTmp!=fileList.at(index.row()).name.length()-1)
					return fileList.at(index.row()).name.left(iTmp);
				else
					return fileList.at(index.row()).name;
				break;
			case 1://Ext
				iTmp=fileList.at(index.row()).name.lastIndexOf(".");
				if (iTmp==0 || (fileList.at(index.row()).fileFlags & ISDIR))
					return "";
				if (iTmp!=-1 && iTmp!=fileList.at(index.row()).name.length()-1)
					return fileList.at(index.row()).name.mid(iTmp+1);
				else
					return "";
				break;
			case 2://size
				if (fileList.at(index.row()).fileFlags & ISFILE)
					return getSizeStr(fileList.at(index.row()).size);
				else
					return "-=DIR=-";
				break;
			case 3://time
				return fileList.at(index.row()).lastUpdateDate.toString("dd.MM.yy hh:mm:ss");
			case 4://attr
				#ifdef Q_WS_WIN
					QString attr;
					if (fileList.at(index.row()).attr & FILE_ATTRIBUTE_READONLY)
						attr+="r";
					else
						attr+="-";
					if (fileList.at(index.row()).attr & FILE_ATTRIBUTE_ARCHIVE)
						attr+="a";
					else
						attr+="-";
					if (fileList.at(index.row()).attr & FILE_ATTRIBUTE_HIDDEN)
						attr+="h";
					else
						attr+="-";
					if (fileList.at(index.row()).attr & FILE_ATTRIBUTE_SYSTEM)
						attr+="s";
					else
						attr+="-";
					if (fileList.at(index.row()).attr & FILE_ATTRIBUTE_COMPRESSED)
						attr+="c";
					else
						attr+="-";
					return attr;
				#endif
				break;
		}
	}
	if (role==Qt::DecorationRole && index.column()==0)
	{
		if (fileList.at(index.row()).isStandardIcon)
		{
			return isDir(index) ? qiFolderIcon : qiFileIcon;
		}
		return *fileList.at(index.row()).icon;
	}
	if (role==Qt::UserRole )
	{
		switch (index.column())
		{
			case 2:
				return fileList.at(index.row()).size;
				break;
		}
	}
	if (role==Qt::EditRole && index.column()==0)
	{
		return fileList.at(index.row()).name;
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
			case 0:return fileList.at(index.row()).name; break;
			case 1:return fileList.at(index.row()).name; break;
			case 2: return fileList.at(index.row()).size; break;
			case 3: return fileList.at(index.row()).lastUpdateDate.toString("dd.MM.yyyy hh:mm:ss"); break;
			default: return QVariant();
		}
		;
	}
	if (role==Qt::ForegroundRole)
	{
		if (fileList.at(index.row()).lastUpdateDate.date().daysTo(QDate::currentDate())<=3)
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
void QFileListModel::setPath(const QString& path)
{
	qsCurrentDir=QDir(path).absolutePath();
	if (QDir::toNativeSeparators(qsCurrentDir).at(qsCurrentDir.length()-1)!=QDir::separator())
		qsCurrentDir+=QDir::separator();
	lastError=getFileList();
	this->reset();
	//this->reset();//If this remove and last dirrectory whith no files, sorting is no enableds
}
//
void QFileListModel::cdUP()
{
	QDir dir(qsCurrentDir);
	dir.cdUp();
	qsCurrentDir=dir.absolutePath();
	if (QDir::toNativeSeparators(qsCurrentDir).at(qsCurrentDir.length()-1)!=QDir::separator())
		qsCurrentDir+=QDir::separator();
	lastError=getFileList();
	this->reset();
	//this->reset();//If this remove and last dirrectory whith no files, sorting is no enableds
}
//
bool QFileListModel::hasChildren ( const QModelIndex & parent) const
{
	return parent.row()==-1;
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
	return (fileList.at(index.row()).fileFlags & ISDIR);
}
//
void QFileListModel::slotRefresh()
{
	lastError=getFileList();
	this->reset();
	//this->reset();//If this remove and last dirrectory whith no files, sorting is no enableds
}
//
void QFileListModel::timerEvent(QTimerEvent *event)
{
	if (lastIconID>=fileList.count())
	{
		this->killTimer(timerID);
		return;
	}
	QIcon icon=provider.icon(QFileInfo(qsCurrentDir+fileList.at(lastIconID).name));
	if (!icon.isNull())
	{
		fileList[lastIconID].icon=new QIcon(icon);
		fileList[lastIconID].isStandardIcon=false;
		emit dataChanged(index(lastIconID,0),index(lastIconID,0));
	}
	lastIconID++;
}
//

