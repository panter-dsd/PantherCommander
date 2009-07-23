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

class QListWidget;
class QTableWidget;
class QDialogButtonBox;
class QAction;

#include <QDialog>

class PCCommandsDialog : public QDialog
{
	Q_OBJECT

private:
	QListWidget *qlwCategoryList;
	QTableWidget *qtwActionsTable;
	QDialogButtonBox *qdbbButtons;

public:
	PCCommandsDialog(QWidget* parent = 0);
	QAction* getCurrentAction();
	QString getCurrentActionName();

private:
	void createControls();
	void setLayouts();
	void loadCategories();

private Q_SLOTS:
	void loadActions(const QString &category = 0);
};

#endif // PCCOMMANDSDIALOG_H
