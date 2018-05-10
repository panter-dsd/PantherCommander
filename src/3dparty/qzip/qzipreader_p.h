#ifndef _QZIPREADER_H
#define _QZIPREADER_H

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QString>

class QZipReaderPrivate;

class QZipReader
{
public:
    QZipReader (const QString &fileName, QIODevice::OpenMode mode = QIODevice::ReadOnly);

    explicit QZipReader (QIODevice *device);

    virtual ~QZipReader ();

    QIODevice *device () const;

    bool isReadable () const;

    bool exists () const;

    struct FileInfo
    {
        FileInfo ();

        FileInfo (const FileInfo &other);

        ~FileInfo ();

        FileInfo &operator= (const FileInfo &other);

        bool isValid () const;

        QString filePath;
        uint isDir
            : 1;
        uint isFile
            : 1;
        uint isSymLink
            : 1;
        QFile::Permissions permissions;
        uint crc32;
        qint64 size;
        QDateTime lastModified;
        void *d;
    };

    QList<FileInfo> fileInfoList () const;

    int count () const;

    FileInfo entryInfoAt (int index) const;

    QByteArray fileData (const QString &fileName) const;

    bool extractAll (const QString &destinationDir) const;

    enum Status
    {
        NoError,
        FileReadError,
        FileOpenError,
        FilePermissionsError,
        FileError
    };

    Status status () const;

    void close ();

private:
    QZipReaderPrivate *d;
    Q_DISABLE_COPY(QZipReader)
};

#endif // _QZIPREADER_H
