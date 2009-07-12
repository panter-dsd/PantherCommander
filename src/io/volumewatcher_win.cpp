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

#define _WIN32_WINNT 0x0501
#include <qplatformdefs.h>

#include "volumewatcher_win_p.h"

#include <QtCore/QAbstractEventDispatcher>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtCore/QMetaObject>
#ifndef QT_NO_THREAD
#  include <QtCore/QMutex>
#endif

#include "qfileoperationsthread.h"

static VolumeWatcherEngine* engine = 0;
#ifndef QT_NO_THREAD
static QMutex mutex;
#endif

static bool bFilterOverrided = false;
static QAbstractEventDispatcher::EventFilter defaultEventFilter = 0;

WindowsVolumeWatcherEngine::WindowsVolumeWatcherEngine() : VolumeWatcherEngine()
{
	QAbstractEventDispatcher* dispatcher = QAbstractEventDispatcher::instance();
	if(dispatcher)
	{
		QAbstractEventDispatcher::EventFilter filter = 0;
		filter = dispatcher->setEventFilter(nativeEventFilter);
		if(filter && filter != nativeEventFilter)
		{
			if(!bFilterOverrided)
			{
				bFilterOverrided = true;
				defaultEventFilter = filter;
			}
			else
			{
				// filter was changed after it was overrided by us.
				// assume our eventFilter still in the filter sequence
				(void)dispatcher->setEventFilter(filter);
			}
		}
	}
}

WindowsVolumeWatcherEngine::~WindowsVolumeWatcherEngine()
{
	QAbstractEventDispatcher* dispatcher = QAbstractEventDispatcher::instance();
	if(dispatcher)
	{
		QAbstractEventDispatcher::EventFilter filter = 0;
		filter = dispatcher->setEventFilter(defaultEventFilter);
		if(filter != nativeEventFilter)
		{
			// filter was changed after it was overrided by us.
			// assume original eventFilter still in the filter sequence
			(void)dispatcher->setEventFilter(filter);
		}
	}
}

bool WindowsVolumeWatcherEngine::nativeEventFilter(void* message)
{
	MSG* msg = reinterpret_cast<MSG*>(message);
	if(msg && msg->message == WM_DEVICECHANGE)
	{
		//###TODO: implement accuracy removables detection
		if(engine)
			QMetaObject::invokeMethod(engine, "volumesChanged", Qt::QueuedConnection);
	}
	return (defaultEventFilter ? defaultEventFilter(message) : false);
}


WindowsVolumeWatcher::WindowsVolumeWatcher(QObject* parent) : VolumeWatcher(parent)
{
	if(!engine)
	{
#ifndef QT_NO_THREAD
		QMutexLocker locker(&mutex);
		// check again, since another thread may have already done the initialization
		if(!engine)
#endif
			engine = new WindowsVolumeWatcherEngine;
	}
	engine->ref.ref();

	connect(engine, SIGNAL(volumesChanged()), this, SIGNAL(volumesChanged()));
}

WindowsVolumeWatcher::~WindowsVolumeWatcher()
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

QFileInfoList WindowsVolumeWatcher::volumes() const
{
	QFileInfoList ret;
	ret.append(QDir::drives());

	// add volume mount points (aka `mounted dirs')
	wchar_t volumeMountPoint[MAX_PATH];
	for(int i = 0, n = ret.size(); i < n; ++i)
	{
		QString drive = ret.at(i).filePath();
		HANDLE handle = FindFirstVolumeMountPoint((wchar_t*)QDir::toNativeSeparators(drive).utf16(),
													volumeMountPoint, MAX_PATH);
		while(handle != INVALID_HANDLE_VALUE)
		{
			QString path = drive;
			path.append(QString::fromWCharArray(volumeMountPoint));
			path.chop(1);
			QFileInfo fi(path);
			ret.append(fi);

			if(!FindNextVolumeMountPoint(handle, volumeMountPoint, MAX_PATH))
			{
				FindVolumeMountPointClose(handle);
				handle = INVALID_HANDLE_VALUE;
			}
		}
	}
	return ret;
}

static QString _rootPath(const QString& path)
{
	wchar_t buf[256];
	DWORD bufSize = 255;
	if(GetVolumePathName((wchar_t*)path.utf16(), buf, bufSize))
		return QString::fromWCharArray(buf);
	return QString();
}

QString WindowsVolumeWatcher::volumeLabel(const QString& volume) const
{
	if(QFileOperationsThread::isLocalFileSystem(volume))
	{
		QString path = _rootPath(volume);
		if(!path.isEmpty())
		{
			wchar_t buf[101];
			DWORD bufSize = 100;
			if(GetVolumeInformation((wchar_t*)path.utf16(), buf, bufSize, 0, 0, 0, 0, 0))
				return QString::fromWCharArray(buf);
		}
		return tr("_ERROR_GETTING_LABEL_");
	}
	return tr("_NO_LABEL_");
}

bool WindowsVolumeWatcher::getDiskFreeSpace(const QString& volume, qint64* total, qint64* free, qint64* available) const
{
	qint64 bytesUserFree, bytesTotalSize, bytesTotalFree;
	if(GetDiskFreeSpaceEx((wchar_t*)volume.utf16(),
							(PULARGE_INTEGER)&bytesUserFree,
							(PULARGE_INTEGER)&bytesTotalSize,
							(PULARGE_INTEGER)&bytesTotalFree))
	{
		if(total)
			*total = bytesTotalSize;
		if(free)
			*free = bytesTotalFree;
		if(available)
			*available = bytesUserFree;

		return true;
	}

	return false;
}
