#include <qplatformdefs.h>

#include "volumewatcher_unix_p.h"

#include <QtCore/QEvent>
#include <QtCore/QDebug>

#ifndef QT_NO_THREAD

#  include <QtCore/QMutex>

#endif

#include <sys/statvfs.h>

class UnixVolumeWatcherEngine : public VolumeWatcherEngine
{
public:
    UnixVolumeWatcherEngine ();

    virtual ~UnixVolumeWatcherEngine ();

    static QStringList volumes ();

    void timerEvent (QTimerEvent *event);

    QStringList m_drives;
    int m_timerId;
};

static UnixVolumeWatcherEngine *engine = 0;
#ifndef QT_NO_THREAD
static QMutex mutex;
#endif

UnixVolumeWatcherEngine::UnixVolumeWatcherEngine ()
    : VolumeWatcherEngine ()
{
    m_drives = volumes ();
    m_timerId = startTimer (2500);
}

UnixVolumeWatcherEngine::~UnixVolumeWatcherEngine ()
{
    m_drives.clear ();
    killTimer (m_timerId);
    m_timerId = 0;
}

static QStringList subtract (const QStringList &from, const QStringList &other)
{
    QStringList ret (from);
    QStringList::const_iterator it = other.constEnd ();
    while (it != other.constBegin ()) {
        --it;
        if (from.contains (*it)) {
            ret.removeOne (*it);
        }
    }
    return ret;
}

void UnixVolumeWatcherEngine::timerEvent (QTimerEvent *event)
{
    if (event->timerId () == m_timerId) {
        QStringList drives = volumes ();

        QStringList list;
        list = subtract (drives, m_drives);
        for (int i = 0; i < list.size (); ++i)
            emit {
            volumeAdded (list.at (i));
        }
        list = subtract (m_drives, drives);
        for (int i = 0; i < list.size (); ++i)
            emit {
            volumeRemoved (list.at (i));
        }

        m_drives = drives;
    }

    QObject::timerEvent (event);
}

QStringList UnixVolumeWatcherEngine::volumes ()
{
    //###TODO: make thread-safe
    QStringList ret;
    QFile file ("/etc/mtab");
    file.open (QIODevice::ReadOnly);
    for (const QByteArray &line : file.readAll ().split ('\n')) {
        const auto params = line.split (' ');
        if (params.size () > 1) {
            const QString driveName = params.at (1).trimmed ();
            if (!ret.contains (driveName)) {
                ret.append (driveName);
            }
        }
    }
    return ret;
}

UnixVolumeWatcher::UnixVolumeWatcher (QObject *parent)
    : VolumeWatcher (parent)
{
    if (!engine) {
#ifndef QT_NO_THREAD
        QMutexLocker locker (&mutex);
        // check again, since another thread may have already done the initialization
        if (!engine)
#endif
        {
            engine = new UnixVolumeWatcherEngine;
        }
    }
    engine->ref.ref ();

    connect (engine, SIGNAL(volumeAdded (
                                const QString&)), this, SIGNAL(volumeAdded (
                                                                   const QString&)));
    connect (engine, SIGNAL(volumeChanged (
                                const QString&)), this, SIGNAL(volumeChanged (
                                                                   const QString&)));
    connect (engine, SIGNAL(volumeRemoved (
                                const QString&)), this, SIGNAL(volumeRemoved (
                                                                   const QString&)));
}

UnixVolumeWatcher::~UnixVolumeWatcher ()
{
    if (engine) {
#ifndef QT_NO_THREAD
        QMutexLocker locker (&mutex);
        // check again, since another thread may have already done the clean-up
        if (engine) {
#endif
            if (!engine->ref.deref ()) {
                delete engine;
                engine = 0;
            }
#ifndef QT_NO_THREAD
        }
#endif
    }
}

QFileInfoList UnixVolumeWatcher::volumes () const
{
    QFileInfoList ret;
        foreach(const QString &volume, engine->m_drives) {
            ret.append (QFileInfo (volume));
        }
    return ret;
}

bool UnixVolumeWatcher::getDiskFreeSpace (const QString &volume, qint64 *total, qint64 *free, qint64 *available) const
{
    struct statvfs st;
    if (statvfs (QFile::encodeName (volume).data (), &st) == 0) {
        if (total) {
            *total = st.f_frsize * st.f_blocks;
        }
        if (free) {
            *free = st.f_frsize * st.f_bfree;
        }
        if (available) {
            *available = st.f_frsize * st.f_bavail;
        }

        return true;
    }

    return false;
}
