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

#ifdef Q_WS_WIN
#  define _WIN32_WINNT 0x0501
#endif

#include <qplatformdefs.h>

#include <QtCore>

#include <QtGui/QDesktopServices>

#ifdef Q_WS_WIN
#ifndef INVALID_FILE_ATTRIBUTES
#  define INVALID_FILE_ATTRIBUTES (DWORD (-1))
#endif
#else

#include <utime.h>

#endif // Q_WS_WIN

#include "appsettings.h"

QFileOperationsThread::QFileOperationsThread (QObject *parent)
    : QThread (parent)
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("Global");
    iLocalBufferSize = settings->value ("LocalBufferSize", 10485760).toInt ();
    iNoLocalBufferSize = settings->value ("NoLocalBufferSize", 65536).toInt ();
    settings->endGroup ();

    bStopped = false;
    isPaused = false;
    isSkipFile = false;
    dirSize = filesCount = dirsCount = iPercent = 0;
}

bool QFileOperationsThread::copyFile (const QString &qsSourceFileName, const QString &qsDestFileName)
{
    QString source = qsSourceFileName, dest = qsDestFileName;
    if (QFileInfo (dest).isDir ()) {
        dest += QFileInfo (source).fileName ();
    }

    QStringList params;
    params << QDir::toNativeSeparators (source)
           << QDir::toNativeSeparators (dest);
    lastError = FO_NO_ERROR;
    lastErrorString.clear ();
    int bufSize = (isSameDisc (source, dest))
                  ? iLocalBufferSize
                  : iNoLocalBufferSize;
    emit currentFileCopyChanged (source,
                                 dest
    );

    if (source == dest) {
        return false;
    }

    QFile qfFirstFile (source);
    QFile qfSecondFile (dest);
    if (qfSecondFile.exists ()) {
        lastError = FO_DEST_FILE_EXISTS;
        if (!error (params)) {
            return false;
        }
        if (!removeFile (dest)) {
            return false;
        }
    }
    while (!qfFirstFile.open (QIODevice::ReadOnly)) {
        lastError = FO_OPEN_ERROR;
        if (!error (QStringList () << params.at (0))) {
            return false;
        }
    }
    while (!qfSecondFile.open (QIODevice::WriteOnly)) {
        lastError = FO_OPEN_ERROR;
        if (!error (QStringList () << params.at (1))) {
            qfFirstFile.close ();
            return false;
        }
    }
    qint64 destSize = (qfFirstFile.size () > 2) ? qfFirstFile.size () - 1 : qfFirstFile.size ();
    while (!qfSecondFile.resize (destSize)) {
        lastError = FO_RESIZE_ERROR;
        if (!error (params)) {
            qfFirstFile.close ();
            qfSecondFile.close ();
            qfSecondFile.remove ();
            return false;
        }
    }
    qint64 readByteCount = 0;
    emit changedPercent (0);
    while (qfFirstFile.size () > readByteCount) {
        if (bStopped) {
            qfFirstFile.close ();
            qfSecondFile.close ();
            qfSecondFile.remove ();
            return false;
        }
        while (isPaused) {
            this->msleep (50);
            if (bStopped) {
                qfFirstFile.close ();
                qfSecondFile.close ();
                qfSecondFile.remove ();
                return false;
            }
        }
        int value = int (double (readByteCount) / double (qfFirstFile.size ()) * 100);
        if (value != iPercent) {
            iPercent = value;
            emit changedPercent (iPercent);
        }
        QByteArray qbaBuffer (qfFirstFile.read (bufSize));
        if (qbaBuffer.isEmpty ()) {
            lastError = FO_READ_ERROR;
            if (!error (params)) {
                qfFirstFile.close ();
                qfSecondFile.close ();
                qfSecondFile.remove ();
                return false;
            }
            qbaBuffer.fill (0, bufSize);
        }
        qint64 writePos = qfSecondFile.pos ();
        while (qfSecondFile.write (qbaBuffer) != qbaBuffer.count ()) {
            lastError = FO_WRITE_ERROR;
            if (!error (params)) {
                qfFirstFile.close ();
                qfSecondFile.close ();
                qfSecondFile.remove ();
                return false;
            }
            qfSecondFile.seek (writePos);
        }
        readByteCount += qbaBuffer.size ();
        emit changedValue (qbaBuffer.size ());
    }
    qfFirstFile.close ();
    qfSecondFile.close ();
//Change file date
    copyFileTime (source, dest);
//Change attributes
    copyPermisions (source, dest);

    emit changedPercent (100);
    return true;
}

bool QFileOperationsThread::copyFileTime (const QString &qsSourceFileName, const QString &qsDestFileName)
{
    if (!isLocalFileSystem (qsDestFileName)) {
        return true;
    }

    const QString qsSource = QDir::toNativeSeparators (qsSourceFileName);
    const QString qsDest = QDir::toNativeSeparators (qsDestFileName);

    bool ret = true;
#ifdef Q_WS_WIN
    FILETIME time;
    FILETIME time1;
    FILETIME time2;
    HANDLE firstFileHandle = INVALID_HANDLE_VALUE;
    HANDLE secondFileHandle = INVALID_HANDLE_VALUE;

    firstFileHandle = CreateFile((wchar_t*)qsSource.utf16(),
                                    0,
                                    FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    0, OPEN_EXISTING, 0, 0);
    if (firstFileHandle == INVALID_HANDLE_VALUE)
        return false;

    secondFileHandle = CreateFile((wchar_t*)qsSource.utf16(),
                                    GENERIC_WRITE,
                                    FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    0, OPEN_EXISTING, 0, 0);
    if (secondFileHandle == INVALID_HANDLE_VALUE) {
        CloseHandle(firstFileHandle);
        return false;
    }

    ret = GetFileTime(firstFileHandle, &time, &time1, &time2)
            && SetFileTime(secondFileHandle, &time, &time1, &time2);
    CloseHandle(firstFileHandle);
    CloseHandle(secondFileHandle);
#else
    struct stat st;
    if (stat (QFile::encodeName (qsSource).data (), &st) != 0) {
        return false;
    }

    struct utimbuf time;
    time.actime = st.st_atime;
    time.modtime = st.st_mtime;
    if (utime (QFile::encodeName (qsDest).data (), &time) != 0) {
        return false;
    }
#endif // Q_WS_WIN
    return ret;
}

bool QFileOperationsThread::copyPermisions (const QString &qsSourceFileName, const QString &qsDestFileName)
{
    if (!isLocalFileSystem (qsDestFileName)) {
        return true;
    }

#ifdef Q_WS_WIN
    return SetFileAttributes((wchar_t*)qsDestFileName.utf16(), GetFileAttributes((wchar_t*)qsSourceFileName.utf16()));
#else
    QFile file (qsDestFileName);
    bool ret = file.setPermissions (QFile (qsSourceFileName).permissions ());
#if QT_VERSION < 0x040600 || !defined(IN_TRUNK)
    // workaround for issue with QFSFileEngine::setPermissions() retval
    if (!ret && file.errorString ().isEmpty ()) {
        ret = true;
    }
#endif
    return ret;
#endif // Q_WS_WIN
}

bool QFileOperationsThread::removeFile (const QString &qsFileName)
{
    QStringList params;
    params << QDir::toNativeSeparators (qsFileName);
    lastError = FO_NO_ERROR;
    lastErrorString.clear ();

    emit currentFileCopyChanged (qsFileName, "");
    emit changedPercent (0);
    QFile file (qsFileName);
    QFileInfo fileInfo (file);

    if (fileInfo.isHidden ()
#ifdef Q_WS_WIN
        || (QFileOperationsThread::winFileAttributes(qsFileName) & FILE_ATTRIBUTE_SYSTEM)
#endif
        ) {
        lastError = FO_PERMISIONS_ERROR;
        if (!error (params)) {
            return false;
        }
    }
#ifdef Q_WS_WIN
    if(file.fileEngine()->fileFlags(QAbstractFileEngine::LocalDiskFlag) & QAbstractFileEngine::LocalDiskFlag)
    {
        // Set null attributes
        SetFileAttributes((wchar_t*)qsFileName.utf16(), 0);
    }
#endif

    while (!file.remove ()) {
        lastError = FO_REMOVE_ERROR;
        if (!error (params)) {
            return false;
        }
    }
    emit changedPercent (100);
    emit changedValue (1);
    return true;
}

void QFileOperationsThread::run ()
{
    dirSize = filesCount = dirsCount = iPercent = 0;
    bStopped = false;
    switch (operation) {
        case CopyFileOperation:
            copyFile (qslParametres.at (0), qslParametres.at (1));
            break;
        case CopyDirOperation:
            copyDir (qslParametres.at (0), qslParametres.at (1));
            break;
        case RemoveFileOperation:
            removeFile (qslParametres.at (0));
            break;
        case RemoveDirOperation:
            removeDir (qslParametres.at (0));
            break;
        case MoveFileOperation:
            moveFile (qslParametres.at (0), qslParametres.at (1));
            break;
        case MoveDirOperation:
            moveDir (qslParametres.at (0), qslParametres.at (1));
            break;
        case GetDirSizeOperation:
            calculateDirSize (qslParametres.at (0));
            break;
        default:
            break;
    }
    bStopped = true;
}

void QFileOperationsThread::calculateDirSize (const QString &qsDir)
{
    QDir dir (qsDir);
    dir.setFilter (QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    QDirIterator it (dir, QDirIterator::Subdirectories);
    while (it.hasNext ()) {
        it.next ();

        QFileInfo info (it.fileInfo ());
        if (info.isDir ()) {
            ++dirsCount;
        } else {
            ++filesCount;
            dirSize += info.size ();
        }
    }
    emit changedDirSize (dirSize, dirsCount, filesCount);
}

void QFileOperationsThread::setJob (FileOperation job, const QStringList &params)
{
    operation = job;
    qslParametres = params;
}

#ifdef Q_WS_WIN
bool SetDirTime(QString fileName, FILETIME* dtCreation, FILETIME* dtLastAccessTime, FILETIME* dtLastWriteTime)
{
    bool res = false;
    HANDLE hDir = CreateFile((wchar_t*)fileName.utf16(),
                                GENERIC_READ | GENERIC_WRITE,
                                0, 0,
                                OPEN_EXISTING,
                                FILE_FLAG_BACKUP_SEMANTICS,
                                0);
    if (hDir!=INVALID_HANDLE_VALUE)
    {
        res = SetFileTime(hDir, dtCreation, dtLastAccessTime, dtLastWriteTime);
        CloseHandle(hDir);
    }
    return res;
}

void copyDirTime(const QString& sourceDir, const QString& destDir)
{
    FILETIME dtCreation;
    FILETIME dtLastAccessTime;
    FILETIME dtLastWriteTime;
    HANDLE hDir = CreateFile((wchar_t*)sourceDir.utf16(),
                                GENERIC_READ | GENERIC_WRITE,
                                0, 0,
                                OPEN_EXISTING,
                                FILE_FLAG_BACKUP_SEMANTICS,
                                0);
    if (hDir != INVALID_HANDLE_VALUE)
    {
        if (GetFileTime(hDir, &dtCreation, &dtLastAccessTime, &dtLastWriteTime))
            SetDirTime(destDir, &dtCreation, &dtLastAccessTime, &dtLastWriteTime);
        CloseHandle(hDir);
    }
}
#endif // Q_WS_WIN

bool QFileOperationsThread::copyDir (const QString &qsDirName, const QString &qsDestDir)
{
    QDir sourceDir (qsDirName);

    QDir destDir (qsDestDir);
    destDir.mkdir (sourceDir.dirName ());
    destDir.cd (sourceDir.dirName ());

    sourceDir.cd (sourceDir.dirName ());
    sourceDir.setFilter (QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    QDirIterator it (sourceDir);
    while (it.hasNext ()) {
        it.next ();

        if (bStopped) {
            return false;
        }
        while (isPaused) {
            msleep (50);
            if (bStopped) {
                return false;
            }
        }

        QFileInfo info (it.fileInfo ());
        if (info.isDir ()) {
            if (!copyDir (info.absoluteFilePath (), destDir.absolutePath ())) {
                return false;
            }
        } else {
            if (!copyFile (info.absoluteFilePath (), destDir.absolutePath () + QDir::separator ())) {
                if (isSkipFile || (confirmation & SKIP_ALL)) {
                    isSkipFile = false;
                    bStopped = false;
                } else {
                    return false;
                }
            }
        }
    }
#ifdef Q_WS_WIN
    // Change dir time
    copyDirTime(sourceDir.absolutePath(), destDir.absolutePath());

    // Change dir attributes
    SetFileAttributes((wchar_t*)destDir.absolutePath().utf16(), GetFileAttributes((wchar_t*)sourceDir.absolutePath().utf16()));
#endif
    return true;
}

bool QFileOperationsThread::removeDir (const QString &qsDirName)
{
    QDir dir (qsDirName);
    dir.setFilter (QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    QDirIterator it (dir);
    while (it.hasNext ()) {
        it.next ();

        if (bStopped) {
            return false;
        }
        while (isPaused) {
            msleep (50);
            if (bStopped) {
                return false;
            }
        }

        QFileInfo info (it.fileInfo ());
        if (info.isDir ()) {
            if (!removeDir (info.absoluteFilePath ())) {
                return false;
            }
        } else {
            if (!removeFile (info.absoluteFilePath ())) {
                if (isSkipFile || (confirmation & SKIP_ALL)) {
                    isSkipFile = false;
                    bStopped = false;
                } else {
                    return false;
                }
            }
        }
    }
#ifdef Q_WS_WIN
    //Set null attributes
    SetFileAttributes((wchar_t*)qsDirName.utf16(), 0);
#endif
//
    emit currentFileCopyChanged (dir.absoluteFilePath (qsDirName), QString ());
    while (!dir.rmdir (qsDirName)) {
        lastError = FO_REMOVE_ERROR;
        if (!error (QStringList () << qsDirName)) {
            return false;
        }
    }
    emit changedValue (1);
    return true;
}

bool QFileOperationsThread::moveFile (const QString &qsSourceFileName, const QString &qsDestFileName)
{
    if (!QFileInfo (qsDestFileName).exists () && isSameDisc (qsSourceFileName, qsDestFileName)) {
        return QFile::rename (qsSourceFileName, qsDestFileName);
    }

    if (copyFile (qsSourceFileName, qsDestFileName)) {
        return removeFile (qsSourceFileName);
    }

    return false;
}

bool QFileOperationsThread::moveDir (const QString &qsSourceDir, const QString &qsDestDir)
{
    if (!QFileInfo (qsDestDir).exists () && isSameDisc (qsSourceDir, qsDestDir)) {
        return QFile::rename (qsSourceDir, qsDestDir + QDir (qsSourceDir).dirName ());
    }

    QDir sourceDir (qsSourceDir);

    QDir destDir (qsDestDir);
    destDir.mkdir (sourceDir.dirName ());
    destDir.cd (sourceDir.dirName ());

    sourceDir.cd (sourceDir.dirName ());
    sourceDir.setFilter (QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    QDirIterator it (sourceDir);
    while (it.hasNext ()) {
        it.next ();

        if (bStopped) {
            return false;
        }
        while (isPaused) {
            msleep (50);
            if (bStopped) {
                return false;
            }
        }

        QFileInfo info (it.fileInfo ());
        if (info.isDir ()) {
            if (!moveDir (info.absolutePath (), destDir.absolutePath ())) {
                return false;
            }
        } else {
            if (!moveFile (info.absoluteFilePath (), destDir.absolutePath () + QDir::separator () + info.fileName ())) {
                if (isSkipFile || (confirmation & SKIP_ALL)) {
                    isSkipFile = false;
                    bStopped = false;
                } else {
                    return false;
                }
            }
        }
    }

#ifdef Q_WS_WIN
    // Change dir attributes
    SetFileAttributes((wchar_t*)destDir.absolutePath().utf16(), GetFileAttributes((wchar_t*)sourceDir.absolutePath().utf16()));

    // Remove dir
    // Remove readOnly AND system
    SetFileAttributes((wchar_t*)sourceDir.absolutePath().utf16(), 0);
#endif
//
    sourceDir.rmdir (sourceDir.absolutePath ());
    emit changedValue (1);

    return true;
}

bool QFileOperationsThread::error (const QStringList &params)
{
    errorParams = params;
    bool b;
    if (lastError == FO_DEST_FILE_EXISTS) {
        b = (confirmation & OVEWRITE_ALL);
        if (b) {
            return true;
        }
        b = (confirmation & OVERWRITE_OLDER) &&
            (QFileInfo (errorParams.at (0)).lastModified () > QFileInfo (errorParams.at (1)).lastModified ());
        if (b) {
            return true;
        }
        b = (confirmation & OVERWRITE_NEW) &&
            (QFileInfo (errorParams.at (0)).lastModified () < QFileInfo (errorParams.at (1)).lastModified ());
        if (b) {
            return true;
        }
        b = (confirmation & OVERWRITE_SMALLEST) &&
            (QFileInfo (errorParams.at (0)).size () > QFileInfo (errorParams.at (1)).size ());
        if (b) {
            return true;
        }
        b = (confirmation & OVERWRITE_LAGER) &&
            (QFileInfo (errorParams.at (0)).size () < QFileInfo (errorParams.at (1)).size ());
        if (b) {
            return true;
        }
        b = (confirmation & SKIP_ALL);
        if (b) {
            return false;
        }
    }
    if (lastError == FO_PERMISIONS_ERROR) {
        b = confirmation & OVEWRITE_REMOVE_HIDDEN_SYSTEM;
        if (b) {
            return true;
        }
    }
    if (lastError == FO_REMOVE_ERROR) {
        b = (confirmation & SKIP_ALL) && !QFileInfo (params.at (0)).isDir ();
        if (b) {
            return false;
        }
    }
    if (lastError == FO_READ_ERROR) {
        b = confirmation & SKIP_ALL;
        if (b) {
            return false;
        }
    }
    if (lastError == FO_WRITE_ERROR) {
        b = confirmation & SKIP_ALL;
        if (b) {
            return false;
        }
    }
    slotPause ();
    emit operationError ();
    while (isPaused) {
        this->msleep (50);
    }
    return !bStopped;
}

bool QFileOperationsThread::isLocalFileSystem (const QString &filePath)
{
#if false
#if QT_VERSION < 0x040600 || !defined(IN_TRUNK)
    bool isLocalDisk = false;
    QAbstractFileEngine* engine = QAbstractFileEngine::create(filePath);
    isLocalDisk = (engine->fileFlags(QAbstractFileEngine::LocalDiskFlag) & QAbstractFileEngine::LocalDiskFlag);
    delete engine;
    return isLocalDisk;
#else
    QFileInfo fi(filePath);
    QAbstractFileEngine* engine = fi.fileEngine();
    if(fe)
        return (engine->fileFlags(QAbstractFileEngine::LocalDiskFlag) & QAbstractFileEngine::LocalDiskFlag);
    return false;
#endif
#endif
}

bool QFileOperationsThread::isSameDisc (const QString &sourcePath, const QString &destPath)
{
    //TODO: `source' and `dest' both must be absolutePath-ed and nativeSeparator-ed here
    QString source = QDir::toNativeSeparators (QFileInfo (sourcePath).absolutePath ());
    QString dest = QDir::toNativeSeparators (QFileInfo (destPath).absolutePath ());

    bool res = false;
#ifdef Q_WS_WIN
    if(!source.endsWith(QLatin1Char('\\')))
        source.append(QLatin1Char('\\'));
    if(!dest.endsWith(QLatin1Char('\\')))
        dest.append(QLatin1Char('\\'));

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

    wchar_t sourceVolume[51];
    wchar_t destVolume[51];
    DWORD bufferSize = 50;
    if(ptrGetVolumeNameForVolumeMountPointW((wchar_t*)source.utf16(), sourceVolume, bufferSize) &&
        ptrGetVolumeNameForVolumeMountPointW((wchar_t*)dest.utf16(), destVolume, bufferSize))
    {
        res = (QString::fromWCharArray(sourceVolume) == QString::fromWCharArray(destVolume));
    }

    FreeLibrary(kernelHnd);
#else
    struct stat stSource;
    struct stat stDest;
    stat (QFile::encodeName (source).data (), &stSource);
    stat (QFile::encodeName (dest).data (), &stDest);
    res = (stSource.st_dev == stDest.st_dev);
#endif // Q_WS_WIN

    return res;
}

QString QFileOperationsThread::rootPath (const QString &filePath)
{
#if 0
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
#endif // Q_WS_WIN
    return rootPath;
#endif // 0
    QDir dir (filePath);
    dir.cdUp ();
    while (!dir.isRoot ()) {
        dir.cdUp ();
    }
    return dir.absolutePath ();
}

bool QFileOperationsThread::isRoot (const QString &path)
{
    return path == rootPath (path);
}

bool QFileOperationsThread::execute (const QString &filePath)
{
    return execute (filePath, QStringList (), QDir::currentPath ());
}

bool QFileOperationsThread::execute (const QString &filePath, const QStringList &arguments)
{
    return execute (filePath, arguments, QDir::currentPath ());
}

bool
QFileOperationsThread::execute (const QString &filePath, const QStringList &arguments, const QString &workingDirectory)
{
#ifndef Q_CC_MSVC
#warning "TODO: *panther: respect `arguments' and `workingDirectory'"
#endif
    QFileInfo fi (filePath);
    if (!fi.exists () || !fi.isFile ()) {
        // absolute path; not exists or not a file
        if (!fi.isRelative ()) {
            return false;
        }
        // relative path; see if file exists in current dir
        fi.setFile (QDir::current (), filePath);
        if (!fi.exists () || !fi.isFile ()) {
            return false;
        }
    }

#ifdef Q_WS_WIN
    QString fpath = QDir::toNativeSeparators(fi.absoluteFilePath());
    QString wpath = QDir::toNativeSeparators(workingDirectory);
    if (fi.suffix().toLower() == QLatin1String("bat"))
    {
        return QProcess::startDetached("cmd.exe",
                                QStringList() << "/C" << fpath << arguments,
                                workingDirectory);
    }
    else
    {
        HINSTANCE__* instance;
        instance = ShellExecute(0,
                                (wchar_t*)QString("Open").utf16(),
                                (wchar_t*)fpath.utf16(),
                                (wchar_t*)arguments.join(" ").utf16(),
                                (wchar_t*)wpath.utf16(),
                                SW_NORMAL);
        if((int)instance > 32)
            return true;

        if((int)instance == SE_ERR_NOASSOC)
        {
            instance = ShellExecute(0,
                                    (wchar_t*)QString("open").utf16(),
                                    (wchar_t*)QString("rundll32.exe").utf16(),
                                    (wchar_t*)QString("shell32.dll,OpenAs_RunDLL "+fpath).utf16(),
                                    0,
                                    SW_SHOWNORMAL);
            if((int)instance > 32)
                return true;
        }
    }
#else
    if (fi.isExecutable ()) {
        if (QProcess::startDetached (fi.absoluteFilePath (), arguments, workingDirectory)) {
            return true;
        }
    }
#endif // Q_WS_WIN
    return QDesktopServices::openUrl (QUrl (filePath));
}

#ifdef Q_WS_WIN
qint64 QFileOperationsThread::winFileAttributes(const QString& filePath)
{
    DWORD fileAttrib = INVALID_FILE_ATTRIBUTES;
    if(isLocalFileSystem(filePath))
    {
        QString path = QDir::fromNativeSeparators(filePath);
        if(path.length() == 2 && path.at(1) == QLatin1Char(':'))
            path += QLatin1Char('\\');

        fileAttrib = ::GetFileAttributes((wchar_t*)path.utf16());
        if (fileAttrib == INVALID_FILE_ATTRIBUTES)
        {
            // path for FindFirstFile should not be end in a trailing slash or slosh
            while(path.endsWith(QLatin1Char('\\')))
                path.resize(path.size() - 1);

            HANDLE findFileHandle = INVALID_HANDLE_VALUE;
            WIN32_FIND_DATA findData;
            findFileHandle = ::FindFirstFile((wchar_t*)path.utf16(), &findData);
            if (findFileHandle != INVALID_HANDLE_VALUE)
            {
                ::FindClose(findFileHandle);
                fileAttrib = findData.dwFileAttributes;
            }
        }
    }
    return qint64(fileAttrib);
}
#endif // Q_WS_WIN
