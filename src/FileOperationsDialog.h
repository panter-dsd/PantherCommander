#pragma once

//
class QLabel;

class QProgressBar;

class QToolButton;

class QAction;

class QListWidget;
//
#include <QDialog>
#include <QTime>
#include "FileOperationsThread.h"
#include "FileOperationsConfirmationDialog.h"

//
class FileOperationsDialog : public QDialog
{
Q_OBJECT
private:
    struct SJob
    {
        FileOperationsThread::FileOperation operation;
        QStringList params;
        qint64 value;
    };
private:
    QLabel *qlSpeed;
    QLabel *qlFrom;
    QLabel *qlTo;
    QLabel *qlCurrentFile;
    QProgressBar *qprbCurrentFile;
    QLabel *qlCurrentJob;
    QProgressBar *qprbCurrentJob;
    QLabel *qlAllJobs;
    QProgressBar *qprbAllJobs;
    QToolButton *qtbPauseResume;
    QToolButton *qtbCancel;
    QToolButton *qtbShowHideJobList;
    QListWidget *qlwJobs;
    SJob currentJob;
    QList<SJob> qlJobs;
    FileOperationsThread *qfotOperatinThread;
    QTime qtWorkTime;

    QAction *actionPauseResume;
    QAction *actionCancel;
    QAction *actionShowHideJobList;

    qint64 currentJobValue;
    qint64 jobsValue;
    qint64 jobsCompleteValue;
    qint64 currentJobCompleteValue;

    bool isBlocked;
    int addingJobs;

public:
    FileOperationsDialog (QWidget *parent = 0,
                           Qt::WindowFlags f = Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);

    ~FileOperationsDialog ();

    void addJob (FileOperationsThread::FileOperation operation, QStringList parameters);

    QString jobName ();

/*Блокировка при множественном добавлении*/
    void setBlocked (bool blocked)
    {
        isBlocked = blocked;
        if (!isBlocked) {
            slotNextJob ();
        }
    }

private:
    void createControls ();

    void setLayouts ();

    void createActions ();

    void setConnects ();

    void updateListJobs ();

    void saveSettings ();

    void loadSettings ();

    QString getSizeStr (double size) const;

    QStringList normalizeFileNames (const QStringList &names);

    QString normalizeFileName (const QString &name);

private Q_SLOTS:

    void slotPauseResume ();

    void slotShowHideJobList ();

    void slotCancel ();

    void slotNextJob ();

    void slotValueChanged (qint64 value);

    void slotCurrentFileCopyChanged (const QString &sourceFile, const QString &destFile);

    void slotAddingJob (SJob &job);

    void slotOperationError ();

Q_SIGNALS:

    void jobChanged ();

    void jobAdding (SJob &job);
};
