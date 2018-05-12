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
    explicit VolumeInfoProvider (QObject *parent = nullptr);

    virtual ~VolumeInfoProvider ();

    Storages volumes () const;

    QString volumeLabel (const QString &volume) const;

    bool getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const;

Q_SIGNALS:

    void volumeAdded (const QString &volume);

    void volumeRemoved (const QString &volume);

private:
    Q_DISABLE_COPY (VolumeInfoProvider)

    void checkVolumes ();

private:
    Storages storages_;

    class QTimer *timer_;
};

