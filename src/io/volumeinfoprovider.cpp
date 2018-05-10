#include "volumeinfoprovider.h"

#ifdef Q_WS_WIN
#  include "volumewatcher_win_p.h"
#else

#  include "volumewatcher_unix_p.h"

#endif

VolumeInfoProvider::VolumeInfoProvider (QObject *parent)
    : QObject (parent)
{
#ifdef Q_WS_WIN
    watcher = new WindowsVolumeWatcher(this);
#else
    watcher = new UnixVolumeWatcher (this);
#endif

    connect (watcher, SIGNAL(volumeAdded (
                                 const QString&)), this, SIGNAL(volumeAdded (
                                                                    const QString&)));
    connect (watcher, SIGNAL(volumeChanged (
                                 const QString&)), this, SIGNAL(volumeChanged (
                                                                    const QString&)));
    connect (watcher, SIGNAL(volumeRemoved (
                                 const QString&)), this, SIGNAL(volumeRemoved (
                                                                    const QString&)));
}

VolumeInfoProvider::~VolumeInfoProvider ()
{
    delete watcher;
}

QFileInfoList VolumeInfoProvider::volumes () const
{
    if (watcher) {
        return watcher->volumes ();
    }
    return QFileInfoList ();
}

QString VolumeInfoProvider::volumeLabel (const QString &volume) const
{
    if (watcher) {
        return watcher->volumeLabel (volume);
    }
    return QString ();
}

bool VolumeInfoProvider::getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const
{
    if (watcher) {
        return watcher->getDiskFreeSpace (volume, total, free, available);
    }
    return false;
}
