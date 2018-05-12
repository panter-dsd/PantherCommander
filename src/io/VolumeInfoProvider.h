#pragma once

#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QStorageInfo>

class VolumeInfoProvider : public QObject
{
Q_OBJECT
public:
    using Storages = QList<QStorageInfo>;

public:
    explicit VolumeInfoProvider (QObject *parent = 0);

    virtual ~VolumeInfoProvider ();

    virtual Storages volumes () const;

    virtual QString volumeLabel (const QString &volume) const;

    virtual bool getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const;

Q_SIGNALS:

    void volumeAdded (const QString &volume);

    void volumeChanged (const QString &volume);

    void volumeRemoved (const QString &volume);

private:
    Q_DISABLE_COPY (VolumeInfoProvider)

private:
    Storages storages_;
};

