#ifndef DIRSORTER_H
#define DIRSORTER_H

#include <QtCore/QDir>
#include <QtCore/QList>

class QFileInfo;

typedef QList<QFileInfo> QFileInfoList;

struct Dir
{
    static QFileInfoList sortFileList (QFileInfoList &infos, QDir::SortFlags sort);
};

#endif // DIRSORTER_H
