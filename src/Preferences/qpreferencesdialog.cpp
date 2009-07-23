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
#include "qpreferencesdialog.h"
//
#include "qpreferenceglobal.h"
#include "qinterfacepreference.h"
//
QPreferencesDialog::QPreferencesDialog(QWidget * parent, Qt::WFlags f)
	: QDialog(parent,f)
{
	this->setWindowTitle(tr("Preferences"));
	createControls();
	setLayouts();
	setConnects();
	qdbbButtons->button(QDialogButtonBox::Apply)->setEnabled(false);
	this->resize(640,480);
}
//
QPreferencesDialog::~QPreferencesDialog()
{

}
//
void QPreferencesDialog::createControls()
{
	qlwPreferencesList=new QListWidget(this);
	QStringList qslPreferenceItems;
	qslPreferenceItems << QPreferenceGlobal::preferenceGroup()
			<< QInterfacePreference::preferenceGroup();
	qlwPreferencesList->addItems(qslPreferenceItems);
	qlwPreferencesList->setCurrentRow(0);
	setMaximumSizePreferencesList();

	QScrollArea *area;
	qswPreferencesWidgets=new QStackedWidget();
	area=new QScrollArea(this);
	area->setWidgetResizable(true);
	area->setWidget(new QPreferenceGlobal(qswPreferencesWidgets));
	qswPreferencesWidgets->addWidget(area);
	area=new QScrollArea(this);
	area->setWidgetResizable(true);
	area->setWidget(new QInterfacePreference(qswPreferencesWidgets));
	qswPreferencesWidgets->addWidget(area);

	qdbbButtons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Apply|QDialogButtonBox::Cancel,
				Qt::Horizontal,
				this);

	qpbSetDefaults=new QPushButton(tr("Default"),qdbbButtons);
	qdbbButtons->addButton(qpbSetDefaults,QDialogButtonBox::ApplyRole);
}
//
void QPreferencesDialog::setLayouts()
{
	QHBoxLayout* qhblPreferencesLayout=new QHBoxLayout();
	qhblPreferencesLayout->addWidget(qlwPreferencesList);
	qhblPreferencesLayout->addWidget(qswPreferencesWidgets);

	QVBoxLayout* qvblMainLayout=new QVBoxLayout();
	qvblMainLayout->addLayout(qhblPreferencesLayout);
	qvblMainLayout->addWidget(qdbbButtons);
	this->setLayout(qvblMainLayout);
}
//
void QPreferencesDialog::setConnects()
{
	connect(qdbbButtons,
				SIGNAL(accepted()),
				this,
				SLOT(slotSavePreferencesAndExit()));
	connect(qdbbButtons,
				SIGNAL(rejected()),
				this,
				SLOT(reject()));
	connect(qdbbButtons->button(QDialogButtonBox::Apply),
				SIGNAL(clicked()),
				this,
				SLOT(slotSavePreferences()));
	connect(qpbSetDefaults,
			SIGNAL(clicked()),
			this,
			SLOT(slotSetDefaults()));
	connect(qlwPreferencesList,
				SIGNAL(currentRowChanged (int)),
				qswPreferencesWidgets,
				SLOT(setCurrentIndex(int)));
	for (int i=0; i<qswPreferencesWidgets->count(); i++)
	{
		QScrollArea *area=qobject_cast<QScrollArea*>(qswPreferencesWidgets->widget(i));
		if (!area)
			continue;
		QAbstractPreferencesPage *widget=qobject_cast<QAbstractPreferencesPage*>(area->widget());
		if (widget)
			connect(widget,
						SIGNAL(modified()),
						this,
						SLOT(slotSetApplyEnabled()));
	}
}
//
void QPreferencesDialog::setMaximumSizePreferencesList()
{
	qlwPreferencesList->setMaximumWidth(50);
	for (int i=0; i<qlwPreferencesList->count(); i++)
	{
		int iWidth=QFontMetrics(qlwPreferencesList->font()).width(qlwPreferencesList->item(i)->text())+50;
		if (qlwPreferencesList->maximumWidth()<iWidth)
		{
			qlwPreferencesList->setMaximumWidth(iWidth);
		}
	}
}
//
void QPreferencesDialog::slotSavePreferences()
{
	for (int i=0; i<qswPreferencesWidgets->count(); i++)
	{
		QScrollArea *area=qobject_cast<QScrollArea*>(qswPreferencesWidgets->widget(i));
		if (!area)
			continue;
		QAbstractPreferencesPage *widget=qobject_cast<QAbstractPreferencesPage*>(area->widget());
		if (widget)
			widget->saveSettings();
	}
	qdbbButtons->button(QDialogButtonBox::Apply)->setEnabled(false);
}
//
void QPreferencesDialog::slotSetApplyEnabled()
{
	qdbbButtons->button(QDialogButtonBox::Apply)->setEnabled(true);
}
//
void QPreferencesDialog::slotSetDefaults()
{
	for (int i=0; i<qswPreferencesWidgets->count(); i++)
	{
		QScrollArea *area=qobject_cast<QScrollArea*>(qswPreferencesWidgets->widget(i));
		if (!area)
			continue;
		QAbstractPreferencesPage *widget=qobject_cast<QAbstractPreferencesPage*>(area->widget());
		if (widget)
			widget->setDefaults();
	}
}
//

