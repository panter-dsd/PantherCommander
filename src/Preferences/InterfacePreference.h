#pragma once
//
#include "AbstractPreferencesPage.h"

//
class QCheckBox;

class QGroupBox;

//
class InterfacePreference : public AbstractPreferencesPage
{
Q_OBJECT

private:
    QGroupBox *qgbGlobal;
    QCheckBox *qchbShowDriveBar;
    QCheckBox *qchbShowTwoDriveBar;
    QCheckBox *qchbShowDriveButton;
    QCheckBox *qchbShowDriveComboBox;
    QCheckBox *qchbShowTabs;
    QCheckBox *qchbShowHeader;
    QCheckBox *qchbShowDirInformation;
    QCheckBox *qchbShowCommandLine;
    QCheckBox *qchbShowFunctionButtons;

    QGroupBox *qgbCurrentPathEditor;
    QCheckBox *qchbShowHistory;
    QCheckBox *qchbShowGotoRoot;
    QCheckBox *qchbShowGotoUp;
    QCheckBox *qchbShowGotoHome;

    QGroupBox *qgbFlatInterface;
    QCheckBox *qchbFlatToolBar;
    QCheckBox *qchbFlatDriveBar;
    QCheckBox *qchbFlatDriveButtons;
    QCheckBox *qchbFlatFunctionButtons;
public:
    InterfacePreference (QWidget *parent = 0);

    ~InterfacePreference ();

    void saveSettings ();

    void loadSettings ();

    void setDefaults ();

    static QString preferenceGroup ()
    {
        return tr ("Interface");
    }

private:
    void createControls ();

    void setLayouts ();

    void setConnects ();
};

