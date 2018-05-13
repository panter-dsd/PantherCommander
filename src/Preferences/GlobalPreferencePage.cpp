#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>

#include "GlobalPreferencePage.h"
#include "src/AppSettings.h"

GlobalPreferencePage::GlobalPreferencePage (QWidget *parent)
    : AbstractPreferencesPage (parent)
    , useNativeDialogs_ (new QCheckBox (tr ("Use native dialogs"), this))
{
    for (const QCheckBox *checkBox : findChildren<QCheckBox*> ()) {
        connect (checkBox, &QCheckBox::stateChanged, this, &GlobalPreferencePage::modified);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout ();
    mainLayout->addWidget (useNativeDialogs_);
    setLayout (mainLayout);

    loadSettings ();
}

GlobalPreferencePage::~GlobalPreferencePage ()
{
}

void GlobalPreferencePage::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->setValue ("Global/UseNativeDialogs", useNativeDialogs_->isChecked ());

    settings->sync ();
}

void GlobalPreferencePage::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    useNativeDialogs_->setChecked (settings->value ("Global/UseNativeDialogs", true).toBool ());
}

void GlobalPreferencePage::setDefaults ()
{
}

QString GlobalPreferencePage::preferenceGroup ()
{
    return tr ("Global");
}

