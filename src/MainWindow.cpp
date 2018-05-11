#include "MainWindow.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtWidgets/QAction>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtCore/QStringListModel>
#include <QtWidgets/QListView>
#include <QtWidgets/QDialogButtonBox>
#include <QtGui/QDropEvent>
#include <QtWidgets/QToolButton>

#include "AppSettings.h"
#include "dialogs/FindFilesDialog.h"
#include "PantherViewer/PantherViewer.h"
#include "widgets/DriveBar.h"
#include "QFilePanel.h"
#include "QFileOperationsDialog.h"
#include "src/Preferences/PreferencesDialog.h"
#include "src/dialogs/CopyMoveDialog.h"
#include "PCToolBar.h"
#include "PCCommands.h"

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent)
    , consolePath_ (0)
{
    resize (640, 480);
    setAcceptDrops (true);

//	QTime time;
//	time.start();
    createWidgets ();
//	qDebug(QString::number(time.elapsed()).toLocal8Bit());
    createActions ();
    createMenus ();
    createCommandButtons ();

    loadSettings ();
    leftPanel_->loadSettings ();
    rightPanel_->loadSettings ();

    leftPanel_->setFocus ();
    focusedFilePanel_ = rightPanel_;
}

//
MainWindow::~MainWindow ()
{
    leftPanel_->saveSettings ();
    rightPanel_->saveSettings ();
    saveSettings ();
}

//
void MainWindow::createWidgets ()
{
    QWidget *widget = new QWidget (this);

    leftDriveBar_ = new DriveBar (this);
    connect (leftDriveBar_, SIGNAL(discChanged (
                                       const QString&)),
             this, SLOT(slotSetDisc (
                            const QString&)));

    rightDriveBar_ = new DriveBar (this);
    connect (rightDriveBar_, SIGNAL(discChanged (
                                        const QString&)),
             this, SLOT(slotSetDisc (
                            const QString&)));

    leftPanel_ = new QFilePanel (this);
    leftPanel_->setObjectName ("LeftPanel");
    connect (leftPanel_, SIGNAL(pathChanged (
                                    const QString&)),
             leftDriveBar_, SLOT(slotSetDisc (
                                     const QString&)));
    connect (leftPanel_, SIGNAL(pathChanged (
                                    const QString&)),
             this, SLOT(slotPathChanged (
                            const QString&)));
    leftDriveBar_->slotSetDisc (leftPanel_->path ());

    rightPanel_ = new QFilePanel (this);
    rightPanel_->setObjectName ("RightPanel");
    connect (rightPanel_, SIGNAL(pathChanged (
                                     const QString&)),
             rightDriveBar_, SLOT(slotSetDisc (
                                      const QString&)));
    connect (rightPanel_, SIGNAL(pathChanged (
                                     const QString&)),
             this, SLOT(slotPathChanged (
                            const QString&)));
    rightDriveBar_->slotSetDisc (rightPanel_->path ());

    panelsSplitter_ = new QSplitter (widget);
    panelsSplitter_->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Expanding);
    panelsSplitter_->setContextMenuPolicy (Qt::CustomContextMenu);
    panelsSplitter_->addWidget (leftPanel_);
    panelsSplitter_->addWidget (rightPanel_);

    QObject::connect (panelsSplitter_, SIGNAL(customContextMenuRequested (
                                                  const QPoint&)),
                      this, SLOT(showSplitterContextMenu (
                                     const QPoint&)));

    consolePath_ = new QLabel (this);
    consolePath_->setAlignment (Qt::AlignRight);

    consoleCommandComboBox_ = new QComboBox (this);
    consoleCommandComboBox_->setEditable (true);
    consoleCommandComboBox_->setInsertPolicy (QComboBox::InsertAtTop);
    consoleCommandComboBox_->setFocusPolicy (Qt::ClickFocus);
    consoleCommandComboBox_->setDuplicatesEnabled (false);

    connect (consoleCommandComboBox_->lineEdit (), SIGNAL(returnPressed ()),
             this, SLOT(slotRunCommand ()));

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

//
void MainWindow::createActions ()
{
    actionCpCurFileName2Cmd_ = new QAction (this);
    actionCpCurFileName2Cmd_->setObjectName ("actionCpCurFileName2Cmd_");
    actionCpCurFileName2Cmd_->setText (tr ("Copy current file name to command string"));
    actionCpCurFileName2Cmd_->setShortcut (QKeySequence (Qt::CTRL + Qt::Key_Return));
    connect (actionCpCurFileName2Cmd_, SIGNAL(triggered (bool)),
             this, SLOT(slotCpCurFileName2Cmd ()));
    addAction (actionCpCurFileName2Cmd_);
    PCCommands::instance ()->addAction (tr ("Command line"), actionCpCurFileName2Cmd_);

    actionCpCurFileNameWhithPath2Cmd_ = new QAction (this);
    actionCpCurFileNameWhithPath2Cmd_->setObjectName ("actionCpCurFileNameWhithPath2Cmd_");
    actionCpCurFileNameWhithPath2Cmd_->setText (tr ("Copy current file name whith path to command string"));
    actionCpCurFileNameWhithPath2Cmd_->setShortcut (QKeySequence (Qt::CTRL + Qt::SHIFT + Qt::Key_Return));
    connect (actionCpCurFileNameWhithPath2Cmd_, SIGNAL(triggered (bool)),
             this, SLOT(slotCpCurFileNameWhithPath2Cmd ()));
    addAction (actionCpCurFileNameWhithPath2Cmd_);
    PCCommands::instance ()->addAction (tr ("Command line"), actionCpCurFileNameWhithPath2Cmd_);

    actionClearCmd_ = new QAction (this);
    actionClearCmd_->setObjectName ("actionClearCmd_");
    actionClearCmd_->setText (tr ("Clear command string"));
    actionClearCmd_->setShortcut (QKeySequence (Qt::Key_Escape));
    connect (actionClearCmd_, SIGNAL(triggered (bool)),
             consoleCommandComboBox_->lineEdit (), SLOT(clear ()));
    addAction (actionClearCmd_);
    PCCommands::instance ()->addAction (tr ("Command line"), actionClearCmd_);

    actionRunConsole_ = new QAction (this);
    actionRunConsole_->setObjectName ("actionRunConsole_");
    actionRunConsole_->setText (tr ("Run console"));
    actionRunConsole_->setShortcut (QKeySequence (Qt::Key_F2));
    connect (actionRunConsole_, SIGNAL(triggered (bool)),
             this, SLOT(slotRunConsole ()));
    addAction (actionRunConsole_);
    PCCommands::instance ()->addAction (tr ("Misc"), actionRunConsole_);

    actionView_ = new QAction (this);
    actionView_->setObjectName ("actionView_");
    actionView_->setText (tr ("View"));
    actionView_->setShortcut (QKeySequence (Qt::Key_F3));
    connect (actionView_, SIGNAL(triggered (bool)),
             this, SLOT(slotView ()));
    addAction (actionView_);
    PCCommands::instance ()->addAction (tr ("File operations"), actionView_);

    actionEdit_ = new QAction (this);
    actionEdit_->setObjectName ("actionEdit_");
    actionEdit_->setText (tr ("Edit"));
    actionEdit_->setShortcut (QKeySequence (Qt::Key_F4));
//	connect(actionEdit_, SIGNAL(triggered(bool)),
//			this, SLOT(slotView()));
    addAction (actionEdit_);
    PCCommands::instance ()->addAction (tr ("File operations"), actionEdit_);

    actionCopy_ = new QAction (this);
    actionCopy_->setObjectName ("actionCopy_");
    actionCopy_->setText (tr ("Copy"));
    actionCopy_->setShortcut (QKeySequence (Qt::Key_F5));
    connect (actionCopy_, SIGNAL(triggered (bool)),
             this, SLOT(slotCopy ()));
    addAction (actionCopy_);
    PCCommands::instance ()->addAction (tr ("File operations"), actionCopy_);

    actionMove_ = new QAction (this);
    actionMove_->setObjectName ("actionMove_");
    actionMove_->setText (tr ("Move"));
    actionMove_->setShortcut (QKeySequence (Qt::Key_F6));
    connect (actionMove_, SIGNAL(triggered (bool)),
             this, SLOT(slotMove ()));
    addAction (actionMove_);
    PCCommands::instance ()->addAction (tr ("File operations"), actionMove_);

    actionRename_ = new QAction (this);
    actionRename_->setObjectName ("actionRename_");
    actionRename_->setText (tr ("Rename"));
    actionRename_->setShortcut (QKeySequence (Qt::SHIFT + Qt::Key_F6));
    connect (actionRename_, SIGNAL(triggered (bool)),
             this, SLOT(slotRename ()));
    addAction (actionRename_);
    PCCommands::instance ()->addAction (tr ("File operations"), actionRename_);

    actionMkDir_ = new QAction (this);
    actionMkDir_->setObjectName ("actionMkDir_");
    actionMkDir_->setText (tr ("Create Dir"));
    actionMkDir_->setShortcut (QKeySequence (Qt::Key_F7));
    connect (actionMkDir_, SIGNAL(triggered (bool)),
             this, SLOT(slotMkDir ()));
    addAction (actionMkDir_);
    PCCommands::instance ()->addAction (tr ("File operations"), actionMkDir_);

    actionRemove_ = new QAction (this);
    actionRemove_->setObjectName ("actionRemove_");
    actionRemove_->setText (tr ("Remove"));
    actionRemove_->setShortcut (QKeySequence (Qt::Key_F8));
    connect (actionRemove_, SIGNAL(triggered (bool)),
             this, SLOT(slotRemove ()));
    addAction (actionRemove_);
    PCCommands::instance ()->addAction (tr ("File operations"), actionRemove_);

    actionExit_ = new QAction (this);
    actionExit_->setObjectName ("actionExit_");
    actionExit_->setText (tr ("Exit"));
    actionExit_->setShortcut (QKeySequence (Qt::ALT + Qt::Key_X));
    connect (actionExit_, SIGNAL(triggered (bool)),
             qApp, SLOT(quit ()));
    addAction (actionExit_);
    PCCommands::instance ()->addAction (tr ("Window"), actionExit_);

    actionFindFiles_ = new QAction (this);
    actionFindFiles_->setObjectName ("actionFindFiles_");
    actionFindFiles_->setText (tr ("Find Files"));
//	actionFindFiles_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    connect (actionFindFiles_, SIGNAL(triggered (bool)),
             this, SLOT(slotFindFiles ()));
    addAction (actionFindFiles_);
    PCCommands::instance ()->addAction (tr ("Tools"), actionFindFiles_);

    actionPreferences_ = new QAction (this);
    actionPreferences_->setObjectName ("actionPreferences_");
    actionPreferences_->setText (tr ("Preferences"));
//	actionPreferences_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
    connect (actionPreferences_, SIGNAL(triggered (bool)),
             this, SLOT(slotPreferences ()));
    addAction (actionPreferences_);
    PCCommands::instance ()->addAction (tr ("Preferences"), actionPreferences_);
}

//
void MainWindow::createMenus ()
{
    QMenu *qmFile = menuBar ()->addMenu (tr ("File"));
    qmFile->addAction (actionRename_);
    qmFile->addAction (actionExit_);

    QMenu *qmActions = menuBar ()->addMenu (tr ("Actions"));
    qmActions->addAction (actionFindFiles_);

    QMenu *qmConfiguration = menuBar ()->addMenu (tr ("Configuration"));
    qmConfiguration->addAction (actionPreferences_);

    QMenu *qmTesting = menuBar ()->addMenu ("testing");
    qmTesting->addAction ("filedialog", this, SLOT(slotTestingFileDialog ()));
#ifdef Q_WS_WIN
    QAction* action = qmTesting->addAction("ntfs_permission_lookup", this, SLOT(slotTestingEnableNTFSPermissionLookup(bool)));
    action->setCheckable(true);
#endif
}

//
void MainWindow::createCommandButtons ()
{
    QFrame *splitter;
    QHBoxLayout *qhblLayout = new QHBoxLayout ();
    qhblLayout->setSpacing (0);
    qhblLayout->setMargin (0);

    runConsoleButton_ = new QPushButton (actionRunConsole_->shortcut ().toString () + " " + actionRunConsole_->text (),
                                         commandButtonsFrame_
    );
    runConsoleButton_->setFlat (true);
    runConsoleButton_->setFocusPolicy (Qt::NoFocus);
    runConsoleButton_->addAction (actionRunConsole_);
    connect (runConsoleButton_, SIGNAL(clicked ()), actionRunConsole_, SIGNAL(triggered ()));
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
    connect (viewButton_, SIGNAL(clicked ()), actionView_, SIGNAL(triggered ()));
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
    connect (editButton_, SIGNAL(clicked ()), actionEdit_, SIGNAL(triggered ()));
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
    connect (copyButton_, SIGNAL(clicked ()), actionCopy_, SIGNAL(triggered ()));
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
    connect (moveButton_, SIGNAL(clicked ()), actionMove_, SIGNAL(triggered ()));
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
    connect (mkDirButton_, SIGNAL(clicked ()), actionMkDir_, SIGNAL(triggered ()));
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
    connect (removeButton_, SIGNAL(clicked ()), actionRemove_, SIGNAL(triggered ()));
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
    connect (exitButton_, SIGNAL(clicked ()), actionExit_, SIGNAL(triggered ()));
    qhblLayout->addWidget (exitButton_);

    commandButtonsFrame_->setLayout (qhblLayout);
}

//
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
        foreach(PCToolBar *toolBar, toolBars_) {
            qslToolBars << toolBar->name ();
            toolBar->save ();
        }

    settings->beginGroup ("Global");
    settings->setValue ("ToolBars", qslToolBars);
    settings->endGroup ();

    settings->sync ();
}

//
void MainWindow::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();

    settings->beginGroup ("Global");
    setCommandHistory (settings->value ("CommandHistory").toStringList ());
    QStringList toolBarNames = settings->value ("ToolBars").toStringList ();
    settings->endGroup ();

    if (toolBarNames.isEmpty ()) {
        PCToolBar *toolBar = new PCToolBar (tr ("Main toolbar"), this);
        connectToolBar (toolBar);
        this->addToolBar (toolBar);
        toolBars_ << toolBar;
    }

        foreach(const QString &toolBarName, toolBarNames) {
            PCToolBar *toolBar = new PCToolBar (toolBarName, this);
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

//
void MainWindow::showSplitterContextMenu (const QPoint &pos)
{
    QMenu *menu = new QMenu (panelsSplitter_);
    for (int size = 20; size <= 80; size += 10) {
        int size2 = 100 - size;
        QString text = QString ("&%1/%2").arg (size).arg (size2);

        QAction *action = new QAction (panelsSplitter_);
        action->setText (text);
        action->setData ((double) size / 100);
        connect (action, SIGNAL(triggered ()), this, SLOT(slotResizeSplitter ()));
        menu->addAction (action);
    }
    menu->exec (panelsSplitter_->mapToGlobal (pos));
    delete menu;
}

//
void MainWindow::slotResizeSplitter ()
{
    const QAction *action = qobject_cast<const QAction *> (sender ());
    if (action) {
        QList<int> sizes;
        sizes << int (width () * action->data ().toDouble ());
        sizes << int (width () * (1 - action->data ().toDouble ()));
        panelsSplitter_->setSizes (sizes);
    }
}
//
#ifndef Q_CC_MSVC
#warning "TODO: make eventFilter instead"
#endif

void MainWindow::slotChangedFocus ()
{
    focusedFilePanel_ = qobject_cast<QFilePanel *> (sender ());
    if (focusedFilePanel_) {
        slotPathChanged (focusedFilePanel_->path ());
    }
}

//
void MainWindow::slotPathChanged (const QString &path)
{
    consolePath_->setText (QDir::toNativeSeparators (path));
    consolePath_->setToolTip (QDir::toNativeSeparators (path));
}

//
void MainWindow::resizeEvent (QResizeEvent *event)
{
    QMainWindow::resizeEvent (event);

    if (consolePath_) {
        consolePath_->setMaximumWidth (int (width () * 0.3));
    }
}

//
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
#ifdef Q_WS_X11
    myProcess->start(qsCommand);
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

//
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

//
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

//
void MainWindow::slotRunConsole ()
{
#ifdef Q_WS_X11
    //	myProcess->startDetached("/bin/sh");
#endif
#ifdef Q_WS_WIN
    QProcess::startDetached("cmd.exe",QStringList(),consolePath_->text());
#endif
}

//
void MainWindow::slotView (const QString &fileName)
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    QFilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;

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

//
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

//
void MainWindow::slotCopy (const QString &destDir, const QStringList &fileList)
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    QFilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;
    QFilePanel *destPanel = rightPanel_->hasFocus () ? leftPanel_ : rightPanel_;

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
        QFileOperationsDialog *queue = 0;
        int queueIndex = copyDialog->queueIndex ();
        if (queueIndex >= 0 && queueList_.at (queueIndex)) {
            queue = queueList_.at (queueIndex);
        }
        for (int i = 0; i < qslFileNames.count (); i++) {
            if (QFileInfo (qslFileNames.at (i)).isDir ()) {
                queue = addJob (queue,
                                QFileOperationsThread::CopyDirOperation,
                                QStringList () << qslFileNames.at (i) + QDir::separator ()
                                               << destPath
                );
            } else {
                queue = addJob (queue,
                                QFileOperationsThread::CopyFileOperation,
                                QStringList () << qslFileNames.at (i)
                                               << copyDialog->dest ());
            }
        }
        queue->setBlocked (false);
        sourcePanel->clearSelection ();
    }
    delete copyDialog;
}

//
void MainWindow::slotRemove (const QStringList &fileList)
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    QFilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;

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
    connect (qdbbButtons,
             SIGNAL(accepted ()),
             qdRemoveDialog,
             SLOT(accept ()));
    connect (qdbbButtons,
             SIGNAL(rejected ()),
             qdRemoveDialog,
             SLOT(reject ()));

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (qlQuestion);
    qvblMainLayout->addWidget (qlvStrings);
    qvblMainLayout->addWidget (qlQueue);
    qvblMainLayout->addWidget (qcbQueue);
    qvblMainLayout->addWidget (qdbbButtons);

    qdRemoveDialog->setLayout (qvblMainLayout);

    if (qdRemoveDialog->exec ()) {
        QFileOperationsDialog *queue = 0;
        int queueIndex = qcbQueue->currentIndex ();
        if (queueIndex >= 0 && queueList_.at (queueIndex)) {
            queue = queueList_.at (queueIndex);
        }
        for (int i = 0; i < qslFiles.count (); i++) {
            if (QFileInfo (qslFiles.at (i)).isDir ()) {
                queue = addJob (queue,
                                QFileOperationsThread::RemoveDirOperation,
                                QStringList () << qslFiles.at (i));
            } else {
                queue = addJob (queue,
                                QFileOperationsThread::RemoveFileOperation,
                                QStringList () << qslFiles.at (i));
            }
        }
        queue->setBlocked (false);
        sourcePanel->clearSelection ();
    }
    delete qdRemoveDialog;
}

//
QFileOperationsDialog *
MainWindow::addJob (QFileOperationsDialog *queue, QFileOperationsThread::FileOperation operation,
                    const QStringList &parameters)
{
    if (!queue) {
        queue = new QFileOperationsDialog (0);
        connect (queue, SIGNAL(finished (int)),
                 this, SLOT(slotQueueFinished ()));
        connect (queue, SIGNAL(jobChanged ()),
                 this, SLOT(slotQueueChanged ()));
        queueList_ << queue;
        queue->show ();
    }
    queue->setBlocked (true);
    queue->addJob (operation, parameters);

    return queue;
}

//
void MainWindow::slotQueueFinished ()
{
    QFileOperationsDialog *dialog = qobject_cast<QFileOperationsDialog *> (sender ());
    if (dialog) {
        if (queueList_.removeOne (dialog)) {
            dialog->deleteLater ();
            slotQueueChanged ();
        }
    }
}

//
void MainWindow::slotQueueChanged ()
{
    qeueuModel_->clear ();
    QStandardItem *item;
    for (int i = 0; i < queueList_.count (); i++) {
        item = new QStandardItem (queueList_.at (i)->jobName ());
        qeueuModel_->appendRow (item);
    }
}

//
void MainWindow::slotMkDir ()
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    QFilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;

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
    connect (qdbbButtons,
             SIGNAL(accepted ()),
             qdMkDirDialog,
             SLOT(accept ()));
    connect (qdbbButtons,
             SIGNAL(rejected ()),
             qdMkDirDialog,
             SLOT(reject ()));

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

//
void MainWindow::slotMove (const QString &destDir, const QStringList &fileList)
{
    if (!rightPanel_->hasFocus () && !leftPanel_->hasFocus ()) {
        return;
    }
    QFilePanel *sourcePanel = rightPanel_->hasFocus () ? rightPanel_ : leftPanel_;
    QFilePanel *destPanel = rightPanel_->hasFocus () ? leftPanel_ : rightPanel_;

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
        QFileOperationsDialog *queue = 0;
        int queueIndex = moveDialog->queueIndex ();
        if (queueIndex >= 0 && queueList_.at (queueIndex)) {
            queue = queueList_.at (queueIndex);
        }
        for (int i = 0; i < qslFileNames.count (); i++) {
            if (QFileInfo (qslFileNames.at (i)).isDir ()) {
                queue = addJob (queue,
                                QFileOperationsThread::MoveDirOperation,
                                QStringList () << qslFileNames.at (i) + QDir::separator ()
                                               << moveDialog->dest () + QDir::separator ());
            } else {
                queue = addJob (queue,
                                QFileOperationsThread::MoveFileOperation,
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

//
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

/*?	QTreeView* view=qobject_cast<QTreeView*>(widget->parent());
	if (view)
	{
		QString destPath;
		if (leftPanel_->isAncestorOf(view))
			destPath=leftPanel_->path();
		else
			destPath=rightPanel_->path();

		QModelIndex index=view->model()->index(view->indexAt(view->viewport()->mapFrom(this,event->pos())).row(),0);
		if (index.isValid())
		{
			QDir dir(destPath);
			dir.cd(index.data(Qt::EditRole).toString());
			destPath=QDir::toNativeSeparators(dir.absolutePath());
			if (destPath.at(destPath.length()-1)!=QDir::separator())
				destPath+=QDir::separator();
		}
		QStringList fileList;
		foreach(QUrl url,event->mimeData()->urls())
			fileList << QDir::toNativeSeparators(url.toLocalFile());
		if (event->mouseButtons() & Qt::RightButton)
			slotMove(destPath,fileList);
		else
			slotCopy(destPath,fileList);
	}*/
/*?*///	QMainWindow::dropEvent(event);
}

//
void MainWindow::dragMoveEvent (QDragMoveEvent *event)
{
    bool isAccepted = false;
    QWidget *widget = childAt (event->pos ());
//Command Buttons
    isAccepted = widget == runConsoleButton_ || widget == viewButton_ || widget == editButton_ ||
                 widget == removeButton_;

        foreach(PCToolBar *toolBar, toolBars_) {
            if (toolBar == widget || toolBar == widget->parentWidget ()) {
                isAccepted = true;
                break;
            }
        }
//TreeView
/*?	QTreeView* view=qobject_cast<QTreeView*>(widget->parent());
	if (view)
	{
		QModelIndex index=view->model()->index(view->indexAt(view->viewport()->mapFrom(this,event->pos())).row(),0);
		if (!index.isValid())
			isAccepted=true;
		else
		{
			if (!(view==event->source() && view->selectionModel()->isSelected(index)))
				isAccepted=true;
		}
	}*/
//Drives toolbar

    event->setAccepted (isAccepted);

/*?*///	QMainWindow::dragMoveEvent(event);
}

//
void MainWindow::dragEnterEvent (QDragEnterEvent *event)
{
    if (event->mimeData ()->hasFormat ("text/uri-list")) {
        event->acceptProposedAction ();
    }

/*?*///	QMainWindow::dragEnterEvent(event);
}

//
void MainWindow::toolBarActionExecute (const SToolBarButton &actionButton)
{
#ifndef Q_CC_MSVC
#warning "TODO: parse params"
#endif
    QFileInfo fi (actionButton.qsCommand);
    if (!fi.exists ()) {
        QAction *action = PCCommands::instance ()->action (actionButton.qsCommand);
        if (action) {
            action->trigger ();
        }
    }
    if (fi.isDir ()) {
        cdExecute (fi.absoluteFilePath ());
    } else {
        QFileOperationsThread::execute (actionButton.qsCommand,
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

//
void MainWindow::slotPreferences ()
{
    PreferencesDialog dialog (this);
    dialog.exec ();
}

//
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

        foreach (PCToolBar *toolBar, toolBars_) {
            if (toolBar->name () == qsToolBarName) {
                QMessageBox::critical (this, "", tr ("This name is not unique."));
                slotAddToolBar ();
                return;
            }
        }

    PCToolBar *toolBar = new PCToolBar (qsToolBarName, this);
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
    PCToolBar *toolBar = toolBars_.at (action->data ().toInt ());
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
        PCToolBar *toolBar = new PCToolBar (tr ("Main toolbar"), this);
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
    PCToolBar *toolBar = toolBars_.at (action->data ().toInt ());
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

        foreach (PCToolBar *toolBar, toolBars_) {
            if (toolBar->name () == qsToolBarNewName) {
                QMessageBox::critical (this, "", tr ("This name is not unique. Break."));
                return;
            }
        }

    toolBar->rename (qsToolBarNewName);

    QSettings *settings = AppSettings::instance ();
    settings->remove ("ToolBar_" + qsName);

    QStringList qslToolBars;
        foreach(PCToolBar *toolBar, toolBars_) {
            qslToolBars << toolBar->name ();
        }

    settings->beginGroup ("Global");
    settings->setValue ("ToolBars", qslToolBars);
    settings->endGroup ();

    toolBar->save ();

    settings->sync ();
}

void MainWindow::connectToolBar (PCToolBar *toolBar)
{
    connect (toolBar, SIGNAL(toolBarActionExecuted (SToolBarButton)),
             this, SLOT(toolBarActionExecute (SToolBarButton)));
    connect (toolBar, SIGNAL(cdExecuted (QString)),
             this, SLOT(cdExecute (QString)));
    connect (toolBar, SIGNAL(toolbarContextMenu (QPoint)),
             this, SLOT(slotToolBarContextMenu (QPoint)));
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

QMenu *MainWindow::createToolBarsMenu (PCToolBar *currentToolBar)
{
    QMenu *qmToolBarMenu = new QMenu (tr ("Toolbars"), this);
    QAction *menuAction;

    menuAction = new QAction (tr ("&Add toolbar"), qmToolBarMenu);
    connect (menuAction, SIGNAL(triggered ()),
             this, SLOT(slotAddToolBar ()));
    qmToolBarMenu->addAction (menuAction);

    if (toolBars_.count () <= 0) {
        return qmToolBarMenu;
    }

    QMenu *removeMenu = new QMenu (tr ("&Remove toolbar"), this);
    int i = 0;
        foreach(PCToolBar *toolBar, toolBars_) {
            menuAction = new QAction (toolBar->name (), removeMenu);
            menuAction->setData (i++);
            connect (menuAction, SIGNAL(triggered ()),
                     this, SLOT(slotRemoveToolBar ()));
            removeMenu->addAction (menuAction);
            if (toolBar == currentToolBar) {
                removeMenu->setDefaultAction (menuAction);
            }
        }
    qmToolBarMenu->addMenu (removeMenu);

    QMenu *renameMenu = new QMenu (tr ("Re&name toolbar"), this);
    i = 0;
        foreach(PCToolBar *toolBar, toolBars_) {
            menuAction = new QAction (toolBar->name (), renameMenu);
            menuAction->setData (i++);
            connect (menuAction, SIGNAL(triggered ()),
                     this, SLOT(slotRenameToolBar ()));
            renameMenu->addAction (menuAction);
            if (toolBar == currentToolBar) {
                renameMenu->setDefaultAction (menuAction);
            }
        }
    qmToolBarMenu->addMenu (renameMenu);

    QMenu *showHideMenu = new QMenu (tr ("&Show/Hide toolbar"), this);
        foreach(PCToolBar *toolBar, toolBars_) {
            menuAction = new QAction (toolBar->name (), showHideMenu);
            menuAction->setCheckable (true);
            menuAction->setChecked (toolBar->isVisible ());
            connect (menuAction, SIGNAL(triggered (bool)),
                     toolBar, SLOT(setVisible (bool)));
            showHideMenu->addAction (menuAction);
            if (toolBar == currentToolBar) {
                showHideMenu->setDefaultAction (menuAction);
            }
        }
    qmToolBarMenu->addMenu (showHideMenu);

    if (currentToolBar) {
        qmToolBarMenu->addSeparator ();

        menuAction = new QAction (tr ("A&dd separator"), qmToolBarMenu);
        connect (menuAction, SIGNAL(triggered ()),
                 currentToolBar, SLOT(slotAddSeparator ()));
        qmToolBarMenu->addAction (menuAction);
    }

    return qmToolBarMenu;
}

void MainWindow::slotToolBarContextMenu (const QPoint &pos)
{
    PCToolBar *pcToolBar = qobject_cast<PCToolBar *> (sender ());
    if (!pcToolBar) {
        return;
    }

    createToolBarsMenu (pcToolBar)->exec (pos);
}

/* ** TESTING PURPOSES ONLY ** */
#include <QFileDialog>

void MainWindow::slotTestingFileDialog ()
{
    QFileDialog dialog (this);
    dialog.exec ();
}

#ifdef Q_WS_WIN
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;

void MainWindow::slotTestingEnableNTFSPermissionLookup(bool enable)
{
    if(enable)
        qt_ntfs_permission_lookup = 1;
    else
        qt_ntfs_permission_lookup = 0;
}
#endif
