#pragma once

#include "FileOperationThread.h"
#include "FileOperations.h"

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

