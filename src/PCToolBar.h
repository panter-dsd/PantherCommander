#ifndef PCTOOLBAR_H
#define PCTOOLBAR_H

#include <QtWidgets/QToolBar>
#include "QToolButtonPreference.h"

class PCToolBar : public QToolBar
{
Q_OBJECT
private:
    QList<SToolBarButton> qlButtons;
    QString qsName;
public:
    PCToolBar (const QString &name, QWidget *parent = 0);

    virtual ~PCToolBar ()
    {
    }

    QString name ()
    {
        return qsName;
    }

    void restore ();

    void save ();

    void rename (const QString &name);

private:
    void refreshActions ();

protected:
    void contextMenuEvent (QContextMenuEvent *event);

    void dropEvent (QDropEvent *event);

    void dragMoveEvent (QDragMoveEvent *event);

    void dragEnterEvent (QDragEnterEvent *event);

private Q_SLOTS:

    void slotToolButtonPress ();

    void slotToolButtonChange ();

    void slotToolButtonDelete ();

    void slotToolButtonCD ();

public Q_SLOTS:

    void slotAddSeparator ();

Q_SIGNALS:

    void toolBarActionExecuted (const SToolBarButton &action);

    void cdExecuted (const QString &path);

    void toolbarContextMenu (const QPoint &pos);
};

#endif // PCTOOLBAR_H
