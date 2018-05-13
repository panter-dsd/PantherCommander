#include <QtWidgets>

#include "GlobalPreferencePage.h"
#include "src/AppSettings.h"


GlobalPreferencePage::GlobalPreferencePage (QWidget *parent)
    : AbstractPreferencesPage (parent)
{
    createControls ();
    setConnects ();
    setLayouts ();
    loadSettings ();
}


GlobalPreferencePage::~GlobalPreferencePage ()
{
}


void GlobalPreferencePage::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->setValue ("Global/UseNativeDialogs", qchbUseNativeDialogs->isChecked ());

    settings->sync ();
}


void GlobalPreferencePage::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    qchbUseNativeDialogs->setChecked (settings->value ("Global/UseNativeDialogs", true).toBool ());
}


void GlobalPreferencePage::setDefaults ()
{
}


void GlobalPreferencePage::createControls ()
{
    qchbUseNativeDialogs = new QCheckBox (tr ("Use native dialogs"), this);
}


void GlobalPreferencePage::setLayouts ()
{
    QVBoxLayout *mainLayout = new QVBoxLayout ();
    mainLayout->addWidget (qchbUseNativeDialogs);

    setLayout (mainLayout);
}


void GlobalPreferencePage::setConnects ()
{
    for (const QCheckBox *checkBox : findChildren<QCheckBox *> ()) {
        connect (checkBox, &QCheckBox::stateChanged, this, &GlobalPreferencePage::modified);
    }
}

