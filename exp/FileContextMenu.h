#pragma once

#include <QtCore/QStringList>

#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

class FileContextMenuPrivate;

class FileContextMenu : public QMenu
{
Q_OBJECT

public:
    explicit FileContextMenu (QWidget *parent = 0);

    explicit FileContextMenu (const QString &title, QWidget *parent = 0);

    virtual ~FileContextMenu ();

    void setPath (const QString &path);

    void setPaths (const QStringList &paths);

    QAction *executeNativeMenu (const QPoint &pos);

protected:
    void moveEvent (QMoveEvent *event);

private:
    friend class FileContextMenuPrivate;

    FileContextMenuPrivate *const d;
    Q_PRIVATE_SLOT(d, void _q_nativeActionTriggered ())
};

class FileContextMenuAction : public QAction
{
Q_OBJECT

public:
    explicit FileContextMenuAction (QObject *parent = 0)
        : QAction (parent)
        , itemData (0)
        , itemUserData (0)
    {
    }

    int itemData;
    qint64 itemUserData;
};

