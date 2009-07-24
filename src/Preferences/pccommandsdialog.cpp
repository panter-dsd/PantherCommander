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
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>

#include "pccommandsdialog.h"
#include "appsettings.h"
#include "pccommandspreference.h"

PCCommandsDialog::PCCommandsDialog(QWidget* parent, Qt::WindowFlags f)
		:QDialog(parent, f)
{
	commandReference = new PCCommandsPreference(this);
	connect(commandReference, SIGNAL(itemActivated()),
			this, SLOT(accept()));

	qdbbButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
									   Qt::Horizontal,
									   this);
	connect(qdbbButtons, SIGNAL(accepted()),
			this, SLOT(saveAndAccept()));
	connect(qdbbButtons, SIGNAL(rejected()),
			this, SLOT(reject()));

	QVBoxLayout *qvblMainLayout = new QVBoxLayout();
	qvblMainLayout->addWidget(commandReference);
	qvblMainLayout->addWidget(qdbbButtons);
	this->setLayout(qvblMainLayout);

	loadSettings();
}

PCCommandsDialog::~PCCommandsDialog()
{
	saveSetings();
}

QAction* PCCommandsDialog::getCurrentAction()
{
	return commandReference->getCurrentAction();
}

QString PCCommandsDialog::getCurrentActionName()
{
	return commandReference->getCurrentActionName();
}

void PCCommandsDialog::loadSettings()
{
	QSettings* settings = AppSettings::instance();
	settings->beginGroup("CommandsDialog");
	move(settings->value("pos", QPoint(0, 0)).toPoint());
	resize(settings->value("size", QSize(640, 480)).toSize());
	if(settings->value("IsMaximized", false).toBool())
		showMaximized();

	settings->endGroup();
}

void PCCommandsDialog::saveSetings()
{
	QSettings* settings = AppSettings::instance();
	settings->beginGroup("CommandsDialog");
	settings->setValue("IsMaximized", isMaximized());
	if(!isMaximized()) {
		settings->setValue("pos", pos());
		settings->setValue("size", size());
	}

	settings->endGroup();
	settings->sync();
}

void PCCommandsDialog::saveAndAccept()
{
	commandReference->saveSettings();
}
