#include <QtCore/QCoreApplication>
#include <QtWidgets/QAction>

#include "Commands.h"
#include "AppSettings.h"

Commands *Commands::pInstance = 0;

Commands *Commands::instance ()
{
    if (!Commands::pInstance) {
        Commands::pInstance = new Commands ();
    }
    return Commands::pInstance;
}

Commands::Commands (QObject *parent)
    : QObject (parent)
{
}

Commands::~Commands ()
{
    if (pInstance == this) {
        pInstance = 0;
    }
}

void Commands::addAction (const QString &category, QAction *action)
{
    actionHash.insert (category, action);
    loadShortcuts (action);
}

QStringList Commands::categories ()
{
    return actionHash.uniqueKeys ();
}

QList<QAction *> Commands::actions (const QString &categoty)
{
    return categoty.isEmpty () ? actionHash.values () : actionHash.values (categoty);
}

QAction *Commands::action (const QString &actionName)
{
        foreach(QAction *action, actionHash.values ()) {
            if (action->objectName () == actionName) {
                return action;
            }
        }
    return 0;
}

void Commands::loadShortcuts (QAction *action)
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("Actions");
    const QString actionName = action->objectName ();
    action->setText (settings->value (actionName + "_text", action->text ()).toString ());
    action->setToolTip (settings->value (actionName + "_toolTip", action->toolTip ()).toString ());
    QStringList shortcuts = settings->value (actionName + "_shortcuts").toStringList ();
    settings->endGroup ();

    QList<QKeySequence> l;
        foreach(QString shortcut, shortcuts) {
            if (!shortcut.isEmpty ()) {
                l << QKeySequence::fromString (shortcut);
            }
        }
    if (!l.isEmpty ()) {
        action->setShortcuts (l);
    }
}

void Commands::saveAction (const QString &actionName)
{
    QAction *savedAction = action (actionName);
    QStringList qslShortcuts;
        foreach(QKeySequence ks, savedAction->shortcuts ()) {
            qslShortcuts << ks.toString ();
        }

    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("Actions");
    settings->setValue (actionName + "_text", savedAction->text ());
    settings->setValue (actionName + "_toolTip", savedAction->toolTip ());
    settings->setValue (actionName + "_shortcuts", qslShortcuts);
    settings->endGroup ();
    settings->sync ();
}
