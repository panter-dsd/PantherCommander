#include <QtCore/QDateTime>

#include "DirSorter.h"

struct DirSortItem
{
    mutable QString fileNameCache_;
    mutable QString suffixCache_;
    QFileInfo item_;
};

class DirSortItemComparator
{

public:
    DirSortItemComparator (int flags)
        : sortFlags_ (flags)
    {
    }

    int naturalCompare (const QString &s1, const QString &s2, Qt::CaseSensitivity cs);

    bool operator() (const DirSortItem &, const DirSortItem &);

private:
    int sortFlags_;
};

static inline QChar getNextChar (const QString &s, int location)
{
    return (location < s.length ()) ? s.at (location) : QChar ();
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
int DirSortItemComparator::naturalCompare (const QString &s1, const QString &s2, Qt::CaseSensitivity cs)
{
    for (int l1 = 0, l2 = 0; l1 <= s1.count () && l2 <= s2.count (); ++l1, ++l2) {
        // skip spaces, tabs and 0's
        QChar c1 = getNextChar (s1, l1);
        while (c1.isSpace ()) {
            c1 = getNextChar (s1, ++l1);
        }
        QChar c2 = getNextChar (s2, l2);
        while (c2.isSpace ()) {
            c2 = getNextChar (s2, ++l2);
        }

        if (c1.isDigit () && c2.isDigit ()) {
            while (c1.digitValue () == 0) {
                c1 = getNextChar (s1, ++l1);
            }
            while (c2.digitValue () == 0) {
                c2 = getNextChar (s2, ++l2);
            }

            int lookAheadLocation1 = l1;
            int lookAheadLocation2 = l2;
            int currentReturnValue = 0;
            // find the last digit, setting currentReturnValue as we go if it isn't equal
            for (QChar lookAhead1 = c1, lookAhead2 = c2;
                 (lookAheadLocation1 <= s1.length () && lookAheadLocation2 <= s2.length ());
                 lookAhead1 = getNextChar (s1, ++lookAheadLocation1),
                     lookAhead2 = getNextChar (s2, ++lookAheadLocation2)) {
                bool is1ADigit = !lookAhead1.isNull () && lookAhead1.isDigit ();
                bool is2ADigit = !lookAhead2.isNull () && lookAhead2.isDigit ();
                if (!is1ADigit && !is2ADigit) {
                    break;
                }
                if (!is1ADigit) {
                    return -1;
                }
                if (!is2ADigit) {
                    return 1;
                }
                if (currentReturnValue == 0) {
                    if (lookAhead1 < lookAhead2) {
                        currentReturnValue = -1;
                    } else if (lookAhead1 > lookAhead2) {
                        currentReturnValue = 1;
                    }
                }
            }
            if (currentReturnValue != 0) {
                return currentReturnValue;
            }
        }

        if (cs == Qt::CaseInsensitive) {
            if (!c1.isLower ()) {
                c1 = c1.toLower ();
            }
            if (!c2.isLower ()) {
                c2 = c2.toLower ();
            }
        }
        int r = QString::localeAwareCompare (c1, c2);
        if (r < 0) {
            return -1;
        }
        if (r > 0) {
            return 1;
        }
    }
    // The two strings are the same (02 == 2) so fall back to the normal sort
    return QString::compare (s1, s2, cs);
}

static bool isDotOrDotDot (const QString &fname)
{
    const int fnameSize = fname.size ();
    return (fnameSize > 0 && fname[0] == QLatin1Char ('.')
            && (fnameSize == 1 || (fnameSize == 2 && fname[1] == QLatin1Char ('.'))));
}

static QString suffix (const QString &fname)
{
    int pos = fname.lastIndexOf (QLatin1Char ('.'));
    if (pos > 0 && pos < fname.size () - 1) {
        return fname.mid (pos + 1);
    }
    return QLatin1String ("");
}

bool DirSortItemComparator::operator() (const DirSortItem &n1, const DirSortItem &n2)
{
    const DirSortItem *f1 = &n1;
    const DirSortItem *f2 = &n2;

    bool isDir1 = f1->item_.isDir () && f1->item_.exists ();
    bool isDir2 = f2->item_.isDir () && f2->item_.exists ();

    if ((sortFlags_ & QDir::DirsFirst) && isDir1 != isDir2) {
        return isDir1;
    }
    if ((sortFlags_ & QDir::DirsLast) && isDir1 != isDir2) {
        return !isDir1;
    }

    if ((sortFlags_ & QDir::DirsFirst) || (sortFlags_ & QDir::DirsLast)) {
        bool dotOrDotDot1 = isDotOrDotDot (f1->item_.fileName ());
        bool dotOrDotDot2 = isDotOrDotDot (f2->item_.fileName ());
        if (dotOrDotDot1 && dotOrDotDot2) {
            return !(sortFlags_ & QDir::Reversed) && f1->item_.fileName ().size () == 1;
        } else if (dotOrDotDot1 != dotOrDotDot2) {
            return !(sortFlags_ & QDir::Reversed) && dotOrDotDot1;
        }
    }

    int r = 0;
    int sortBy = (sortFlags_ & QDir::SortByMask) | (sortFlags_ & QDir::Type);
    if ((sortFlags_ & QDir::DirsFirst) || (sortFlags_ & QDir::DirsLast)) {
        if (sortFlags_ & 0x100/*QDir::AlwaysSortDirsByName*/ && sortBy != QDir::Name && isDir1 && isDir2) {
            sortBy = QDir::Name;
        }
    }

    switch (sortBy) {
        case QDir::Time:
            r = f2->item_.lastModified ().secsTo (f1->item_.lastModified ());
            break;
        case QDir::Size:
            r = int (qBound<qint64> (-1, f1->item_.size () - f2->item_.size (), 1));
            break;
        case QDir::Type: {
            bool ic = sortFlags_ & QDir::IgnoreCase;

            if (f1->suffixCache_.isNull ()) {
                if (sortFlags_ & 0x300/*QDir::Suffix*/) {
                    f1->suffixCache_ = isDir1 ? QLatin1String ("") :
                                       ic ? suffix (f1->item_.fileName ()).toLower ()
                                          : suffix (f1->item_.fileName ());
                } else {
                    f1->suffixCache_ = ic ? f1->item_.suffix ().toLower ()
                                          : f1->item_.suffix ();
                }
            }
            if (f2->suffixCache_.isNull ()) {
                if (sortFlags_ & 0x300/*QDir::Suffix*/) {
                    f2->suffixCache_ = isDir2 ? QLatin1String ("") :
                                       ic ? suffix (f2->item_.fileName ()).toLower ()
                                          : suffix (f2->item_.fileName ());
                } else {
                    f2->suffixCache_ = ic ? f2->item_.suffix ().toLower ()
                                          : f2->item_.suffix ();
                }
            }

            if (sortFlags_ & 0x200/*QDir::Natural*/) {
                r = naturalCompare (f1->suffixCache_, f2->suffixCache_, Qt::CaseSensitive);
            } else {
                r = sortFlags_ & QDir::LocaleAware
                    ? f1->suffixCache_.localeAwareCompare (f2->suffixCache_)
                    : f1->suffixCache_.compare (f2->suffixCache_);
            }
        }
            break;
        default:
            break;
    }

    if (r == 0 && sortBy != QDir::Unsorted) {
        // Still not sorted - sort by name
        bool ic = sortFlags_ & QDir::IgnoreCase;

        if (f1->fileNameCache_.isNull ()) {
            f1->fileNameCache_ = ic ? f1->item_.fileName ().toLower ()
                                    : f1->item_.fileName ();
        }
        if (f2->fileNameCache_.isNull ()) {
            f2->fileNameCache_ = ic ? f2->item_.fileName ().toLower ()
                                    : f2->item_.fileName ();
        }

        if (sortFlags_ & 0x200/*QDir::Natural*/) {
            r = naturalCompare (f1->fileNameCache_, f2->fileNameCache_, Qt::CaseSensitive);
        } else {
            r = sortFlags_ & QDir::LocaleAware
                ? f1->fileNameCache_.localeAwareCompare (f2->fileNameCache_)
                : f1->fileNameCache_.compare (f2->fileNameCache_);
        }
    }

    if (r == 0) { // Enforce an order - the order the items appear in the array
        r = (&n1) - (&n2);
    }

    if (sortFlags_ & QDir::Reversed) {
        return r > 0;
    }

    return r < 0;
}

QFileInfoList Dir::sortFileList (QFileInfoList &infos, QDir::SortFlags sort)
{
    QFileInfoList result {};
    const int count = infos.size ();
    result.reserve (count);
    if (count > 0) {
        if (count == 1 || ((sort & QDir::SortByMask) == QDir::Unsorted)) {
            result = infos;
        } else {
            DirSortItem *si = new DirSortItem[count];
            for (int i = 0; i < count; ++i) {
                si[i].item_ = infos.at (i);
            }
            qSort (si, si + count, DirSortItemComparator (sort));
            // put them back in the list(s)
            for (int i = 0; i < count; ++i) {
                result.append (si[i].item_);
            }
            delete[] si;
        }
    }
    return result;
}
