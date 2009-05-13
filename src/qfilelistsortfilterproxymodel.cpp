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

#include "qfilelistsortfilterproxymodel.h"

#include <QtCore/QDateTime>

#include <QtGui/QFileSystemModel>

static bool isDir(const QModelIndex& index)
{
	const QFileSystemModel* model = qobject_cast<const QFileSystemModel*>(index.model());
	if(model)
		return model->isDir(index);

	return false;
}


QFileListSortFilterProxyModel::QFileListSortFilterProxyModel(QObject* parent)
	: QSortFilterProxyModel(parent)
{
}

QFileListSortFilterProxyModel::~QFileListSortFilterProxyModel()
{
}

#ifndef Q_CC_MSVC
	#warning "wow! hardcoding is evil!"
#endif
QVariant QFileListSortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	const QFileSystemModel* model = qobject_cast<const QFileSystemModel*>(sourceModel());
	if(model && orientation == Qt::Horizontal)
	{
		if(section == 1)
			section = 2;
		else if(section == 2)
			section = 1;

		return QSortFilterProxyModel::headerData(section, orientation, role);
	}

	return QSortFilterProxyModel::headerData(section, orientation, role);
}

QVariant QFileListSortFilterProxyModel::data(const QModelIndex& idx, int role) const
{
	const QFileSystemModel* model = qobject_cast<const QFileSystemModel*>(sourceModel());
	if(model)
	{
		int row = idx.row();
		int column = idx.column();
		if(column == 1)
			column = 2;
		else if(column == 2)
			column = 1;
		const QModelIndex index = QSortFilterProxyModel::index(row, column, idx.parent());

		const QModelIndex sourceIndex = mapToSource(index);
		if(index.isValid() && !sourceIndex.isValid())
			return QVariant();

		QVariant data = sourceModel()->data(sourceIndex, role);

		if(role == Qt::DisplayRole)
		{
			switch(sourceIndex.column())
			{
				case 0:
					if(isDir(sourceIndex))
					{
						QString dirName = data.toString();
						dirName.prepend(QLatin1Char('[')).append(QLatin1Char(']'));
						data = QVariant(dirName);
					}
					else
					{
						QString fileName = data.toString();
						int pos = fileName.lastIndexOf(QLatin1Char('.'));
						if(pos > 0)
						{
							fileName = fileName.left(pos);
							data = QVariant(fileName);
						}
					}
					break;
				case 2:
					if(!isDir(sourceIndex))
					{
						const QModelIndex sourceIndex0 = sourceModel()->index(sourceIndex.row(), 0, sourceIndex.parent());
						QString fileName = sourceModel()->data(sourceIndex0).toString();
						QString suffix;
						int pos = fileName.lastIndexOf(QLatin1Char('.'));
						if(pos > 0 && pos < fileName.size() - 1)
							suffix = fileName.mid(pos + 1);
						data = QVariant(suffix);
					}
					break;
				default:
					break;
			}
		}

		return data;
	}

	return QSortFilterProxyModel::data(idx, role);
}

bool QFileListSortFilterProxyModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
	QModelIndex sourceIndex = sourceModel()->index(row, 0, parent);
	if(!sourceIndex.isValid())
		return false;

	QString key = sourceModel()->data(sourceIndex, Qt::EditRole).toString();
	if(key == QLatin1String("."))
		return false;

	return QSortFilterProxyModel::filterAcceptsRow(row, parent);
}

bool QFileListSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	QVariant l = (left.model() ? left.model()->data(left, sortRole()) : QVariant());
	QVariant r = (right.model() ? right.model()->data(right, sortRole()) : QVariant());

	bool isUpDir = (left.model() ? isDir(left) : false);
	bool isDownDir = (right.model() ? isDir(right) : false);

	if(isUpDir && l.toString() == QLatin1String(".."))
		return !sortOrder();
	if(isDownDir && r.toString() == QLatin1String(".."))
		return sortOrder();

	if(isUpDir && isDownDir)
	{
		if(isSortLocaleAware())
			return l.toString().localeAwareCompare(r.toString()) < 0 && !sortOrder();
		return l.toString().compare(r.toString(), sortCaseSensitivity()) < 0 && !sortOrder();
	}
	if(isUpDir && !isDownDir)
	{
		return !sortOrder();
	}
	if(!isUpDir && isDownDir)
	{
		return sortOrder();
	}

	// Data comparison
	if(sortColumn() == 0)
	{
		if(isSortLocaleAware())
			return l.toString().localeAwareCompare(r.toString()) < 0;
		return l.toString().compare(r.toString(), sortCaseSensitivity()) < 0;
	}
	else if(sortColumn() == 1)
	{
		int res = 0;
		if(isSortLocaleAware())
			res = l.toString().localeAwareCompare(r.toString());
		else
			res = l.toString().compare(r.toString(), sortCaseSensitivity());
		if(res != 0)
			return res < 0;
	}
	else if(sortColumn() == 2)
	{
		if(l.toDouble() != r.toDouble())
			return l.toDouble() < r.toDouble();
	}
	else if(l.type() == QVariant::DateTime)
	{
		if(l.toDateTime() != r.toDateTime())
			return l.toDateTime() < r.toDateTime();
	}

	// falling back to compare names
	l = (left.model() ? left.model()->data(left.model()->index(left.row(), 0), sortRole()) : QVariant());
	r = (right.model() ? right.model()->data(right.model()->index(right.row(), 0), sortRole()) : QVariant());

	if(isSortLocaleAware())
		return l.toString().localeAwareCompare(r.toString()) < 0;
	return l.toString().compare(r.toString(), sortCaseSensitivity()) < 0;
}
