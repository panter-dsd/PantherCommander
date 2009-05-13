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
#include <QtCore>
#include "qfileoperationsthread.h"
#ifdef Q_WS_X11
	#include <sys/statvfs.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
//
#ifdef Q_WS_WIN
	#include <windows.h>
	#define stat64 _stati64
	bool SetDirTime(QString fileName, FILETIME* dtCreation, FILETIME* dtLastAccessTime, FILETIME* dtLastWriteTime);
	void copyDirTime(const QString& sourceDir,const QString& destDir);
#endif
//
QFileOperationsThread::QFileOperationsThread()
{
	QSettings* settings=new QSettings(this);
	iLocalBufferSize=settings->value("Global/LocalBufferSize",10485760).toInt();
	iNoLocalBufferSize=settings->value("Global/NoLocalBufferSize",65536).toInt();
	delete settings;
	bStopped=false;
	isPaused=false;
	isSkipFile=false;
	dirSize=filesCount=dirsCount=iPercent=0;
}
//
bool QFileOperationsThread::copyFile(const QString& qsSourceFileName,const QString& qsDestFileName)
{
	QStringList params;
	params << QDir::toNativeSeparators(qsSourceFileName)
			<< QDir::toNativeSeparators(qsDestFileName);
	lastError=FO_NO_ERROR;
	lastErrorString.clear();
	int bufSize=(isInSameDisc(qsSourceFileName,qsDestFileName))
			? iLocalBufferSize
			: iNoLocalBufferSize;
	emit currentFileCopyChanged(QDir::toNativeSeparators(qsSourceFileName),
							QDir::toNativeSeparators(qsDestFileName));

	QFile qfFirstFile(qsSourceFileName);
	QFile qfSecondFile(qsDestFileName);
	//Если файлы одинаковые, то прерываемся
	if (qsSourceFileName==qsDestFileName)
		return false;
	if (qfSecondFile.exists())
	{
		lastError=FO_DEST_FILE_EXISTS;
		if (!error(params))
			return false;
		if (!removeFile(qsDestFileName))
			return false;
	}
	while (!qfFirstFile.open(QIODevice::ReadOnly))
	{
		lastError=FO_OPEN_ERROR;
		if (!error(QStringList() << params.at(0)))
			return false;
	}
	while (!qfSecondFile.open(QIODevice::WriteOnly))
	{
		lastError=FO_OPEN_ERROR;
		if (!error(QStringList() << params.at(1)))
		{
			qfFirstFile.close();
			return false;
		}
	}
	qint64 destSize=(qfFirstFile.size()>2) ? qfFirstFile.size()-1 : qfFirstFile.size();
	while (!qfSecondFile.resize(destSize))
	{
		lastError=FO_RESIZE_ERROR;
		if (!error(params))
		{
			qfFirstFile.close();
			qfSecondFile.close();
			qfSecondFile.remove();
			return false;
		}
	}
	qint64 readByteCount=0;
	emit changedPercent(0);
	while (qfFirstFile.size()>readByteCount)
	{
		if (bStopped)
		{
			qfFirstFile.close();
			qfSecondFile.close();
			qfSecondFile.remove();
			return false;
		}
		while (isPaused)
		{
			this->msleep(50);
			if (bStopped)
			{
				qfFirstFile.close();
				qfSecondFile.close();
				qfSecondFile.remove();
				return false;
			}
		}
		int value=int(double(readByteCount)/double(qfFirstFile.size())*100);
		if (value!=iPercent)
		{
			iPercent=value;
			emit changedPercent(iPercent);
		}
		QByteArray qbaBuffer(qfFirstFile.read(bufSize));
		if (qbaBuffer.isEmpty())
		{
			lastError=FO_READ_ERROR;
			if (!error(params))
			{
				qfFirstFile.close();
				qfSecondFile.close();
				qfSecondFile.remove();
				return false;
			}
			qbaBuffer.fill(0,bufSize);
		}
		qint64 writePos=qfSecondFile.pos();
		while (qfSecondFile.write(qbaBuffer)!=qbaBuffer.count())
		{
			lastError=FO_WRITE_ERROR;
			if (!error(params))
				{
					qfFirstFile.close();
					qfSecondFile.close();
					qfSecondFile.remove();
					return false;
				}
			qfSecondFile.seek(writePos);
		}
		readByteCount+=qbaBuffer.size();
		emit changedValue(qbaBuffer.size());
	}
	qfFirstFile.close();
	qfSecondFile.close();
//Change file date
#ifdef Q_WS_WIN
	FILETIME* time=new FILETIME();
	FILETIME* time1=new FILETIME();
	FILETIME* time2=new FILETIME();
	HANDLE firstFileHandle=CreateFileA(qsSourceFileName.toLocal8Bit(),
							0,
							FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
							0,
							OPEN_EXISTING,
							0,
							0);
	HANDLE secondFileHandle=CreateFileA(qsDestFileName.toLocal8Bit(),
							GENERIC_WRITE,
							FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
							0,
							OPEN_EXISTING,
							0,
							0);
	if (int(firstFileHandle)==-1 || int(secondFileHandle)==-1)
		qDebug("Error");
	if (GetFileTime(firstFileHandle,time,time1,time2))
		SetFileTime(secondFileHandle,time,time1,time2);
	CloseHandle(firstFileHandle);
	CloseHandle(secondFileHandle);
#endif
//Change attributes
#ifdef Q_WS_WIN
	SetFileAttributesA(qsDestFileName.toLocal8Bit(),
					GetFileAttributesA(qsSourceFileName.toLocal8Bit()));
#endif
#ifdef Q_WS_X11
	if (!qfSecondFile.setPermissions(qfFirstFile.permissions()))
	{
//		lastError=qfSecondFile.error();
//		lastErrorString=qfSecondFile.errorString();
	}
#endif
//
	emit changedPercent(100);
	return true;
}
//
bool QFileOperationsThread::removeFile(const QString& qsFileName)
{
	QStringList params;
	params << QDir::toNativeSeparators(qsFileName);
	lastError=FO_NO_ERROR;
	lastErrorString.clear();

	emit currentFileCopyChanged(QDir::toNativeSeparators(qsFileName),"");
	emit changedPercent(0);
	QFile file(qsFileName);

#ifdef Q_WS_X11
	if (QFileInfo(qsFileName).isHidden())
	{
		lastError=FO_PERMISIONS_ERROR;
		if (!error(params))
			return false;
	}
#endif

#ifdef Q_WS_WIN
	DWORD attr=GetFileAttributesA(qsFileName.toLocal8Bit());
	if ((attr & FILE_ATTRIBUTE_SYSTEM) || (attr & FILE_ATTRIBUTE_HIDDEN) || (attr & FILE_ATTRIBUTE_READONLY))
	{
		lastError=FO_PERMISIONS_ERROR;
		if (!error(params))
			return false;
	}
	SetFileAttributesA(qsFileName.toLocal8Bit(),0);
#endif

	while(!file.remove())
	{
		lastError=FO_REMOVE_ERROR;
		if (!error(params))
			return false;
	}
	emit changedPercent(100);
	emit changedValue(1);
	return true;
}
//
void QFileOperationsThread::run()
{
	dirSize=filesCount=dirsCount=iPercent=0;
	bStopped=false;
	switch (operation)
	{
	case CopyFileOperation: {copyFile(qslParametres.at(0),qslParametres.at(1)); break;}
	case CopyDirOperation:
		{
			copyDir(qslParametres.at(0),qslParametres.at(1));
			break;
		}
	case RemoveFileOperation: {removeFile(qslParametres.at(0)); break;}
	case RemoveDirOperation:
		{
			removeDir(qslParametres.at(0));
			break;
		}
	case MoveFileOperation: {moveFile(qslParametres.at(0),qslParametres.at(1)); break;}
	case MoveDirOperation:
		{
			moveDir(qslParametres.at(0),qslParametres.at(1));
			break;
		}
	case GetDirSizeOperation: {calculateDirSize(qslParametres.at(0)); break;}
	default: return;
	}
	bStopped=true;
}
//
void QFileOperationsThread::calculateDirSize(const QString& qsDir)
{
	QDir dir(qsDir);
	dir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	QDirIterator qdiIterator(dir);
	while(qdiIterator.hasNext())
	{
		qdiIterator.next();
		QString qsName=qdiIterator.fileName();
		if ((qsName==".") || (qsName==".."))
			continue;

		struct stat64 st;
		stat64((QDir(qsDir).absolutePath()+QDir::separator()+qsName).toLocal8Bit(),&st);
		if ((st.st_mode & S_IFMT)==S_IFDIR)
		{
			dirsCount++;
			calculateDirSize(QDir(qsDir).absolutePath()+QDir::separator()+qsName);
		}
		else
		{
			filesCount++;
			dirSize+=qint64(st.st_size);
		}
	}
	emit changedDirSize(dirSize,dirsCount,filesCount);
}
//
void QFileOperationsThread::setJob(FileOperation job,const QStringList& params)
{
	operation=job;
	qslParametres=params;
}
//
bool QFileOperationsThread::copyDir(const QString& qsDirName,const QString& qsDestDir)
{
	QDir sourceDir(qsDirName);
	sourceDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	QDir destDir(qsDestDir);
	destDir.mkdir(sourceDir.dirName());
	sourceDir.cd(sourceDir.dirName());
	destDir.cd(sourceDir.dirName());

	QDirIterator qdiIterator(sourceDir);
	if (!qdiIterator.hasNext())
		return false;
	while(qdiIterator.hasNext())
	{
		qdiIterator.next();
		if (bStopped)
		{
			return false;
		}
		while (isPaused)
		{
			this->msleep(50);
			if (bStopped)
			{
				return false;
			}
		}
		QString qsName=qdiIterator.fileName();
		if ((qsName==".") || (qsName==".."))
			continue;
		struct stat64 st;
		stat64((sourceDir.absolutePath()+QDir::separator()+qsName).toLocal8Bit(),&st);
		if ((st.st_mode & S_IFMT)==S_IFDIR)
		{
			if (!copyDir(sourceDir.absolutePath()+QDir::separator()+qsName,destDir.absolutePath()+QDir::separator()))
			{
				return false;
			}
		}
		else
		{
			if (!copyFile(sourceDir.absolutePath()+QDir::separator()+qsName,
					destDir.absolutePath()+QDir::separator()+qsName))
			{
				if (isSkipFile || (confirmation & SKIP_ALL))
				{
					isSkipFile=false;
					bStopped=false;
				}
				else
				{
					return false;
				}
			}
		}
	}
//Change dir time
#ifdef Q_WS_WIN
	copyDirTime(sourceDir.absolutePath(),destDir.absolutePath());
#endif
//Change dir attributes
#ifdef Q_WS_WIN
	SetFileAttributesA(destDir.absolutePath().toLocal8Bit(),
					GetFileAttributesA(sourceDir.absolutePath().toLocal8Bit()));
#endif
//
	return true;
}
//
bool QFileOperationsThread::removeDir(const QString& qsDirName)
{
	QDir dir(qsDirName);
	dir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	QDirIterator qdiIterator(dir);
	if (!qdiIterator.hasNext())
		return false;
	while(qdiIterator.hasNext())
	{
		qdiIterator.next();
		if (bStopped)
		{
			return false;
		}
		while (isPaused)
		{
			this->msleep(50);
			if (bStopped)
			{
				return false;
			}
		}
		QString qsName=qdiIterator.fileName();
		if ((qsName==".") || (qsName==".."))
			continue;
		struct stat64 st;
		stat64((qsDirName+qsName).toLocal8Bit(),&st);
		if ((st.st_mode & S_IFMT)==S_IFDIR)
		{
			if (!removeDir(qsDirName+qsName+QDir::separator()))
			{
				return false;
			}
		}
		else
		{
			emit currentFileCopyChanged(qsDirName+qsName,"");
			if (!removeFile(qsDirName+qsName))
			{
				if (isSkipFile || (confirmation & SKIP_ALL))
				{
					isSkipFile=false;
					bStopped=false;
				}
				else
				{
					return false;
				}
			}
		}
	}
//Remove readOnly AND system
#ifdef Q_WS_WIN
	SetFileAttributesA((qsDirName).toLocal8Bit(),0);
#endif
//
	while (!dir.rmdir(qsDirName))
	{
		lastError=FO_REMOVE_ERROR;
		if (!error(QStringList() << qsDirName))
			return false;
	}
	emit changedValue(1);
	return true;
}
//
#ifdef Q_WS_WIN
void copyDirTime(const QString& sourceDir,const QString& destDir)
{
	FILETIME* dtCreation=new FILETIME();
	FILETIME* dtLastAccessTime=new FILETIME();
	FILETIME* dtLastWriteTime=new FILETIME();
	HANDLE hDir=CreateFileA(sourceDir.toLocal8Bit(),
					GENERIC_READ|GENERIC_WRITE,
					0,
					0,
					OPEN_EXISTING,
					FILE_FLAG_BACKUP_SEMANTICS,
					0);
	if (hDir!=INVALID_HANDLE_VALUE)
	{
		if (GetFileTime(hDir, dtCreation, dtLastAccessTime, dtLastWriteTime))
			SetDirTime(destDir, dtCreation, dtLastAccessTime, dtLastWriteTime);
		CloseHandle(hDir);
	}
}
//
bool SetDirTime(QString fileName, FILETIME* dtCreation, FILETIME* dtLastAccessTime, FILETIME* dtLastWriteTime)
{
	HANDLE hDir=CreateFileA(fileName.toLocal8Bit(),
					GENERIC_READ|GENERIC_WRITE,
					0,
					0,
					OPEN_EXISTING,
					FILE_FLAG_BACKUP_SEMANTICS,
					0);
	if (hDir!=INVALID_HANDLE_VALUE)
	{
		bool bRez=SetFileTime(hDir, dtCreation, dtLastAccessTime, dtLastWriteTime);
		CloseHandle(hDir);
		return bRez;
	}
	return false;
}
#endif
//
bool QFileOperationsThread::moveFile(const QString& qsSourceFileName,const QString& qsDestFileName)
{
	if (!QFileInfo(qsDestFileName).exists() && isInSameDisc(qsSourceFileName,qsDestFileName))
	{
		return QFile::rename(qsSourceFileName,qsDestFileName);
	}
	if (copyFile(qsSourceFileName,qsDestFileName))
		if (removeFile(qsSourceFileName))
			return true;
	return false;
}
//
bool QFileOperationsThread::moveDir(const QString& qsSourceDir,const QString& qsDestDir)
{
	if (!QFileInfo(qsDestDir).exists() && isInSameDisc(qsSourceDir,qsDestDir))
	{
		return QFile::rename(qsSourceDir,qsDestDir+QDir(qsSourceDir).dirName());
	}
	QDir sourceDir(qsSourceDir);
	sourceDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	QDir destDir(qsDestDir);
	destDir.mkdir(sourceDir.dirName());
	sourceDir.cd(sourceDir.dirName());
	destDir.cd(sourceDir.dirName());

	QDirIterator qdiIterator(sourceDir);
	if (!qdiIterator.hasNext())
		return false;
	while (qdiIterator.hasNext())
	{
		qdiIterator.next();
		if (bStopped)
		{
			return false;
		}
		while (isPaused)
		{
			this->msleep(50);
			if (bStopped)
			{
				return false;
			}
		}
		QString qsName=qdiIterator.fileName();
		if ((qsName==".") || (qsName==".."))
			continue;
		struct stat64 st;
		stat64((sourceDir.absolutePath()+QDir::separator()+qsName).toLocal8Bit(),&st);
		if ((st.st_mode & S_IFMT)==S_IFDIR)
		{
			if (!moveDir(sourceDir.absolutePath()+QDir::separator()+qsName+QDir::separator(),destDir.absolutePath()+QDir::separator()))
			{
				return false;
			}
		}
		else
		{
			if (!moveFile(sourceDir.absolutePath()+QDir::separator()+qsName,
						destDir.absolutePath()+QDir::separator()+qsName))
				{
					if (isSkipFile || (confirmation & SKIP_ALL))
					{
						isSkipFile=false;
						bStopped=false;
					}
					else
					{
						return false;
					}
				}
		}
	}
//Change dir attributes
#ifdef Q_WS_WIN
	SetFileAttributesA(destDir.absolutePath().toLocal8Bit(),
					GetFileAttributesA(sourceDir.absolutePath().toLocal8Bit()));
#endif
//Remove dir
//Remove readOnly AND system
#ifdef Q_WS_WIN
	SetFileAttributesA(sourceDir.absolutePath().toLocal8Bit(),0);
#endif
//
	QDir dir;
	dir.rmdir(sourceDir.absolutePath());
	emit changedValue(1);
	return true;
}
//
bool QFileOperationsThread::error(const QStringList& params)
{
	errorParams=params;
	bool b;
	if (lastError==FO_DEST_FILE_EXISTS)
	{
		b=(confirmation & OVEWRITE_ALL);
		if (b)
			return true;
		b=(confirmation & OVERWRITE_OLDER) &&
					(QFileInfo(errorParams.at(0)).lastModified()>QFileInfo(errorParams.at(1)).lastModified());
		if (b)
			return true;
		b=(confirmation & OVERWRITE_NEW) &&
					(QFileInfo(errorParams.at(0)).lastModified()<QFileInfo(errorParams.at(1)).lastModified());
		if (b)
			return true;
		b=(confirmation & OVERWRITE_SMALLEST) &&
					(QFileInfo(errorParams.at(0)).size()>QFileInfo(errorParams.at(1)).size());
		if (b)
			return true;
		b=(confirmation & OVERWRITE_LAGER) &&
					(QFileInfo(errorParams.at(0)).size()<QFileInfo(errorParams.at(1)).size());
		if (b)
			return true;
		b=(confirmation & SKIP_ALL);
		if (b)
			return false;
	}
	if (lastError==FO_PERMISIONS_ERROR)
	{
		b=confirmation & OVEWRITE_REMOVE_HIDDEN_SYSTEM;
		if (b)
			return true;
	}
	if (lastError==FO_REMOVE_ERROR)
	{
		b=(confirmation & SKIP_ALL) && !QFileInfo(params.at(0)).isDir();
		if (b)
			return false;
	}
	if (lastError==FO_READ_ERROR)
	{
		b=confirmation & SKIP_ALL;
		if (b)
			return false;
	}
	if (lastError==FO_WRITE_ERROR)
	{
		b=confirmation & SKIP_ALL;
		if (b)
			return false;
	}
	slotPause();
	emit operationError();
	while (isPaused)
	{
		this->msleep(50);
	}
	return !bStopped;
}
//
bool QFileOperationsThread::isInSameDisc(const QString& source,const QString& dest)
{
#ifdef Q_WS_WIN
#ifndef Q_CC_MSVC
	#warning "this ignore mounted disc to dirrectory"
#endif
	if (source.mid(0,1).toUpper()==dest.mid(0,1).toUpper())
		return true;
#endif
#ifdef Q_WS_X11
	struct stat stSource;
	struct stat stDest;
	stat(QFileInfo(source).absolutePath().toLocal8Bit(),&stSource);
	stat(QFileInfo(dest).absolutePath().toLocal8Bit(),&stDest);
	return stSource.st_dev==stDest.st_dev;
#endif
	return false;
}
//
bool QFileOperationsThread::getDiskSpace(const QString& fileName,qint64& availableBytes,qint64& totalBytes,qint64 &freeBytes)
{
#ifdef Q_WS_WIN

	ULARGE_INTEGER BytesAvailableToCaller,TotalNumberOfBytes,TotalNumberOfFreeBytes;
	int rez=GetDiskFreeSpaceExA(fileName.toLocal8Bit(),
						&BytesAvailableToCaller,
						&TotalNumberOfBytes,
						&TotalNumberOfFreeBytes);
	availableBytes=BytesAvailableToCaller.QuadPart;
	totalBytes=TotalNumberOfBytes.QuadPart;
	freeBytes=TotalNumberOfFreeBytes.QuadPart;
	if (rez==0)
		return false;
#endif

#ifdef Q_WS_X11
	struct statvfs fs;
	if (statvfs(fileName.toLocal8Bit(),&fs)<0)
		return false;
	availableBytes=fs.f_frsize*fs.f_bavail;
	totalBytes=fs.f_frsize*fs.f_blocks;
	freeBytes=fs.f_frsize*fs.f_bfree;
#endif
	return true;
}
//
QString QFileOperationsThread::getDiskLabel(const QString& fileName)
{
	QString label;
#ifdef Q_WS_WIN
	int lenght=100;
	char* volumeLabel=new char[lenght];
	GetVolumeInformationA(fileName.mid(0,3).toLocal8Bit(),
						volumeLabel,
						lenght,
						0,
						0,
						0,
						0,
						0);
	label=QString(volumeLabel);
	delete volumeLabel;
#endif

#ifdef Q_WS_X11
	label="";
#endif
	return label;
}
//
