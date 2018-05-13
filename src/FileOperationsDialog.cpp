#include "FileOperationsDialog.h"

#include  <QtGui>
#include  <QtWidgets>

#include "AppSettings.h"

FileOperationsDialog::FileOperationsDialog (QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    createControls ();
    setLayouts ();
    createActions ();
    loadSettings ();
    qfotOperatinThread = new FileOperationsThread ();
    jobsCompleteValue = jobsValue = addingJobs = 0;
    qtWorkTime = QTime::currentTime ();
    setConnects ();
    slotShowHideJobList ();
    isBlocked = false;
}


FileOperationsDialog::~FileOperationsDialog ()
{
    saveSettings ();
    delete qfotOperatinThread;
}


void FileOperationsDialog::createControls ()
{
    qlSpeed = new QLabel (this);
    qlSpeed->setAlignment (Qt::AlignHCenter);
    qlFrom = new QLabel (this);
    qlFrom->setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Preferred);
    qlTo = new QLabel (this);
    qlTo->setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Preferred);
    qlCurrentFile = new QLabel (tr ("Current file"), this);
    qprbCurrentFile = new QProgressBar (this);
    qprbCurrentFile->setRange (0, 100);
    qlCurrentJob = new QLabel (tr ("Current job"), this);
    qprbCurrentJob = new QProgressBar (this);
    qprbCurrentJob->setRange (0, 100);
    qlAllJobs = new QLabel (tr ("All jobs"), this);
    qprbAllJobs = new QProgressBar (this);
    qprbAllJobs->setRange (0, 100);

    qtbPauseResume = new QToolButton (this);
    qtbPauseResume->setToolButtonStyle (Qt::ToolButtonTextOnly);
    qtbCancel = new QToolButton (this);
    qtbCancel->setToolButtonStyle (Qt::ToolButtonTextOnly);
    qtbShowHideJobList = new QToolButton (this);
    qtbShowHideJobList->setToolButtonStyle (Qt::ToolButtonTextOnly);

    qlwJobs = new QListWidget (this);
}


void FileOperationsDialog::setLayouts ()
{
    QGridLayout *qglProgressLayuout = new QGridLayout ();
    qglProgressLayuout->addWidget (qlCurrentFile, 0, 0);
    qglProgressLayuout->addWidget (qprbCurrentFile, 0, 1);
    qglProgressLayuout->addWidget (qlCurrentJob, 1, 0);
    qglProgressLayuout->addWidget (qprbCurrentJob, 1, 1);
    qglProgressLayuout->addWidget (qlAllJobs, 2, 0);
    qglProgressLayuout->addWidget (qprbAllJobs, 2, 1);

    QHBoxLayout *qhblButtonsLayout = new QHBoxLayout ();
    qhblButtonsLayout->addWidget (qtbShowHideJobList);
    qhblButtonsLayout->addWidget (qtbPauseResume);
    qhblButtonsLayout->addWidget (qtbCancel);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (qlSpeed);
    qvblMainLayout->addWidget (qlFrom);
    qvblMainLayout->addWidget (qlTo);
    qvblMainLayout->addLayout (qglProgressLayuout);
    qvblMainLayout->addLayout (qhblButtonsLayout);
    qvblMainLayout->addWidget (qlwJobs);

    this->setLayout (qvblMainLayout);
}


void FileOperationsDialog::createActions ()
{
    actionPauseResume = new QAction (tr ("Pause"), this);
    actionPauseResume->setCheckable (true);
    qtbPauseResume->setDefaultAction (actionPauseResume);

    actionCancel = new QAction (tr ("Cancel"), this);
    qtbCancel->setDefaultAction (actionCancel);

    actionShowHideJobList = new QAction (tr ("More"), this);
    actionShowHideJobList->setCheckable (true);
    qtbShowHideJobList->setDefaultAction (actionShowHideJobList);
}


void FileOperationsDialog::setConnects ()
{
    connect (actionPauseResume, &QAction::triggered, this, &FileOperationsDialog::slotPauseResume);
    connect (actionCancel, &QAction::triggered, this, &FileOperationsDialog::slotCancel);
    connect (actionShowHideJobList, &QAction::triggered, this, &FileOperationsDialog::slotShowHideJobList);
    connect (this, &FileOperationsDialog::jobAdding, this, &FileOperationsDialog::slotAddingJob);

    connect (qfotOperatinThread, &FileOperationsThread::finished,
             this, &FileOperationsDialog::slotNextJob
    );
    connect (qfotOperatinThread, &FileOperationsThread::changedPercent,
             qprbCurrentFile, &QProgressBar::setValue
    );
    connect (qfotOperatinThread, &FileOperationsThread::changedValue,
             this, &FileOperationsDialog::slotValueChanged
    );
    connect (qfotOperatinThread, &FileOperationsThread::currentFileCopyChanged,
             this, &FileOperationsDialog::slotCurrentFileCopyChanged,
             Qt::BlockingQueuedConnection
    );
    connect (qfotOperatinThread, &FileOperationsThread::operationError,
             this, &FileOperationsDialog::slotOperationError
    );
}


void FileOperationsDialog::slotPauseResume ()
{
    if (actionPauseResume->isChecked ()) {
        actionPauseResume->setText (tr ("Resume"));
        qfotOperatinThread->slotPause ();
    } else {
        actionPauseResume->setText (tr ("Pause"));
        qfotOperatinThread->slotResume ();
    }
}


void FileOperationsDialog::slotCancel ()
{
    qfotOperatinThread->slotPause ();
    if (qlJobs.isEmpty ()) {
        int rez = QMessageBox::question (this,
                                         "",
                                         tr ("Are you shure canceled this job?"),
                                         QMessageBox::Yes | QMessageBox::No
        );
        if (rez == QMessageBox::Yes) {
            qfotOperatinThread->stop ();
        } else {
            qfotOperatinThread->slotResume ();
        }
        return;
    }
    QMessageBox *qmbCancelDialog = new QMessageBox (this);
    QPushButton *currenJob = qmbCancelDialog->addButton (tr ("Current job"),
                                                         QMessageBox::ActionRole
    );
    QPushButton *allJobs = qmbCancelDialog->addButton (tr ("All job"),
                                                       QMessageBox::ActionRole
    );
    qmbCancelDialog->addButton (QMessageBox::Cancel);

    qmbCancelDialog->setText (tr ("What do you want canceled?"));
    qmbCancelDialog->exec ();
    if (qmbCancelDialog->clickedButton () == currenJob) {
        jobsCompleteValue = 0;
        jobsValue -= currentJob.value;
        qfotOperatinThread->stop ();
    }
    if (qmbCancelDialog->clickedButton () == allJobs) {
        qlJobs.clear ();
        qfotOperatinThread->stop ();
    }
    delete qmbCancelDialog;
}


void FileOperationsDialog::addJob (FileOperationsThread::FileOperation operation, QStringList parameters)
{
    addingJobs++;
    SJob job;
    job.operation = operation;
    job.params = normalizeFileNames (parameters);
    job.value = 0;
    qlJobs << job;
    emit jobAdding (qlJobs.last ());
    updateListJobs ();
    addingJobs--;
}


void FileOperationsDialog::slotAddingJob (SJob &job)
{
    FileOperationsThread *qfotCalculateValueThread = new FileOperationsThread ();
    qfotCalculateValueThread->setJob (FileOperationsThread::GetDirSizeOperation, job.params);
    qfotCalculateValueThread->start ();
    while (qfotCalculateValueThread->isRunning ()) {
        qfotCalculateValueThread->wait (50);
        qApp->processEvents ();
    }
    qint64 size, dirsCount, filesCount, value;
    qfotCalculateValueThread->getDirSize (size, dirsCount, filesCount);
    delete qfotCalculateValueThread;
    switch (job.operation) {
        case FileOperationsThread::CopyDirOperation: {
            value = size;
            break;
        }
        case FileOperationsThread::RemoveDirOperation: {
            value = dirsCount + filesCount;
            break;
        }
        case FileOperationsThread::CopyFileOperation: {
            value = QFile (job.params.at (0)).size ();
            break;
        }
        case FileOperationsThread::RemoveFileOperation: {
            value = 1;
            break;
        }
        case FileOperationsThread::MoveFileOperation: {
            value = QFile (job.params.at (0)).size ();
            break;
        }
        case FileOperationsThread::MoveDirOperation: {
            value = dirsCount + filesCount + size;
            break;
        }
        default:
            value = 1;
    }

    value = (value == 0) ? 1 : value;
    job.value = value;
    jobsValue += value;
}


void FileOperationsDialog::updateListJobs ()
{
    qlwJobs->clear ();
    QListWidgetItem *item;
    for (int i = 0; i < qlJobs.count (); i++) {
        QString jobText;
        switch (qlJobs.at (i).operation) {
            case FileOperationsThread::CopyFileOperation:
                jobText = tr ("Copy: ");
                break;
            case FileOperationsThread::CopyDirOperation:
                jobText = tr ("Copy: ");
                break;
            case FileOperationsThread::RemoveFileOperation:
                jobText = tr ("Remove: ");
                break;
            case FileOperationsThread::RemoveDirOperation:
                jobText = tr ("Remove: ");
                break;
            default:
                break;
        }
        jobText += qlJobs.at (i).params.join (" -> ");
        item = new QListWidgetItem (jobText, qlwJobs);
        qlwJobs->addItem (item);
    }
}


void FileOperationsDialog::slotNextJob ()
{
    if (qfotOperatinThread->isRunning ()) {
        return;
    }
    if (qlJobs.isEmpty () && !isBlocked && addingJobs == 0) {
        this->close ();
        return;
    }
    while (qlJobs.first ().value == 0) {
        qApp->processEvents ();
        qlSpeed->setText (tr ("Calculating job value"));
    }
    qfotOperatinThread->setJob (qlJobs.first ().operation, qlJobs.first ().params);
    currentJobValue = qlJobs.first ().value;
    currentJobCompleteValue = 0;
//Current job
    currentJob.operation = qlJobs.first ().operation;
    currentJob.params = qlJobs.first ().params;
    currentJob.value = qlJobs.first ().value;

    qlJobs.removeFirst ();
    updateListJobs ();
    emit jobChanged ();
//	if (actionPauseResume->isChecked())
//	{
//		actionPauseResume->setChecked(false);
//		slotPauseResume();
//	}
//	qfotOperatinThread->slotResume();
    qfotOperatinThread->start ();
}


void FileOperationsDialog::slotValueChanged (qint64 value)
{
    qApp->processEvents ();
    currentJobCompleteValue += value;
    qprbCurrentJob->setValue (int ((double) currentJobCompleteValue / (double) (currentJob.value) * 100));
    jobsCompleteValue += value;
    qprbAllJobs->setValue (int ((double) jobsCompleteValue / (double) jobsValue * 100));
//Caption
    QString caption = "[" + QString::number (qprbAllJobs->value ()) + " %] ";
    switch (currentJob.operation) {
        case FileOperationsThread::CopyFileOperation:
            caption += tr ("Copy");
            break;
        case FileOperationsThread::RemoveFileOperation:
            caption += tr ("Remove");
            break;
        case FileOperationsThread::CopyDirOperation:
            caption += tr ("Copy");
            break;
        case FileOperationsThread::RemoveDirOperation:
            caption += tr ("Remove");
            break;
        case FileOperationsThread::MoveFileOperation:
            caption += tr ("Move");
            break;
        case FileOperationsThread::MoveDirOperation:
            caption += tr ("Move");
            break;
        default:
            break;
    }
    this->setWindowTitle (caption);

    int time = qtWorkTime.secsTo (QTime::currentTime ());
    if (time == 0) {
        qlSpeed->setText (tr ("Working"));
    } else {
        double speed = (double) jobsCompleteValue / (double) time;
        int secs = int ((jobsValue - jobsCompleteValue) / speed);
        switch (currentJob.operation) {
            case FileOperationsThread::CopyFileOperation:
                qlSpeed->setText (tr ("Coping %1/s. To job complete %2").arg (getSizeStr (speed)).arg (
                    QTime ().addSecs (secs).toString ("hh:mm:ss")));
                break;
            case FileOperationsThread::RemoveFileOperation:
                qlSpeed->setText (
                    tr ("Deleting. To job complete %1").arg (QTime ().addSecs (secs).toString ("hh:mm:ss")));
                break;
            case FileOperationsThread::CopyDirOperation:
                qlSpeed->setText (tr ("Coping %1/s. To job complete %2").arg (getSizeStr (speed)).arg (
                    QTime ().addSecs (secs).toString ("hh:mm:ss")));
                break;
            case FileOperationsThread::RemoveDirOperation:
                qlSpeed->setText (
                    tr ("Deleting. To job complete %1").arg (QTime ().addSecs (secs).toString ("hh:mm:ss")));
                break;
            case FileOperationsThread::MoveFileOperation:
                qlSpeed->setText (tr ("Moving %1/s. To job complete %2").arg (getSizeStr (speed)).arg (
                    QTime ().addSecs (secs).toString ("hh:mm:ss")));
                break;
            case FileOperationsThread::MoveDirOperation:
                qlSpeed->setText (tr ("Moving %1/s. To job complete %2").arg (getSizeStr (speed)).arg (
                    QTime ().addSecs (secs).toString ("hh:mm:ss")));
                break;
            default:
                break;
        }
    }
}


void FileOperationsDialog::slotCurrentFileCopyChanged (const QString &sourceFile, const QString &destFile)
{
    QString source = QDir::toNativeSeparators (sourceFile);

    qlFrom->setText (qlFrom->fontMetrics ().elidedText (source, Qt::ElideMiddle, qlFrom->width ()));
    qlFrom->setToolTip (source);

    if (!destFile.isEmpty ()) {
        QString dest = QDir::toNativeSeparators (destFile);
        qlTo->setText (qlTo->fontMetrics ().elidedText (dest, Qt::ElideMiddle, qlTo->width ()));
        qlTo->setToolTip (dest);
    }
    qApp->processEvents ();
}


QString FileOperationsDialog::jobName ()
{
    return currentJob.params.join (" -> ");
}


QString FileOperationsDialog::getSizeStr (double size) const
{
    if (size < 1024) {
        return QString (tr ("%1 b ")).arg (size);
    }
    if ((size >= 1024) && (size < 1048576)) {
        return QString (tr ("%1 Kb")).arg (double (size) / 1024, 0, 'f', 2);
    }
    if ((size >= 1048576) && (size < 1073741824)) {
        return QString (tr ("%1 Mb")).arg (double (size) / 1048576, 0, 'f', 2);
    }
    if (size >= 1073741824) {
        return QString (tr ("%1 Gb")).arg (double (size) / 1073741824, 0, 'f', 2);
    }
    return "";
}


void FileOperationsDialog::slotShowHideJobList ()
{
    qlwJobs->setVisible (actionShowHideJobList->isChecked ());
#ifndef Q_CC_MSVC
#warning "This is not good, but i'm not know which"
#endif
    qApp->processEvents ();
    resize (size ().width (), 1);
}


void FileOperationsDialog::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("FileOperationsDialog");
    settings->setValue ("size", size ());
    settings->setValue ("ShowJobsList", actionShowHideJobList->isChecked ());
    settings->endGroup ();
}


void FileOperationsDialog::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("FileOperationsDialog");
    resize (settings->value ("size", QSize (200, 100)).toSize ());
    actionShowHideJobList->setChecked (settings->value ("ShowJobsList", false).toBool ());
    settings->endGroup ();
}


void FileOperationsDialog::slotOperationError ()
{
    FileOperationsConfirmationDialog *qfocdDialog;
    qfocdDialog = new FileOperationsConfirmationDialog (this);
    const QStringList &params = qfotOperatinThread->getErrorParams ();
    switch (qfotOperatinThread->getLastError ()) {
        case FileOperationsThread::FO_DEST_FILE_EXISTS:
            qfocdDialog->setFiles (params.at (0), params.at (1));
            qfocdDialog->setButtons (FileOperationsConfirmationDialog::Ovewrite |
                                     FileOperationsConfirmationDialog::OvewriteAll |
                                     FileOperationsConfirmationDialog::Cancel |
                                     FileOperationsConfirmationDialog::Skip |
                                     FileOperationsConfirmationDialog::SkipAll |
                                     FileOperationsConfirmationDialog::Rename |
                                     FileOperationsConfirmationDialog::OvewriteAllOlder |
                                     FileOperationsConfirmationDialog::OvewriteAllNew |
                                     FileOperationsConfirmationDialog::OvewriteAllSmallest |
                                     FileOperationsConfirmationDialog::OvewriteAllLager
            );
            break;
        case FileOperationsThread::FO_PERMISIONS_ERROR:
            qfocdDialog->setButtons (FileOperationsConfirmationDialog::Delete |
                                     FileOperationsConfirmationDialog::All |
                                     FileOperationsConfirmationDialog::Skip |
                                     FileOperationsConfirmationDialog::Cancel
            );
            qfocdDialog->setText (tr ("File %1 is hidden or system. Delete anyway?").arg (params.at (0)));
            break;
        case FileOperationsThread::FO_REMOVE_ERROR:
            qfocdDialog->setButtons (FileOperationsConfirmationDialog::Skip |
                                     FileOperationsConfirmationDialog::SkipAll |
                                     FileOperationsConfirmationDialog::Retry |
                                     FileOperationsConfirmationDialog::Cancel
            );
            qfocdDialog->setText (tr ("Error remove file %1").arg (params.at (0)));
            break;
        case FileOperationsThread::FO_OPEN_ERROR:
            qfocdDialog->setButtons (FileOperationsConfirmationDialog::Skip |
                                     FileOperationsConfirmationDialog::Retry |
                                     FileOperationsConfirmationDialog::Cancel
            );
            qfocdDialog->setText (tr ("Error open file %1").arg (params.at (0)));
            break;
        case FileOperationsThread::FO_READ_ERROR:
            qfocdDialog->setButtons (FileOperationsConfirmationDialog::Skip |
                                     FileOperationsConfirmationDialog::SkipAll |
                                     FileOperationsConfirmationDialog::Retry |
                                     FileOperationsConfirmationDialog::Cancel
            );
            qfocdDialog->setText (tr ("Error read file %1").arg (params.at (0)));
            break;
        case FileOperationsThread::FO_WRITE_ERROR:
            qfocdDialog->setButtons (FileOperationsConfirmationDialog::Skip |
                                     FileOperationsConfirmationDialog::SkipAll |
                                     FileOperationsConfirmationDialog::Retry |
                                     FileOperationsConfirmationDialog::Cancel
            );
            qfocdDialog->setText (tr ("Error write file %1").arg (params.at (1)));
            break;
        case FileOperationsThread::FO_RESIZE_ERROR:
            qfocdDialog->setButtons (FileOperationsConfirmationDialog::Skip |
                                     FileOperationsConfirmationDialog::SkipAll |
                                     FileOperationsConfirmationDialog::Retry |
                                     FileOperationsConfirmationDialog::Cancel
            );
            qfocdDialog->setText (tr ("Error write file %1.\nMaby not free space.").arg (params.at (1)));
            break;
        default:
            qfocdDialog->setButtons (FileOperationsConfirmationDialog::Skip |
                                     FileOperationsConfirmationDialog::Cancel
            );
            qfocdDialog->setText (tr ("Unknow error %1").arg (params.join ("\n")));
            break;
    }
    int res = -1;
    if (qfocdDialog->exec ()) {
        res = qfocdDialog->getResult ();
    }
    delete qfocdDialog;
    switch (res) {
        case FileOperationsConfirmationDialog::Ovewrite:
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::OvewriteAll:
            qfotOperatinThread->confirmation |= FileOperationsThread::OVEWRITE_ALL;
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::Cancel:
            slotCancel ();
            qfotOperatinThread->stop ();
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::Skip:
            qfotOperatinThread->skipFile ();
            qfotOperatinThread->stop ();
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::SkipAll:
            qfotOperatinThread->confirmation |= FileOperationsThread::SKIP_ALL;
            qfotOperatinThread->skipFile ();
            qfotOperatinThread->stop ();
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::OvewriteAllOlder:
            qfotOperatinThread->confirmation |= FileOperationsThread::OVERWRITE_OLDER;
            if (QFileInfo (params.at (0)).lastModified () <= QFileInfo (params.at (1)).lastModified ()) {
                qfotOperatinThread->stop ();
            }
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::OvewriteAllNew:
            qfotOperatinThread->confirmation |= FileOperationsThread::OVERWRITE_NEW;
            if (QFileInfo (params.at (0)).lastModified () >= QFileInfo (params.at (1)).lastModified ()) {
                qfotOperatinThread->stop ();
            }
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::OvewriteAllSmallest:
            qfotOperatinThread->confirmation |= FileOperationsThread::OVERWRITE_SMALLEST;
            if (QFileInfo (params.at (0)).size () <= QFileInfo (params.at (1)).size ()) {
                qfotOperatinThread->stop ();
            }
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::OvewriteAllLager:
            qfotOperatinThread->confirmation |= FileOperationsThread::OVERWRITE_LAGER;
            if (QFileInfo (params.at (0)).size () >= QFileInfo (params.at (1)).size ()) {
                qfotOperatinThread->stop ();
            }
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::Delete:
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::All:
            qfotOperatinThread->confirmation |= FileOperationsThread::OVEWRITE_REMOVE_HIDDEN_SYSTEM;
            qfotOperatinThread->slotResume ();
            break;
        case FileOperationsConfirmationDialog::Retry:
            qfotOperatinThread->slotResume ();
            break;
        default:
            qfotOperatinThread->stop ();
            qfotOperatinThread->slotResume ();
            break;
    }
}


QStringList FileOperationsDialog::normalizeFileNames (const QStringList &names)
{
    QStringList list;
        foreach(const QString &name, names) {
            list.append (normalizeFileName (name));
        }
    return list;
}


QString FileOperationsDialog::normalizeFileName (const QString &name)
{
    static QString sep (2, QDir::separator ());

    QString res (name);
    res.replace (sep, QDir::separator ());
    if (res.startsWith (QDir::separator ())) {
        res.insert (0, QDir::separator ());
    }
    return res;
}

