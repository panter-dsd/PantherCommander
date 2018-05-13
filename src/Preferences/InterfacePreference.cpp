#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>

#include "src/AppSettings.h"

#include "InterfacePreference.h"

InterfacePreference::InterfacePreference (QWidget *parent)
    : AbstractPreferencesPage (parent)
{
    globalGroupBox_ = new QGroupBox (tr ("Global"), this);
    showDriveBar_ = new QCheckBox (tr ("Show drive bars"), this);
    showTwoDriveBars_ = new QCheckBox (tr ("Show two drive bars"), this);
    showDriveButton_ = new QCheckBox (tr ("Show drive button"), this);
    showDriveComboBox_ = new QCheckBox (tr ("Show drive combobox"), this);
    showTabs_ = new QCheckBox (tr ("Show tabs"), this);
    showHeader_ = new QCheckBox (tr ("Show headers"), this);
    showDirInformation_ = new QCheckBox (tr ("Show dir information"), this);
    showCommandLine_ = new QCheckBox (tr ("Show command line"), this);
    showFunctionButtons_ = new QCheckBox (tr ("Show function buttons"), this);

    currentPathEditorGroupBox_ = new QGroupBox (tr ("Show current path editor"), this);
    currentPathEditorGroupBox_->setCheckable (true);
    showHistory_ = new QCheckBox (tr ("Show history button"), this);
    showGoRoot_ = new QCheckBox (tr ("Show goto root button"), this);
    showGoUp_ = new QCheckBox (tr ("Show goto up button"), this);
    showGoHome_ = new QCheckBox (tr ("Show goto home button"), this);

    flatInterface_ = new QGroupBox (tr ("Flat interface"), this);
    flatInterface_->setCheckable (true);
    flatToolBar_ = new QCheckBox (tr ("Flat tool bars"), this);
    flatDriveBar_ = new QCheckBox (tr ("Flat drive bars"), this);
    flatDriveButtons_ = new QCheckBox (tr ("Flat drive buttons"), this);
    flatFunctionButtons_ = new QCheckBox (tr ("Flat function buttons"), this);

    QVBoxLayout *globalLayout = new QVBoxLayout ();
    globalLayout->addWidget (showDriveBar_);
    globalLayout->addWidget (showTwoDriveBars_);
    globalLayout->addWidget (showDriveButton_);
    globalLayout->addWidget (showDriveComboBox_);
    globalLayout->addWidget (showTabs_);
    globalLayout->addWidget (showHeader_);
    globalLayout->addWidget (showDirInformation_);
    globalLayout->addWidget (showCommandLine_);
    globalLayout->addWidget (showFunctionButtons_);
    globalGroupBox_->setLayout (globalLayout);

    QVBoxLayout *currentPathEditorLayout = new QVBoxLayout ();
    currentPathEditorLayout->addWidget (showHistory_);
    currentPathEditorLayout->addWidget (showGoRoot_);
    currentPathEditorLayout->addWidget (showGoUp_);
    currentPathEditorLayout->addWidget (showGoHome_);
    currentPathEditorGroupBox_->setLayout (currentPathEditorLayout);

    QVBoxLayout *flatInterfaceLayout = new QVBoxLayout ();
    flatInterfaceLayout->addWidget (flatToolBar_);
    flatInterfaceLayout->addWidget (flatDriveBar_);
    flatInterfaceLayout->addWidget (flatDriveButtons_);
    flatInterfaceLayout->addWidget (flatFunctionButtons_);
    flatInterface_->setLayout (flatInterfaceLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout ();
    mainLayout->addWidget (globalGroupBox_);
    mainLayout->addWidget (currentPathEditorGroupBox_);
    mainLayout->addWidget (flatInterface_);
    setLayout (mainLayout);
    connect (showDriveBar_, &QCheckBox::toggled, showTwoDriveBars_, &QCheckBox::setEnabled);

    for (const QCheckBox *checkBox: findChildren<QCheckBox *> ()) {
        connect (checkBox, &QCheckBox::stateChanged, this, &InterfacePreference::modified);
    }

    connect (currentPathEditorGroupBox_, &QGroupBox::toggled, this, &InterfacePreference::modified);
    connect (flatInterface_, &QGroupBox::toggled, this, &InterfacePreference::modified);
    loadSettings ();
}

InterfacePreference::~InterfacePreference ()
{

}

void InterfacePreference::saveSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->setValue ("Interface/ShowDriveBar", showDriveBar_->isChecked ());
    settings->setValue ("Interface/ShowTwoDriveBar", showTwoDriveBars_->isChecked ());
    settings->setValue ("Interface/ShowDriveButton", showDriveButton_->isChecked ());
    settings->setValue ("Interface/ShowDriveComboBox", showDriveComboBox_->isChecked ());
    settings->setValue ("Interface/ShowTabs", showTabs_->isChecked ());
    settings->setValue ("Interface/ShowHeader", showHeader_->isChecked ());
    settings->setValue ("Interface/ShowDirInformation", showDirInformation_->isChecked ());
    settings->setValue ("Interface/ShowCommandLine", showCommandLine_->isChecked ());
    settings->setValue ("Interface/ShowFunctionButtons", showFunctionButtons_->isChecked ());
    settings->setValue ("Interface/ShowCurrentPathEditor", currentPathEditorGroupBox_->isChecked ());
    settings->setValue ("Interface/ShowHistory", showHistory_->isChecked ());
    settings->setValue ("Interface/ShowGotoRoot", showGoRoot_->isChecked ());
    settings->setValue ("Interface/ShowGotoUp", showGoUp_->isChecked ());
    settings->setValue ("Interface/ShowGotoHome", showGoHome_->isChecked ());
    settings->setValue ("Interface/FlatInterface", flatInterface_->isChecked ());
    settings->setValue ("Interface/FlatToolBar", flatToolBar_->isChecked ());
    settings->setValue ("Interface/FlatDriveBar", flatDriveBar_->isChecked ());
    settings->setValue ("Interface/FlatDriveButtons", flatDriveButtons_->isChecked ());
    settings->setValue ("Interface/FlatFunctionButtons", flatFunctionButtons_->isChecked ());

    settings->sync ();
}

void InterfacePreference::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    showDriveBar_->setChecked (settings->value ("Interface/ShowDriveBar", true).toBool ());
    showTwoDriveBars_->setChecked (settings->value ("Interface/ShowTwoDriveBar", true).toBool ());
    showDriveButton_->setChecked (settings->value ("Interface/ShowDriveButton", true).toBool ());
    showDriveComboBox_->setChecked (settings->value ("Interface/ShowDriveComboBox", true).toBool ());
    showTabs_->setChecked (settings->value ("Interface/ShowTabs", true).toBool ());
    showHeader_->setChecked (settings->value ("Interface/ShowHeader", true).toBool ());
    showDirInformation_->setChecked (settings->value ("Interface/ShowDirInformation", true).toBool ());
    showCommandLine_->setChecked (settings->value ("Interface/ShowCommandLine", true).toBool ());
    showFunctionButtons_->setChecked (settings->value ("Interface/ShowFunctionButtons", true).toBool ());
    currentPathEditorGroupBox_->setChecked (settings->value ("Interface/ShowCurrentPathEditor", true).toBool ());
    showHistory_->setChecked (settings->value ("Interface/ShowHistory", true).toBool ());
    showGoRoot_->setChecked (settings->value ("Interface/ShowGotoRoot", true).toBool ());
    showGoUp_->setChecked (settings->value ("Interface/ShowGotoUp", true).toBool ());
    showGoHome_->setChecked (settings->value ("Interface/ShowGotoHome", true).toBool ());
    flatInterface_->setChecked (settings->value ("Interface/FlatInterface", true).toBool ());
    flatToolBar_->setChecked (settings->value ("Interface/FlatToolBar", true).toBool ());
    flatDriveBar_->setChecked (settings->value ("Interface/FlatDriveBar", true).toBool ());
    flatDriveButtons_->setChecked (settings->value ("Interface/FlatDriveButtons", true).toBool ());
    flatFunctionButtons_->setChecked (settings->value ("Interface/FlatFunctionButtons", true).toBool ());
}

void InterfacePreference::setDefaults ()
{
}

QString InterfacePreference::preferenceGroup ()
{
    return tr ("Interface");
}

