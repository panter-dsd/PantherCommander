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

#ifndef PCCOMMANDSDIALOG_H
#define PCCOMMANDSDIALOG_H

class PCCommandsPreference;

class QDialogButtonBox;

#include <QDialog>

class PCCommandsDialog : public QDialog
{
Q_OBJECT

private:
    PCCommandsPreference *commandReference;
    QDialogButtonBox *qdbbButtons;

public:
    PCCommandsDialog (QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint);

    ~PCCommandsDialog ();

    QAction *getCurrentAction ();

    QString getCurrentActionName ();

    void loadSettings ();

    void saveSetings ();

private Q_SLOTS:

    void saveAndAccept ();
};

#endif // PCCOMMANDSDIALOG_H
