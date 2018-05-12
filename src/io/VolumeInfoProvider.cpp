#include <QtCore/QTimer>

#include "VolumeInfoProvider.h"

VolumeInfoProvider::VolumeInfoProvider (QObject *parent)
    : QObject (parent)
    , storages_ (QStorageInfo::mountedVolumes ())
    , timer_ (new QTimer (this))
{
    connect (timer_, &QTimer::timeout, this, &VolumeInfoProvider::checkVolumes);
    timer_->setSingleShot (false);
    timer_->start (100);
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

void VolumeInfoProvider::checkVolumes ()
{
    const Storages storages = QStorageInfo::mountedVolumes ();
    if (storages != storages_) {
        for (const QStorageInfo &info : storages) {
            if (!storages_.contains (info)) {
                emit volumeAdded (info.rootPath ());
            }
        }
        for (const QStorageInfo &info : storages_) {
            if (!storages.contains (info)) {
                emit volumeRemoved (info.rootPath ());
            }
        }

        storages_ = storages;
    }
}
