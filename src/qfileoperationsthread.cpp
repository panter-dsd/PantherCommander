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

#include "qfileoperationsthread.h"

#include <qplatformdefs.h>

#include <QtCore>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <QtGui/QDesktopServices>

#ifdef Q_OS_UNIX
#include <sys/statvfs.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#ifdef Q_WS_WIN
#include "qt_windows.h"
#define stat64 _stati64
bool SetDirTime(QString fileName, FILETIME* dtCreation, FILETIME* dtLastAccessTime, FILETIME* dtLastWriteTime);
void copyDirTime(const QString& sourceDir,const QString& destDir);

#ifndef INVALID_FILE_ATTRIBUTES
#  define INVALID_FILE_ATTRIBUTES (DWORD (-1))
#endif
#endif
//
QFileOperationsThread::QFileOperationsThread(QObject* parent) : QThread(parent)
{
	QSettings settings;
	settings.beginGroup("Global");
	iLocalBufferSize = settings.value("LocalBufferSize", 10485760).toInt();
	iNoLocalBufferSize = settings.value("NoLocalBufferSize", 65536).toInt();
	settings.endGroup();

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
	int bufSize=(isSameDisc(qsSourceFileName,qsDestFileName))
			? iLocalBufferSize
			: iNoLocalBufferSize;
	emit currentFileCopyChanged(QDir::toNativeSeparators(qsSourceFileName),
							QDir::toNativeSeparators(qsDestFileName));

	if (qsSourceFileName==qsDestFileName)
		return false;

	QFile qfFirstFile(qsSourceFileName);
	QFile qfSecondFile(qsDestFileName);
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
	copyFileTime(qsSourceFileName,qsDestFileName);
#ifndef Q_CC_MSVC
	#warning "Need error?"
#endif
//Change attributes
	copyPermisions(qsSourceFileName,qsDestFileName);
#ifndef Q_CC_MSVC
	#warning "Need error?"
#endif
//
	emit changedPercent(100);
	return true;
}
//
bool QFileOperationsThread::copyFileTime(const QString& qsSourceFileName,const QString& qsDestFileName)
{
	if(!isLocalFileSystem(qsDestFileName))
		return true;

#ifndef Q_WS_WIN
	FILETIME time;
	FILETIME time1;
	FILETIME time2;
	HANDLE firstFileHandle = INVALID_HANDLE_VALUE;
	HANDLE secondFileHandle = INVALID_HANDLE_VALUE;
	QT_WA({
		firstFileHandle = CreateFileW((TCHAR*)qsSourceFileName.utf16(),
							0,
							FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
							0,
							OPEN_EXISTING,
							0,
							0);
	} , {
		firstFileHandle = CreateFileA(qsSourceFileName.toLocal8Bit(),
							0,
							FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
							0,
							OPEN_EXISTING,
							0,
							0);
	});
	if (firstFileHandle == INVALID_HANDLE_VALUE)
		return false;

	QT_WA({
		secondFileHandle = CreateFileW((TCHAR*)qsDestFileName.utf16(),
							GENERIC_WRITE,
							FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
							0,
							OPEN_EXISTING,
							0,
							0);
	} , {
		secondFileHandle = CreateFileA(qsDestFileName.toLocal8Bit(),
							GENERIC_WRITE,
							FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
							0,
							OPEN_EXISTING,
							0,
							0);
	});
	if (secondFileHandle == INVALID_HANDLE_VALUE)
	{
		CloseHandle(firstFileHandle);
		return false;
	}

	if (!GetFileTime(firstFileHandle,&time,&time1,&time2) && !SetFileTime(secondFileHandle,&time,&time1,&time2))
	{
		CloseHandle(firstFileHandle);
		CloseHandle(secondFileHandle);
		return false;
	}
	CloseHandle(firstFileHandle);
	CloseHandle(secondFileHandle);
#endif
#ifdef Q_OS_UNIX
	struct stat st;
	if (stat(QFile::encodeName(qsSourceFileName).data(), &st) != 0)
		return false;
	struct utimbuf time;
	time.actime = st.st_atime;
	time.modtime = st.st_mtime;
	if (utime(QFile::encodeName(qsDestFileName).data(), &time) != 0)
		return false;
#endif

	return true;
}
//
bool QFileOperationsThread::copyPermisions(const QString& qsSourceFileName,const QString& qsDestFileName)
{
	if(!isLocalFileSystem(qsDestFileName))
		return true;

#ifdef Q_WS_WIN
	bool rez;
	QT_WA({rez=SetFileAttributesW((TCHAR*)qsDestFileName.utf16(),GetFileAttributesW((TCHAR*)qsSourceFileName.utf16()));},
		{rez=SetFileAttributesA(qsDestFileName.toLocal8Bit(),GetFileAttributesA(qsSourceFileName.toLocal8Bit()));})
	return rez;
#else
	return QFile(qsDestFileName).setPermissions(QFile(qsSourceFileName).permissions());
#endif
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
	QFileInfo fileInfo(file);

#ifdef Q_WS_WIN
	if (isSystemFile(qsFileName) || fileInfo.isHidden())
#else
	if (fileInfo.isHidden())
#endif
	{
		lastError=FO_PERMISIONS_ERROR;
		if (!error(params))
			return false;
	}
#ifdef Q_WS_WIN
	if(file.fileEngine()->fileFlags(QAbstractFileEngine::FlagsMask) & QAbstractFileEngine::LocalDiskFlag)
	{
		// Set null attributes
		QT_WA({
			SetFileAttributesW((TCHAR*)qsFileName.utf16(),0);
		}, {
			SetFileAttributesA(qsFileName.toLocal8Bit(),0);
		});
	}
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
	dirSize = filesCount = dirsCount = iPercent = 0;
	bStopped = false;
	switch (operation)
	{
	case CopyFileOperation:
		copyFile(qslParametres.at(0), qslParametres.at(1));
		break;
	case CopyDirOperation:
		copyDir(qslParametres.at(0),qslParametres.at(1));
		break;
	case RemoveFileOperation:
		removeFile(qslParametres.at(0));
		break;
	case RemoveDirOperation:
		removeDir(qslParametres.at(0));
		break;
	case MoveFileOperation:
		moveFile(qslParametres.at(0), qslParametres.at(1));
		break;
	case MoveDirOperation:
		moveDir(qslParametres.at(0),qslParametres.at(1));
		break;
	case GetDirSizeOperation:
		calculateDirSize(qslParametres.at(0));
		break;
	default:
		break;
	}
	bStopped = true;
}
//
void QFileOperationsThread::calculateDirSize(const QString& qsDir)
{
	QDir dir(qsDir);
	dir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
	QDirIterator it(dir);
	while(it.hasNext())
	{
		it.next();

		QFileInfo info(it.fileInfo());
		if(info.isDir())
		{
			++dirsCount;
			calculateDirSize(info.absolutePath());
		}
		else
		{
			++filesCount;
			dirSize += info.size();
		}
	}

	emit changedDirSize(dirSize, dirsCount, filesCount);
}
//
void QFileOperationsThread::setJob(FileOperation job, const QStringList& params)
{
	operation = job;
	qslParametres = params;
}
//
bool QFileOperationsThread::copyDir(const QString& qsDirName, const QString& qsDestDir)
{
	QDir sourceDir(qsDirName);

	QDir destDir(qsDestDir);
	destDir.mkdir(sourceDir.dirName());
	destDir.cd(sourceDir.dirName());

	sourceDir.cd(sourceDir.dirName());
	sourceDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
	QDirIterator it(sourceDir);
	while(it.hasNext())
	{
		it.next();

		if (bStopped)
			return false;
		while (isPaused)
		{
			msleep(50);
			if (bStopped)
				return false;
		}

		QFileInfo info(it.fileInfo());
		if(info.isDir())
		{
			if(!copyDir(info.absolutePath(), destDir.absolutePath()))
				return false;
		}
		else
		{
			if(!copyFile(info.absoluteFilePath(), destDir.absolutePath() + QDir::separator() + info.fileName()))
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
#ifdef Q_WS_WIN
	// Change dir time
	copyDirTime(sourceDir.absolutePath(), destDir.absolutePath());

	// Change dir attributes
	QT_WA({
		SetFileAttributesW((TCHAR*)destDir.absolutePath().utf16(), GetFileAttributesW((TCHAR*)sourceDir.absolutePath().utf16()));
	} , {
		SetFileAttributesA(destDir.absolutePath().toLocal8Bit(), GetFileAttributesA(sourceDir.absolutePath().toLocal8Bit()));
	});
#endif

	return true;
}
//
bool QFileOperationsThread::removeDir(const QString& qsDirName)
{
	QDir dir(qsDirName);
	dir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
	QDirIterator it(dir);
	while(it.hasNext())
	{
		it.next();

		if (bStopped)
			return false;
		while (isPaused)
		{
			msleep(50);
			if (bStopped)
				return false;
		}

		QFileInfo info(it.fileInfo());
		if(info.isDir())
		{
			if(!removeDir(info.absolutePath()))
				return false;
		}
		else
		{
			emit currentFileCopyChanged(info.absoluteFilePath(), QString());
			if(!removeFile(info.absoluteFilePath()))
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
#ifdef Q_WS_WIN
//Set null attributes
	QT_WA({
		SetFileAttributesW((TCHAR*)qsDirName.utf16(), 0);
	} , {
		SetFileAttributesA(qsDirName.toLocal8Bit(), 0);
	});
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
	FILETIME dtCreation;
	FILETIME dtLastAccessTime;
	FILETIME dtLastWriteTime;
	HANDLE hDir = INVALID_HANDLE_VALUE;
	QT_WA({
		hDir = CreateFileW((TCHAR*)sourceDir.utf16(),
							GENERIC_READ|GENERIC_WRITE,
							0, 0,
							OPEN_EXISTING,
							FILE_FLAG_BACKUP_SEMANTICS,
							0);
	} , {
		hDir = CreateFileA(sourceDir.toLocal8Bit(),
							GENERIC_READ|GENERIC_WRITE,
							0, 0,
							OPEN_EXISTING,
							FILE_FLAG_BACKUP_SEMANTICS,
							0);
	});
	if (hDir != INVALID_HANDLE_VALUE)
	{
		if (GetFileTime(hDir, &dtCreation, &dtLastAccessTime, &dtLastWriteTime))
			SetDirTime(destDir, &dtCreation, &dtLastAccessTime, &dtLastWriteTime);
		CloseHandle(hDir);
	}
}
//
bool SetDirTime(QString fileName, FILETIME* dtCreation, FILETIME* dtLastAccessTime, FILETIME* dtLastWriteTime)
{
	bool res = false;
	HANDLE hDir = INVALID_HANDLE_VALUE;
	QT_WA({
		hDir = CreateFileW((TCHAR*)fileName.utf16(),
							GENERIC_READ|GENERIC_WRITE,
							0, 0,
							OPEN_EXISTING,
							FILE_FLAG_BACKUP_SEMANTICS,
							0);
	} , {
		hDir = CreateFileA(fileName.toLocal8Bit(),
							GENERIC_READ|GENERIC_WRITE,
							0, 0,
							OPEN_EXISTING,
							FILE_FLAG_BACKUP_SEMANTICS,
							0);
	});
	if (hDir!=INVALID_HANDLE_VALUE)
	{
		res = SetFileTime(hDir, dtCreation, dtLastAccessTime, dtLastWriteTime);
		CloseHandle(hDir);
	}
	return res;
}
#endif
//
bool QFileOperationsThread::moveFile(const QString& qsSourceFileName,const QString& qsDestFileName)
{
	if (!QFileInfo(qsDestFileName).exists() && isSameDisc(qsSourceFileName,qsDestFileName))
		return QFile::rename(qsSourceFileName, qsDestFileName);

	if (copyFile(qsSourceFileName, qsDestFileName))
		return removeFile(qsSourceFileName);

	return false;
}
//
bool QFileOperationsThread::moveDir(const QString& qsSourceDir, const QString& qsDestDir)
{
	if (!QFileInfo(qsDestDir).exists() && isSameDisc(qsSourceDir, qsDestDir))
		return QFile::rename(qsSourceDir, qsDestDir+QDir(qsSourceDir).dirName());

	QDir sourceDir(qsSourceDir);

	QDir destDir(qsDestDir);
	destDir.mkdir(sourceDir.dirName());
	destDir.cd(sourceDir.dirName());

	sourceDir.cd(sourceDir.dirName());
	sourceDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
	QDirIterator it(sourceDir);
	while(it.hasNext())
	{
		it.next();

		if (bStopped)
			return false;
		while (isPaused)
		{
			msleep(50);
			if (bStopped)
				return false;
		}

		QFileInfo info(it.fileInfo());
		if(info.isDir())
		{
			if(!moveDir(info.absolutePath(), destDir.absolutePath()))
				return false;
		}
		else
		{
			if(!moveFile(info.absoluteFilePath(), destDir.absolutePath() + QDir::separator() + info.fileName()))
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

#ifdef Q_WS_WIN
	// Change dir attributes
	QT_WA({
		SetFileAttributesW((TCHAR*)destDir.absolutePath().utf16(), GetFileAttributesW((TCHAR*)sourceDir.absolutePath().utf16()));
	} , {
		SetFileAttributesA(destDir.absolutePath().toLocal8Bit(), GetFileAttributesA(sourceDir.absolutePath().toLocal8Bit()));
	});

	// Remove dir
	// Remove readOnly AND system
	QT_WA({
		SetFileAttributesW((TCHAR*)sourceDir.absolutePath().utf16(), 0);
	}, {
		SetFileAttributesA(sourceDir.absolutePath().toLocal8Bit(), 0);
	});
#endif
//
	sourceDir.rmdir(sourceDir.absolutePath());
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
bool QFileOperationsThread::isLocalFileSystem(const QString& filePath)
{
	bool isLocalDisk = false;
	QAbstractFileEngine* engine = QAbstractFileEngine::create(filePath);
	isLocalDisk = (engine->fileFlags(QAbstractFileEngine::FlagsMask) & QAbstractFileEngine::LocalDiskFlag);
	delete engine;

	return isLocalDisk;
}
//
bool QFileOperationsThread::isSameDisc(const QString& sourcePath, const QString& destPath)
{
	//TODO: `source' and `dest' both must be absolutePath-ed and nativeSeparator-ed here
	QString source = QDir::toNativeSeparators(QFileInfo(sourcePath).absolutePath());
	QString dest = QDir::toNativeSeparators(QFileInfo(destPath).absolutePath());

	bool res = false;
#ifdef Q_WS_WIN
	if(!source.endsWith(QLatin1Char('\\')))
		source.append(QLatin1Char('\\'));
	if(!dest.endsWith(QLatin1Char('\\')))
		dest.append(QLatin1Char('\\'));

	// ANSI win functions support will be dropped soon. so fuck them.
	QT_WA({;} , {return false;});
#ifndef Q_CC_MSVC
	#warning "init only once + add postroutine for uninit"
#endif
	typedef BOOL (WINAPI *PtrGetVolumeNameForVolumeMountPointW)(LPCTSTR, LPTSTR, DWORD);
	PtrGetVolumeNameForVolumeMountPointW ptrGetVolumeNameForVolumeMountPointW = 0;
	HINSTANCE kernelHnd = LoadLibraryW(L"kernel32");
	if(kernelHnd)
		ptrGetVolumeNameForVolumeMountPointW = (PtrGetVolumeNameForVolumeMountPointW)GetProcAddress(kernelHnd, "GetVolumeNameForVolumeMountPointW");
	if(!ptrGetVolumeNameForVolumeMountPointW)
		return false;

	QT_WA({
		TCHAR sourceVolume[51];
		TCHAR destVolume[51];
		DWORD bufferSize = 50;
		if(ptrGetVolumeNameForVolumeMountPointW((TCHAR*)source.utf16(), sourceVolume, bufferSize) &&
			ptrGetVolumeNameForVolumeMountPointW((TCHAR*)dest.utf16(), destVolume, bufferSize))
		{
			res = (QString::fromUtf16((ushort*)sourceVolume) == QString::fromUtf16((ushort*)destVolume));
		}
	} , {
		//res = false;
	});

	FreeLibrary(kernelHnd);
#endif // Q_WS_WIN
#ifdef Q_OS_UNIX
	struct stat stSource;
	struct stat stDest;
	stat(QFile::encodeName(source).data(), &stSource);
	stat(QFile::encodeName(dest).data(), &stDest);
	res = (stSource.st_dev == stDest.st_dev);
#endif

	return res;
}
//
bool QFileOperationsThread::getDiskSpace(const QString& dirPath, qint64* total, qint64* free, qint64* available)
{
	bool res = false;
#ifdef Q_WS_WIN
	qint64 bytesUserFree, bytesTotalSize, bytesTotalFree;
	QT_WA({
		res = GetDiskFreeSpaceExW((TCHAR*)dirPath.utf16(),
									(PULARGE_INTEGER)&bytesUserFree,
									(PULARGE_INTEGER)&bytesTotalSize,
									(PULARGE_INTEGER)&bytesTotalFree);
	}, {
		res = GetDiskFreeSpaceExA(dirPath.toLocal8Bit(),
									(PULARGE_INTEGER)&bytesUserFree,
									(PULARGE_INTEGER)&bytesTotalSize,
									(PULARGE_INTEGER)&bytesTotalFree);
	});
	if(res)
	{
		if(total)
			*total = bytesTotalSize;
		if(free)
			*free = bytesTotalFree;
		if(available)
			*available = bytesUserFree;
	}
#endif
#ifdef Q_OS_UNIX
	struct statvfs fs;
	res = (statvfs(QFile::encodeName(dirPath).data(), &fs) < 0);
	if(res)
	{
		if(total)
			*total = fs.f_frsize * fs.f_blocks;
		if(free)
			*free = fs.f_frsize * fs.f_bfree;
		if(available)
			*available = fs.f_frsize * fs.f_bavail;
	}
#endif

	return res;
}
//
QString QFileOperationsThread::rootPath(const QString& filePath)
{
	QString rootPath;
#ifdef Q_WS_WIN
	if(isLocalFileSystem(filePath))
	{
		QString path = QDir::fromNativeSeparators(filePath);
		if(path.size() >= 2 && path[0].isLetter() && path[1] == QLatin1Char(':'))
		{
			rootPath = path.left(2);
		}
		else if(path.size() >= 3 && path[0] == QLatin1Char('/') && path[1].isLetter() && path[2] == QLatin1Char('/'))
		{
			rootPath = path[1];
			rootPath.append(QLatin1Char(':'));
		}
		else if(path.startsWith(QLatin1String("//")))
		{
			QString prefix = QLatin1String("//");
			if(path.startsWith(QLatin1String("//?/")))
			{
				prefix = QLatin1String("//?/");
				if(path.startsWith(QLatin1String("//?/UNC/")))
					prefix = QLatin1String("//?/UNC/");
				path.remove(0, prefix.size());
			}
			QStringList parts = path.split(QLatin1Char('/'), QString::SkipEmptyParts);
			if(!parts.isEmpty())
			{
				rootPath = prefix;
				rootPath.append(parts[0]);
			}
		}
	}
	if(!rootPath.isEmpty())
		rootPath.append(QLatin1Char('/'));
	else
		rootPath = QDir::rootPath();
#else
	rootPath = QDir::rootPath();
#endif

	return rootPath;
}
//
QString QFileOperationsThread::diskLabel(const QString& fileName)
{
	QString label;
#ifdef Q_WS_WIN
	QString path = QDir::toNativeSeparators(rootPath(fileName));
	QT_WA({
		TCHAR volumeLabel[101];
		DWORD bufferSize = 100;
		GetVolumeInformationW((TCHAR*)path.utf16(), volumeLabel, bufferSize, 0, 0, 0, 0, 0);
		label = QString::fromUtf16((ushort*)volumeLabel);
	} , {
		char volumeLabel[101];
		DWORD bufferSize = 100;
		GetVolumeInformationA(path.toLocal8Bit(), volumeLabel, bufferSize, 0, 0, 0, 0, 0);
		label = QString::fromLocal8Bit(volumeLabel);
	});
#endif
	return label;
}
//
bool QFileOperationsThread::execute(const QString& filePath)
{
	return execute(filePath, QStringList(), QDir::currentPath());
}

bool QFileOperationsThread::execute(const QString& filePath, const QStringList& arguments)
{
	return execute(filePath, arguments, QDir::currentPath());
}

bool QFileOperationsThread::execute(const QString& filePath, const QStringList& arguments, const QString& workingDirectory)
{
#ifndef Q_CC_MSVC
	#warning "TODO: *panther: respect `arguments' and `workingDirectory'"
#endif
	QFileInfo qfiFileInfo(filePath);
	if(!qfiFileInfo.exists() || !qfiFileInfo.isFile())
	{
		// absolute path; not exists or not a file
		if(!qfiFileInfo.isRelative())
			return false;
		// relative path; see if file exists in current dir
		qfiFileInfo.setFile(QDir::current(), filePath);
		if(!qfiFileInfo.exists() || !qfiFileInfo.isFile())
			return false;
	}

#ifdef Q_WS_WIN
	if (qfiFileInfo.suffix().toLower() == QLatin1String("bat"))
	{
		return QProcess::startDetached("cmd.exe",
								QStringList() << "/C" << QDir::toNativeSeparators(qfiFileInfo.absoluteFilePath()),
								qfiFileInfo.absolutePath());
	}
	else
	{
		HINSTANCE__* instance;
		QT_WA({instance=ShellExecuteW(0,
						(TCHAR*)QString("Open").utf16(),
						(TCHAR*)QDir::toNativeSeparators(qfiFileInfo.absoluteFilePath()).utf16(),
						NULL,
						(TCHAR*)QDir::toNativeSeparators(qfiFileInfo.absolutePath()).utf16(),
						SW_NORMAL);},
						{instance=ShellExecuteA(0,
						"Open",
						QDir::toNativeSeparators(qfiFileInfo.absoluteFilePath()).toLocal8Bit(),
						NULL,
						QDir::toNativeSeparators(qfiFileInfo.absolutePath()).toLocal8Bit(),
						SW_NORMAL);});
		if (int(instance)<32)
		{
			if (int(instance)==SE_ERR_NOASSOC)
			{
				QT_WA({ShellExecuteW(0,
								(TCHAR*)QString("open").utf16(),
								(TCHAR*)QString("rundll32.exe").utf16(),
								(TCHAR*)QString("shell32.dll,OpenAs_RunDLL "+QDir::toNativeSeparators(qfiFileInfo.absoluteFilePath())).utf16(),
								NULL,
								SW_SHOWNORMAL);},
							{ShellExecuteA(0,
								"open",
								"rundll32.exe",
								"shell32.dll,OpenAs_RunDLL "+
								QDir::toNativeSeparators(qfiFileInfo.absoluteFilePath()).toLocal8Bit(),
								NULL,
								SW_SHOWNORMAL);});
				return true;
			}
		}
	}
#else
	if (qfiFileInfo.isExecutable())
	{
		if(QProcess::startDetached(qfiFileInfo.absoluteFilePath(),
								QStringList(),
								qfiFileInfo.absolutePath()))
			return true;
	}

	return QDesktopServices::openUrl(QUrl::fromLocalFile(qfiFileInfo.absoluteFilePath()));
#endif
	return false;
}
//
#ifdef Q_WS_WIN
bool QFileOperationsThread::isSystemFile(const QString& filePath)
{
	bool isSystem = false;

	if(isLocalFileSystem(filePath))
	{
		QString path = filePath;
		if(path.length() == 2 && path.at(1) == QLatin1Char(':'))
			path += QLatin1Char('\\');

		DWORD fileAttrib = INVALID_FILE_ATTRIBUTES;
		QT_WA({
			fileAttrib = ::GetFileAttributesW((TCHAR*)path.utf16());
		} , {
			QString fpath = QFileInfo(path).absoluteFilePath();
			fileAttrib = ::GetFileAttributesA(fpath.toLocal8Bit());
		});
		if(fileAttrib == INVALID_FILE_ATTRIBUTES)
		{
			// path for FindFirstFile should not be end in a trailing slash or slosh
			while(path.endsWith(QLatin1Char('\\')))
				path.resize(path.size() - 1);

			HANDLE findFileHandle = INVALID_HANDLE_VALUE;
			WIN32_FIND_DATA findData;
			QT_WA({
				findFileHandle = ::FindFirstFileW((TCHAR*)path.utf16(),
													&findData);
			} , {
				// Cast is safe, since char is at end of WIN32_FIND_DATA
				QString fpath = QFileInfo(path).absoluteFilePath();
				findFileHandle = ::FindFirstFileA(fpath.toLocal8Bit(),
													(WIN32_FIND_DATAA*)&findData);
			});
			if (findFileHandle != INVALID_HANDLE_VALUE) {
				::FindClose(findFileHandle);
				fileAttrib = findData.dwFileAttributes;
			}
		}

		isSystem = (fileAttrib != INVALID_FILE_ATTRIBUTES && (fileAttrib & FILE_ATTRIBUTE_SYSTEM));
	}

	return isSystem;
}
#endif
//
QStringList QFileOperationsThread::getDrivesList()
{
	QStringList list;
#ifdef Q_WS_WIN
	foreach(const QFileInfo& fileInfo, QDir::drives())
		list.append(fileInfo.absolutePath());
#endif
#ifdef Q_OS_UNIX
	QFile file;
	file.setFileName("/etc/mtab");
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);
		QString buffer;
		int i=-1;
		while(!stream.atEnd())
		{
			buffer=stream.readLine();
			QFileInfo fileInfo(buffer.split(" ").at(1));
			if (fileInfo.isDir())
				list.append(fileInfo.absolutePath());
		}
	}
	file.close();
	list.removeDuplicates();
#endif

	return list;
}
