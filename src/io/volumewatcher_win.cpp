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

#include "volumewatcher_win_p.h"

#include <QtCore/QDir>
#include <QtCore/QMetaObject>
#ifndef QT_NO_THREAD
#  include <QtCore/QMutex>
#endif
#include <QtCore/QStringList>

#define _WIN32_WINNT  0x0500
#include <qt_windows.h>
#include <dbt.h>

#include "qfileoperationsthread.h"

#ifndef DBT_CUSTOMEVENT
#  define DBT_CUSTOMEVENT 0x8006
#endif

static VolumeWatcherEngine* engine = 0;
#ifndef QT_NO_THREAD
static QMutex mutex;
#endif

Q_CORE_EXPORT HINSTANCE qWinAppInst();

static QStringList drivesFromMask(quint32 driveBits)
{
	QStringList ret;

	char driveName[] = "A:/";
	driveBits = (driveBits & 0x3ffffff);
	while(driveBits)
	{
		if(driveBits & 0x1)
			ret.append(QString::fromLatin1(driveName));
		++driveName[0];
		driveBits = driveBits >> 1;
	}

	return ret;
}

LRESULT CALLBACK vw_internal_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_DEVICECHANGE)
	{
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
		switch(wParam)
		{
			case DBT_DEVNODES_CHANGED:
				qWarning("DBT_DEVNODES_CHANGED message received, no extended info.");
				break;

			case DBT_QUERYCHANGECONFIG:
				qWarning("DBT_QUERYCHANGECONFIG message received, no extended info.");
				break;
			case DBT_CONFIGCHANGED:
				qWarning("DBT_CONFIGCHANGED message received, no extended info.");
				break;
			case DBT_CONFIGCHANGECANCELED:
				qWarning("DBT_CONFIGCHANGECANCELED message received, no extended info.");
				break;

			case DBT_DEVICEARRIVAL:
			case DBT_DEVICEQUERYREMOVE:
			case DBT_DEVICEQUERYREMOVEFAILED:
			case DBT_DEVICEREMOVEPENDING:
			case DBT_DEVICEREMOVECOMPLETE:
				if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
				{
					DEV_BROADCAST_VOLUME* db_volume = (DEV_BROADCAST_VOLUME*)lpdb;
					const QStringList& drives = drivesFromMask(db_volume->dbcv_unitmask);
					if(wParam == DBT_DEVICEARRIVAL)
					{
						foreach(const QString& drive, drives)
						{
							if(db_volume->dbcv_flags & DBTF_MEDIA)
								qWarning("Drive %c: Media has been arrived.", drive.at(0).toAscii());
							else if(db_volume->dbcv_flags & DBTF_NET)
								qWarning("Drive %c: Network share has been mounted.", drive.at(0).toAscii());
							else
								qWarning("Drive %c: Device has been added.", drive.at(0).toAscii());
						}
						QMetaObject::invokeMethod(engine, "volumesChanged", Qt::QueuedConnection);
					}
					else if(wParam == DBT_DEVICEQUERYREMOVE)
					{
					}
					else if(wParam == DBT_DEVICEQUERYREMOVEFAILED)
					{
					}
					else if(wParam == DBT_DEVICEREMOVEPENDING)
					{
					}
					else if(wParam == DBT_DEVICEREMOVECOMPLETE)
					{
						foreach(const QString& drive, drives)
						{
							if(db_volume->dbcv_flags & DBTF_MEDIA)
								qWarning("Drive %c: Media has been removed.", drive.at(0).toAscii());
							else if(db_volume->dbcv_flags & DBTF_NET)
								qWarning("Drive %c: Network share has been unmounted.", drive.at(0).toAscii());
							else
								qWarning("Drive %c: Device has been removed.", drive.at(0).toAscii());
						}
						QMetaObject::invokeMethod(engine, "volumesChanged", Qt::QueuedConnection);
					}
				}
				break;
			case DBT_DEVICETYPESPECIFIC:
				qWarning("DBT_DEVICETYPESPECIFIC message received, can contain extended info.");
				break;
			case DBT_CUSTOMEVENT:
				qWarning("DBT_CUSTOMEVENT message received, contains extended info.");
				break;
			case DBT_USERDEFINED:
				qWarning("WM_DEVICECHANGE userdefined message received, can not handle.");
				break;
			default:
				qWarning("WM_DEVICECHANGE message received, unhandled value %d.", wParam);
				break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

static HWND vw_create_internal_window(const void* userData)
{
	QString className = QLatin1String("VolumeWatcher_Internal_Widget") + QString::number(quintptr(vw_internal_proc));

	HINSTANCE hi = qWinAppInst();

	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = vw_internal_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hi;
	wc.hIcon = 0;
	wc.hCursor = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = reinterpret_cast<const wchar_t*>(className.utf16());
	RegisterClass(&wc);

	HWND wnd = CreateWindow(wc.lpszClassName,	// classname
							wc.lpszClassName,	// window name
							0,					// style
							0, 0, 0, 0,			// geometry
							0,					// parent
							0,					// menu handle
							hi,					// application
							0);					// windows creation data.
	if(!wnd)
	{
		qWarning("WindowsVolumeWatcherEngine: Failed to create internal window: %d", (int)GetLastError());
	}
	else if(userData)
	{
#ifdef GWLP_USERDATA
		SetWindowLongPtrA(wnd, GWLP_USERDATA, (LONG_PTR)userData);
#else
		SetWindowLongA(wnd, GWL_USERDATA, (LONG)userData);
#endif
	}
	return wnd;
}

static void vw_destroy_internal_window(HWND wnd)
{
	if(wnd)
		DestroyWindow(wnd);
	QString className = QLatin1String("VolumeWatcher_Internal_Widget") + QString::number(quintptr(vw_internal_proc));
	UnregisterClass((wchar_t*)className.utf16(), qWinAppInst());
}


class WindowsVolumeWatcherEngine : public VolumeWatcherEngine
{
public:
	WindowsVolumeWatcherEngine();
	virtual ~WindowsVolumeWatcherEngine();

	HWND hwnd;
};

WindowsVolumeWatcherEngine::WindowsVolumeWatcherEngine() : VolumeWatcherEngine()
{
	hwnd = vw_create_internal_window(this);
}

WindowsVolumeWatcherEngine::~WindowsVolumeWatcherEngine()
{
	vw_destroy_internal_window(hwnd);
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
#if 0
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
#endif
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
			if(GetLastError() == ERROR_NO_VOLUME_LABEL)
				return tr("_NO_LABEL_");
		}
	}
	return tr("_ERROR_GETTING_LABEL_");
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
