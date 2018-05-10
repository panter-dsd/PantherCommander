#pragma once

#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QObject>

class VolumeWatcher;

class VolumeInfoProvider : public QObject
{
Q_OBJECT

public:
    explicit VolumeInfoProvider (QObject *parent = 0);

    virtual ~VolumeInfoProvider ();

    virtual QFileInfoList volumes () const;

    virtual QString volumeLabel (const QString &volume) const;

    virtual bool getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const;

Q_SIGNALS:

    void volumeAdded (const QString &volume);

    void volumeChanged (const QString &volume);

    void volumeRemoved (const QString &volume);

protected:
    VolumeWatcher *watcher;
};

