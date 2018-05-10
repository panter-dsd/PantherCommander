#ifndef _QZIPWRITER_H
#define _QZIPWRITER_H

#include <QtCore/QFile>
#include <QtCore/QString>

class QZipWriterPrivate;

class QZipWriter
{
public:
    QZipWriter (const QString &fileName, QIODevice::OpenMode mode = (QIODevice::WriteOnly | QIODevice::Truncate));

    explicit QZipWriter (QIODevice *device);

    ~QZipWriter ();

    QIODevice *device () const;

    bool isWritable () const;

    bool exists () const;

    enum Status
    {
        NoError,
        FileWriteError,
        FileOpenError,
        FilePermissionsError,
        FileError
    };

    Status status () const;

    enum CompressionPolicy
    {
        AlwaysCompress,
        NeverCompress,
        AutoCompress
    };

    void setCompressionPolicy (CompressionPolicy policy);

    CompressionPolicy compressionPolicy () const;

    void setCreationPermissions (QFile::Permissions permissions);

    QFile::Permissions creationPermissions () const;

    void addFile (const QString &fileName, const QByteArray &data);

    void addFile (const QString &fileName, QIODevice *device);

    void addDirectory (const QString &dirName);

    void addSymLink (const QString &fileName, const QString &destination);

    void close ();

private:
    QZipWriterPrivate *d;
    Q_DISABLE_COPY(QZipWriter)
};

#endif // _QZIPWRITER_H
