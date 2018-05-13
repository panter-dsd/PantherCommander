#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtCore/QStringListModel>

#include <QtGui/QStandardItemModel>
#include <QtGui/QDropEvent>

#include <QtWidgets/QAction>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QListView>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QToolButton>

#include "dialogs/FindFilesDialog.h"
#include "PantherViewer/PantherViewer.h"
#include "widgets/DriveBar.h"
#include "AppSettings.h"
#include "FilePanel.h"
#include "FileOperationsDialog.h"
#include "src/Preferences/PreferencesDialog.h"
#include "src/dialogs/CopyMoveDialog.h"
#include "ToolBar.h"
#include "Commands.h"

#include "MainWindow.h"

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent)
    , consolePath_ (0)
{
    resize (640, 480);
    setAcceptDrops (true);

    createWidgets ();
    createActions ();
    createMenus ();
    createCommandButtons ();

    loadSettings ();
    leftPanel_->loadSettings ();
    rightPanel_->loadSettings ();

    leftPanel_->setFocus ();
    focusedFilePanel_ = rightPanel_;
}

MainWindow::~MainWindow ()
{
    leftPanel_->saveSettings ();
    rightPanel_->saveSettings ();
    saveSettings ();
}

void MainWindow::createWidgets ()
{
    QWidget *widget = new QWidget (this);

    leftDriveBar_ = new DriveBar (this);
    connect (leftDriveBar_, &DriveBar::discChanged, this, &MainWindow::slotSetDisc);

    rightDriveBar_ = new DriveBar (this);
    connect (rightDriveBar_, &DriveBar::discChanged, this, &MainWindow::slotSetDisc);

    leftPanel_ = new FilePanel (this);
    leftPanel_->setObjectName ("LeftPanel");
    connect (leftPanel_, &FilePanel::pathChanged, leftDriveBar_, &DriveBar::slotSetDisc);
    connect (leftPanel_, &FilePanel::pathChanged, this, &MainWindow::slotPathChanged);
    leftDriveBar_->slotSetDisc (leftPanel_->path ());

    rightPanel_ = new FilePanel (this);
    rightPanel_->setObjectName ("RightPanel");
    connect (rightPanel_, &FilePanel::pathChanged, rightDriveBar_, &DriveBar::slotSetDisc);
    connect (rightPanel_, &FilePanel::pathChanged, this, &MainWindow::slotPathChanged);
    rightDriveBar_->slotSetDisc (rightPanel_->path ());

    panelsSplitter_ = new QSplitter (widget);
    panelsSplitter_->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Expanding);
    panelsSplitter_->setContextMenuPolicy (Qt::CustomContextMenu);
    panelsSplitter_->addWidget (leftPanel_);
    panelsSplitter_->addWidget (rightPanel_);

    connect (panelsSplitter_, &QSplitter::customContextMenuRequested, this, &MainWindow::showSplitterContextMenu);

    consolePath_ = new QLabel (this);
    consolePath_->setAlignment (Qt::AlignRight);

    consoleCommandComboBox_ = new QComboBox (this);
    consoleCommandComboBox_->setEditable (true);
    consoleCommandComboBox_->setInsertPolicy (QComboBox::InsertAtTop);
    consoleCommandComboBox_->setFocusPolicy (Qt::ClickFocus);
    consoleCommandComboBox_->setDuplicatesEnabled (false);

    connect (consoleCommandComboBox_->lineEdit (), &QLineEdit::returnPressed, this, &MainWindow::slotRunCommand);

    commandButtonsFrame_ = new QFrame (this);
    commandButtonsFrame_->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Maximum);

    qeueuModel_ = new QStandardItemModel (this);

    QHBoxLayout *qhblDriveBarLayout = new QHBoxLayout;
    qhblDriveBarLayout->addWidget (leftDriveBar_);
    qhblDriveBarLayout->addWidget (rightDriveBar_);

    QHBoxLayout *qhblConsoleCommandLayout = new QHBoxLayout;
    qhblConsoleCommandLayout->addWidget (consolePath_);
    qhblConsoleCommandLayout->addWidget (consoleCommandComboBox_);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins (-1, -1, -1, 0);
    layout->addLayout (qhblDriveBarLayout);
    layout->addWidget (panelsSplitter_);
    layout->addLayout (qhblConsoleCommandLayout);
    layout->addWidget (commandButtonsFrame_);
    widget->setLayout (layout);

    setCentralWidget (widget);
}

void MainWindow::createActions ()
{
    actionCpCurFileName2Cmd_ = new QAction (this);
    actionCpCurFileName2Cmd_->setObjectName ("actionCpCurFileName2Cmd_");
    actionCpCurFileName2Cmd_->setText (tr ("Copy current file name to command string"));
    actionCpCurFileName2Cmd_->setShortcut (QKeySequence (Qt::CTRL + Qt::Key_Return));
    connect (actionCpCurFileName2Cmd_, &QAction::triggered, this, &MainWindow::slotCpCurFileName2Cmd);
    addAction (actionCpCurFileName2Cmd_);
    Commands::instance ()->addAction (tr ("Command line"), actionCpCurFileName2Cmd_);

    actionCpCurFileNameWhithPath2Cmd_ = new QAction (this);
    actionCpCurFileNameWhithPath2Cmd_->setObjectName ("actionCpCurFileNameWhithPath2Cmd_");
    actionCpCurFileNameWhithPath2Cmd_->setText (tr ("Copy current file name whith path to command string"));
    actionCpCurFileNameWhithPath2Cmd_->setShortcut (QKeySequence (Qt::CTRL + Qt::SHIFT + Qt::Key_Return));
    connect (actionCpCurFileNameWhithPath2Cmd_, &QAction::triggered,
             this, &MainWindow::slotCpCurFileNameWhithPath2Cmd
    );
    addAction (actionCpCurFileNameWhithPath2Cmd_);
    Commands::instance ()->addAction (tr ("Command line"), actionCpCurFileNameWhithPath2Cmd_);

    actionClearCmd_ = new QAction (this);
    actionClearCmd_->setObjectName ("actionClearCmd_");
    actionClearCmd_->setText (tr ("Clear command string"));
    actionClearCmd_->setShortcut (QKeySequence (Qt::Key_Escape));
    connect (actionClearCmd_, &QAction::triggered, consoleCommandComboBox_->lineEdit (), &QLineEdit::clear);
    addAction (actionClearCmd_);
    Commands::instance ()->addAction (tr ("Command line"), actionClearCmd_);

    actionRunConsole_ = new QAction (this);
    actionRunConsole_->setObjectName ("actionRunConsole_");
    actionRunConsole_->setText (tr ("Run console"));
    actionRunConsole_->setShortcut (QKeySequence (Qt::Key_F2));
    connect (actionRunConsole_, &QAction::triggered, this, &MainWindow::slotRunConsole);
    addAction (actionRunConsole_);
    Commands::instance ()->addAction (tr ("Misc"), actionRunConsole_);

    actionView_ = new QAction (this);
    actionView_->setObjectName ("actionView_");
    actionView_->setText (tr ("View"));
    actionView_->setShortcut (QKeySequence (Qt::Key_F3));
    connect (actionView_, &QAction::triggered, [this] () { slotView (); });
    addAction (actionView_);
    Commands::instance ()->addAction (tr ("File operations"), actionView_);

    actionEdit_ = new QAction (this);
    actionEdit_->setObjectName ("actionEdit_");
    actionEdit_->setText (tr ("Edit"));
    actionEdit_->setShortcut (QKeySequence (Qt::Key_F4));
    connect (actionEdit_, &QAction::triggered, [this] () { slotView (); });
    addAction (actionEdit_);
    Commands::instance ()->addAction (tr ("File operations"), actionEdit_);

    actionCopy_ = new QAction (this);
    actionCopy_->setObjectName ("actionCopy_");
    actionCopy_->setText (tr ("Copy"));
    actionCopy_->setShortcut (QKeySequence (Qt::Key_F5));
    connect (actionCopy_, &QAction::triggered, [this] () { slotCopy (); });
    addAction (actionCopy_);
    Commands::instance ()->addAction (tr ("File operations"), actionCopy_);

    actionMove_ = new QAction (this);
    actionMove_->setObjectName ("actionMove_");
    actionMove_->setText (tr ("Move"));
    actionMove_->setShortcut (QKeySequence (Qt::Key_F6));
    connect (actionMove_, &QAction::triggered, [this] () { slotMove (); });
    addAction (actionMove_);
    Commands::instance ()->addAction (tr ("File operations"), actionMove_);

    actionRename_ = new QAction (this);
    actionRename_->setObjectName ("actionRename_");
    actionRename_->setText (tr ("Rename"));
    actionRename_->setShortcut (QKeySequence (Qt::SHIFT + Qt::Key_F6));
    connect (actionRename_, &QAction::triggered, this, &MainWindow::slotRename);
    addAction (actionRename_);
    Commands::instance ()->addAction (tr ("File operations"), actionRename_);

    actionMkDir_ = new QAction (this);
    actionMkDir_->setObjectName ("actionMkDir_");
    actionMkDir_->setText (tr ("Create Dir"));
    actionMkDir_->setShortcut (QKeySequence (Qt::Key_F7));
    connect (actionMkDir_, &QAction::triggered, this, &MainWindow::slotMkDir);
    addAction (actionMkDir_);
    Commands::instance ()->addAction (tr ("File operations"), actionMkDir_);

    actionRemove_ = new QAction (this);
    actionRemove_->setObjectName ("actionRemove_");
    actionRemove_->setText (tr ("Remove"));
    actionRemove_->setShortcut (QKeySequence (Qt::Key_F8));
    connect (actionRemove_, &QAction::triggered, [this] () { slotRemove (); });
    addAction (actionRemove_);
    Commands::instance ()->addAction (tr ("File operations"), actionRemove_);

    actionExit_ = new QAction (this);
    actionExit_->setObjectName ("actionExit_");
    actionExit_->setText (tr ("Exit"));
    actionExit_->setShortcut (QKeySequence (Qt::ALT + Qt::Key_X));
    connect (actionExit_, &QAction::triggered, qApp, &QApplication::quit);
    addAction (actionExit_);
    Commands::instance ()->addAction (tr ("Window"), actionExit_);

    actionFindFiles_ = new QAction (this);
    actionFindFiles_->setObjectName ("actionFindFiles_");
    actionFindFiles_->setText (tr ("Find Files"));
//	actionFindFiles_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    connect (actionFindFiles_, &QAction::triggered, this, &MainWindow::slotFindFiles);
    addAction (actionFindFiles_);
    Commands::instance ()->addAction (tr ("Tools"), actionFindFiles_);

    actionPreferences_ = new QAction (this);
    actionPreferences_->setObjectName ("actionPreferences_");
    actionPreferences_->setText (tr ("Preferences"));
//	actionPreferences_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
    connect (actionPreferences_, &QAction::triggered, this, &MainWindow::slotPreferences);
    addAction (actionPreferences_);
    Commands::instance ()->addAction (tr ("Preferences"), actionPreferences_);
}

void MainWindow::createMenus ()
{
    QMenu *fileMenu = menuBar ()->addMenu (tr ("File"));
    fileMenu->addAction (actionRename_);
    fileMenu->addAction (actionExit_);

    QMenu *actionsMenu = menuBar ()->addMenu (tr ("Actions"));
    actionsMenu->addAction (actionFindFiles_);

    QMenu *configurationMenu = menuBar ()->addMenu (tr ("Configuration"));
    configurationMenu->addAction (actionPreferences_);
}

void MainWindow::createCommandButtons ()
{
    auto connectButtonToAction = [] (QPushButton *b, QAction *a) {
        connect (b, &QPushButton::clicked, a, &QAction::triggered);
    };

    QFrame *splitter = nullptr;
    QHBoxLayout *qhblLayout = new QHBoxLayout ();
    qhblLayout->setSpacing (0);
    qhblLayout->setMargin (0);

    runConsoleButton_ = new QPushButton (actionRunConsole_->shortcut ().toString () + " " + actionRunConsole_->text (),
                                         commandButtonsFrame_
    );
    runConsoleButton_->setFlat (true);
    runConsoleButton_->setFocusPolicy (Qt::NoFocus);
    runConsoleButton_->addAction (actionRunConsole_);
    connectButtonToAction (runConsoleButton_, actionRunConsole_);
    qhblLayout->addWidget (runConsoleButton_);
    splitter = new QFrame (commandButtonsFrame_);
    splitter->setFrameShape (QFrame::VLine);
    splitter->setFrameShadow (QFrame::Sunken);
    qhblLayout->addWidget (splitter);

    viewButton_ = new QPushButton (actionView_->shortcut ().toString () + " " + actionView_->text (),
                                   commandButtonsFrame_
    );
    viewButton_->setFlat (true);
    viewButton_->setFocusPolicy (Qt::NoFocus);
    viewButton_->addAction (actionView_);
    connectButtonToAction (viewButton_, actionView_);
    qhblLayout->addWidget (viewButton_);
    splitter = new QFrame (commandButtonsFrame_);
    splitter->setFrameShape (QFrame::VLine);
    splitter->setFrameShadow (QFrame::Sunken);
    qhblLayout->addWidget (splitter);

    editButton_ = new QPushButton (actionEdit_->shortcut ().toString () + " " + actionEdit_->text (),
                                   commandButtonsFrame_
    );
    editButton_->setFlat (true);
    editButton_->setFocusPolicy (Qt::NoFocus);
    editButton_->addAction (actionEdit_);
    connectButtonToAction (editButton_, actionEdit_);
    qhblLayout->addWidget (editButton_);
    splitter = new QFrame (commandButtonsFrame_);
    splitter->setFrameShape (QFrame::VLine);
    splitter->setFrameShadow (QFrame::Sunken);
    qhblLayout->addWidget (splitter);

    copyButton_ = new QPushButton (actionCopy_->shortcut ().toString () + " " + actionCopy_->text (),
                                   commandButtonsFrame_
    );
    copyButton_->setFlat (true);
    copyButton_->setFocusPolicy (Qt::NoFocus);
    copyButton_->addAction (actionCopy_);
    connectButtonToAction (copyButton_, actionCopy_);
    qhblLayout->addWidget (copyButton_);
    splitter = new QFrame (commandButtonsFrame_);
    splitter->setFrameShape (QFrame::VLine);
    splitter->setFrameShadow (QFrame::Sunken);
    qhblLayout->addWidget (splitter);

    moveButton_ = new QPushButton (actionMove_->shortcut ().toString () + " " + actionMove_->text (),
                                   commandButtonsFrame_
    );
    moveButton_->setFlat (true);
    moveButton_->setFocusPolicy (Qt::NoFocus);
    moveButton_->addAction (actionMove_);
    connectButtonToAction (moveButton_, actionMove_);
    qhblLayout->addWidget (moveButton_);
    splitter = new QFrame (commandButtonsFrame_);
    splitter->setFrameShape (QFrame::VLine);
    splitter->setFrameShadow (QFrame::Sunken);
    qhblLayout->addWidget (splitter);

    mkDirButton_ = new QPushButton (actionMkDir_->shortcut ().toString () + " " + actionMkDir_->text (),
                                    commandButtonsFrame_
    );
    mkDirButton_->setFlat (true);
    mkDirButton_->setFocusPolicy (Qt::NoFocus);
    mkDirButton_->addAction (actionMkDir_);
    connectButtonToAction (mkDirButton_, actionMkDir_);
    qhblLayout->addWidget (mkDirButton_);
    splitter = new QFrame (commandButtonsFrame_);
    splitter->setFrameShape (QFrame::VLine);
    splitter->setFrameShadow (QFrame::Sunken);
    qhblLayout->addWidget (splitter);

    removeButton_ = new QPushButton (actionRemove_->shortcut ().toString () + " " + actionRemove_->text (),
                                     commandButtonsFrame_
    );
    removeButton_->setFlat (true);
    removeButton_->setFocusPolicy (Qt::NoFocus);
    removeButton_->addAction (actionRemove_);
    connectButtonToAction (removeButton_, actionRemove_);
    qhblLayout->addWidget (removeButton_);
    splitter = new QFrame (commandButtonsFrame_);
    splitter->setFrameShape (QFrame::VLine);
    splitter->setFrameShadow (QFrame::Sunken);
    qhblLayout->addWidget (splitter);

    exitButton_ = new QPushButton (actionExit_->shortcut ().toString () + " " + actionExit_->text (),
                                   commandButtonsFrame_
    );
    exitButton_->setFlat (true);
    exitButton_->setFocusPolicy (Qt::NoFocus);
    exitButton_->addAction (actionExit_);
    connectButtonToAction (exitButton_, actionExit_);
    qhblLayout->addWidget (exitButton_);

    commandButtonsFrame_->setLayout (qhblLayout);
}

void MainWindow::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("MainWindow");
    settings->setValue ("IsMaximized", isMaximized ());
    if (!isMaximized ()) {
        settings->setValue ("pos", pos ());
        settings->setValue ("size", size ());
    }
    settings->setValue ("Splitter", panelsSplitter_->saveState ());
    settings->setValue ("State", this->saveState ());
    settings->endGroup ();

    settings->beginGroup ("Global");
    settings->setValue ("CommandHistory", commandHistory ());
    settings->endGroup ();

    QStringList qslToolBars;
    for (ToolBar *toolBar : toolBars_) {
        qslToolBars << toolBar->name ();
        toolBar->save ();
    }

    settings->beginGroup ("Global");
    settings->setValue ("ToolBars", qslToolBars);
    settings->endGroup ();

    settings->sync ();
}

void MainWindow::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();

    settings->beginGroup ("Global");
    setCommandHistory (settings->value ("CommandHistory").toStringList ());
    QStringList toolBarNames = settings->value ("ToolBars").toStringList ();
    settings->endGroup ();

    if (toolBarNames.isEmpty ()) {
        ToolBar *toolBar = new ToolBar (tr ("Main toolbar"), this);
        connectToolBar (toolBar);
        this->addToolBar (toolBar);
        toolBars_ << toolBar;
    }

    for (const QString &toolBarName : toolBarNames) {
        ToolBar *toolBar = new ToolBar (toolBarName, this);
        connectToolBar (toolBar);
        this->addToolBar (toolBar);
        toolBars_ << toolBar;
    }

    leftDriveBar_->setVisible (settings->value ("Interface/ShowDriveBar", true).toBool ());
    rightDriveBar_->setVisible (settings->value ("Interface/ShowTwoDriveBar", true).toBool ()
                                && settings->value ("Interface/ShowDriveBar", true).toBool ());

    consolePath_->setVisible (settings->value ("Interface/ShowCommandLine", true).toBool ());
    consoleCommandComboBox_->setVisible (consolePath_->isVisible ());
    commandButtonsFrame_->setVisible (settings->value ("Interface/ShowFunctionButtons", true).toBool ());

    settings->beginGroup ("MainWindow");
    this->restoreState (settings->value ("State", QByteArray ()).toByteArray ());
    move (settings->value ("pos", QPoint (0, 0)).toPoint ());
    resize (settings->value ("size", QSize (640, 480)).toSize ());
    if (settings->value ("IsMaximized", false).toBool ()) {
        showMaximized ();
    }
    panelsSplitter_->restoreState (settings->value ("Splitter").toByteArray ());
    settings->endGroup ();
}

void MainWindow::showSplitterContextMenu (const QPoint &pos)
{
    QMenu menu (panelsSplitter_);
    for (int size = 20; size <= 80; size += 10) {
        int size2 = 100 - size;
        QString text = QString ("&%1/%2").arg (size).arg (size2);

        QAction *action = new QAction (panelsSplitter_);
        action->setText (text);
        action->setData ((double) size / 100);
        connect (action, &QAction::triggered, this, &MainWindow::slotResizeSplitter);
        menu.addAction (action);
    }
    menu.exec (panelsSplitter_->mapToGlobal (pos));
}

void MainWindow::slotResizeSplitter ()
{
    const QAction *action = qobject_cast<const QAction *> (sender ());
    if (action) {
        const QList<int> sizes {
            int (width () * action->data ().toDouble ()),
            int (width () * (1 - action->data ().toDouble ()))
        };
        panelsSplitter_->setSizes (sizes);
    }
}

void MainWindow::slotChangedFocus ()
{
    focusedFilePanel_ = qobject_cast<FilePanel *> (sender ());
    if (focusedFilePanel_) {
        slotPathChanged (focusedFilePanel_->path ());
    }
}

void MainWindow::slotPathChanged (const QString &path)
{
    consolePath_->setText (QDir::toNativeSeparators (path));
    consolePath_->setToolTip (QDir::toNativeSeparators (path));
}

void MainWindow::resizeEvent (QResizeEvent *event)
{
    QMainWindow::resizeEvent (event);

    if (consolePath_) {
        consolePath_->setMaximumWidth (int (width () * 0.3));
    }
}

void MainWindow::slotRunCommand ()
{
#ifndef Q_CC_MSVC
#warning "TODO: `_localpath_ _params_' command must not run consle - just execute program"
#endif
    QString qsCommand = consoleCommandComboBox_->currentText ();
    if (qsCommand.isEmpty ()) {
        return;
    }
    consoleCommandComboBox_->removeItem (consoleCommandComboBox_->currentIndex ());
    consoleCommandComboBox_->insertItem (0, qsCommand);
    consoleCommandComboBox_->setCurrentIndex (-1);
    QProcess *myProcess = new QProcess ();
    myProcess->setWorkingDirectory (consolePath_->text ());
#ifdef Q_OS_LINUX
    myProcess->start (qsCommand);
#endif
#ifdef Q_WS_WIN
    if (qsCommand.contains(QRegExp("^cd ")))
    {
        QDir dir(consolePath_->text());
        dir.cd(qsCommand.remove(QRegExp("^cd ")));
        focusedFilePanel_->setPath(dir.absolutePath());
        return;
    }
    myProcess->start(qsCommand);

//		QProcess::startDetached("cmd.exe /C "+qsCommand);
//	if (!rez)
//		myProcess->start("cmd.exe /C "+qsCommand);
#endif
}

void MainWindow::slotCpCurFileName2Cmd ()
{
    if (focusedFilePanel_->currentFileName ().contains (" ")) {
        consoleCommandComboBox_->setEditText (consoleCommandComboBox_->currentText () +
                                              "\"" +
                                              focusedFilePanel_->currentFileName () +
                                              "\" "
        );
    } else {
        consoleCommandComboBox_->setEditText (consoleCommandComboBox_->currentText () +
                                              focusedFilePanel_->currentFileName () +
                                              " "
        );
    }
    consoleCommandComboBox_->setFocus ();
}

void MainWindow::slotCpCurFileNameWhithPath2Cmd ()
{
    QString qsName = focusedFilePanel_->path ();
    if (qsName.at (qsName.length () - 1) != QDir::separator ()) {
        qsName += QDir::separator ();
    }
    qsName += focusedFilePanel_->currentFileName ();
    if (qsName.contains (" ")) {
        consoleCommandComboBox_->setEditText (consoleCommandComboBox_->currentText () +
                                              "\"" +
                                              qsName +
                                              "\" "
        );
    } else {
        consoleCommandComboBox_->setEditText (consoleCommandComboBox_->currentText () +
                                              qsName +
                                              " "
        );
    }
    consoleCommandComboBox_->setFocus ();
}

void MainWindow::slotRunConsole ()
{
#ifdef Q_OS_LINUX
    //	myProcess->startDetached("/bin/sh");
#endif
#ifdef Q_WS_WIN
    QProcess::startDetached("cmd.exe",QStringList(),consolePath_->text());
#endif
}

void MainWindow::slotView (const QString &fileName)
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    FilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;

    if (!viewer_) {
        viewer_ = new PantherViewer ();
        viewer_->setAttribute (Qt::WA_DeleteOnClose);
    }
    QString qsName = fileName;
    if (qsName.isEmpty ()) {
        qsName = sourcePanel->currentFileName ();
    }
    if (!QFileInfo (qsName).isFile ()) {
        QMessageBox::information (this, tr ("Error viewing"), tr ("Not selected files"));
        return;
    }
    viewer_->viewFile (qsName);
    viewer_->show ();
    viewer_->activateWindow ();
    viewer_->setFocus ();
}

void MainWindow::slotRename ()
{
    bool ok;
    QString newName = QInputDialog::getText (this,
                                             tr ("Rename"),
                                             tr ("New name"),
                                             QLineEdit::Normal,
                                             focusedFilePanel_->currentFileName (),
                                             &ok
    );
    if (ok && !newName.isEmpty ()) {
        QFile::rename (focusedFilePanel_->path () + focusedFilePanel_->currentFileName (),
                       focusedFilePanel_->path () + newName
        );
    }
}

void MainWindow::slotCopy (const QString &destDir, const QStringList &fileList)
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    FilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;
    FilePanel *destPanel = rightPanel_->hasFocus () ? leftPanel_ : rightPanel_;

    QStringList qslFileNames = fileList.isEmpty ()
                               ? sourcePanel->selectedFiles ()
                               : fileList;
    QString destPath, sourcePath;
    sourcePath = destDir.isEmpty () ? sourcePanel->path () : "";
    destPath = destDir.isEmpty () ? destPanel->path () : destDir;

    CopyMoveDialog *copyDialog = new CopyMoveDialog (this);
    copyDialog->setSource (qslFileNames);
    copyDialog->setDest (destPath);
    copyDialog->setOperation (tr ("Copy"));
    copyDialog->setQueueModel (qeueuModel_);

    if (copyDialog->exec ()) {
        FileOperationsDialog *queue = 0;
        int queueIndex = copyDialog->queueIndex ();
        if (queueIndex >= 0 && queueList_.at (queueIndex)) {
            queue = queueList_.at (queueIndex);
        }
        for (int i = 0; i < qslFileNames.count (); i++) {
            if (QFileInfo (qslFileNames.at (i)).isDir ()) {
                queue = addJob (queue,
                                FileOperationsThread::CopyDirOperation,
                                QStringList () << qslFileNames.at (i) + QDir::separator ()
                                               << destPath
                );
            } else {
                queue = addJob (queue,
                                FileOperationsThread::CopyFileOperation,
                                QStringList () << qslFileNames.at (i)
                                               << copyDialog->dest ());
            }
        }
        queue->setBlocked (false);
        sourcePanel->clearSelection ();
    }
    delete copyDialog;
}

void MainWindow::slotRemove (const QStringList &fileList)
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    FilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;

    QString path = fileList.isEmpty () ? sourcePanel->path () : "";

    QDialog *qdRemoveDialog = new QDialog (this);
    qdRemoveDialog->setWindowTitle (tr ("Remove"));

    QLabel *qlQuestion = new QLabel (tr ("Do you really want to delete this file(s)?"), qdRemoveDialog);

    QStringList qslFiles = fileList.isEmpty () ? sourcePanel->selectedFiles () : fileList;
    for (int i = 0; i < qslFiles.size (); i++) {
        qslFiles[i] = QDir::toNativeSeparators (qslFiles[i]);
    }
    QStringListModel *qslmStrings = new QStringListModel (qslFiles, qdRemoveDialog);

    QListView *qlvStrings = new QListView (qdRemoveDialog);
    qlvStrings->setModel (qslmStrings);

    QLabel *qlQueue = new QLabel (tr ("Queue"), qdRemoveDialog);

    QComboBox *qcbQueue = new QComboBox (qdRemoveDialog);
    qcbQueue->setModel (qeueuModel_);
    qcbQueue->setModelColumn (0);
    qcbQueue->setCurrentIndex (-1);

    QDialogButtonBox *qdbbButtons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                          Qt::Horizontal,
                                                          qdRemoveDialog
    );
    connect (qdbbButtons, &QDialogButtonBox::accepted, qdRemoveDialog, &QDialog::accept);
    connect (qdbbButtons, &QDialogButtonBox::rejected, qdRemoveDialog, &QDialog::reject);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (qlQuestion);
    qvblMainLayout->addWidget (qlvStrings);
    qvblMainLayout->addWidget (qlQueue);
    qvblMainLayout->addWidget (qcbQueue);
    qvblMainLayout->addWidget (qdbbButtons);

    qdRemoveDialog->setLayout (qvblMainLayout);

    if (qdRemoveDialog->exec ()) {
        FileOperationsDialog *queue = 0;
        int queueIndex = qcbQueue->currentIndex ();
        if (queueIndex >= 0 && queueList_.at (queueIndex)) {
            queue = queueList_.at (queueIndex);
        }
        for (int i = 0; i < qslFiles.count (); i++) {
            if (QFileInfo (qslFiles.at (i)).isDir ()) {
                queue = addJob (queue,
                                FileOperationsThread::RemoveDirOperation,
                                QStringList () << qslFiles.at (i));
            } else {
                queue = addJob (queue,
                                FileOperationsThread::RemoveFileOperation,
                                QStringList () << qslFiles.at (i));
            }
        }
        queue->setBlocked (false);
        sourcePanel->clearSelection ();
    }
    delete qdRemoveDialog;
}

FileOperationsDialog *MainWindow::addJob (FileOperationsDialog *queue,
                                          FileOperationsThread::FileOperation operation,
                                          const QStringList &parameters)
{
    if (!queue) {
        queue = new FileOperationsDialog (0);
        connect (queue, &FileOperationsDialog::finished, this, &MainWindow::slotQueueFinished);
        connect (queue, &FileOperationsDialog::jobChanged, this, &MainWindow::slotQueueChanged);
        queueList_ << queue;
        queue->show ();
    }
    queue->setBlocked (true);
    queue->addJob (operation, parameters);

    return queue;
}

void MainWindow::slotQueueFinished ()
{
    FileOperationsDialog *dialog = qobject_cast<FileOperationsDialog *> (sender ());
    if (dialog) {
        if (queueList_.removeOne (dialog)) {
            dialog->deleteLater ();
            slotQueueChanged ();
        }
    }
}

void MainWindow::slotQueueChanged ()
{
    qeueuModel_->clear ();
    QStandardItem *item;
    for (int i = 0; i < queueList_.count (); i++) {
        item = new QStandardItem (queueList_.at (i)->jobName ());
        qeueuModel_->appendRow (item);
    }
}

void MainWindow::slotMkDir ()
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    FilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;

    QDialog *qdMkDirDialog = new QDialog (this);
    qdMkDirDialog->setWindowTitle (tr ("Creating directory"));

    QComboBox *qcbDirName = new QComboBox (qdMkDirDialog);
    qcbDirName->setEditable (true);
//Load history
    QSettings *settings = AppSettings::instance ();
    qcbDirName->addItems (settings->value ("Global/MkDirHistory", QStringList ()).toStringList ());
    qcbDirName->setCurrentIndex (-1);
//
    QString fileName = sourcePanel->currentFileName ();
    if (QFileInfo (sourcePanel->path () + fileName).isFile ()) {
        qcbDirName->setEditText (QFileInfo (sourcePanel->path () + fileName).baseName ());
    } else {
        qcbDirName->setEditText (QFileInfo (fileName).fileName ());
    }
    qcbDirName->lineEdit ()->selectAll ();

    QDialogButtonBox *qdbbButtons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                          Qt::Horizontal,
                                                          qdMkDirDialog
    );
    connect (qdbbButtons, &QDialogButtonBox::accepted, qdMkDirDialog, &QDialog::accept);
    connect (qdbbButtons, &QDialogButtonBox::rejected, qdMkDirDialog, &QDialog::reject);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (qcbDirName);
    qvblMainLayout->addWidget (qdbbButtons);
    qdMkDirDialog->setLayout (qvblMainLayout);

    if (qdMkDirDialog->exec ()) {
        if (!QDir ().mkdir (sourcePanel->path () + "/" + qcbDirName->currentText ())) {
            QMessageBox::critical (this, qdMkDirDialog->windowTitle (), tr ("Error creating directory"));
        } else {
            //Move last index to top
            QString qsLastDirName = qcbDirName->currentText ();
            if (qsLastDirName.isEmpty ()) {
                return;
            }
            qcbDirName->removeItem (qcbDirName->findText (qsLastDirName));
            qcbDirName->insertItem (0, qsLastDirName);
            //
            //Save history
#ifndef Q_CC_MSVC
#warning "Maybe have property, which return QStringList???"
#endif
            QStringList qslMkDirHistory;
            for (int i = 0; i < qcbDirName->count (); i++) {
                qslMkDirHistory << qcbDirName->itemText (i);
            }
            settings->setValue ("Global/MkDirHistory", qslMkDirHistory);
            settings->sync ();
            //
        }
    }
    delete qdMkDirDialog;
}

QStringList MainWindow::commandHistory () const
{
    QStringList history;
    for (int i = 0, count = consoleCommandComboBox_->count (); i < count; ++i) {
        history.append (consoleCommandComboBox_->itemText (i));
    }
    return history;
}

void MainWindow::setCommandHistory (const QStringList &commandHistory)
{
    consoleCommandComboBox_->clear ();
    consoleCommandComboBox_->addItems (commandHistory);
    consoleCommandComboBox_->setCurrentIndex (-1);
}

void MainWindow::slotMove (const QString &destDir, const QStringList &fileList)
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    FilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;
    FilePanel *destPanel = rightPanel_->hasFocus () ? leftPanel_ : rightPanel_;

    QStringList qslFileNames = fileList.isEmpty ()
                               ? sourcePanel->selectedFiles ()
                               : fileList;
    QString destPath, sourcePath;
    sourcePath = destDir.isEmpty () ? sourcePanel->path () : "";
    destPath = destDir.isEmpty () ? destPanel->path () : destDir;

    CopyMoveDialog *moveDialog = new CopyMoveDialog (this);
    moveDialog->setSource (qslFileNames);
    moveDialog->setDest (destPath);
    moveDialog->setOperation (tr ("Move"));
    moveDialog->setQueueModel (qeueuModel_);

    if (moveDialog->exec ()) {
        FileOperationsDialog *queue = 0;
        int queueIndex = moveDialog->queueIndex ();
        if (queueIndex >= 0 && queueList_.at (queueIndex)) {
            queue = queueList_.at (queueIndex);
        }
        for (int i = 0; i < qslFileNames.count (); i++) {
            if (QFileInfo (qslFileNames.at (i)).isDir ()) {
                queue = addJob (queue,
                                FileOperationsThread::MoveDirOperation,
                                QStringList () << qslFileNames.at (i) + QDir::separator ()
                                               << moveDialog->dest () + QDir::separator ());
            } else {
                queue = addJob (queue,
                                FileOperationsThread::MoveFileOperation,
                                QStringList () << qslFileNames.at (i)
                                               << moveDialog->dest () + QDir::separator () +
                                                  QFileInfo (qslFileNames.at (i)).fileName ());
            }
        }
        queue->setBlocked (false);
        sourcePanel->clearSelection ();
    }
    delete moveDialog;
}

void MainWindow::dropEvent (QDropEvent *event)
{
    QWidget *widget = childAt (event->pos ());
    if (widget == runConsoleButton_) {
        qWarning () << event->mimeData ()->urls ();
#ifndef Q_CC_MSVC
#warning "dnd for F2 not implemented yet"
#endif
    } else if (widget == viewButton_) {
#ifndef Q_CC_MSVC
#warning "will not work for vfs since `QUrl::toLocalFile()'"
#endif
        slotView (event->mimeData ()->urls ().at (0).toLocalFile ());
    } else if (widget == editButton_) {
#ifndef Q_CC_MSVC
#warning "dnd for F4 not implemented yet"
#endif
    } else if (widget == removeButton_) {
#ifndef Q_CC_MSVC
#warning "will not work for vfs since `QUrl::toLocalFile()'"
#endif
        QStringList list;
            foreach(QUrl url, event->mimeData ()->urls ()) {
                list << QDir::toNativeSeparators (url.toLocalFile ());
            }
        slotRemove (list);
    }
}

void MainWindow::dragMoveEvent (QDragMoveEvent *event)
{
    bool isAccepted = false;
    QWidget *widget = childAt (event->pos ());
//Command Buttons
    isAccepted = widget == runConsoleButton_ || widget == viewButton_ || widget == editButton_ ||
                 widget == removeButton_;

        foreach(ToolBar *toolBar, toolBars_) {
            if (toolBar == widget || toolBar == widget->parentWidget ()) {
                isAccepted = true;
                break;
            }
        }

    event->setAccepted (isAccepted);
}

void MainWindow::dragEnterEvent (QDragEnterEvent *event)
{
    if (event->mimeData ()->hasFormat ("text/uri-list")) {
        event->acceptProposedAction ();
    }
}

void MainWindow::toolBarActionExecute (const ToolBarButton &actionButton)
{
#ifndef Q_CC_MSVC
#warning "TODO: parse params"
#endif
    QFileInfo fi (actionButton.qsCommand);
    if (!fi.exists ()) {
        QAction *action = Commands::instance ()->action (actionButton.qsCommand);
        if (action) {
            action->trigger ();
        }
    }
    if (fi.isDir ()) {
        cdExecute (fi.absoluteFilePath ());
    } else {
        FileOperationsThread::execute (actionButton.qsCommand,
                                       actionButton.qsParams.split (QLatin1Char (' ')),
                                       actionButton.qsWorkDir
        );
    }
}

void MainWindow::slotFindFiles ()
{
    FindFilesDialog dialog (this);
    dialog.exec ();
}

void MainWindow::slotPreferences ()
{
    PreferencesDialog dialog (this);
    dialog.exec ();
}

void MainWindow::slotSetDisc (const QString &path)
{
    if (sender () == leftDriveBar_) {
        if (rightDriveBar_->isVisible ()) {
            leftPanel_->setPath (path);
            leftPanel_->setFocus ();
        } else {
#ifndef Q_CC_MSVC
#warning "focusedFilePanel_ must be automatical"
#endif
            focusedFilePanel_ = leftPanel_->hasFocus () ? leftPanel_ : rightPanel_;
            focusedFilePanel_->setPath (path);
        }
    }
    if (sender () == rightDriveBar_) {
        rightPanel_->setPath (path);
        rightPanel_->setFocus ();
    }
}

void MainWindow::slotAddToolBar ()
{
    bool ok;
    QString qsToolBarName = QInputDialog::getText (this,
                                                   tr ("Set toolbar name"),
                                                   tr ("Name"),
                                                   QLineEdit::Normal,
                                                   QString (),
                                                   &ok
    );
    if (!ok) {
        return;
    }

    if (qsToolBarName.isEmpty ()) {
        QMessageBox::critical (this, "", tr ("Toolbar name is empty. Break."));
        return;
    }

        foreach (ToolBar *toolBar, toolBars_) {
            if (toolBar->name () == qsToolBarName) {
                QMessageBox::critical (this, "", tr ("This name is not unique."));
                slotAddToolBar ();
                return;
            }
        }

    ToolBar *toolBar = new ToolBar (qsToolBarName, this);
    connectToolBar (toolBar);
    this->addToolBar (toolBar);
    toolBars_ << toolBar;
}

void MainWindow::slotRemoveToolBar ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }
    ToolBar *toolBar = toolBars_.at (action->data ().toInt ());
    if (!toolBar) {
        return;
    }

    QString qsName = toolBar->name ();
    this->removeToolBar (toolBar);
    toolBars_.removeOne (toolBar);
    toolBar->deleteLater ();

    QSettings *settings = AppSettings::instance ();
    settings->remove ("ToolBar_" + qsName);
    settings->sync ();

    if (toolBars_.count () == 0) {
        ToolBar *toolBar = new ToolBar (tr ("Main toolbar"), this);
        toolBar->hide ();
        connectToolBar (toolBar);
        this->addToolBar (toolBar);
        toolBars_ << toolBar;
    }
}

void MainWindow::slotRenameToolBar ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }
    ToolBar *toolBar = toolBars_.at (action->data ().toInt ());
    if (!toolBar) {
        return;
    }

    QString qsName = toolBar->name ();
    bool ok;
    QString qsToolBarNewName = QInputDialog::getText (this,
                                                      tr ("Set toolbar name"),
                                                      tr ("Name"),
                                                      QLineEdit::Normal,
                                                      qsName,
                                                      &ok
    );
    if (!ok) {
        return;
    }

    if (qsToolBarNewName.isEmpty ()) {
        QMessageBox::critical (this, "", tr ("Toolbar name is empty. Break."));
        return;
    }

    if (qsToolBarNewName == qsName) {
        return;
    }

    for (ToolBar *toolBar : toolBars_) {
        if (toolBar->name () == qsToolBarNewName) {
            QMessageBox::critical (this, "", tr ("This name is not unique. Break."));
            return;
        }
    }

    toolBar->rename (qsToolBarNewName);

    QSettings *settings = AppSettings::instance ();
    settings->remove ("ToolBar_" + qsName);

    QStringList qslToolBars;
        foreach(ToolBar *toolBar, toolBars_) {
            qslToolBars << toolBar->name ();
        }

    settings->beginGroup ("Global");
    settings->setValue ("ToolBars", qslToolBars);
    settings->endGroup ();

    toolBar->save ();

    settings->sync ();
}

void MainWindow::connectToolBar (ToolBar *toolBar)
{
    connect (toolBar, &ToolBar::toolBarActionExecuted, this, &MainWindow::toolBarActionExecute);
    connect (toolBar, &ToolBar::cdExecuted, this, &MainWindow::cdExecute);
    connect (toolBar, &ToolBar::toolbarContextMenu, this, &MainWindow::slotToolBarContextMenu);
}

void MainWindow::cdExecute (const QString &path)
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    focusedFilePanel_ = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;

    QDir dir (path);
    focusedFilePanel_->setPath (dir.absolutePath ());
}

QMenu *MainWindow::createToolBarsMenu (ToolBar *currentToolBar)
{
    QMenu *qmToolBarMenu = new QMenu (tr ("Toolbars"), this);
    QAction *menuAction;

    menuAction = new QAction (tr ("&Add toolbar"), qmToolBarMenu);
    connect (menuAction, &QAction::triggered, this, &MainWindow::slotAddToolBar);
    qmToolBarMenu->addAction (menuAction);

    if (toolBars_.count () <= 0) {
        return qmToolBarMenu;
    }

    QMenu *removeMenu = new QMenu (tr ("&Remove toolbar"), this);
    int i = 0;
    for (ToolBar *toolBar : toolBars_) {
        menuAction = new QAction (toolBar->name (), removeMenu);
        menuAction->setData (i++);
        connect (menuAction, &QAction::triggered, this, &MainWindow::slotRemoveToolBar);
        removeMenu->addAction (menuAction);
        if (toolBar == currentToolBar) {
            removeMenu->setDefaultAction (menuAction);
        }
    }
    qmToolBarMenu->addMenu (removeMenu);

    QMenu *renameMenu = new QMenu (tr ("Re&name toolbar"), this);
    i = 0;
    for (ToolBar *toolBar : toolBars_) {
        menuAction = new QAction (toolBar->name (), renameMenu);
        menuAction->setData (i++);
        connect (menuAction, &QAction::triggered, this, &MainWindow::slotRenameToolBar);
        renameMenu->addAction (menuAction);
        if (toolBar == currentToolBar) {
            renameMenu->setDefaultAction (menuAction);
        }
    }
    qmToolBarMenu->addMenu (renameMenu);

    QMenu *showHideMenu = new QMenu (tr ("&Show/Hide toolbar"), this);
    for (ToolBar *toolBar : toolBars_) {
        menuAction = new QAction (toolBar->name (), showHideMenu);
        menuAction->setCheckable (true);
        menuAction->setChecked (toolBar->isVisible ());
        connect (menuAction, &QAction::triggered, toolBar, &ToolBar::setVisible);
        showHideMenu->addAction (menuAction);
        if (toolBar == currentToolBar) {
            showHideMenu->setDefaultAction (menuAction);
        }
    }
    qmToolBarMenu->addMenu (showHideMenu);

    if (currentToolBar) {
        qmToolBarMenu->addSeparator ();

        menuAction = new QAction (tr ("A&dd separator"), qmToolBarMenu);
        connect (menuAction, &QAction::triggered, currentToolBar, &ToolBar::slotAddSeparator);
        qmToolBarMenu->addAction (menuAction);
    }

    return qmToolBarMenu;
}

void MainWindow::slotToolBarContextMenu (const QPoint &pos)
{
    ToolBar *pcToolBar = qobject_cast<ToolBar *> (sender ());
    if (!pcToolBar) {
        return;
    }

    createToolBarsMenu (pcToolBar)->exec (pos);
}
