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
#include <QFuture>
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

private:
	QDir									qdCurrentDir;
	QList<QFileInfo>				infoList;
	QFuture<void>					future;
	QIcon									qiFolderIcon;
	QIcon									qiFileIcon;
	QList<QIcon>					iconList;
	QFileIconProvider*				provider;
	int										lastError;
	int										dirsCount;
	int										filesCount;
	qint64									filesSize;
public:
	QFileListModel(QObject *parent = 0);
	~QFileListModel();
	virtual void setNameFilterDisables(bool) {;}
	void setFilter(QDir::Filters filters);
	virtual void setReadOnly(bool) {;}
	qint64 size(const QModelIndex& index);
	QFile::Permissions permissions(const QModelIndex& index);
	QString filePath(const QModelIndex& index);
	QString fileName(const QModelIndex& index);
	QDir rootDirectory();
	QString rootPath() const {return qdCurrentDir.absolutePath();}
	QVariant myComputer();
	QModelIndex setRootPath(const QString& path);
	QDir::Filters filter() {return qdCurrentDir.filter();}
	QFileIconProvider *iconProvider() {return provider;}
	void setIconProvider(QFileIconProvider *iconProvider) {delete provider; provider=iconProvider;}
	QModelIndex index(const QString& fileName);
	bool hasChildren(const QModelIndex &parent) const;


	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	bool setData(const QModelIndex &index, const QVariant &value,int role);
	QModelIndex parent(const QModelIndex&) const;
	Qt::ItemFlags flags ( const QModelIndex & index ) const;
	int rowCount(const QModelIndex& =QModelIndex()) const {return infoList.count();}
	int columnCount(const QModelIndex& =QModelIndex()) const {return 5;}
	int sortColumn() {return 1;}
	Qt::SortOrder sortOrder() {return Qt::AscendingOrder;}
	int getLastError() {return lastError;}
	bool isDir(const QModelIndex& index) const;
	QString getSizeStr(double size) const;
	int getDirsCount() {return dirsCount;}
	int getFilesCount() {return filesCount;}
	qint64 getFilesSize() {return filesSize;}
private:
	int getFileList();
	inline int getIndex(const QFileInfo& info);
	static void getIcons(QList<QFileInfo>& info,QList<QIcon>* icons,QFileIconProvider* prov);
public slots:
	void slotRefresh();
signals:
	void rootPathChanged(const QString&);
};
#endif // QFILELISTMODEL_H
