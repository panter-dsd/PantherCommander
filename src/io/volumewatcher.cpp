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

#include "volumewatcher_p.h"

VolumeWatcherEngine::VolumeWatcherEngine() : QObject(), ref(0)
{
}

VolumeWatcherEngine::~VolumeWatcherEngine()
{
	if(ref > 0)
		qWarning("~VolumeWatcherEngine: instance still in use (%d references)", int(ref));
}


VolumeWatcher::VolumeWatcher(QObject* parent) : QObject(parent)
{
}

VolumeWatcher::~VolumeWatcher()
{
}

QFileInfoList VolumeWatcher::volumes() const
{
	return QFileInfoList();
}

QString VolumeWatcher::volumeLabel(const QString& volume) const
{
	Q_UNUSED(volume)
	return QString();
}

bool VolumeWatcher::getDiskFreeSpace(const QString& volume, qint64* total, qint64* free, qint64* available) const
{
	Q_UNUSED(volume)
	Q_UNUSED(total)
	Q_UNUSED(free)
	Q_UNUSED(available)
	return false;
}
