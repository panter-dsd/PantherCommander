#ifndef QINTERFACEPREFERENCE_H
#define QINTERFACEPREFERENCE_H
//
#include "qabstractpreferencespage.h"
#include "appsettings.h"
//
class QCheckBox;
class QGroupBox;
//
class QInterfacePreference : public QAbstractPreferencesPage
{
	Q_OBJECT
private:
	AppSettings						*appSettings;
	QGroupBox							*qgbGlobal;
	QCheckBox							*qchbShowDriveBar;
	QCheckBox							*qchbShowTwoDriveBar;
	QCheckBox							*qchbShowDriveButton;
	QCheckBox							*qchbShowTabs;
	QCheckBox							*qchbShowHeader;
	QCheckBox							*qchbShowDirInformation;
	QCheckBox							*qchbShowCommandLine;
	QCheckBox							*qchbShowFunctionButtons;

	QGroupBox							*qgbCurrentPathEditor;
	QCheckBox							*qchbShowHistory;
	QCheckBox							*qchbShowGotoRoot;
	QCheckBox							*qchbShowGotoUp;
	QCheckBox							*qchbShowGotoHome;

	QGroupBox							*qgbFlatInterface;
	QCheckBox							*qchbFlatToolBar;
	QCheckBox							*qchbFlatDriveBar;
	QCheckBox							*qchbFlatDriveButtons;
	QCheckBox							*qchbFlatFunctionButtons;
public:
	QInterfacePreference(QWidget *parent = 0);
	~QInterfacePreference();

	void saveSettings();
	void loadSettings();
	void setDefaults();

	static QString preferenceGroup() { return tr("Interface"); }

private:
	void createControls();
	void setLayouts();
	void setConnects();
};

#endif // QINTERFACEPREFERENCE_H
