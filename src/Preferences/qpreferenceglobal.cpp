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

#include <QtWidgets>

#include "qpreferenceglobal.h"
#include "appsettings.h"

//
QPreferenceGlobal::QPreferenceGlobal (QWidget *parent)
    : QAbstractPreferencesPage (parent)
{
    createControls ();
    setConnects ();
    setLayouts ();
    loadSettings ();
}

//
QPreferenceGlobal::~QPreferenceGlobal ()
{
}

//
void QPreferenceGlobal::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->setValue ("Global/UseNativeDialogs", qchbUseNativeDialogs->isChecked ());

    settings->sync ();
}

//
void QPreferenceGlobal::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    qchbUseNativeDialogs->setChecked (settings->value ("Global/UseNativeDialogs", true).toBool ());
}

//
void QPreferenceGlobal::setDefaults ()
{
}

//
void QPreferenceGlobal::createControls ()
{
    qchbUseNativeDialogs = new QCheckBox (tr ("Use native dialogs"), this);
}

//
void QPreferenceGlobal::setLayouts ()
{
    QVBoxLayout *mainLayout = new QVBoxLayout ();
    mainLayout->addWidget (qchbUseNativeDialogs);

    this->setLayout (mainLayout);
}

//
void QPreferenceGlobal::setConnects ()
{
    QList<QCheckBox *> checkBoxList = this->findChildren<QCheckBox *> ();
        foreach(const QCheckBox *checkBox, checkBoxList) {
            connect (checkBox, SIGNAL(stateChanged (int)), this, SIGNAL(modified ()));
        }
}
//
