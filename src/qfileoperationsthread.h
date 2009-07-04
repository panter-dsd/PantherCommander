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

#ifndef QFILEOPERATIONSTHREAD_H
#define QFILEOPERATIONSTHREAD_H

#include <QFileInfo>
#include <QList>
#include <QMutex>
#include <QStringList>
#include <QThread>

class QFileOperationsThread : public QThread
{
	Q_OBJECT

public:
	enum FileOperation {CopyFileOperation,
						RemoveFileOperation,
						GetDirSizeOperation,
						CopyDirOperation,
						RemoveDirOperation,
						MoveFileOperation,
						MoveDirOperation};
	enum FileOperationError
	{
		FO_NO_ERROR,
		FO_DEST_FILE_EXISTS,//+
		FO_READ_ERROR,//+
		FO_WRITE_ERROR,//+
		FO_OPEN_ERROR,//+
		FO_REMOVE_ERROR,//+
		FO_RENAME_ERROR,
		FO_RESIZE_ERROR,
		FO_SET_ATTR_ERROR,
		FO_SET_DATE_ERROR,
		FO_PERMISIONS_ERROR//+
	};
	enum Confirmations
	{
		REMOVE_NO_EMPTY_DIR=0x1,
		OVEWRITE_ALL=0x2,
		OVERWRITE_OLDER=0x4,
		OVERWRITE_NEW=0x8,
		OVERWRITE_SMALLEST=0x10,
		OVERWRITE_LAGER=0x20,
		SKIP_ALL=0x40,
		AUTO_RENAME_SOURCE=0x80,
		AUTO_RENAME_DEST=0x100,
		OVEWRITE_REMOVE_HIDDEN_SYSTEM=0x200,
		OVEWRITE_REMOVE_READONLY=0x400,
	};
	Q_DECLARE_FLAGS(FileOperationConfirmations, Confirmations)
private:
	mutable QMutex		mutex;
	volatile bool				bStopped;
	volatile bool				isPaused;
	FileOperation			operation;
	FileOperationError lastError;
	QStringList				errorParams;
	QStringList				qslParametres;
	int							iLocalBufferSize;
	int							iNoLocalBufferSize;
	qint64						dirSize;
	qint64						filesCount;
	qint64						dirsCount;
	int							iPercent;
	QString					lastErrorString;
	bool							isSkipFile;

public:
	FileOperationConfirmations confirmation;
public:
	QFileOperationsThread(QObject* parent = 0);

	void stop() {QMutexLocker locker(&mutex);bStopped=true;}
	void setJob(FileOperation job,const QStringList& params);
	void getDirSize(qint64& size, qint64& dirs, qint64& files)
	{
		size=dirSize; dirs=dirsCount; files=filesCount;
	}
	FileOperationError getLastError() {return lastError;}
	QStringList getErrorParams () {return errorParams;}
	void skipFile() {isSkipFile=true;}
	static bool isLocalFileSystem(const QString& filePath);
	static bool isSameDisc(const QString& source, const QString& dest);
	static bool getDiskSpace(const QString& dirPath, qint64* total, qint64* free, qint64* available);
	static QString rootPath(const QString& filePath);
	static QString diskLabel(const QString& fileName);
	static bool isRoot(const QString &path);

	static bool execute(const QString& filePath);
	static bool execute(const QString& filePath, const QStringList& arguments);
	static bool execute(const QString& filePath, const QStringList& arguments, const QString& workingDirectory);
#ifdef Q_WS_WIN
	static qint64 winFileAttributes(const QString& filePath);
#endif
	static QFileInfoList volumes();
private:
	bool copyFile(const QString& qsSourceFileName,const QString& qsDestFileName);
	bool removeFile(const QString& qsFileName);
	bool removeDir(const QString& qsDirName);
	bool copyDir(const QString& qsDirName,const QString& qsDestDir);
	void calculateDirSize(const QString& qsDir);
	bool moveFile(const QString& qsSourceFileName,const QString& qsDestFileName);
	bool moveDir(const QString& qsSourceDir,const QString& qsDestDir);
	bool error(const QStringList& params);
	bool copyFileTime(const QString& qsSourceFileName,const QString& qsDestFileName);
	bool copyPermisions(const QString& qsSourceFileName,const QString& qsDestFileName);
protected:
	void run();
signals:
	void changedPercent(int percent);
	void changedValue(qint64 value);
	void changedDirSize(qint64 size, qint64 dirs, qint64 files);
	void currentFileCopyChanged(const QString& sourceFile,const QString& destFile);
	void operationError();
public slots:
	void slotPause() {QMutexLocker locker(&mutex);isPaused=true;}
	void slotResume() {QMutexLocker locker(&mutex);isPaused=false;}
};

#endif // QFILEOPERATIONSTHREAD_H
