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

#include <QtGui/QAction>
#include <QtGui/QListWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>

#include "pccommandsdialog.h"

#include "appsettings.h"
#include "pccommands.h"

const QString allCategoryName = QObject::tr("All");

PCCommandsDialog::PCCommandsDialog(QWidget* parent)
		:QDialog(parent)
{
	createControls();
	setLayouts();
	loadCategories();
}

void PCCommandsDialog::createControls()
{
	qlwCategoryList = new QListWidget(this);
	connect (qlwCategoryList, SIGNAL(currentTextChanged(QString)),
			 this, SLOT(loadActions(QString)));

	qtwActionsTable = new QTableWidget(this);
	qtwActionsTable->setColumnCount(3);
	QStringList columns;
	columns << tr("Command")
			<< tr("Name")
			<< tr("Tool tip");
	qtwActionsTable->setHorizontalHeaderLabels(columns);
	qtwActionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	qdbbButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
									   Qt::Horizontal,
									   this);
	connect(qdbbButtons, SIGNAL(accepted()),
			this, SLOT(accept()));
	connect(qdbbButtons, SIGNAL(rejected()),
			this, SLOT(reject()));
}

void PCCommandsDialog::setLayouts()
{
	QHBoxLayout *qhblFirstLayout = new QHBoxLayout();
	qhblFirstLayout->addWidget(qlwCategoryList);
	qhblFirstLayout->addWidget(qtwActionsTable);

	QVBoxLayout *qvblMainLayout = new QVBoxLayout();
	qvblMainLayout->addLayout(qhblFirstLayout);
	qvblMainLayout->addWidget(qdbbButtons);

	this->setLayout(qvblMainLayout);
}

void PCCommandsDialog::loadCategories()
{
	qlwCategoryList->addItems(PCCommands::instance()->categories());
	qlwCategoryList->insertItem(0, allCategoryName);
}

void PCCommandsDialog::loadActions(const QString &category)
{
	QString qsCategoty = (category == allCategoryName) ? QString() : category;
	QTableWidgetItem *item;

	qtwActionsTable->setRowCount(0);
	QList<QAction*> l = PCCommands::instance()->actions(qsCategoty);
	qtwActionsTable->setRowCount(l.count());

	int i = 0;
	foreach(QAction *action, l) {
		item = new QTableWidgetItem(action->objectName());
		qtwActionsTable->setItem(i, 0, item);

		item = new QTableWidgetItem(action->text());
		qtwActionsTable->setItem(i, 1, item);

		item = new QTableWidgetItem(action->toolTip());
		qtwActionsTable->setItem(i++, 2, item);
	}

	qtwActionsTable->resizeColumnsToContents();
	qtwActionsTable->resizeRowsToContents();
}

QAction* PCCommandsDialog::getCurrentAction()
{
	QTableWidgetItem *item;
	item = qtwActionsTable->item(qtwActionsTable->currentRow(), 0);
	return item ? PCCommands::instance()->action(item->text()) : 0;
}

QString PCCommandsDialog::getCurrentActionName()
{
	QAction *action = getCurrentAction();
	return action ? action->objectName() : QString();
}