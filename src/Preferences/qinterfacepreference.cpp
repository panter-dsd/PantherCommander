#include <QtGui/QGroupBox>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include "qinterfacepreference.h"

QInterfacePreference::QInterfacePreference(QWidget *parent)
		:QAbstractPreferencesPage(parent)
{
	appSettings=AppSettings::getInstance();
	createControls();
	setLayouts();
	setConnects();
	loadSettings();
}
//
QInterfacePreference::~QInterfacePreference()
{

}
//
void QInterfacePreference::saveSettings()
{
	appSettings->setValue("Interface/ShowDriveBar", qchbShowDriveBar->isChecked());
	appSettings->setValue("Interface/ShowTwoDriveBar", qchbShowTwoDriveBar->isChecked());
	appSettings->setValue("Interface/ShowDriveButton", qchbShowDriveButton->isChecked());
	appSettings->setValue("Interface/ShowTabs", qchbShowTabs->isChecked());
	appSettings->setValue("Interface/ShowHeader", qchbShowHeader->isChecked());
	appSettings->setValue("Interface/ShowDirInformation", qchbShowDirInformation->isChecked());
	appSettings->setValue("Interface/ShowCommandLine", qchbShowCommandLine->isChecked());
	appSettings->setValue("Interface/ShowFunctionButtons", qchbShowFunctionButtons->isChecked());
	appSettings->setValue("Interface/ShowCurrentPathEditor", qgbCurrentPathEditor->isChecked());
	appSettings->setValue("Interface/ShowHistory", qchbShowHistory->isChecked());
	appSettings->setValue("Interface/ShowGotoRoot", qchbShowGotoRoot->isChecked());
	appSettings->setValue("Interface/ShowGotoUp",qchbShowGotoUp ->isChecked());
	appSettings->setValue("Interface/ShowGotoHome", qchbShowGotoHome->isChecked());
	appSettings->setValue("Interface/FlatInterface", qgbFlatInterface->isChecked());
	appSettings->setValue("Interface/FlatToolBar", qchbFlatToolBar->isChecked());
	appSettings->setValue("Interface/FlatDriveBar", qchbFlatDriveBar->isChecked());
	appSettings->setValue("Interface/FlatDriveButtons", qchbFlatDriveButtons->isChecked());
	appSettings->setValue("Interface/FlatFunctionButtons", qchbFlatFunctionButtons->isChecked());
}
//
void QInterfacePreference::loadSettings()
{
	qchbShowDriveBar->setChecked(appSettings->value("Interface/ShowDriveBar", true).toBool());
	qchbShowTwoDriveBar->setChecked(appSettings->value("Interface/ShowTwoDriveBar", true).toBool());
	qchbShowDriveButton->setChecked(appSettings->value("Interface/ShowDriveButton", true).toBool());
	qchbShowTabs->setChecked(appSettings->value("Interface/ShowTabs", true).toBool());
	qchbShowHeader->setChecked(appSettings->value("Interface/ShowHeader", true).toBool());
	qchbShowDirInformation->setChecked(appSettings->value("Interface/ShowDirInformation", true).toBool());
	qchbShowCommandLine->setChecked(appSettings->value("Interface/ShowCommandLine", true).toBool());
	qchbShowFunctionButtons->setChecked(appSettings->value("Interface/ShowFunctionButtons", true).toBool());
	qgbCurrentPathEditor->setChecked(appSettings->value("Interface/ShowCurrentPathEditor", true).toBool());
	qchbShowHistory->setChecked(appSettings->value("Interface/ShowHistory", true).toBool());
	qchbShowGotoRoot->setChecked(appSettings->value("Interface/ShowGotoRoot", true).toBool());
	qchbShowGotoUp->setChecked(appSettings->value("Interface/ShowGotoUp", true).toBool());
	qchbShowGotoHome->setChecked(appSettings->value("Interface/ShowGotoHome", true).toBool());
	qgbFlatInterface->setChecked(appSettings->value("Interface/FlatInterface", true).toBool());
	qchbFlatToolBar->setChecked(appSettings->value("Interface/FlatToolBar", true).toBool());
	qchbFlatDriveBar->setChecked(appSettings->value("Interface/FlatDriveBar", true).toBool());
	qchbFlatDriveButtons->setChecked(appSettings->value("Interface/FlatDriveButtons", true).toBool());
	qchbFlatFunctionButtons->setChecked(appSettings->value("Interface/FlatFunctionButtons", true).toBool());
}
//
void QInterfacePreference::setDefaults()
{
}
//
void QInterfacePreference::createControls()
{
	qgbGlobal = new QGroupBox(tr("Global"), this);
	qchbShowDriveBar = new QCheckBox(tr("Show drive bars"), this);
	qchbShowTwoDriveBar = new QCheckBox(tr("Show two drive bars"), this);
	qchbShowDriveButton = new QCheckBox(tr("Show drive button"), this);
	qchbShowTabs = new QCheckBox(tr("Show tabs"), this);
	qchbShowHeader = new QCheckBox(tr("Show headers"), this);
	qchbShowDirInformation = new QCheckBox(tr("Show dir information"), this);
	qchbShowCommandLine = new QCheckBox(tr("Show command line"), this);
	qchbShowFunctionButtons = new QCheckBox(tr("Show function buttons"), this);

	qgbCurrentPathEditor = new QGroupBox(tr("Show current path editor"), this);
	qgbCurrentPathEditor->setCheckable(true);
	qchbShowHistory = new QCheckBox(tr("Show history button"), this);
	qchbShowGotoRoot = new QCheckBox(tr("Show goto root button"), this);
	qchbShowGotoUp = new QCheckBox(tr("Show goto up button"), this);
	qchbShowGotoHome = new QCheckBox(tr("Show goto home button"), this);

	qgbFlatInterface = new QGroupBox(tr("Flat interface"), this);
	qgbFlatInterface->setCheckable(true);
	qchbFlatToolBar = new QCheckBox(tr("Flat tool bars"), this);
	qchbFlatDriveBar = new QCheckBox(tr("Flat drive bars"), this);
	qchbFlatDriveButtons = new QCheckBox(tr("Flat drive buttons"), this);
	qchbFlatFunctionButtons = new QCheckBox(tr("Flat function buttons"), this);
}
//
void QInterfacePreference::setLayouts()
{
	QVBoxLayout *qvblGlobalLayout = new QVBoxLayout();
	qvblGlobalLayout->addWidget(qchbShowDriveBar);
	qvblGlobalLayout->addWidget(qchbShowTwoDriveBar);
	qvblGlobalLayout->addWidget(qchbShowDriveButton);
	qvblGlobalLayout->addWidget(qchbShowTabs);
	qvblGlobalLayout->addWidget(qchbShowHeader);
	qvblGlobalLayout->addWidget(qchbShowDirInformation);
	qvblGlobalLayout->addWidget(qchbShowCommandLine);
	qvblGlobalLayout->addWidget(qchbShowFunctionButtons);
	qgbGlobal->setLayout(qvblGlobalLayout);

	QVBoxLayout *qvblCurrentPathEditorLayout = new QVBoxLayout();
	qvblCurrentPathEditorLayout->addWidget(qchbShowHistory);
	qvblCurrentPathEditorLayout->addWidget(qchbShowGotoRoot);
	qvblCurrentPathEditorLayout->addWidget(qchbShowGotoUp);
	qvblCurrentPathEditorLayout->addWidget(qchbShowGotoHome);
	qgbCurrentPathEditor->setLayout(qvblCurrentPathEditorLayout);

	QVBoxLayout *qvblFlatInterfaceLayout = new QVBoxLayout();
	qvblFlatInterfaceLayout->addWidget(qchbFlatToolBar);
	qvblFlatInterfaceLayout->addWidget(qchbFlatDriveBar);
	qvblFlatInterfaceLayout->addWidget(qchbFlatDriveButtons);
	qvblFlatInterfaceLayout->addWidget(qchbFlatFunctionButtons);
	qgbFlatInterface->setLayout(qvblFlatInterfaceLayout);

	QVBoxLayout *qvblMainLayout = new QVBoxLayout();
	qvblMainLayout->addWidget(qgbGlobal);
	qvblMainLayout->addWidget(qgbCurrentPathEditor);
	qvblMainLayout->addWidget(qgbFlatInterface);
	this->setLayout(qvblMainLayout);
}
//
void QInterfacePreference::setConnects()
{
	connect (qchbShowDriveBar, SIGNAL(toggled(bool)), qchbShowTwoDriveBar, SLOT(setEnabled(bool)));

	QList<QCheckBox*> checkBoxList = this->findChildren<QCheckBox*> ();
	foreach(const QCheckBox* checkBox, checkBoxList) {
			connect (checkBox, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
	}

	connect (qgbCurrentPathEditor, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
	connect (qgbFlatInterface, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
}
//
