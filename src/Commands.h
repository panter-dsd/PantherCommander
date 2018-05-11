#pragma once

class QAction;

#include <QtCore/QObject>
#include <QtCore/QMultiHash>
#include <QtCore/QList>

class Commands : public QObject
{
Q_OBJECT
private:
    QMultiHash<QString, QAction *> actionHash;
public:
    static Commands *instance ();

    virtual ~Commands ();

    void addAction (const QString &category, QAction *action);

    QStringList categories ();

    QList<QAction *> actions (const QString &categoty);

    QAction *action (const QString &actionName);

    void saveAction (const QString &actionName);

protected:
    explicit Commands (QObject *parent = 0);

private:
    static Commands *pInstance;

    void loadShortcuts (QAction *action);
};

