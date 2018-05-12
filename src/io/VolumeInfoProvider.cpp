#include "VolumeInfoProvider.h"

VolumeInfoProvider::VolumeInfoProvider (QObject *parent)
    : QObject (parent)
    , storages_ (QStorageInfo::mountedVolumes ())
{
}

VolumeInfoProvider::~VolumeInfoProvider ()
{
}

VolumeInfoProvider::Storages VolumeInfoProvider::volumes () const
{
    return storages_;
}

QString VolumeInfoProvider::volumeLabel (const QString &volume) const
{
    return QStorageInfo (volume).name ();
}

bool VolumeInfoProvider::getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const
{
    const QStorageInfo info (volume);
    *total = info.bytesTotal ();
    *free = info.bytesFree ();
    *available = info.bytesAvailable ();
    return true;
}
