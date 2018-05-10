#pragma once

#include <QThread>

class FileOperation;

class FileOperationThreadPrivate;

class FileOperation;

class FileOperationThread : public QThread
{
Q_OBJECT

public:
    explicit FileOperationThread (QObject *parent = 0);

    virtual ~FileOperationThread ();

    int addOperation (const FileOperation &operation);

    bool removeOperation (int ID);

    bool insertOperation (int ID, const FileOperation &operation);

    FileOperation operation (int ID) const;

protected:
    void run ();

private:
    Q_DISABLE_COPY(FileOperationThread)

    Q_DECLARE_PRIVATE(FileOperationThread)

    FileOperationThreadPrivate *const d_ptr;
};

