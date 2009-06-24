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

#ifndef QFILELISTMODEL_P_H
#define QFILELISTMODEL_P_H

#include "qfilelistmodel.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QtCore/QAbstractFileEngine>

#include <QtGui/QIcon>

class QFileIconProvider;
class QFileSystemWatcher;

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
		QAbstractFileEngine* fe = QAbstractFileEngine::create(mFileInfo.absoluteFilePath());
		bool cs = fe->caseSensitive();
		delete fe;
		return cs;
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


class QFileListModelPrivate
{
	Q_DECLARE_PUBLIC(QFileListModel)

public:
	QFileListModelPrivate();
	~QFileListModelPrivate();

	QPCFileInfo* node(const QModelIndex& index) const;
	QModelIndex index(const QPCFileInfo* node) const;

	inline bool indexValid(const QModelIndex& index) const
	{
		Q_Q(const QFileListModel);
		return (index.isValid() && index.model() == q
				&& index.row() < q->rowCount(index.parent())
				&& index.column() < q->columnCount(index.parent()));
	}

	void getFileList();
	static QString size(qint64 bytes);

	void _q_refresh();
	void _q_finishedLoadIcons();

	QFileListModel* q_ptr;

	QDir qdCurrentDir;
	QPCFileInfo root;
	QList<QPCFileInfo*> infoList;

	QFuture<void> future;
	QFutureWatcher<void> futureWatcher;

	QFileIconProvider* provider;
	QIcon qiFolderIcon;
	QIcon qiFileIcon;

	QFileSystemWatcher* fileSystemWatcher;
};

#endif // QFILELISTMODEL_P_H
