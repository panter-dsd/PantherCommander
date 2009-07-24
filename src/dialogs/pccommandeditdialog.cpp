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

#include <QtCore/QCoreApplication>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QDialogButtonBox>

#include "pccommandeditdialog.h"
#include "pcshortcutedit.h"

class PCShortcutWidget : public QWidget
{
public:
	PCShortcutWidget(QWidget *parent);
	void setShortcut(const QKeySequence& ks);
	QKeySequence shortcut();
	void clear();

private:
	PCShortcutEdit *qcseEdit;
	QToolButton *qtbDefault;
	QToolButton *qtbClear;
};

PCShortcutWidget::PCShortcutWidget(QWidget *parent)
		: QWidget(parent)
{
	qcseEdit = new PCShortcutEdit(this);

	qtbDefault = new QToolButton(this);

	qtbClear = new QToolButton(this);
	connect(qtbClear, SIGNAL(clicked()),
			qcseEdit, SLOT(clear()));

	QHBoxLayout *qhblMainLayout = new QHBoxLayout();
	qhblMainLayout->addWidget(qcseEdit);
	qhblMainLayout->addWidget(qtbDefault);
	qhblMainLayout->addWidget(qtbClear);

	this->setLayout(qhblMainLayout);
}

void PCShortcutWidget::setShortcut(const QKeySequence& ks)
{
	qcseEdit->setShortcut(ks);
}

QKeySequence PCShortcutWidget::shortcut()
{
	return qcseEdit->shortcut();
}

void PCShortcutWidget::clear()
{
	qcseEdit->clear();
}

PCCommandEditDialog::PCCommandEditDialog(QWidget* parent, Qt::WindowFlags f)
		:QDialog(parent, f)
{
	qgbMainBox = new QGroupBox(this);

	qlObjectName = new QLabel(tr("Command name"), this);

	qleObjectName = new QLineEdit(this);
	qleObjectName->setEnabled(false);

	qlText = new QLabel(tr("Command text"), this);

	qleText = new QLineEdit(this);

	qlToolTip = new QLabel(tr("Command tooltip"), this);

	qleToolTip = new QLineEdit(this);

	qgbShortcutBox = new QGroupBox(tr("Shortcuts"), this);

	qpbAddShortcut = new QPushButton(tr("Add shortcut"), this);
	connect(qpbAddShortcut, SIGNAL(clicked()),
			this, SLOT(addShortcut()));

	qpbRemoveShortcut = new QPushButton(tr("Remove shortcut"), this);
	connect(qpbRemoveShortcut, SIGNAL(clicked()),
			this, SLOT(removeShortcut()));

	qdbbButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
									   Qt::Horizontal,
									   this);
	connect(qdbbButtons, SIGNAL(accepted()),
			this, SLOT(accept()));
	connect(qdbbButtons, SIGNAL(rejected()),
			this, SLOT(reject()));

	qhblButtonsLayout = new QHBoxLayout();
	qhblButtonsLayout->addWidget(qpbAddShortcut);
	qhblButtonsLayout->addWidget(qpbRemoveShortcut);

	QVBoxLayout *qvblShortcutLayout = new QVBoxLayout();
	qvblShortcutLayout->addLayout(qhblButtonsLayout);

	qgbShortcutBox->setLayout(qvblShortcutLayout);

	QVBoxLayout *qglFirstLayout = new QVBoxLayout();
	qglFirstLayout->addWidget(qlObjectName);
	qglFirstLayout->addWidget(qleObjectName);
	qglFirstLayout->addWidget(qlText);
	qglFirstLayout->addWidget(qleText);
	qglFirstLayout->addWidget(qlToolTip);
	qglFirstLayout->addWidget(qleToolTip);

	qgbMainBox->setLayout(qglFirstLayout);

	QVBoxLayout *qvblMainLayout = new QVBoxLayout();
	qvblMainLayout->addWidget(qgbMainBox);
	qvblMainLayout->addWidget(qgbShortcutBox);
	qvblMainLayout->addWidget(qdbbButtons);

	this->setLayout(qvblMainLayout);
}

void PCCommandEditDialog::setCommandObjectName(const QString& objectName)
{
	qleObjectName->setText(objectName);
}

void PCCommandEditDialog::setCommandText(const QString& text)
{
	qleText->setText(text);
}

QString PCCommandEditDialog::commandText()
{
	return qleText->text();
}

void PCCommandEditDialog::setCommandToolTip(const QString& text)
{
	qleToolTip->setText(text);
}

QString PCCommandEditDialog::commandToolTip()
{
	return qleToolTip->text();
}

void PCCommandEditDialog::setCommandShortcuts(QList<QKeySequence> shortcuts)
{
	PCShortcutWidget *shortcut;
	foreach(shortcut, qlShortcutWidgets)
		delete shortcut;
	qlShortcutWidgets.clear();

	foreach(QKeySequence ks, shortcuts) {
		shortcut = new PCShortcutWidget(this);
		shortcut->setShortcut(ks);
		qlShortcutWidgets << shortcut;
		qgbShortcutBox->layout()->removeItem(qhblButtonsLayout);
		qgbShortcutBox->layout()->addWidget(shortcut);
		qgbShortcutBox->layout()->addItem(qhblButtonsLayout);
	}

	if (qlShortcutWidgets.isEmpty()) {
		shortcut = new PCShortcutWidget(this);
		qlShortcutWidgets << shortcut;
		qgbShortcutBox->layout()->removeItem(qhblButtonsLayout);
		qgbShortcutBox->layout()->addWidget(shortcut);
		qgbShortcutBox->layout()->addItem(qhblButtonsLayout);
	}
}

QList<QKeySequence> PCCommandEditDialog::commandShortcuts()
{
	QList<QKeySequence> l;
	foreach(PCShortcutWidget *w, qlShortcutWidgets) {
		QKeySequence ks = w->shortcut();
		if (!ks.isEmpty())
			l << ks;
	}
	return l;
}

void PCCommandEditDialog::addShortcut()
{
	PCShortcutWidget *shortcut = new PCShortcutWidget(this);
	qlShortcutWidgets << shortcut;
	qgbShortcutBox->layout()->removeItem(qhblButtonsLayout);
	qgbShortcutBox->layout()->addWidget(shortcut);
	qgbShortcutBox->layout()->addItem(qhblButtonsLayout);
}

void PCCommandEditDialog::removeShortcut()
{
	if (qlShortcutWidgets.count() == 1) {
		qlShortcutWidgets.first()->clear();
		return;
	}
	delete qlShortcutWidgets.last();
	qlShortcutWidgets.removeLast();
	QCoreApplication::processEvents();
	resize(size().width(), 1);
}
