#ifndef TABBAR_H
#define TABBAR_H

#include <QtWidgets/QTabBar>

class TabBar : public QTabBar
{
Q_OBJECT
    Q_PROPERTY(bool showTabBarWhenOneTab
                   READ
                   showTabBarWhenOneTab
                   WRITE
                   setShowTabBarWhenOneTab)

public:
    explicit TabBar (QWidget *parent = 0);

    virtual ~TabBar ();

    bool showTabBarWhenOneTab () const;

    void setShowTabBarWhenOneTab (bool enabled);

protected:
    void mouseDoubleClickEvent (QMouseEvent *event);

    void mouseReleaseEvent (QMouseEvent *event);

    QSize tabSizeHint (int index) const;

    void tabInserted (int position);

    void tabRemoved (int position);

Q_SIGNALS:

    void newTab ();

    void cloneTab (int index);

private Q_SLOTS:

    void cloneTab ();

    void closeTab (int index = -1);

    void closeOtherTabs ();

    void contextMenuRequested (const QPoint &pos);

private:
    void updateVisibility ();

    bool m_showTabBarWhenOneTab;
};

#endif // TABBAR_H
