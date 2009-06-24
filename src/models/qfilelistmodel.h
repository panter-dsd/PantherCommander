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

#ifndef QFILELISTMODEL_H
#define QFILELISTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QDir>

class QFileIconProvider;

class QFileListModelPrivate;
class QFileListModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	enum Columns
	{
		NAME = 0,
		EXT,
		SIZE,
		TIME_LAST_UPDATE,
		ATTR,

		COLUMNS
	};

public:
	explicit QFileListModel(QObject* parent = 0);
	virtual ~QFileListModel();

	void setNameFilterDisables(bool) {;}

	QDir::Filters filter() const;
	void setFilter(QDir::Filters filters);

	bool isReadOnly() const
	{ return false; }
	void setReadOnly(bool)
	{}

	QFileIconProvider* iconProvider() const;
	void setIconProvider(QFileIconProvider* iconProvider);

	QDir rootDirectory() const;
	QString rootPath() const;
	QModelIndex setRootPath(const QString& path);

	QModelIndex index(const QString& fileName) const;
	bool isDir(const QModelIndex& index) const;
	QString fileName(const QModelIndex& index) const;
	QString filePath(const QModelIndex& index) const;
	QFile::Permissions permissions(const QModelIndex& index) const;
	qint64 size(const QModelIndex& index) const;
	QVariant myComputer() const;

	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	Qt::DropActions supportedDropActions() const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	bool hasChildren(const QModelIndex &parent) const;

	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

signals:
	void rootPathChanged(const QString&);

private:
	Q_DISABLE_COPY(QFileListModel)
	Q_DECLARE_PRIVATE(QFileListModel)
	QFileListModelPrivate* const d_ptr;
	Q_PRIVATE_SLOT(d_func(), void _q_refresh())
	Q_PRIVATE_SLOT(d_func(), void _q_finishedLoadIcons())
};
#endif // QFILELISTMODEL_H
