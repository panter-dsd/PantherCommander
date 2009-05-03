#ifndef QFILELISTMODEL_H
#define QFILELISTMODEL_H
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
#include <QIcon>
#include <QDateTime>
#include <QAbstractItemModel>
#include <QFileIconProvider>
#include <QDir>
#ifdef Q_WS_WIN
	#include <windows.h>
#endif
//
enum GetFileListError
{
	FLM_NO_ERROR,
	FLM_DISC_ERROR,
	FLM_PRIVILEGY_ERROR
};
//
class QFileListModel : public QAbstractItemModel
{
Q_OBJECT
public:
	enum FileFlag
	{
		ISFILE=0x1,
		ISDIR=0x2
	};
	Q_DECLARE_FLAGS(FileFlags, FileFlag)
	struct SFileInfo
	{
		QString					name;
		qint64						size;
		QIcon*						icon;
		QDateTime				createDate;
		QDateTime				lastUpdateDate;
		QFlags<FileFlags>	fileFlags;
		bool							isStandardIcon;
		#ifdef Q_WS_WIN
		DWORD					attr;
		#endif
	};
private:
	QString								qsCurrentDir;
	mutable QList<SFileInfo>	fileList;
	QIcon									qiFolderIcon;
	QIcon									qiFileIcon;
	QFileIconProvider				provider;
	int										lastError;
	int										dirsCount;
	int										filesCount;
	qint64									filesSize;
	int										timerID;
	int										lastIconID;
protected:
	void timerEvent(QTimerEvent *event);
public:
	QFileListModel(const QString& path,QObject *parent = 0);
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	bool setData(const QModelIndex &index, const QVariant &value,int role);
	QModelIndex parent(const QModelIndex&) const;
	Qt::ItemFlags flags ( const QModelIndex & index ) const;
	bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
	int rowCount(const QModelIndex& =QModelIndex()) const {return fileList.count();}
	int columnCount(const QModelIndex& =QModelIndex()) const {return 5;}
	QString path() {return QDir::toNativeSeparators(qsCurrentDir);}
	void setPath(const QString& path);
	void cdUP();
	int getLastError() {return lastError;}
	bool isDir(const QModelIndex& index) const;
	QString getSizeStr(double size) const;
	int getDirsCount() {return dirsCount;}
	int getFilesCount() {return filesCount;}
	qint64 getFilesSize() {return filesSize;}
private:
	int getFileList();
public slots:
	void slotRefresh();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QFileListModel::FileFlags)
#endif // QFILELISTMODEL_H
