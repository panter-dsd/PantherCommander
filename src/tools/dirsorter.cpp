#if QT_VERSION < 0x040600
/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#endif // QT_VERSION
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
* Author:		Ritt K.
* Contact:		ritt.ks@gmail.com
*******************************************************************/

#include "dirsorter.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

#include <qalgorithms.h>

#if QT_VERSION < 0x040600
struct DirSortItem
{
	mutable QString filename_cache;
	mutable QString suffix_cache;
	QFileInfo item;
};

class DirSortItemComparator
{
	int sort_flags;

public:
	DirSortItemComparator(int flags) :
		sort_flags(flags)
	{}

	bool operator()(const DirSortItem&, const DirSortItem&);
};

bool DirSortItemComparator::operator()(const DirSortItem& n1, const DirSortItem& n2)
{
	const DirSortItem* f1 = &n1;
	const DirSortItem* f2 = &n2;

	if(f1->item.isDir() || f2->item.isDir())
	{
		bool bDot1 = (f1->item.fileName() == QLatin1String(".") || f1->item.fileName() == QLatin1String(".."));
		bool bDot2 = (f2->item.fileName() == QLatin1String(".") || f2->item.fileName() == QLatin1String(".."));
		if(sort_flags & QDir::DirsFirst)
			return (bDot1 && bDot2 ? f1->item.fileName() == QLatin1String(".") : bDot1);
		if(sort_flags & QDir::DirsLast)
			return (bDot1 && bDot2 ? f1->item.fileName() != QLatin1String(".") : !bDot1);

		if((sort_flags & QDir::DirsFirst) && (f1->item.isDir() != f2->item.isDir()))
			return f1->item.isDir();
		if((sort_flags & QDir::DirsLast) && (f1->item.isDir() != f2->item.isDir()))
			return !f1->item.isDir();
	}

	int r = 0;
	int sortBy = (sort_flags & QDir::SortByMask) | (sort_flags & QDir::Type);

	switch(sortBy)
	{
		case QDir::Time:
			r = f1->item.lastModified().secsTo(f2->item.lastModified());
			break;
		case QDir::Size:
			r = int(qBound<qint64>(-1, f2->item.size() - f1->item.size(), 1));
			break;
		case QDir::Type:
		{
			bool ic = sort_flags & QDir::IgnoreCase;

			if(f1->suffix_cache.isNull())
				f1->suffix_cache = ic ? f1->item.suffix().toLower()
									: f1->item.suffix();
			if(f2->suffix_cache.isNull())
				f2->suffix_cache = ic ? f2->item.suffix().toLower()
									: f2->item.suffix();

			r = sort_flags & QDir::LocaleAware
				? f1->suffix_cache.localeAwareCompare(f2->suffix_cache)
				: f1->suffix_cache.compare(f2->suffix_cache);
		}
			break;
		default:
			break;
	}

	if(r == 0 && sortBy != QDir::Unsorted)
	{
		// Still not sorted - sort by name
		bool ic = sort_flags & QDir::IgnoreCase;

		if(f1->filename_cache.isNull())
			f1->filename_cache = ic ? f1->item.fileName().toLower()
									: f1->item.fileName();
		if(f2->filename_cache.isNull())
			f2->filename_cache = ic ? f2->item.fileName().toLower()
									: f2->item.fileName();

		r = sort_flags & QDir::LocaleAware
			? f1->filename_cache.localeAwareCompare(f2->filename_cache)
			: f1->filename_cache.compare(f2->filename_cache);
	}

	if(sort_flags & QDir::Reversed)
		return r > 0;

	return r < 0;
}

QFileInfoList Dir::sortFileList(QFileInfoList& infos, QDir::SortFlags sort)
{
	QFileInfoList ret;
	int n = infos.size();
	if(n > 0)
	{
		if(n == 1 || ((sort & QDir::SortByMask) == QDir::Unsorted))
		{
			ret = infos;
		}
		else
		{
			DirSortItem* si= new DirSortItem[n];
			for(int i = 0; i < n; ++i)
				si[i].item = infos.at(i);
			qStableSort(si, si + n, DirSortItemComparator(sort));
			// put them back in the list(s)
			for(int i = 0; i < n; ++i)
				ret.append(si[i].item);
			delete [] si;
		}
	}
	return ret;
}
#else
QFileInfoList Dir::sortFileList(QFileInfoList& infos, QDir::SortFlags sort)
{
	return QDir::sortFileList(infos, sort);
}
#endif // QT_VERSION
