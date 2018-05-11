#include <QtWidgets>

#include "QPreferenceGlobal.h"
#include "src/AppSettings.h"

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
