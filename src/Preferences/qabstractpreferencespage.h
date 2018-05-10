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

#ifndef QABSTRACTPREFERENCESPAGE_H
#define QABSTRACTPREFERENCESPAGE_H

#include <QtCore/QSettings>

#include <QtWidgets/QWidget>

class QString;

class QAbstractPreferencesPage : public QWidget
{
Q_OBJECT

public:
    QAbstractPreferencesPage (QWidget *parent = 0);

    virtual void saveSettings () = 0;

    virtual void loadSettings () = 0;

    virtual void setDefaults () = 0;

    //virtual QString preferenceGroup() const = 0;
    static QString preferenceGroup ()
    {
        return QString ();
    }

Q_SIGNALS:

    void modified ();
};

#endif // QABSTRACTPREFERENCESPAGE_H
