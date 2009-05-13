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

#ifndef QFILELISTSORTFILTERPROXYMODEL_H
#define QFILELISTSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class QFileListSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit QFileListSortFilterProxyModel(QObject* parent = 0);
	virtual ~QFileListSortFilterProxyModel();

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

protected:
	bool filterAcceptsRow(int row, const QModelIndex& parent) const;
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
};

#endif // QFILELISTSORTFILTERPROXYMODEL_H
