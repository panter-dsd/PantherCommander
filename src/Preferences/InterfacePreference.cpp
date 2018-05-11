#include "InterfacePreference.h"

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>

#include "src/AppSettings.h"

InterfacePreference::InterfacePreference (QWidget *parent)
    : AbstractPreferencesPage (parent)
{
    createControls ();
    setLayouts ();
    setConnects ();
    loadSettings ();
}

//
InterfacePreference::~InterfacePreference ()
{

}

//
void InterfacePreference::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->setValue ("Interface/ShowDriveBar", qchbShowDriveBar->isChecked ());
    settings->setValue ("Interface/ShowTwoDriveBar", qchbShowTwoDriveBar->isChecked ());
    settings->setValue ("Interface/ShowDriveButton", qchbShowDriveButton->isChecked ());
    settings->setValue ("Interface/ShowDriveComboBox", qchbShowDriveComboBox->isChecked ());
    settings->setValue ("Interface/ShowTabs", qchbShowTabs->isChecked ());
    settings->setValue ("Interface/ShowHeader", qchbShowHeader->isChecked ());
    settings->setValue ("Interface/ShowDirInformation", qchbShowDirInformation->isChecked ());
    settings->setValue ("Interface/ShowCommandLine", qchbShowCommandLine->isChecked ());
    settings->setValue ("Interface/ShowFunctionButtons", qchbShowFunctionButtons->isChecked ());
    settings->setValue ("Interface/ShowCurrentPathEditor", qgbCurrentPathEditor->isChecked ());
    settings->setValue ("Interface/ShowHistory", qchbShowHistory->isChecked ());
    settings->setValue ("Interface/ShowGotoRoot", qchbShowGotoRoot->isChecked ());
    settings->setValue ("Interface/ShowGotoUp", qchbShowGotoUp->isChecked ());
    settings->setValue ("Interface/ShowGotoHome", qchbShowGotoHome->isChecked ());
    settings->setValue ("Interface/FlatInterface", qgbFlatInterface->isChecked ());
    settings->setValue ("Interface/FlatToolBar", qchbFlatToolBar->isChecked ());
    settings->setValue ("Interface/FlatDriveBar", qchbFlatDriveBar->isChecked ());
    settings->setValue ("Interface/FlatDriveButtons", qchbFlatDriveButtons->isChecked ());
    settings->setValue ("Interface/FlatFunctionButtons", qchbFlatFunctionButtons->isChecked ());

    settings->sync ();
}

//
void InterfacePreference::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    qchbShowDriveBar->setChecked (settings->value ("Interface/ShowDriveBar", true).toBool ());
    qchbShowTwoDriveBar->setChecked (settings->value ("Interface/ShowTwoDriveBar", true).toBool ());
    qchbShowDriveButton->setChecked (settings->value ("Interface/ShowDriveButton", true).toBool ());
    qchbShowDriveComboBox->setChecked (settings->value ("Interface/ShowDriveComboBox", true).toBool ());
    qchbShowTabs->setChecked (settings->value ("Interface/ShowTabs", true).toBool ());
    qchbShowHeader->setChecked (settings->value ("Interface/ShowHeader", true).toBool ());
    qchbShowDirInformation->setChecked (settings->value ("Interface/ShowDirInformation", true).toBool ());
    qchbShowCommandLine->setChecked (settings->value ("Interface/ShowCommandLine", true).toBool ());
    qchbShowFunctionButtons->setChecked (settings->value ("Interface/ShowFunctionButtons", true).toBool ());
    qgbCurrentPathEditor->setChecked (settings->value ("Interface/ShowCurrentPathEditor", true).toBool ());
    qchbShowHistory->setChecked (settings->value ("Interface/ShowHistory", true).toBool ());
    qchbShowGotoRoot->setChecked (settings->value ("Interface/ShowGotoRoot", true).toBool ());
    qchbShowGotoUp->setChecked (settings->value ("Interface/ShowGotoUp", true).toBool ());
    qchbShowGotoHome->setChecked (settings->value ("Interface/ShowGotoHome", true).toBool ());
    qgbFlatInterface->setChecked (settings->value ("Interface/FlatInterface", true).toBool ());
    qchbFlatToolBar->setChecked (settings->value ("Interface/FlatToolBar", true).toBool ());
    qchbFlatDriveBar->setChecked (settings->value ("Interface/FlatDriveBar", true).toBool ());
    qchbFlatDriveButtons->setChecked (settings->value ("Interface/FlatDriveButtons", true).toBool ());
    qchbFlatFunctionButtons->setChecked (settings->value ("Interface/FlatFunctionButtons", true).toBool ());
}

//
void InterfacePreference::setDefaults ()
{
}

//
void InterfacePreference::createControls ()
{
    qgbGlobal = new QGroupBox (tr ("Global"), this);
    qchbShowDriveBar = new QCheckBox (tr ("Show drive bars"), this);
    qchbShowTwoDriveBar = new QCheckBox (tr ("Show two drive bars"), this);
    qchbShowDriveButton = new QCheckBox (tr ("Show drive button"), this);
    qchbShowDriveComboBox = new QCheckBox (tr ("Show drive combobox"), this);
    qchbShowTabs = new QCheckBox (tr ("Show tabs"), this);
    qchbShowHeader = new QCheckBox (tr ("Show headers"), this);
    qchbShowDirInformation = new QCheckBox (tr ("Show dir information"), this);
    qchbShowCommandLine = new QCheckBox (tr ("Show command line"), this);
    qchbShowFunctionButtons = new QCheckBox (tr ("Show function buttons"), this);

    qgbCurrentPathEditor = new QGroupBox (tr ("Show current path editor"), this);
    qgbCurrentPathEditor->setCheckable (true);
    qchbShowHistory = new QCheckBox (tr ("Show history button"), this);
    qchbShowGotoRoot = new QCheckBox (tr ("Show goto root button"), this);
    qchbShowGotoUp = new QCheckBox (tr ("Show goto up button"), this);
    qchbShowGotoHome = new QCheckBox (tr ("Show goto home button"), this);

    qgbFlatInterface = new QGroupBox (tr ("Flat interface"), this);
    qgbFlatInterface->setCheckable (true);
    qchbFlatToolBar = new QCheckBox (tr ("Flat tool bars"), this);
    qchbFlatDriveBar = new QCheckBox (tr ("Flat drive bars"), this);
    qchbFlatDriveButtons = new QCheckBox (tr ("Flat drive buttons"), this);
    qchbFlatFunctionButtons = new QCheckBox (tr ("Flat function buttons"), this);
}

//
void InterfacePreference::setLayouts ()
{
    QVBoxLayout *qvblGlobalLayout = new QVBoxLayout ();
    qvblGlobalLayout->addWidget (qchbShowDriveBar);
    qvblGlobalLayout->addWidget (qchbShowTwoDriveBar);
    qvblGlobalLayout->addWidget (qchbShowDriveButton);
    qvblGlobalLayout->addWidget (qchbShowDriveComboBox);
    qvblGlobalLayout->addWidget (qchbShowTabs);
    qvblGlobalLayout->addWidget (qchbShowHeader);
    qvblGlobalLayout->addWidget (qchbShowDirInformation);
    qvblGlobalLayout->addWidget (qchbShowCommandLine);
    qvblGlobalLayout->addWidget (qchbShowFunctionButtons);
    qgbGlobal->setLayout (qvblGlobalLayout);

    QVBoxLayout *qvblCurrentPathEditorLayout = new QVBoxLayout ();
    qvblCurrentPathEditorLayout->addWidget (qchbShowHistory);
    qvblCurrentPathEditorLayout->addWidget (qchbShowGotoRoot);
    qvblCurrentPathEditorLayout->addWidget (qchbShowGotoUp);
    qvblCurrentPathEditorLayout->addWidget (qchbShowGotoHome);
    qgbCurrentPathEditor->setLayout (qvblCurrentPathEditorLayout);

    QVBoxLayout *qvblFlatInterfaceLayout = new QVBoxLayout ();
    qvblFlatInterfaceLayout->addWidget (qchbFlatToolBar);
    qvblFlatInterfaceLayout->addWidget (qchbFlatDriveBar);
    qvblFlatInterfaceLayout->addWidget (qchbFlatDriveButtons);
    qvblFlatInterfaceLayout->addWidget (qchbFlatFunctionButtons);
    qgbFlatInterface->setLayout (qvblFlatInterfaceLayout);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (qgbGlobal);
    qvblMainLayout->addWidget (qgbCurrentPathEditor);
    qvblMainLayout->addWidget (qgbFlatInterface);
    this->setLayout (qvblMainLayout);
}

//
void InterfacePreference::setConnects ()
{
    connect (qchbShowDriveBar, SIGNAL(toggled (bool)), qchbShowTwoDriveBar, SLOT(setEnabled (bool)));

    QList<QCheckBox *> checkBoxList = this->findChildren<QCheckBox *> ();
        foreach(const QCheckBox *checkBox, checkBoxList) {
            connect (checkBox, SIGNAL(stateChanged (int)), this, SIGNAL(modified ()));
        }

    connect (qgbCurrentPathEditor, SIGNAL(toggled (bool)), this, SIGNAL(modified ()));
    connect (qgbFlatInterface, SIGNAL(toggled (bool)), this, SIGNAL(modified ()));
}
//
