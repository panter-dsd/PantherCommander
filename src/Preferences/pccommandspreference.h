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

#ifndef PCCOMMANDSPREFERENCE_H
#define PCCOMMANDSPREFERENCE_H

class QListWidget;

class QTableWidget;

class QDialogButtonBox;

class QAction;

class QGroupBox;

class QLabel;

class QLineEdit;

class QToolButton;

class QTableWidgetItem;

#include "qabstractpreferencespage.h"

class PCCommandsPreference : public QAbstractPreferencesPage
{
Q_OBJECT
    enum Columns
    {
        COMMAND = 0,
        NAME,
        SHORTCUT,
        TOOLTTIP,
        COLUMN_COUNT
    };

private:
    QGroupBox *qgbCommands;
    QListWidget *qlwCategoryList;
    QTableWidget *qtwActionsTable;
    QGroupBox *qgbFilter;
    QLineEdit *qleFilter;
    QToolButton *qtbClearFilter;

    QAction *qaEditCommand;

    QMap<QString, QAction *> editingActions;
public:
    PCCommandsPreference (QWidget *parent = 0);

    virtual ~PCCommandsPreference ()
    {
    }

    void saveSettings ();

    void loadSettings ();

    void setDefaults ();

    static QString preferenceGroup ()
    {
        return tr ("Commands");
    }

    QAction *getCurrentAction ();

    QString getCurrentActionName ();

private:
    void loadCategories ();

    void setMaximumSizeCategoriesList ();

private Q_SLOTS:

    void loadActions (const QString &category = 0);

    void filterChange (const QString &filter);

    void editCommand ();

Q_SIGNALS:

    void itemActivated ();
};

#endif // PCCommandsPreference_H
