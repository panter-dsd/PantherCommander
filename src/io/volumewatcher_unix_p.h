#ifndef VOLUMEWATCHER_UNIX_P_H
#define VOLUMEWATCHER_UNIX_P_H

#include "volumewatcher_p.h"

class UnixVolumeWatcher : public VolumeWatcher
{
Q_OBJECT

public:
    explicit UnixVolumeWatcher (QObject *parent = 0);

    virtual ~UnixVolumeWatcher ();

    QFileInfoList volumes () const;

    bool getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const;
};

#endif // VOLUMEWATCHER_UNIX_P_H
