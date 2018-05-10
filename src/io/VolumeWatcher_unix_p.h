#pragma once

#include "VolumeWatcher_p.h"

class UnixVolumeWatcher : public VolumeWatcher
{
Q_OBJECT

public:
    explicit UnixVolumeWatcher (QObject *parent = 0);

    virtual ~UnixVolumeWatcher ();

    QFileInfoList volumes () const;

    bool getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const;
};

