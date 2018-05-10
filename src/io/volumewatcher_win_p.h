#ifndef VOLUMEWATCHER_WIN_P_H
#define VOLUMEWATCHER_WIN_P_H

#include "volumewatcher_p.h"

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

#endif // VOLUMEWATCHER_WIN_P_H
