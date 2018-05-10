#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QMimeData>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

#include "PCToolBar.h"
#include "AppSettings.h"

PCToolBar::PCToolBar (const QString &name, QWidget *parent)
    : QToolBar (name, parent)
{
    qsName = name;
    this->setObjectName (qsName);
    restore ();
    this->setAcceptDrops (true);
}

void PCToolBar::restore ()
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("ToolBar_" + qsName);
    int buttonsCount = settings->value ("ButtonsCount", 0).toInt ();
    for (int i = 0; i < buttonsCount; i++) {
        SToolBarButton button;
        button.qsCommand = settings->value ("Command_" + QString::number (i), "").toString ();
        button.qsParams = settings->value ("Params_" + QString::number (i), "").toString ();
        button.qsWorkDir = settings->value ("WorkDir_" + QString::number (i), "").toString ();
        button.qsIconFile = settings->value ("IconFile_" + QString::number (i), "").toString ();
        button.qiIcon = settings->value ("Icon_" + QString::number (i), "").value<QIcon> ();
        button.iconNumber = settings->value ("IconNumber_" + QString::number (i), -1).toInt ();
        button.qsCaption = settings->value ("Caption_" + QString::number (i), "").toString ();
        qlButtons << button;
    }
    settings->endGroup ();
    refreshActions ();
}

void PCToolBar::save ()
{
    QSettings *settings = AppSettings::instance ();

    settings->remove ("ToolBar_" + qsName);
    settings->beginGroup ("ToolBar_" + qsName);
    settings->setValue ("ButtonsCount", qlButtons.count ());

    int i = 0;
        foreach(const SToolBarButton &button, qlButtons) {
            settings->setValue ("Command_" + QString::number (i), button.qsCommand);
            settings->setValue ("Params_" + QString::number (i), button.qsParams);
            settings->setValue ("WorkDir_" + QString::number (i), button.qsWorkDir);
            settings->setValue ("IconFile_" + QString::number (i), button.qsIconFile);
            settings->setValue ("IconNumber_" + QString::number (i), button.iconNumber);
            settings->setValue ("Icon_" + QString::number (i), button.qiIcon);
            settings->setValue ("Caption_" + QString::number (i), button.qsCaption);
            i++;
        }
    settings->endGroup ();
    settings->sync ();
}

void PCToolBar::refreshActions ()
{
        foreach(QAction *act, this->actions ()) {
            this->removeAction (act);
            delete act;
        }

    QAction *action;
    int i = 0;
        foreach(SToolBarButton button, qlButtons) {
            if (!button.qsCommand.isEmpty ()) {
                action = new QAction (button.qiIcon, button.qsCaption, this);
                connect (action, SIGNAL(triggered ()),
                         this, SLOT(slotToolButtonPress ()));
                action->setData (QString::number (i));
                this->addAction (action);
            } else {
                action = this->addSeparator ();
                action->setData (QString::number (i));
            }
            i++;
        }
}

void PCToolBar::contextMenuEvent (QContextMenuEvent *event)
{
    QMenu *qmToolBarMenu = new QMenu (this);
    QAction *action = this->actionAt (event->pos ());
    QAction *menuAction;

    if (action) {
        SToolBarButton button = qlButtons.at (action->data ().toInt ());
        if (!button.qsCommand.isEmpty ()) {
            menuAction = new QAction (tr ("&Execute ") + button.qsCaption, qmToolBarMenu);
            connect (menuAction, SIGNAL(triggered ()),
                     action, SLOT(trigger ()));
            qmToolBarMenu->addAction (menuAction);
            qmToolBarMenu->addSeparator ();
        }

        menuAction = new QAction (tr ("&Change..."), qmToolBarMenu);
        menuAction->setData (action->data ());
        connect (menuAction, SIGNAL(triggered ()),
                 this, SLOT(slotToolButtonChange ()));
        qmToolBarMenu->addAction (menuAction);

        menuAction = new QAction (tr ("&Delete"), qmToolBarMenu);
        menuAction->setData (action->data ());
        connect (menuAction, SIGNAL(triggered ()),
                 this, SLOT(slotToolButtonDelete ()));
        qmToolBarMenu->addAction (menuAction);

        if (!button.qsCommand.isEmpty ()) {
            menuAction = new QAction (tr ("cd ") + button.qsWorkDir, qmToolBarMenu);
            menuAction->setData (action->data ());
            connect (menuAction, SIGNAL(triggered ()),
                     this, SLOT(slotToolButtonCD ()));
            qmToolBarMenu->addAction (menuAction);
        }

        qmToolBarMenu->exec (event->globalPos ());
    } else {
        emit toolbarContextMenu (event->globalPos ());
    }
    delete qmToolBarMenu;
}

void PCToolBar::slotToolButtonPress ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }

    SToolBarButton button = qlButtons.at (action->data ().toInt ());
    emit toolBarActionExecuted (button);
}

void PCToolBar::rename (const QString &name)
{
    qsName = name;
    this->setObjectName (qsName);
    save ();
}

void PCToolBar::slotToolButtonChange ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }

    int index = action->data ().toInt ();

    SToolBarButton button = qlButtons.at (index);

    QDialog *toolButtonChangeDialog = new QDialog (this);

    QToolButtonPreference *qtbpToolButtonPreference = new QToolButtonPreference (toolButtonChangeDialog);
    qtbpToolButtonPreference->setButton (button);

    QDialogButtonBox *qdbbButtonBox = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                            Qt::Horizontal,
                                                            toolButtonChangeDialog
    );
    connect (qdbbButtonBox, SIGNAL(accepted ()),
             toolButtonChangeDialog, SLOT(accept ()));
    connect (qdbbButtonBox, SIGNAL(rejected ()),
             toolButtonChangeDialog, SLOT(reject ()));

    QVBoxLayout *layout = new QVBoxLayout ();
    layout->addWidget (qtbpToolButtonPreference);
    layout->addWidget (qdbbButtonBox);
    toolButtonChangeDialog->setLayout (layout);

    if (toolButtonChangeDialog->exec ()) {
        button = qtbpToolButtonPreference->getButton ();
        qlButtons.removeAt (index);
        qlButtons.insert (index, button);
        refreshActions ();
    }
    delete toolButtonChangeDialog;
}

void PCToolBar::slotToolButtonDelete ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }
    int index = action->data ().toInt ();
    qlButtons.removeAt (index);
    refreshActions ();
}

void PCToolBar::slotToolButtonCD ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }

    SToolBarButton button = qlButtons.at (action->data ().toInt ());
    emit cdExecuted (button.qsWorkDir);
}

void PCToolBar::dropEvent (QDropEvent *event)
{
    QAction *action = this->actionAt (event->pos ());
    if (action && !(event->keyboardModifiers () & Qt::ShiftModifier)) {
        SToolBarButton button = qlButtons.at (action->data ().toInt ());
        if (!button.qsCommand.isEmpty ()) {
            QStringList qslParams;
                foreach(QUrl url, event->mimeData ()->urls ()) {
                    qslParams << QDir::toNativeSeparators (url.toLocalFile ());
                }
            button.qsParams += qslParams.join (QLatin1String (" "));
            emit toolBarActionExecuted (button);
        } else {//If separator then insert
            int index = action->data ().toInt () + 1;
                foreach(QUrl url, event->mimeData ()->urls ()) {
                    SToolBarButton button = QToolButtonPreference::getButton (url.toLocalFile ());
                    qlButtons.insert (index++, button);
                }
            refreshActions ();
        }
    } else if (action) {
        int index = action->data ().toInt ();
            foreach(QUrl url, event->mimeData ()->urls ()) {
                SToolBarButton button = QToolButtonPreference::getButton (url.toLocalFile ());
                qlButtons.insert (++index, button);
            }
        refreshActions ();
    } else {
            foreach(QUrl url, event->mimeData ()->urls ()) {
                SToolBarButton button = QToolButtonPreference::getButton (url.toLocalFile ());
                qlButtons << button;
            }
        refreshActions ();
    }
    event->accept ();
}

void PCToolBar::dragMoveEvent (QDragMoveEvent *event)
{
    event->setAccepted (true);
    event->accept ();
}

void PCToolBar::dragEnterEvent (QDragEnterEvent *event)
{
    if (event->mimeData ()->hasFormat ("text/uri-list")) {
        event->acceptProposedAction ();
    }
}

void PCToolBar::slotAddSeparator ()
{
    qlButtons << SToolBarButton ();
    refreshActions ();
}
