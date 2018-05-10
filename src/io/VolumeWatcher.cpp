#include "VolumeWatcher_p.h"

VolumeWatcherEngine::VolumeWatcherEngine ()
    : QObject ()
    , ref (0)
{
}

VolumeWatcherEngine::~VolumeWatcherEngine ()
{
    if (ref > 0)
        qWarning ("~VolumeWatcherEngine: instance still in use (%d references)", int (ref));
}

VolumeWatcher::VolumeWatcher (QObject *parent)
    : QObject (parent)
{
}

VolumeWatcher::~VolumeWatcher ()
{
}

QFileInfoList VolumeWatcher::volumes () const
{
    return QFileInfoList ();
}

QString VolumeWatcher::volumeLabel (const QString &volume) const
{
    Q_UNUSED(volume)
    return QString ();
}

bool VolumeWatcher::getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const
{
    Q_UNUSED(volume)
    Q_UNUSED(total)
    Q_UNUSED(free)
    Q_UNUSED(available)
    return false;
}
