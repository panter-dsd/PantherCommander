#include "findfilesdialog.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>

#include "dirsorter.h"
#include "appsettings.h"

//TODO: implement in-text binary search algorithm
//TODO: support regexp patterns in file mask and search text
//TODO: search for dirs - not for files only
//TODO: separate thread for searching (dir entries + entries sort + text search)
//TODO: implement search configuration gui
//TODO: QProgressDialog --> QLabel (or QProgressBar at very least)
//TODO: QTableWidget --> QTreeView
//NOTE: No settings load/save!!!
//NOTE: Remember about QtCodingStyle ;)
//NOTE: Text-only content searching for now

static QFileInfoList enumerateDir (const QDir &dir, QDirIterator::IteratorFlags iteratorFlags)
{
    QFileInfoList entries;
    QDirIterator it (dir);
    while (it.hasNext ()) {
        it.next ();
        entries.append (it.fileInfo ());
    }
    Dir::sortFileList (entries, dir.sorting ());

    if (iteratorFlags & QDirIterator::Subdirectories) {
        for (int i = 0, n = entries.size (); i < n; ++i) {
            QFileInfo fi (entries[i]);
            if ((iteratorFlags & QDirIterator::FollowSymlinks) && fi.isSymLink ()) {
                fi.setFile (fi.symLinkTarget ());
            }
            if (fi.isDir ()) {
                QDir subdir (fi.absoluteFilePath ());
                subdir.setNameFilters (dir.nameFilters ());
                subdir.setFilter (dir.filter ());
                subdir.setSorting (dir.sorting ());

                entries.append (enumerateDir (subdir, iteratorFlags));
            }
        }
    }

    return entries;
}

static bool findText (const QFileInfo &fileInfo, const QString &text)
{
    if (!fileInfo.isFile ()) {
        return true;
    }

    QFile file (fileInfo.absoluteFilePath ());
    if (file.open (QIODevice::ReadOnly)) {
        QTextStream in (&file);
        while (!in.atEnd ()) {
            if (in.readLine ().contains (text)) {
                return true;
            }
        }
    }

    return false;
}

//TODO: preferences, tabs, etc.
FindFilesDialog::FindFilesDialog (QWidget *parent)
    : QDialog (parent)
{
    tabber = new QTabWidget (this);

    QWidget *preferencesPage = new QWidget (this);

    browseButton = new QPushButton (tr ("&Browse..."), this);
    connect (browseButton, SIGNAL(clicked ()), this, SLOT(browse ()));

    fileComboBox = new QComboBox (this);
    fileComboBox->setEditable (true);
    fileComboBox->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Preferred);
    fileComboBox->addItem (QLatin1String ("*"));

    textComboBox = new QComboBox (this);
    textComboBox->setEditable (true);
    textComboBox->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Preferred);

    directoryComboBox = new QComboBox (this);
    directoryComboBox->setEditable (true);
    directoryComboBox->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Preferred);
    directoryComboBox->addItem (QDir::currentPath ());

    fileLabel = new QLabel (tr ("Named:"), this);
    textLabel = new QLabel (tr ("Containing text:"), this);
    directoryLabel = new QLabel (tr ("In directory:"), this);

    QGridLayout *preferencesPageLayout = new QGridLayout;
    preferencesPageLayout->addWidget (fileLabel, 0, 0);
    preferencesPageLayout->addWidget (fileComboBox, 0, 1, 1, 2);
    preferencesPageLayout->addWidget (textLabel, 1, 0);
    preferencesPageLayout->addWidget (textComboBox, 1, 1, 1, 2);
    preferencesPageLayout->addWidget (directoryLabel, 2, 0);
    preferencesPageLayout->addWidget (directoryComboBox, 2, 1);
    preferencesPageLayout->addWidget (browseButton, 2, 2);
    preferencesPageLayout->addWidget (new QWidget (this), 3, 0, 1, 3);
    preferencesPageLayout->setRowStretch (3, 10);
    preferencesPage->setLayout (preferencesPageLayout);

    tabber->addTab (preferencesPage, tr ("&Preferences"));

    QWidget *resultsPage = new QWidget (this);

    filesTable = new QTableWidget (0, 3, this);
    filesTable->setSelectionBehavior (QAbstractItemView::SelectRows);
    filesTable->setHorizontalHeaderLabels (QStringList () << tr ("File Name") << tr ("File Path") << tr ("Size"));
#if false
    filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
#endif
    filesTable->verticalHeader ()->hide ();
    filesTable->setShowGrid (false);

    connect (filesTable, SIGNAL(cellActivated (int, int)),
             this, SLOT(_slot_cellActivated (int, int)));

    filesFoundLabel = new QLabel (this);

    QVBoxLayout *resultsPageLayout = new QVBoxLayout;
    resultsPageLayout->addWidget (filesTable);
    resultsPageLayout->addWidget (filesFoundLabel);
    resultsPage->setLayout (resultsPageLayout);

    tabber->addTab (resultsPage, tr ("&Results"));

    findButton = new QPushButton (tr ("&Find"), this);
    connect (findButton, SIGNAL(clicked ()), this, SLOT(find ()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch ();
    buttonsLayout->addWidget (findButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget (tabber);
    mainLayout->addLayout (buttonsLayout);
    setLayout (mainLayout);

    setWindowTitle (tr ("Find Files"));
    resize (700, 300);
}

FindFilesDialog::~FindFilesDialog ()
{
}

void FindFilesDialog::browse ()
{
    QFileDialog::Options options;
    if (!AppSettings::instance ()->useNativeDialogs ()) {
        options = QFileDialog::DontUseNativeDialog;
    }
    QString directory = QFileDialog::getExistingDirectory (this,
                                                           tr ("Find Files"),
                                                           QDir::currentPath (),
                                                           options
    );
    if (!directory.isEmpty ()) {
        if (directoryComboBox->findText (directory) == -1) {
            directoryComboBox->addItem (directory);
        }
        directoryComboBox->setCurrentIndex (directoryComboBox->findText (directory));
    }
}

static void updateComboBox (QComboBox *comboBox)
{
    if (comboBox->findText (comboBox->currentText ()) == -1) {
        comboBox->addItem (comboBox->currentText ());
    }
}

void FindFilesDialog::find ()
{
    filesTable->setRowCount (0);
    tabber->setCurrentIndex (1);

    QString fileName = fileComboBox->currentText ();
    QString text = textComboBox->currentText ();
    QString path = directoryComboBox->currentText ();

    updateComboBox (fileComboBox);
    updateComboBox (textComboBox);
    updateComboBox (directoryComboBox);

    //TODO: filename validation
    //TODO: regexp support
    if (fileName.isEmpty ()) {
        fileName = QLatin1String ("*");
    }

    //TODO: fetch cs from QFSFileEngine
    static QDir::Filter caseSensitiveFilter = QDir::CaseSensitive;
    static QDir::SortFlag ignoreCaseSorting = QDir::IgnoreCase;

    QDir dir (path);
    dir.setNameFilters (QDir::nameFiltersFromString (fileName));
    dir.setFilter (QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot | caseSensitiveFilter);
    dir.setSorting (QDir::Name | QDir::DirsLast | ignoreCaseSorting);

    QFileInfoList files = enumerateDir (dir, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    if (!text.isEmpty ()) {
        files = findFiles (files, text);
    }
    showFiles (files);
}

QFileInfoList FindFilesDialog::findFiles (const QFileInfoList &files, const QString &text)
{
    QProgressDialog progressDialog (this);
    progressDialog.setCancelButtonText (tr ("&Cancel"));
    progressDialog.setWindowTitle (tr ("Find Files"));
    progressDialog.setRange (0, files.size ());

/*	// Create a QFutureWatcher and conncect signals and slots.
	QFutureWatcher<QFileInfo> futureWatcher;
	connect(&futureWatcher, SIGNAL(started()), &progressDialog, SLOT(exec()));
	connect(&futureWatcher, SIGNAL(finished()), &progressDialog, SLOT(reset()));
	connect(&futureWatcher, SIGNAL(progressRangeChanged(int, int)), &progressDialog, SLOT(setRange(int, int)));
	connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &progressDialog, SLOT(setValue(int)));
	connect(&progressDialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));

	futureWatcher.setFuture(QtConcurrent::filtered(files, findText));
	futureWatcher.waitForFinished();*/

    QFileInfoList foundFiles;
    for (int i = 0, n = files.size (); i < n; ++i) {
        progressDialog.setValue (i);
        progressDialog.setLabelText (tr ("Searching file number %1 of %2...").arg (i).arg (n));
        QCoreApplication::processEvents ();
        if (progressDialog.wasCanceled ()) {
            break;
        }

        if (findText (files[i], text)) {
            foundFiles.append (files[i]);
        }
    }
    return foundFiles;
}

void FindFilesDialog::showFiles (const QFileInfoList &files)
{
    for (int i = 0, n = files.size (); i < n; ++i) {
        QString name = files[i].fileName ();
        QString path = files[i].absolutePath ();
        qint64 size = files[i].size ();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem (name);
        fileNameItem->setFlags (fileNameItem->flags () ^ Qt::ItemIsEditable);
        QTableWidgetItem *filePathItem = new QTableWidgetItem (path);
        filePathItem->setFlags (filePathItem->flags () ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem (tr ("%1 KB").arg (int ((size + 1023) / 1024)));
        sizeItem->setFlags (sizeItem->flags () ^ Qt::ItemIsEditable);
        sizeItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);

        int row = filesTable->rowCount ();
        filesTable->insertRow (row);
        filesTable->setItem (row, 0, fileNameItem);
        filesTable->setItem (row, 1, filePathItem);
        filesTable->setItem (row, 2, sizeItem);
    }
    filesFoundLabel->setText (tr ("%1 file(s) found").arg (files.size ()) +
                              (" (Double click on a file to open it)"));
}

void FindFilesDialog::_slot_cellActivated (int row, int /* column */)
{
    QTableWidgetItem *fileNameItem = filesTable->item (row, 0);
    QTableWidgetItem *filePathItem = filesTable->item (row, 1);

    QFileInfo fi (QDir (filePathItem->text ()), fileNameItem->text ());

    QDesktopServices::openUrl (QUrl (fi.absoluteFilePath ()));
}

#include "moc_findfilesdialog.cpp"
