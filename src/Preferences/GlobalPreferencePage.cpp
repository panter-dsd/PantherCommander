#include <QtWidgets>

#include "GlobalPreferencePage.h"
#include "src/AppSettings.h"

//
GlobalPreferencePage::GlobalPreferencePage (QWidget *parent)
    : AbstractPreferencesPage (parent)
{
    createControls ();
    setConnects ();
    setLayouts ();
    loadSettings ();
}

//
GlobalPreferencePage::~GlobalPreferencePage ()
{
}

//
void GlobalPreferencePage::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->setValue ("Global/UseNativeDialogs", qchbUseNativeDialogs->isChecked ());

    settings->sync ();
}

//
void GlobalPreferencePage::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    qchbUseNativeDialogs->setChecked (settings->value ("Global/UseNativeDialogs", true).toBool ());
}

//
void GlobalPreferencePage::setDefaults ()
{
}

//
void GlobalPreferencePage::createControls ()
{
    qchbUseNativeDialogs = new QCheckBox (tr ("Use native dialogs"), this);
}

//
void GlobalPreferencePage::setLayouts ()
{
    QVBoxLayout *mainLayout = new QVBoxLayout ();
    mainLayout->addWidget (qchbUseNativeDialogs);

    this->setLayout (mainLayout);
}

//
void GlobalPreferencePage::setConnects ()
{
    QList<QCheckBox *> checkBoxList = this->findChildren<QCheckBox *> ();
        foreach(const QCheckBox *checkBox, checkBoxList) {
            connect (checkBox, SIGNAL(stateChanged (int)), this, SIGNAL(modified ()));
        }
}
//
