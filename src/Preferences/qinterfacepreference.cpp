#include <QtGui/QGroupBox>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include "qinterfacepreference.h"

QInterfacePreference::QInterfacePreference( QSettings *qsetSettings, QWidget *parent)
		:QAbstractPreferencesPage(qsetSettings,parent)
{
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
	qsetAppSettings->setValue("Interface/ShowDriveBar", qchbShowDriveBar->isChecked());
	qsetAppSettings->setValue("Interface/ShowTwoDriveBar", qchbShowTwoDriveBar->isChecked());
	qsetAppSettings->setValue("Interface/ShowDriveButton", qchbShowDriveButton->isChecked());
	qsetAppSettings->setValue("Interface/ShowTabs", qchbShowTabs->isChecked());
	qsetAppSettings->setValue("Interface/ShowShowHeader", qchbShowHeader->isChecked());
	qsetAppSettings->setValue("Interface/ShowDirInformation", qchbShowDirInformation->isChecked());
	qsetAppSettings->setValue("Interface/ShowCommandLine", qchbShowCommandLine->isChecked());
	qsetAppSettings->setValue("Interface/ShowFunctionButtons", qchbShowFunctionButtons->isChecked());
	qsetAppSettings->setValue("Interface/ShowCurrentPathEditor", qgbCurrentPathEditor->isChecked());
	qsetAppSettings->setValue("Interface/ShowHistory", qchbShowHistory->isChecked());
	qsetAppSettings->setValue("Interface/ShowGotoRoot", qchbShowGotoRoot->isChecked());
	qsetAppSettings->setValue("Interface/ShowGotoUp",qchbShowGotoUp ->isChecked());
	qsetAppSettings->setValue("Interface/ShowGotoHome", qchbShowGotoHome->isChecked());
	qsetAppSettings->setValue("Interface/FlatInterface", qgbFlatInterface->isChecked());
	qsetAppSettings->setValue("Interface/FlatToolBar", qchbFlatToolBar->isChecked());
	qsetAppSettings->setValue("Interface/FlatDriveBar", qchbFlatDriveBar->isChecked());
	qsetAppSettings->setValue("Interface/FlatDriveButtons", qchbFlatDriveButtons->isChecked());
	qsetAppSettings->setValue("Interface/FlatFunctionButtons", qchbFlatFunctionButtons->isChecked());
}
//
void QInterfacePreference::loadSettings()
{
	qchbShowDriveBar->setChecked(qsetAppSettings->value("Interface/ShowDriveBar", true).toBool());
	qchbShowTwoDriveBar->setChecked(qsetAppSettings->value("Interface/ShowTwoDriveBar", true).toBool());
	qchbShowDriveButton->setChecked(qsetAppSettings->value("Interface/ShowDriveButton", true).toBool());
	qchbShowTabs->setChecked(qsetAppSettings->value("Interface/ShowTabs", true).toBool());
	qchbShowHeader->setChecked(qsetAppSettings->value("Interface/ShowShowHeader", true).toBool());
	qchbShowDirInformation->setChecked(qsetAppSettings->value("Interface/ShowDirInformation", true).toBool());
	qchbShowCommandLine->setChecked(qsetAppSettings->value("Interface/ShowCommandLine", true).toBool());
	qchbShowFunctionButtons->setChecked(qsetAppSettings->value("Interface/ShowFunctionButtons", true).toBool());
	qgbCurrentPathEditor->setChecked(qsetAppSettings->value("Interface/ShowCurrentPathEditor", true).toBool());
	qchbShowHistory->setChecked(qsetAppSettings->value("Interface/ShowHistory", true).toBool());
	qchbShowGotoRoot->setChecked(qsetAppSettings->value("Interface/ShowGotoRoot", true).toBool());
	qchbShowGotoUp->setChecked(qsetAppSettings->value("Interface/ShowGotoUp", true).toBool());
	qchbShowGotoHome->setChecked(qsetAppSettings->value("Interface/ShowGotoHome", true).toBool());
	qgbFlatInterface->setChecked(qsetAppSettings->value("Interface/FlatInterface", true).toBool());
	qchbFlatToolBar->setChecked(qsetAppSettings->value("Interface/FlatToolBar", true).toBool());
	qchbFlatDriveBar->setChecked(qsetAppSettings->value("Interface/FlatDriveBar", true).toBool());
	qchbFlatDriveButtons->setChecked(qsetAppSettings->value("Interface/FlatDriveButtons", true).toBool());
	qchbFlatFunctionButtons->setChecked(qsetAppSettings->value("Interface/FlatFunctionButtons", true).toBool());
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
