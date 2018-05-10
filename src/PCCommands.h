#ifndef PCCOMMANDS_H
#define PCCOMMANDS_H

class QAction;

#include <QtCore/QObject>
#include <QtCore/QMultiHash>
#include <QtCore/QList>

class PCCommands : public QObject
{
Q_OBJECT
private:
    QMultiHash<QString, QAction *> actionHash;
public:
    static PCCommands *instance ();

    virtual ~PCCommands ();

    void addAction (const QString &category, QAction *action);

    QStringList categories ();

    QList<QAction *> actions (const QString &categoty);

    QAction *action (const QString &actionName);

    void saveAction (const QString &actionName);

protected:
    explicit PCCommands (QObject *parent = 0);

private:
    static PCCommands *pInstance;

    void loadShortcuts (QAction *action);
};

#endif // PCCOMMANDS_H
