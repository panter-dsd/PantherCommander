#include "PantherViewer.h"

#include <QtWidgets>

#include "AppSettings.h"

PantherViewer::PantherViewer (QWidget *parent, Qt::WindowFlags f)
    : QMainWindow (parent, f)
{
    setWindowTitle (tr ("PantherViewer"));
    createControls ();
    setLayouts ();
    createActions ();
    createMenu ();
    createToolBar ();
    setConnects ();
    loadSettings ();
}

//
PantherViewer::~PantherViewer ()
{
    saveSettings ();
}

//
void PantherViewer::createControls ()
{
    qtabwTabs = new QTabWidget ();
}

//
void PantherViewer::setLayouts ()
{
    this->setCentralWidget (qtabwTabs);
}

//
void PantherViewer::setConnects ()
{
    connect (actionCloseCurrentTab,
             SIGNAL(triggered ()),
             this,
             SLOT(slotCloseCurrentTab ()));
    connect (actionExit,
             SIGNAL(triggered ()),
             this,
             SLOT(close ()));

    connect (qtabwTabs,
             SIGNAL(currentChanged (int)),
             this,
             SLOT(slotCurrentTabChanged (int)));
}

//
QStringList PantherViewer::codecsList ()
{
    QStringList qslCodecs;
    QList<int> list = QTextCodec::availableMibs ();
    for (int i = 0; i < list.count (); i++) {
        qslCodecs << QTextCodec::codecForMib (list.at (i))->name ();
    }
    qslCodecs.sort ();
    return qslCodecs;
}

//
void PantherViewer::createActions ()
{
    qagCodecs = new QActionGroup (this);
    QAction *action;
    QStringList qslCodecs = codecsList ();
    for (int i = 0; i < qslCodecs.count (); i++) {
        action = new QAction (QString (qslCodecs.at (i)), this);
        action->setCheckable (true);
        if (qslCodecs.at (i) == QTextCodec::codecForLocale ()->name ()) {
            action->setChecked (true);
        }
        connect (action,
                 SIGNAL(triggered ()),
                 this,
                 SLOT(slotSetEncoding ()));
        qagCodecs->addAction (action);
    }

    actionExit = new QAction (tr ("Exit"), this);
    actionExit->setShortcut (Qt::ALT + Qt::Key_X);
    this->addAction (actionExit);

    actionCloseCurrentTab = new QAction (tr ("Close tab"), this);
    actionCloseCurrentTab->setShortcut (Qt::Key_Escape);
    this->addAction (actionCloseCurrentTab);
}

//
void PantherViewer::createMenu ()
{
    qmbMainMenu = new QMenuBar (this);

    QMenu *qmFile = new QMenu (tr ("File"), this);
    qmFile->addAction (actionCloseCurrentTab);
    qmFile->addAction (actionExit);
    qmbMainMenu->addMenu (qmFile);

    QMenu *qmCodecs = new QMenu (tr ("Text codecs"), this);
    qmCodecs->addActions (qagCodecs->actions ());
    qmbMainMenu->addMenu (qmCodecs);

    this->setMenuBar (qmbMainMenu);
}

//
void PantherViewer::createToolBar ()
{
    qtbarMainToolBar = new QToolBar (tr ("Main panel"), this);
    qtbarMainToolBar->addAction (actionCloseCurrentTab);
    qtbarMainToolBar->addAction (actionExit);

    this->addToolBar (qtbarMainToolBar);
}

//
void PantherViewer::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    move (settings->value ("PantherViewer/pos", QPoint (0, 0)).toPoint ());
    resize (settings->value ("PantherViewer/size", QSize (640, 480)).toSize ());
    bool bIsMaximized = settings->value ("PantherViewer/IsMaximized", false).toBool ();
    if (bIsMaximized) {
        setWindowState (Qt::WindowMaximized);
    }
}

//
void PantherViewer::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    if (this->windowState () != Qt::WindowMaximized) {
        settings->setValue ("PantherViewer/pos", this->pos ());
        settings->setValue ("PantherViewer/size", this->size ());
        settings->setValue ("PantherViewer/IsMaximized", false);
    } else {
        settings->setValue ("PantherViewer/IsMaximized", true);
    }
    settings->sync ();
}

//
void PantherViewer::viewFile (const QString &fileName)
{
    QFileInfo fileInfo (fileName);
    if (!fileInfo.exists ()) {
        QMessageBox::critical (this, tr ("Open error"), tr ("File %1 not exists").arg (fileName));
        return;
    }
    int tabIndex;
    if (PlainView::isOpen (fileName)) {
        tabIndex = qtabwTabs->addTab (new PlainView (fileName), fileInfo.fileName ());
    }
    qtabwTabs->setTabToolTip (tabIndex, fileName);
}

//
void PantherViewer::slotSetEncoding ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }
    AbstractView *view = qobject_cast<AbstractView *> (qtabwTabs->currentWidget ());
    if (view) {
        view->setTextCodec (action->text ());
    }
}

//
void PantherViewer::slotCurrentTabChanged (int index)
{
    AbstractView *view = qobject_cast<AbstractView *> (qtabwTabs->widget (index));
    if (!view) {
        return;
    }
    QList<QAction *> actionList = qagCodecs->actions ();
    QString codec = view->textCodec ();
    for (int i = 0; i < actionList.count (); i++) {
        if (actionList.at (i)->text () == codec) {
            actionList.at (i)->setChecked (true);
            break;
        }
    }
}

//
void PantherViewer::slotCloseCurrentTab ()
{
    qtabwTabs->removeTab (qtabwTabs->currentIndex ());
    if (qtabwTabs->count () == 0) {
        this->close ();
    } else {
        this->lower ();
    }
}
//