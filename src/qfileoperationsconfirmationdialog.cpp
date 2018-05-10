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
#include <QtGui>
#include <QtWidgets>
#include <QtWidgets/QPushButton>
#include "qfileoperationsconfirmationdialog.h"
//
QFileOperationsConfirmationDialog::QFileOperationsConfirmationDialog(QWidget* parent)
		:QDialog(parent)
{
	qvblMainLayout=new QVBoxLayout();
	this->setLayout(qvblMainLayout);
}
//
void QFileOperationsConfirmationDialog::setButtons(int buttons)
{
	QPushButton* button;
	QGridLayout* buttonsLayout=new QGridLayout();
	int row=0,column=0;
	if (buttons & Ovewrite)
	{
		button=new QPushButton(tr("Ovewrite"),this);
		button->setObjectName(QString::number(Ovewrite));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & OvewriteAll)
	{
		button=new QPushButton(tr("OvewriteAll"),this);
		button->setObjectName(QString::number(OvewriteAll));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & Skip)
	{
		button=new QPushButton(tr("Skip"),this);
		button->setObjectName(QString::number(Skip));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & SkipAll)
	{
		button=new QPushButton(tr("SkipAll"),this);
		button->setObjectName(QString::number(SkipAll));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & Cancel)
	{
		button=new QPushButton(tr("Cancel"),this);
		button->setObjectName(QString::number(Cancel));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & OvewriteAllOlder)
	{
		button=new QPushButton(tr("OvewriteAllOlder"),this);
		button->setObjectName(QString::number(OvewriteAllOlder));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & OvewriteAllNew)
	{
		button=new QPushButton(tr("OvewriteAllNew"),this);
		button->setObjectName(QString::number(OvewriteAllNew));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & OvewriteAllSmallest)
	{
		button=new QPushButton(tr("OvewriteAllSmallest"),this);
		button->setObjectName(QString::number(OvewriteAllSmallest));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & OvewriteAllLager)
	{
		button=new QPushButton(tr("OvewriteAllLager"),this);
		button->setObjectName(QString::number(OvewriteAllLager));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & Rename)
	{
		button=new QPushButton(tr("Rename"),this);
		button->setObjectName(QString::number(Rename));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & Delete)
	{
		button=new QPushButton(tr("Delete"),this);
		button->setObjectName(QString::number(Delete));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & All)
	{
		button=new QPushButton(tr("All"),this);
		button->setObjectName(QString::number(All));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	if (buttons & Retry)
	{
		button=new QPushButton(tr("Retry"),this);
		button->setObjectName(QString::number(Retry));
		buttonsLayout->addWidget(button,row,column++);
		if (column>=3)
		{
			row++;
			column=0;
		}
		connect(button,SIGNAL(clicked()),this,SLOT(slotButtonClick()));
	}
	qvblMainLayout->addLayout(buttonsLayout);
}
//
void QFileOperationsConfirmationDialog::setFiles(const QString& source,const QString& dest)
{
	QString qsTmp;
	QGroupBox* qgrbSource=new QGroupBox(tr("Source file"),this);
	QLabel* qlSourceName=new QLabel(source,this);
	QLabel* qlSourceSizeDate=new QLabel(this);
	qsTmp=tr("%1 bytes, %2").arg(QFileInfo(source).size()).arg(QFileInfo(source).lastModified().toString("dd.MM.yyyy hh:mm:ss"));
	qlSourceSizeDate->setText(qsTmp);
	QVBoxLayout* qvblSourceLayout=new QVBoxLayout();
	qvblSourceLayout->addWidget(qlSourceName);
	qvblSourceLayout->addWidget(qlSourceSizeDate);
	qgrbSource->setLayout(qvblSourceLayout);
	qvblMainLayout->insertWidget(0,qgrbSource);

	QGroupBox* qgrbDest=new QGroupBox(tr("Dest file"),this);
	QLabel* qlDestName=new QLabel(dest,this);
	QLabel* qlDestSizeDate=new QLabel(this);
	qsTmp=tr("%1 bytes, %2").arg(QFileInfo(dest).size()).arg(QFileInfo(dest).lastModified().toString("dd.MM.yyyy hh:mm:ss"));
	qlDestSizeDate->setText(qsTmp);
	QVBoxLayout* qvblDestLayout=new QVBoxLayout();
	qvblDestLayout->addWidget(qlDestName);
	qvblDestLayout->addWidget(qlDestSizeDate);
	qgrbDest->setLayout(qvblDestLayout);
	qvblMainLayout->insertWidget(1,qgrbDest);
}
//
void QFileOperationsConfirmationDialog::setText(const QString& text)
{
	QLabel* qlText=new QLabel(text,this);
	qvblMainLayout->insertWidget(0,qlText);
}
//
void QFileOperationsConfirmationDialog::slotButtonClick()
{
	bool ok;
	result=sender()->objectName().toInt(&ok,10);
	this->accept();
}
//

