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
