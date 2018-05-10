/********************************************************************
* Copyright (C) PanteR
*-------------------------------------------------------------------
*
* Panther Commander is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* Panther Commander is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Panther Commander; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301 USA
*-------------------------------------------------------------------
* Project:		Panther Commander
* Author:		Ritt K.
* Contact:		ritt.ks@gmail.com
*******************************************************************/

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
