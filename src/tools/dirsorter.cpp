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
	DirSortItemComparator(int flags) : sort_flags(flags)
	{}

	int naturalCompare(const QString &s1, const QString &s2, Qt::CaseSensitivity cs);
	bool operator()(const DirSortItem&, const DirSortItem&);
};

static inline QChar getNextChar(const QString &s, int location)
{
	return (location < s.length()) ? s.at(location) : QChar();
}

/*!
	Natural number sort, skips spaces.

	Examples:
	1, 2, 10, 55, 100
	01.jpg, 2.jpg, 10.jpg

	Note on the algorithm:
	Only as many characters as necessary are looked at and at most they all
	are looked at once.

	Slower then QString::compare() (of course)
*/
int DirSortItemComparator::naturalCompare(const QString& s1, const QString& s2, Qt::CaseSensitivity cs)
{
	for(int l1 = 0, l2 = 0; l1 <= s1.count() && l2 <= s2.count(); ++l1, ++l2)
	{
		// skip spaces, tabs and 0's
		QChar c1 = getNextChar(s1, l1);
		while(c1.isSpace())
			c1 = getNextChar(s1, ++l1);
		QChar c2 = getNextChar(s2, l2);
		while(c2.isSpace())
			c2 = getNextChar(s2, ++l2);

		if(c1.isDigit() && c2.isDigit())
		{
			while(c1.digitValue() == 0)
				c1 = getNextChar(s1, ++l1);
			while(c2.digitValue() == 0)
				c2 = getNextChar(s2, ++l2);

			int lookAheadLocation1 = l1;
			int lookAheadLocation2 = l2;
			int currentReturnValue = 0;
			// find the last digit, setting currentReturnValue as we go if it isn't equal
			for(QChar lookAhead1 = c1, lookAhead2 = c2;
				(lookAheadLocation1 <= s1.length() && lookAheadLocation2 <= s2.length());
				lookAhead1 = getNextChar(s1, ++lookAheadLocation1),
				lookAhead2 = getNextChar(s2, ++lookAheadLocation2))
			{
				bool is1ADigit = !lookAhead1.isNull() && lookAhead1.isDigit();
				bool is2ADigit = !lookAhead2.isNull() && lookAhead2.isDigit();
				if(!is1ADigit && !is2ADigit)
					break;
				if(!is1ADigit)
					return -1;
				if(!is2ADigit)
					return 1;
				if(currentReturnValue == 0)
				{
					if(lookAhead1 < lookAhead2)
						currentReturnValue = -1;
					else if(lookAhead1 > lookAhead2)
						currentReturnValue = 1;
				}
			}
			if(currentReturnValue != 0)
				return currentReturnValue;
		}

		if(cs == Qt::CaseInsensitive)
		{
			if(!c1.isLower())
				c1 = c1.toLower();
			if(!c2.isLower())
				c2 = c2.toLower();
		}
		int r = QString::localeAwareCompare(c1, c2);
		if(r < 0)
			return -1;
		if(r > 0)
			return 1;
	}
	// The two strings are the same (02 == 2) so fall back to the normal sort
	return QString::compare(s1, s2, cs);
}

static bool isDotOrDotDot(const QString& fname)
{
	const int fnameSize = fname.size();
	return (fnameSize > 0 && fname[0] == QLatin1Char('.')
			&& (fnameSize == 1 || (fnameSize == 2 && fname[1] == QLatin1Char('.'))));
}

static QString suffix(const QString& fname)
{
	int pos = fname.lastIndexOf(QLatin1Char('.'));
	if(pos > 0 && pos < fname.size() - 1)
		return fname.mid(pos + 1);
	return QLatin1String("");
}

bool DirSortItemComparator::operator()(const DirSortItem& n1, const DirSortItem& n2)
{
	const DirSortItem* f1 = &n1;
	const DirSortItem* f2 = &n2;

	bool isF1Dir = f1->item.isDir() && !f1->item.isSymLink();
	bool isF2Dir = f2->item.isDir() && !f2->item.isSymLink();

	if((sort_flags & QDir::DirsFirst) && (isF1Dir != isF2Dir))
		return isF1Dir;
	if((sort_flags & QDir::DirsLast) && (isF1Dir != isF2Dir))
		return !isF1Dir;

	if((sort_flags & QDir::DirsFirst) || (sort_flags & QDir::DirsLast))
	{
		bool dotOrDotDot1 = isDotOrDotDot(f1->item.fileName());
		bool dotOrDotDot2 = isDotOrDotDot(f2->item.fileName());
		if(dotOrDotDot1 && dotOrDotDot2)
			return !(sort_flags & QDir::Reversed) && f1->item.fileName().size() == 1;
		else if(dotOrDotDot1 != dotOrDotDot2)
			return !(sort_flags & QDir::Reversed) && dotOrDotDot1;
	}

	int r = 0;
	int sortBy = (sort_flags & QDir::SortByMask) | (sort_flags & QDir::Type);
	if((sort_flags & QDir::DirsFirst) || (sort_flags & QDir::DirsLast))
	{
		if(sort_flags & 0x100/*QDir::AlwaysSortDirsByName*/ && sortBy != QDir::Name
			&& isF1Dir && isF2Dir)
		{
			sortBy = QDir::Name;
		}
	}

	switch(sortBy)
	{
		case QDir::Time:
			r = f2->item.lastModified().secsTo(f1->item.lastModified());
			break;
		case QDir::Size:
			r = int(qBound<qint64>(-1, f1->item.size() - f2->item.size(), 1));
			break;
		case QDir::Type:
		{
			bool ic = sort_flags & QDir::IgnoreCase;

			if(f1->suffix_cache.isNull())
			{
				if(sort_flags & 0x300/*QDir::Suffix*/)
				{
					f1->suffix_cache = isF1Dir ? QLatin1String("") :
														ic ? suffix(f1->item.fileName()).toLower()
															: suffix(f1->item.fileName());
				}
				else
				{
					f1->suffix_cache = ic ? f1->item.suffix().toLower()
										: f1->item.suffix();
				}
			}
			if(f2->suffix_cache.isNull())
			{
				if(sort_flags & 0x300/*QDir::Suffix*/)
				{
					f2->suffix_cache = isF2Dir ? QLatin1String("") :
														ic ? suffix(f2->item.fileName()).toLower()
															: suffix(f2->item.fileName());
				}
				else
				{
					f2->suffix_cache = ic ? f2->item.suffix().toLower()
										: f2->item.suffix();
				}
			}

			if(sort_flags & 0x200/*QDir::Natural*/)
			{
				r = naturalCompare(f1->suffix_cache, f2->suffix_cache, Qt::CaseSensitive);
			}
			else
			{
				r = sort_flags & QDir::LocaleAware
					? f1->suffix_cache.localeAwareCompare(f2->suffix_cache)
					: f1->suffix_cache.compare(f2->suffix_cache);
			}
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
		{
			f1->filename_cache = ic ? f1->item.fileName().toLower()
									: f1->item.fileName();
		}
		if(f2->filename_cache.isNull())
		{
			f2->filename_cache = ic ? f2->item.fileName().toLower()
									: f2->item.fileName();
		}

		if(sort_flags & 0x200/*QDir::Natural*/)
		{
			r = naturalCompare(f1->filename_cache, f2->filename_cache, Qt::CaseSensitive);
		}
		else
		{
			r = sort_flags & QDir::LocaleAware
				? f1->filename_cache.localeAwareCompare(f2->filename_cache)
				: f1->filename_cache.compare(f2->filename_cache);
		}
	}

	if(r == 0) // Enforce an order - the order the items appear in the array
		r = (&n1) - (&n2);

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
			qSort(si, si + n, DirSortItemComparator(sort));
			// put them back in the list(s)
			for(int i = 0; i < n; ++i)
				ret.append(si[i].item);
			delete [] si;
		}
	}
	return ret;
}
