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

#include <QtCore/QAbstractFileEngine>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QtCore/QTimer>

#include <QtGui/QIcon>
#include <QtGui/QFileIconProvider>

class QPCFileInfo
{
public:
	enum Type { Dir, File, System };

	QPCFileInfo() : m_permissions(0)
	{}
	QPCFileInfo(const QFileInfo& fileInfo) : m_fileInfo(fileInfo), m_permissions(0)
	{}

	bool operator==(const QPCFileInfo& fileInfo) const
	{ return m_fileInfo == fileInfo.m_fileInfo && m_permissions == fileInfo.m_permissions; }

	inline QFileInfo fileInfo() const
	{ return m_fileInfo; }
	inline void setFileInfo(const QFileInfo& fileInfo)
	{ m_fileInfo = fileInfo; }

	bool isCaseSensitive() const
	{
#if QT_VERSION < 0x040600
		QAbstractFileEngine* fe = QAbstractFileEngine::create(m_fileInfo.absoluteFilePath());
		bool cs = fe->caseSensitive();
		delete fe;
		return cs;
#else
		QAbstractFileEngine* fe = m_fileInfo.fileEngine();
		if(fe)
			return fe->caseSensitive();
#ifdef Q_WS_WIN
		return false;
#else
		return true;
#endif
#endif
	}
	bool isLocalFile() const
	{
#if QT_VERSION < 0x040600
		QAbstractFileEngine* fe = QAbstractFileEngine::create(m_fileInfo.absoluteFilePath());
		bool localDisc = (fe->fileFlags(QAbstractFileEngine::LocalDiskFlag) & QAbstractFileEngine::LocalDiskFlag);
		delete fe;
		return localDisc;
#else
		QAbstractFileEngine* fe = m_fileInfo.fileEngine();
		if(fe)
			return (fe->fileFlags(QAbstractFileEngine::LocalDiskFlag) & QAbstractFileEngine::LocalDiskFlag);
		return false;
#endif
	}

	inline bool isDir() const
	{ return m_fileInfo.isDir(); }
	inline bool isFile() const
	{ return m_fileInfo.isFile(); }
	inline bool isSymLink() const
	{ return m_fileInfo.isSymLink(); }

	inline bool isHidden() const
	{ return m_fileInfo.isHidden(); }
	inline bool isSystem() const
	{ return type() == System; }
	Type type() const
	{
		if(m_fileInfo.isDir())
			return QPCFileInfo::Dir;
		if(m_fileInfo.isFile())
			return QPCFileInfo::File;
/*#ifdef Q_WS_WIN
		//TODO: implement
#else
		//!isDir() && !isFile() can be omitted since they are checked above
		if(m_fileInfo.exists() && !m_fileInfo.isSymLink())
			return QPCFileInfo::System;
#endif*/
		return QPCFileInfo::System;
	}

	inline QString fileName() const
	{ return m_fileInfo.fileName(); }
	inline QString filePath() const
	{ return m_fileInfo.filePath(); }
	inline QString absoluteFilePath() const
	{ return m_fileInfo.absoluteFilePath(); }

	QString name() const
	{
		QString fname(m_fileInfo.fileName());
		if(!fname.isEmpty() && !m_fileInfo.isDir())
		{
			int pos = fname.lastIndexOf(QLatin1Char('.'));
			if(pos > 0 && pos < fname.size() - 1)
				return fname.left(pos);
		}
		return fname;
	}
	QString ext() const
	{
		QString fname(m_fileInfo.fileName());
		if(!fname.isEmpty() && !m_fileInfo.isDir())
		{
			int pos = fname.lastIndexOf(QLatin1Char('.'));
			if(pos > 0 && pos < fname.size() - 1)
				return fname.mid(pos + 1);
		}
		return QString();
	}

	inline QDateTime created() const
	{ return m_fileInfo.created(); }
	inline QDateTime lastModified() const
	{ return m_fileInfo.lastModified(); }
	inline QDateTime lastRead() const
	{ return m_fileInfo.lastRead(); }

	qint64 size() const
	{
		Type t = type();
		if(t == File)
			return m_fileInfo.size();
		if(t == Dir)
			return 0;
		return -1;
	}

	inline QFile::Permissions permissions() const
	{ return m_permissions ? m_permissions : m_fileInfo.permissions(); }
	inline void setPermissions(QFile::Permissions permissions)
	{ m_permissions = permissions; }

	inline QIcon icon() const
	{ return m_icon; }
	inline void setIcon(const QIcon& icon)
	{ m_icon = icon; }

private:
	QFileInfo m_fileInfo;
	QFile::Permissions m_permissions;
	QIcon m_icon;
};


class QFileListModelPrivate
{
	Q_DECLARE_PUBLIC(QFileListModel)

public:
	QFileListModelPrivate();
	~QFileListModelPrivate();

	QPCFileInfo* node(const QModelIndex& index) const;
	QModelIndex index(const QPCFileInfo* node) const;

	void fetchFileList();
	void updateFileList();

	void _q_directoryChanged();
	void _q_finishedLoadIcons();

	inline bool indexValid(const QModelIndex& index) const
	{
		Q_Q(const QFileListModel);
		return (index.isValid() && index.model() == q
				&& index.row() < q->rowCount(index.parent())
				&& index.column() < q->columnCount(index.parent()));
	}
	static QString size(qint64 bytes);

	QFileListModel* q_ptr;

	QDir rootDir;
	QPCFileInfo root;
	QList<QPCFileInfo*> nodes;

	int sortColumn;
	Qt::SortOrder sortOrder;
	QDir::SortFlags sort;

	QFuture<void> future;
	QFutureWatcher<void> futureWatcher;

	QFileIconProvider* iconProvider;

#ifndef QT_NO_FILESYSTEMWATCHER
	QFileSystemWatcher* fileSystemWatcher;
#endif
	QBasicTimer pendingUpdateTimer;
};

#endif // QFILELISTMODEL_P_H
