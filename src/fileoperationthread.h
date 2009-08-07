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

#ifndef FILEOPERATIONTHREAD_H
#define FILEOPERATIONTHREAD_H

#include <QThread>

class FileOperation;
class FileOperationThreadPrivate;
class FileOperation;

class FileOperationThread : public QThread
{
	Q_OBJECT

public:
	explicit FileOperationThread(QObject* parent = 0);
	virtual ~FileOperationThread();

	int addOperation(const FileOperation& operation);
	bool removeOperation(int ID);
	bool insertOperation(int ID, const FileOperation& operation);
	FileOperation operation(int ID) const;


protected:
	void run();

private:
	Q_DISABLE_COPY(FileOperationThread)
	Q_DECLARE_PRIVATE(FileOperationThread)

	FileOperationThreadPrivate* const d_ptr;
};

#endif // FILEOPERATIONTHREAD_H
