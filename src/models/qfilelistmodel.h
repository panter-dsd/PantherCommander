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
#include <QtGui/QIcon>
#include <QtCore/QDateTime>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QFileIconProvider>
#include <QtCore/QDir>
#include <QtCore/QFuture>
#include <QtCore/QFSFileEngine>
#include <QtCore/QFileSystemWatcher>
#ifdef Q_WS_WIN
	#include <windows.h>
#endif
//
class QPCFileInfo
{
public:
	enum Type { Dir, File, System };

	QPCFileInfo()
	{}
	QPCFileInfo(const QFileInfo &info) : mFileInfo(info)
	{}

	inline bool isDir() const { return type() == Dir; }
	inline bool isFile() const { return type() == File; }
	inline bool isSystem() const { return type() == System; }

	bool operator ==(const QPCFileInfo &fileInfo) const {
		return mFileInfo == fileInfo.mFileInfo && permissions() == fileInfo.permissions();
	}

	bool isCaseSensitive() const {
		QFSFileEngine fe(mFileInfo.absoluteFilePath());
		return fe.caseSensitive();
	}
	QFile::Permissions permissions() const {
		return mPermissions;
	}

	void setPermissions (QFile::Permissions permissions) {
		mPermissions = permissions;
	}

	Type type() const {
		if (mFileInfo.isDir()) {
			return QPCFileInfo::Dir;
		}
		if (mFileInfo.isFile()) {
			return QPCFileInfo::File;
		}
		if (!mFileInfo.exists() && mFileInfo.isSymLink()) {
			return QPCFileInfo::System;
		}
		return QPCFileInfo::System;
	}

	bool isSymLink() const {
		return mFileInfo.isSymLink();
	}

	bool isHidden() const {
		return mFileInfo.isHidden();
	}

	QFileInfo fileInfo() const {
		return mFileInfo;
	}

	QDateTime lastModified() const {
		return mFileInfo.lastModified();
	}

	QDateTime created() const {
		return mFileInfo.created();
	}

	QDateTime lastRead() const {
		return mFileInfo.lastRead();
	}

	QString fileName() const{
		return mFileInfo.fileName();
	}

	QString absoluteFilePath() const{
		return mFileInfo.absoluteFilePath();
	}

	QString filePath() const{
		return mFileInfo.filePath();
	}

	qint64 size() const {
		return mFileInfo.size();
	}

	QIcon icon;

private :
	QFileInfo mFileInfo;
	QFile::Permissions mPermissions;
};
//
class QFileListModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	enum Columns {
		NAME,
		EXT,
		SIZE,
		TIME_LAST_UPDATE,
		ATTR};
private:
	QDir									qdCurrentDir;
	QList<QPCFileInfo>			infoList;
	QFuture<void>					future;
	QIcon									qiFolderIcon;
	QIcon									qiFileIcon;
	QFileIconProvider*				provider;
	QFileSystemWatcher			fileSystemWatcher;
	int										dirsCount;
	int										filesCount;
	qint64									filesSize;
public:
	QFileListModel(QObject *parent = 0);
	~QFileListModel();
	virtual void setNameFilterDisables(bool) {;}
	void setFilter(QDir::Filters filters);
	virtual void setReadOnly(bool) {;}
	qint64 size(const QModelIndex& index) const;
	QFile::Permissions permissions(const QModelIndex& index) const;
	QString filePath(const QModelIndex& index) const;
	QString fileName(const QModelIndex& index) const;
	QDir rootDirectory() const;
	QString rootPath() const {return qdCurrentDir.absolutePath();}
	QVariant myComputer() const;
	QModelIndex setRootPath(const QString& path);
	QDir::Filters filter() const {return qdCurrentDir.filter();}
	QFileIconProvider *iconProvider() const {return provider;}
	void setIconProvider(QFileIconProvider *iconProvider) {delete provider; provider=iconProvider;}
	QModelIndex index(const QString& fileName) const;
	bool hasChildren(const QModelIndex &parent) const;


	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	bool setData(const QModelIndex &index, const QVariant &value,int role);
	QModelIndex parent(const QModelIndex&) const;
	Qt::ItemFlags flags ( const QModelIndex & index ) const;
	int rowCount(const QModelIndex& =QModelIndex()) const {return infoList.count();}
	int columnCount(const QModelIndex& =QModelIndex()) const {return 5;}
	int sortColumn() const {return 1;}
	Qt::SortOrder sortOrder() const {return Qt::AscendingOrder;}
	bool isDir(const QModelIndex& index) const;
	QString getSizeStr(double size) const;
private:
	void getFileList();
	static void getIcons(QList<QPCFileInfo>* info,QFileIconProvider* prov);
	static void getInfoList(QFileInfoList *fileInfoList,const QDir& dir);
public slots:
	void slotRefresh();
signals:
	void rootPathChanged(const QString&);
	void rowsInserted(const QModelIndex &parent, int first, int last);
	void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
};
#endif // QFILELISTMODEL_H