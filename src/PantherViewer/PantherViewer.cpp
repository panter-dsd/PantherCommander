#include <QtCore/QTextCodec>
#include <QtCore/QFileInfo>

#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMessageBox>

#include "src/AppSettings.h"
#include "PlainView.h"
#include "AbstractView.h"

#include "PantherViewer.h"

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


PantherViewer::~PantherViewer ()
{
    saveSettings ();
}


void PantherViewer::createControls ()
{
    tabs_ = new QTabWidget ();
}


void PantherViewer::setLayouts ()
{
    setCentralWidget (tabs_);
}


void PantherViewer::setConnects ()
{
    connect (actionCloseCurrentTab_, &QAction::triggered, this, &PantherViewer::slotCloseCurrentTab);
    connect (actionExit_, &QAction::triggered, this, &PantherViewer::close);
    connect (tabs_, &QTabWidget::currentChanged, this, &PantherViewer::slotCurrentTabChanged);
}


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


void PantherViewer::createActions ()
{
    codecs_ = new QActionGroup (this);
    QAction *action;
    QStringList qslCodecs = codecsList ();
    for (int i = 0; i < qslCodecs.count (); i++) {
        action = new QAction (QString (qslCodecs.at (i)), this);
        action->setCheckable (true);
        if (qslCodecs.at (i) == QTextCodec::codecForLocale ()->name ()) {
            action->setChecked (true);
        }
        connect (action, &QAction::triggered, this, &PantherViewer::slotSetEncoding);
        codecs_->addAction (action);
    }

    actionExit_ = new QAction (tr ("Exit"), this);
    actionExit_->setShortcut (Qt::ALT + Qt::Key_X);
    addAction (actionExit_);

    actionCloseCurrentTab_ = new QAction (tr ("Close tab"), this);
    actionCloseCurrentTab_->setShortcut (Qt::Key_Escape);
    addAction (actionCloseCurrentTab_);
}


void PantherViewer::createMenu ()
{
    mainMenu_ = new QMenuBar (this);

    QMenu *qmFile = new QMenu (tr ("File"), this);
    qmFile->addAction (actionCloseCurrentTab_);
    qmFile->addAction (actionExit_);
    mainMenu_->addMenu (qmFile);

    QMenu *qmCodecs = new QMenu (tr ("Text codecs"), this);
    qmCodecs->addActions (codecs_->actions ());
    mainMenu_->addMenu (qmCodecs);

    setMenuBar (mainMenu_);
}


void PantherViewer::createToolBar ()
{
    mainToolBar_ = new QToolBar (tr ("Main panel"), this);
    mainToolBar_->addAction (actionCloseCurrentTab_);
    mainToolBar_->addAction (actionExit_);

    addToolBar (mainToolBar_);
}


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


void PantherViewer::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    if (windowState () != Qt::WindowMaximized) {
        settings->setValue ("PantherViewer/pos", pos ());
        settings->setValue ("PantherViewer/size", size ());
        settings->setValue ("PantherViewer/IsMaximized", false);
    } else {
        settings->setValue ("PantherViewer/IsMaximized", true);
    }
    settings->sync ();
}


void PantherViewer::viewFile (const QString &fileName)
{
    QFileInfo fileInfo (fileName);
    if (!fileInfo.exists ()) {
        QMessageBox::critical (this, tr ("Open error"), tr ("File %1 not exists").arg (fileName));
        return;
    }
    int tabIndex;
    if (PlainView::isOpen (fileName)) {
        tabIndex = tabs_->addTab (new PlainView (fileName), fileInfo.fileName ());
    }
    tabs_->setTabToolTip (tabIndex, fileName);
}


void PantherViewer::slotSetEncoding ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }
    AbstractView *view = qobject_cast<AbstractView *> (tabs_->currentWidget ());
    if (view) {
        view->setTextCodec (action->text ());
    }
}


void PantherViewer::slotCurrentTabChanged (int index)
{
    AbstractView *view = qobject_cast<AbstractView *> (tabs_->widget (index));
    if (!view) {
        return;
    }
    QList<QAction *> actionList = codecs_->actions ();
    QString codec = view->textCodec ();
    for (int i = 0; i < actionList.count (); i++) {
        if (actionList.at (i)->text () == codec) {
            actionList.at (i)->setChecked (true);
            break;
        }
    }
}


void PantherViewer::slotCloseCurrentTab ()
{
    tabs_->removeTab (tabs_->currentIndex ());
    if (tabs_->count () == 0) {
        close ();
    } else {
        lower ();
    }
}

