#pragma once

#include "VolumeWatcher_p.h"

class WindowsVolumeWatcher : public VolumeWatcher
{
Q_OBJECT

public:
    explicit WindowsVolumeWatcher (QObject *parent = 0);

    virtual ~WindowsVolumeWatcher ();

    QFileInfoList volumes () const;

    QString volumeLabel (const QString &volume) const;

    bool getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const;
};