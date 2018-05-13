#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QMimeData>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

#include "ToolBar.h"
#include "AppSettings.h"

ToolBar::ToolBar (const QString &name, QWidget *parent)
    : QToolBar (name, parent)
{
    qsName = name;
    setObjectName (qsName);
    restore ();
    setAcceptDrops (true);
}

void ToolBar::restore ()
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("ToolBar_" + qsName);
    int buttonsCount = settings->value ("ButtonsCount", 0).toInt ();
    for (int i = 0; i < buttonsCount; i++) {
        ToolBarButton button;
        button.command_ = settings->value ("Command_" + QString::number (i), "").toString ();
        button.parameters_ = settings->value ("Params_" + QString::number (i), "").toString ();
        button.workDir_ = settings->value ("WorkDir_" + QString::number (i), "").toString ();
        button.iconFile_ = settings->value ("IconFile_" + QString::number (i), "").toString ();
        button.icon_ = settings->value ("Icon_" + QString::number (i), "").value<QIcon> ();
        button.iconNumber_ = settings->value ("IconNumber_" + QString::number (i), -1).toInt ();
        button.caption_ = settings->value ("Caption_" + QString::number (i), "").toString ();
        qlButtons << button;
    }
    settings->endGroup ();
    refreshActions ();
}

void ToolBar::save ()
{
    QSettings *settings = AppSettings::instance ();

    settings->remove ("ToolBar_" + qsName);
    settings->beginGroup ("ToolBar_" + qsName);
    settings->setValue ("ButtonsCount", qlButtons.count ());

    int i = 0;
        foreach(const ToolBarButton &button, qlButtons) {
            settings->setValue ("Command_" + QString::number (i), button.command_);
            settings->setValue ("Params_" + QString::number (i), button.parameters_);
            settings->setValue ("WorkDir_" + QString::number (i), button.workDir_);
            settings->setValue ("IconFile_" + QString::number (i), button.iconFile_);
            settings->setValue ("IconNumber_" + QString::number (i), button.iconNumber_);
            settings->setValue ("Icon_" + QString::number (i), button.icon_);
            settings->setValue ("Caption_" + QString::number (i), button.caption_);
            i++;
        }
    settings->endGroup ();
    settings->sync ();
}

void ToolBar::refreshActions ()
{
        foreach(QAction *act, actions ()) {
            removeAction (act);
            delete act;
        }

    QAction *action;
    int i = 0;
    for (const ToolBarButton &button : qlButtons) {
        if (!button.command_.isEmpty ()) {
            action = new QAction (button.icon_, button.caption_, this);
            connect (action, &QAction::triggered, this, &ToolBar::slotToolButtonPress);
            action->setData (QString::number (i));
            addAction (action);
        } else {
            action = addSeparator ();
            action->setData (QString::number (i));
        }
        i++;
    }
}

void ToolBar::contextMenuEvent (QContextMenuEvent *event)
{
    QMenu *qmToolBarMenu = new QMenu (this);
    QAction *action = actionAt (event->pos ());
    QAction *menuAction;

    if (action) {
        ToolBarButton button = qlButtons.at (action->data ().toInt ());
        if (!button.command_.isEmpty ()) {
            menuAction = new QAction (tr ("&Execute ") + button.caption_, qmToolBarMenu);
            connect (menuAction, &QAction::triggered, action, &QAction::trigger);
            qmToolBarMenu->addAction (menuAction);
            qmToolBarMenu->addSeparator ();
        }

        menuAction = new QAction (tr ("&Change..."), qmToolBarMenu);
        menuAction->setData (action->data ());
        connect (menuAction, &QAction::triggered, this, &ToolBar::slotToolButtonChange);
        qmToolBarMenu->addAction (menuAction);

        menuAction = new QAction (tr ("&Delete"), qmToolBarMenu);
        menuAction->setData (action->data ());
        connect (menuAction, &QAction::triggered, this, &ToolBar::slotToolButtonDelete);
        qmToolBarMenu->addAction (menuAction);

        if (!button.command_.isEmpty ()) {
            menuAction = new QAction (tr ("cd ") + button.workDir_, qmToolBarMenu);
            menuAction->setData (action->data ());
            connect (menuAction, &QAction::triggered, this, &ToolBar::slotToolButtonCD);
            qmToolBarMenu->addAction (menuAction);
        }

        qmToolBarMenu->exec (event->globalPos ());
    } else {
        emit toolbarContextMenu (event->globalPos ());
    }
    delete qmToolBarMenu;
}

void ToolBar::slotToolButtonPress ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }

    ToolBarButton button = qlButtons.at (action->data ().toInt ());
    emit toolBarActionExecuted (button);
}

void ToolBar::rename (const QString &name)
{
    qsName = name;
    setObjectName (qsName);
    save ();
}

void ToolBar::slotToolButtonChange ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }

    int index = action->data ().toInt ();

    ToolBarButton button = qlButtons.at (index);

    QDialog *toolButtonChangeDialog = new QDialog (this);

    ToolButtonPreference *qtbpToolButtonPreference = new ToolButtonPreference (toolButtonChangeDialog);
    qtbpToolButtonPreference->setButton (button);

    QDialogButtonBox *qdbbButtonBox = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                            Qt::Horizontal,
                                                            toolButtonChangeDialog
    );
    connect (qdbbButtonBox, &QDialogButtonBox::accepted, toolButtonChangeDialog, &QDialog::accept);
    connect (qdbbButtonBox, &QDialogButtonBox::rejected, toolButtonChangeDialog, &QDialog::reject);

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

void ToolBar::slotToolButtonDelete ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }
    int index = action->data ().toInt ();
    qlButtons.removeAt (index);
    refreshActions ();
}

void ToolBar::slotToolButtonCD ()
{
    QAction *action = qobject_cast<QAction *> (sender ());
    if (!action) {
        return;
    }

    ToolBarButton button = qlButtons.at (action->data ().toInt ());
    emit cdExecuted (button.workDir_);
}

void ToolBar::dropEvent (QDropEvent *event)
{
    QAction *action = actionAt (event->pos ());
    if (action && !(event->keyboardModifiers () & Qt::ShiftModifier)) {
        ToolBarButton button = qlButtons.at (action->data ().toInt ());
        if (!button.command_.isEmpty ()) {
            QStringList qslParams;
                foreach(QUrl url, event->mimeData ()->urls ()) {
                    qslParams << QDir::toNativeSeparators (url.toLocalFile ());
                }
            button.parameters_ += qslParams.join (QLatin1String (" "));
            emit toolBarActionExecuted (button);
        } else {//If separator then insert
            int index = action->data ().toInt () + 1;
                foreach(QUrl url, event->mimeData ()->urls ()) {
                    ToolBarButton button = ToolButtonPreference::getButton (url.toLocalFile ());
                    qlButtons.insert (index++, button);
                }
            refreshActions ();
        }
    } else if (action) {
        int index = action->data ().toInt ();
            foreach(QUrl url, event->mimeData ()->urls ()) {
                ToolBarButton button = ToolButtonPreference::getButton (url.toLocalFile ());
                qlButtons.insert (++index, button);
            }
        refreshActions ();
    } else {
            foreach(QUrl url, event->mimeData ()->urls ()) {
                ToolBarButton button = ToolButtonPreference::getButton (url.toLocalFile ());
                qlButtons << button;
            }
        refreshActions ();
    }
    event->accept ();
}

void ToolBar::dragMoveEvent (QDragMoveEvent *event)
{
    event->setAccepted (true);
    event->accept ();
}

void ToolBar::dragEnterEvent (QDragEnterEvent *event)
{
    if (event->mimeData ()->hasFormat ("text/uri-list")) {
        event->acceptProposedAction ();
    }
}

void ToolBar::slotAddSeparator ()
{
    qlButtons << ToolBarButton ();
    refreshActions ();
}
