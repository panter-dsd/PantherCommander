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
* Author:		PanteR
* Contact:	panter.dsd@gmail.com
*******************************************************************/

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
