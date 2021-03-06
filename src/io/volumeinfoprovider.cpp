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

#include "volumeinfoprovider.h"

#ifdef Q_WS_WIN
#  include "volumewatcher_win_p.h"
#else
#  include "volumewatcher_unix_p.h"
#endif

VolumeInfoProvider::VolumeInfoProvider(QObject* parent) : QObject(parent)
{
#ifdef Q_WS_WIN
	watcher = new WindowsVolumeWatcher(this);
#else
	watcher = new UnixVolumeWatcher(this);
#endif

	connect(watcher, SIGNAL(volumeAdded(const QString&)), this, SIGNAL(volumeAdded(const QString&)));
	connect(watcher, SIGNAL(volumeChanged(const QString&)), this, SIGNAL(volumeChanged(const QString&)));
	connect(watcher, SIGNAL(volumeRemoved(const QString&)), this, SIGNAL(volumeRemoved(const QString&)));
}

VolumeInfoProvider::~VolumeInfoProvider()
{
	delete watcher;
}

QFileInfoList VolumeInfoProvider::volumes() const
{
	if(watcher)
		return watcher->volumes();
	return QFileInfoList();
}

QString VolumeInfoProvider::volumeLabel(const QString& volume) const
{
	if(watcher)
		return watcher->volumeLabel(volume);
	return QString();
}

bool VolumeInfoProvider::getDiskFreeSpace(const QString& volume, qint64* total, qint64* free, qint64* available) const
{
	if(watcher)
		return watcher->getDiskFreeSpace(volume, total, free, available);
	return false;
}
