#include "TabBar.h"

#include <QtCore/QEvent>

#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtGui/QMouseEvent>

TabBar::TabBar (QWidget *parent)
    : QTabBar (parent)
    , m_showTabBarWhenOneTab (true)
{
    setContextMenuPolicy (Qt::CustomContextMenu);

    connect (this, &TabBar::customContextMenuRequested, this, &TabBar::contextMenuRequested);
    connect (this, &TabBar::tabCloseRequested, this, &TabBar::closeTab);
}

TabBar::~TabBar ()
{
}

bool TabBar::showTabBarWhenOneTab () const
{
    return m_showTabBarWhenOneTab;
}

void TabBar::setShowTabBarWhenOneTab (bool enabled)
{
    m_showTabBarWhenOneTab = enabled;
    updateVisibility ();
}

void TabBar::mouseDoubleClickEvent (QMouseEvent *event)
{
    if (!childAt (event->pos ())
        // Remove the line below when QTabWidget does not have a one pixel frame
        && event->pos ().y () < y () + height ()) {
        emit newTab ();
        return;
    }

    QTabBar::mouseDoubleClickEvent (event);
}

void TabBar::mouseReleaseEvent (QMouseEvent *event)
{
    if (event->button () == Qt::MidButton) {
        int index = tabAt (event->pos ());
        if (index != -1) {
            removeTab (index);
            return;
        }
    }

    QTabBar::mouseReleaseEvent (event);
}

QSize TabBar::tabSizeHint (int index) const
{
    QSize sizeHint = QTabBar::tabSizeHint (index);
    QFontMetrics fm = fontMetrics ();
    return sizeHint.boundedTo (QSize (fm.width (QLatin1Char ('M')) * 18, sizeHint.height ()));
}

void TabBar::tabInserted (int position)
{
    Q_UNUSED(position);
    updateVisibility ();
}

void TabBar::tabRemoved (int position)
{
    Q_UNUSED(position);
    updateVisibility ();
}

void TabBar::cloneTab ()
{
    if (QAction *action = qobject_cast<QAction *> (sender ())) {
        int index = action->data ().toInt ();
        if (index != -1)
            emit {
            tabCloned (index);
        }
    }
}

void TabBar::closeTab (int index)
{
    if (index == -1) {
        if (QAction *action = qobject_cast<QAction *> (sender ())) {
            index = action->data ().toInt ();
        }
    }
    if (index != -1 && count () > 1) {
        removeTab (index);
    }
}

void TabBar::closeOtherTabs ()
{
    if (QAction *action = qobject_cast<QAction *> (sender ())) {
        int index = action->data ().toInt ();
        if (index != -1) {
            if (currentIndex () != index) {
                setCurrentIndex (index);
            }
            for (int i = count () - 1; i > index; --i) {
                removeTab (i);
            }
            for (int i = index - 1; i >= 0; --i) {
                removeTab (i);
            }
        }
    }
}

void TabBar::contextMenuRequested (const QPoint &pos)
{
    QMenu menu;
    QAction *action = nullptr;
    menu.addAction (tr ("Add Tab"), this, &TabBar::newTab);

    int index = tabAt (pos);
    if (index != -1) {
        action = menu.addAction (tr ("Duplicate Tab"), this, &TabBar::cloneTab);
        action->setData (index);
        if (count () > 1) {
            menu.addSeparator ();

            action = menu.addAction (tr ("&Close Tab"), this, &TabBar::closeTab, QKeySequence::Close);
            action->setData (index);

            action = menu.addAction (tr ("Close &Other Tabs"), this, &TabBar::closeOtherTabs);
            action->setData (index);
        }
    }
    menu.exec (mapToGlobal (pos));
}

void TabBar::updateVisibility ()
{
    setVisible (count () > 1 || m_showTabBarWhenOneTab);
}
