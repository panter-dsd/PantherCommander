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

#include "qselectdiscdialog.h"

#include <QtCore/QDir>

#include <QtGui/QLayout>
#include <QtGui/QListWidget>

#include "volumeinfoprovider.h"

QSelectDiscDialog::QSelectDiscDialog(QWidget* parent) : QDialog(parent)
{
	//resize(width(), 200);
	qlwDiscList=new QListWidget(this);
	foreach(const QFileInfo& fi, VolumeInfoProvider().volumes())
	{
		QString path = fi.absoluteFilePath();
		qlwDiscList->addItem(QDir::toNativeSeparators(path));
	}
	qlwDiscList->setCurrentRow(0);
	connect(qlwDiscList, SIGNAL(itemActivated(QListWidgetItem*)),
			this, SLOT(accept()));
	connect(qlwDiscList, SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(accept()));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(qlwDiscList);
	setLayout(layout);
}

QString QSelectDiscDialog::discName() const
{
	return qlwDiscList->currentItem()->text();
}
