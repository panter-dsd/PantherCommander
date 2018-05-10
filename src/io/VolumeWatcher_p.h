#ifndef VOLUMEWATCHER_P_H
#define VOLUMEWATCHER_P_H

#include <QtCore/QAtomicInt>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QObject>

class VolumeWatcher : public QObject
{
Q_OBJECT

public:
    explicit VolumeWatcher (QObject *parent = 0);

    virtual ~VolumeWatcher ();

    virtual QFileInfoList volumes () const;

    virtual QString volumeLabel (const QString &volume) const;

    virtual bool getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const;

Q_SIGNALS:

    void volumeAdded (const QString &volume);

    void volumeChanged (const QString &volume);

    void volumeRemoved (const QString &volume);
};

class VolumeWatcherEngine : public QObject
{
Q_OBJECT

public:
    VolumeWatcherEngine ();

    virtual ~VolumeWatcherEngine ();

    QAtomicInt ref;

Q_SIGNALS:

    void volumeAdded (const QString &volume);

    void volumeChanged (const QString &volume);

    void volumeRemoved (const QString &volume);
};

#endif // VOLUMEWATCHER_P_H
