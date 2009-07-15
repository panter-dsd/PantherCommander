/********************************************************************
* Copyright (C) PanteR
*-------------------------------------------------------------------
*
* Panther Commander is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* Panther Commander is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Panther Commander; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301 USA
*-------------------------------------------------------------------
* Project:		Panther Commander
* Author:		Ritt K.
* Contact:		ritt.ks@gmail.com
*******************************************************************/

#include <qplatformdefs.h>

#include "volumewatcher_unix_p.h"

#include <QtCore/QEvent>
#include <QtCore/QFile>
#ifndef QT_NO_THREAD
#  include <QtCore/QMutex>
#endif
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <sys/statvfs.h>
#include <sys/types.h>

static VolumeWatcherEngine* engine = 0;
#ifndef QT_NO_THREAD
static QMutex mutex;
#endif

UnixVolumeWatcherEngine::UnixVolumeWatcherEngine() : VolumeWatcherEngine()
{
	m_timerId = 0;
}

UnixVolumeWatcherEngine::~UnixVolumeWatcherEngine()
{
	killTimer(m_timerId);
	m_timerId = 0;
}

void UnixVolumeWatcherEngine::connectNotify(const char* signal)
{
	if(QLatin1String(signal) == SIGNAL(volumesChanged())
		&& receivers(SIGNAL(volumesChanged())) == 1)
	{
		m_drivesCount = volumes().count();
		m_timerId = startTimer(2500);
	}
}

void UnixVolumeWatcherEngine::disconnectNotify(const char* signal)
{
	if(QLatin1String(signal) == SIGNAL(volumesChanged())
		&& receivers(SIGNAL(volumesChanged())) == 0/* 0! */)
	{
		killTimer(m_timerId);
		m_timerId = 0;
	}
}

void UnixVolumeWatcherEngine::timerEvent(QTimerEvent* event)
{
	if(event->timerId() == m_timerId)
	{
		int drivesCount = volumes().count();
		if(m_drivesCount != drivesCount)
		{
			m_drivesCount = drivesCount;
			emit volumesChanged();
		}
	}

	QObject::timerEvent(event);
}


QFileInfoList UnixVolumeWatcherEngine::volumes()
{
	//###TODO: make thread-safe
	QFileInfoList ret;
	QFile file("/etc/mtab");
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		while(!stream.atEnd())
		{
			QStringList params = stream.readLine().split(QLatin1Char(' '));
			if(params.size() > 1)
			{
				QFileInfo fi(params.at(1));
				ret.append(fi);
			}
		}
		file.close();
	}
	return ret;
}


UnixVolumeWatcher::UnixVolumeWatcher(QObject* parent) : VolumeWatcher(parent)
{
	if(!engine)
	{
#ifndef QT_NO_THREAD
		QMutexLocker locker(&mutex);
		// check again, since another thread may have already done the initialization
		if(!engine)
#endif
			engine = new UnixVolumeWatcherEngine;
	}
	engine->ref.ref();
}

UnixVolumeWatcher::~UnixVolumeWatcher()
{
	if(engine)
	{
#ifndef QT_NO_THREAD
		QMutexLocker locker(&mutex);
		// check again, since another thread may have already done the clean-up
		if(engine)
		{
#endif
			if(!engine->ref.deref())
			{
				delete engine;
				engine = 0;
			}
#ifndef QT_NO_THREAD
		}
#endif
	}
}

QFileInfoList UnixVolumeWatcher::volumes() const
{
	return UnixVolumeWatcherEngine::volumes();
}

bool UnixVolumeWatcher::getDiskFreeSpace(const QString& volume, qint64* total, qint64* free, qint64* available) const
{
	struct statvfs st;
	if(statvfs(QFile::encodeName(volume).data(), &st) == 0)
	{
		if(total)
			*total = st.f_frsize * st.f_blocks;
		if(free)
			*free = st.f_frsize * st.f_bfree;
		if(available)
			*available = st.f_frsize * st.f_bavail;

		return true;
	}

	return false;
}

void UnixVolumeWatcher::connectNotify(const char* signal)
{
	if(engine && QLatin1String(signal) == SIGNAL(volumesChanged())
		&& receivers(SIGNAL(volumesChanged())) == 1)
	{
		connect(engine, SIGNAL(volumesChanged()), this, SIGNAL(volumesChanged()));
	}
}

void UnixVolumeWatcher::disconnectNotify(const char* signal)
{
	if(engine && QLatin1String(signal) == SIGNAL(volumesChanged())
		&& receivers(SIGNAL(volumesChanged())) == 1/* 1; not 0! */)
	{
		disconnect(engine, SIGNAL(volumesChanged()), this, SIGNAL(volumesChanged()));
	}
}
