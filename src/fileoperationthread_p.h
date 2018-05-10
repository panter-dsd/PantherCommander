#ifndef FILEOPERATIONTHREAD_P_H
#define FILEOPERATIONTHREAD_P_H

#include "fileoperationthread.h"
#include "fileoperations.h"

class QMutex;

#include <QtCore/QObject>

class FileOperationThreadPrivate
{
    Q_DECLARE_PUBLIC(FileOperationThread)

public:
    FileOperationThreadPrivate ();

    ~FileOperationThreadPrivate ();

    void nextOperation ();

    void copy (FileOperation *operation);

    void copyDir (const QString &source, const QString &dest);

    void copyFile (const QString &source, const QString &dest);

    FileOperationThread *q_ptr;

    QList<FileOperation> operationList;
    int currentID;

private:
    mutable QMutex mutex;
};

#endif // FILEOPERATIONTHREAD_P_H
