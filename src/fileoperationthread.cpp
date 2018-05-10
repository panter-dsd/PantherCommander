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
* Author:		PanteR
* Contact:	panter.dsd@gmail.com
*******************************************************************/

#include <QtCore/QMutex>
#include <QtCore/QFileInfo>

#include "fileoperationthread.h"
#include "fileoperationthread_p.h"

FileOperationThreadPrivate::FileOperationThreadPrivate ()
    : q_ptr (0)
    , currentID (0)
{

}

FileOperationThreadPrivate::~FileOperationThreadPrivate ()
{
    q_ptr = 0;
}

void FileOperationThreadPrivate::nextOperation ()
{
    if (currentID >= operationList.size ()) {
        return;
    }

    FileOperation fo = operationList.at (++currentID);

    switch (fo.type ()) {
        case FileOperation::Copy: {
            copy (&fo);
            break;
        }
    }
}

void FileOperationThreadPrivate::copy (FileOperation *operation)
{
    CopyFileOperation *cfo = static_cast<CopyFileOperation *> (operation);

        foreach(const QString &source, cfo->getSources ()) {
            if (QFileInfo (source).isDir ()) {
                copyDir (source, cfo->getDest ());
            } else {
                copyFile (source, cfo->getDest ());
            }
        }
}

void FileOperationThreadPrivate::copyDir (const QString &source, const QString &dest)
{

}

void FileOperationThreadPrivate::copyFile (const QString &source, const QString &dest)
{

}

FileOperationThread::FileOperationThread (QObject *parent)
    : QThread (parent)
    , d_ptr (new FileOperationThreadPrivate)
{
}

FileOperationThread::~FileOperationThread ()
{
    delete d_ptr;
}

int FileOperationThread::addOperation (const FileOperation &operation)
{
    Q_D(FileOperationThread);

    QMutexLocker l (&d->mutex);
    d->operationList.append (operation);
    return d->operationList.size () - 1;
}

bool FileOperationThread::removeOperation (int ID)
{
    Q_D(FileOperationThread);

    if (d->currentID < ID && ID > 0 && ID < d->operationList.size ()) {
        d->operationList.removeAt (ID);
        return true;
    }

    return false;
}

bool FileOperationThread::insertOperation (int ID, const FileOperation &operation)
{
    Q_D(FileOperationThread);

    if (d->currentID < ID && ID > 0 && ID <= d->operationList.size ()) {
        d->operationList.insert (ID, operation);
        return true;
    }

    return false;
}

FileOperation FileOperationThread::operation (int ID) const
{
    Q_D(const FileOperationThread);

    if (ID > 0 && ID < d->operationList.size ()) {
        d->operationList.at (ID);
    }

    return FileOperation (FileOperation::None, 0);
}

void FileOperationThread::run ()
{
    Q_D(FileOperationThread);

    bool stop = false;

    while (!stop) {
        d->mutex.lock ();
        if (d->currentID == d->operationList.size ()) {
            stop = true;
        }
        d->mutex.unlock ();
    }
    deleteLater ();
}
