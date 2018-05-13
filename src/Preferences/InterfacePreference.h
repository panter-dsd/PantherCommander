#pragma once

#include "AbstractPreferencesPage.h"

class QGroupBox;

class QCheckBox;

class InterfacePreference : public AbstractPreferencesPage
{
Q_OBJECT

public:
    explicit InterfacePreference (QWidget *parent = nullptr);

    virtual ~InterfacePreference ();

    void saveSettings ();

    void loadSettings ();

    void setDefaults ();

    static QString preferenceGroup ();

private:
    void createControls ();

    void setLayouts ();

    void setConnects ();

private:
    QGroupBox *globalGroupBox_;
    QCheckBox *showDriveBar_;
    QCheckBox *showTwoDriveBars_;
    QCheckBox *showDriveButton_;
    QCheckBox *showDriveComboBox_;
    QCheckBox *showTabs_;
    QCheckBox *showHeader_;
    QCheckBox *showDirInformation_;
    QCheckBox *showCommandLine_;
    QCheckBox *showFunctionButtons_;

    QGroupBox *currentPathEditorGroupBox_;
    QCheckBox *showHistory_;
    QCheckBox *showGoRoot_;
    QCheckBox *showGoUp_;
    QCheckBox *showGoHome_;

    QGroupBox *flatInterface_;
    QCheckBox *flatToolBar_;
    QCheckBox *flatDriveBar_;
    QCheckBox *flatDriveButtons_;
    QCheckBox *flatFunctionButtons_;
};

