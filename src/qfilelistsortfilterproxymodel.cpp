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
#include "qfilelistsortfilterproxymodel.h"

QFileListSortFilterProxyModel::QFileListSortFilterProxyModel(QObject * parent)
		:QSortFilterProxyModel(parent)
{
}
//
void QFileListSortFilterProxyModel::sort ( int column, Qt::SortOrder order)
{
	iColumn=column;
	qsoOrder=order;
	QSortFilterProxyModel::sort(column,order);
}
//
bool QFileListSortFilterProxyModel::lessThan(const QModelIndex &qmiUp,
											const QModelIndex &qmiDown) const
{
	QVariant UpData = sourceModel()->data(sourceModel()->index(qmiUp.row(),0));
	QVariant DownData = sourceModel()->data(sourceModel()->index(qmiDown.row(),0));
	if (UpData.toString()=="..")
		return !qsoOrder;
	else
		if (DownData.toString()=="..")
			return qsoOrder;

	bool isUpDir=(sourceModel()->data(sourceModel()->index(qmiUp.row(),2)).toString()=="-=DIR=-");
	bool isDownDir=(sourceModel()->data(sourceModel()->index(qmiDown.row(),2)).toString()=="-=DIR=-");
	if (isUpDir && isDownDir)
	{
		return (QString::compare(UpData.toString(), DownData.toString(),Qt::CaseInsensitive) < 0)
&& !qsoOrder;
	}
	if (isUpDir && (!isDownDir))
	{
		return !qsoOrder;
	}
	if ((!isUpDir) && isDownDir)
	{
		return qsoOrder;
	}

//Data compare
	if (iColumn==0)
		return QString::compare(sourceModel()->data(qmiUp).toString(),
							sourceModel()->data(qmiDown).toString(),Qt::CaseInsensitive) < 0;
	if (iColumn==1)
		if (QString::compare(sourceModel()->data(qmiUp).toString(),
							sourceModel()->data(qmiDown).toString(),Qt::CaseInsensitive)!=0)
			return QString::compare(sourceModel()->data(qmiUp).toString(),
								sourceModel()->data(qmiDown).toString(),Qt::CaseInsensitive) < 0;
		else
			return
				QString::compare(sourceModel()->data(sourceModel()->index(qmiUp.row(),0)).toString(),sourceModel()->data(sourceModel()->index(qmiDown.row(),0)).toString(),Qt::CaseInsensitive) < 0;
	if (iColumn==2)
		if (sourceModel()->data(qmiUp,Qt::UserRole).toDouble() !=
				sourceModel()->data(qmiDown,Qt::UserRole).toDouble())
			return sourceModel()->data(qmiUp,Qt::UserRole).toDouble() <
					sourceModel()->data(qmiDown,Qt::UserRole).toDouble();
		else
			return
				QString::compare(sourceModel()->data(sourceModel()->index(qmiUp.row(),0)).toString(),sourceModel()->data(sourceModel()->index(qmiDown.row(),0)).toString(),Qt::CaseInsensitive) < 0;
	if (iColumn==3)
		if (sourceModel()->data(qmiUp).toDateTime() !=
				sourceModel()->data(qmiDown).toDateTime())
			return sourceModel()->data(qmiUp).toDateTime() <
					sourceModel()->data(qmiDown).toDateTime();
			else
				return
					QString::compare(sourceModel()->data(sourceModel()->index(qmiUp.row(),0)).toString(),sourceModel()->data(sourceModel()->index(qmiDown.row(),0)).toString(),Qt::CaseInsensitive) < 0;
	return QString::compare(sourceModel()->data(sourceModel()->index(qmiUp.row(),0)).toString(),
							sourceModel()->data(sourceModel()->index(qmiDown.row(),0)).toString(),
							Qt::CaseInsensitive) < 0;
}
