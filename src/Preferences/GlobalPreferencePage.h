#pragma once
//
#include "AbstractPreferencesPage.h"

//
class QLabel;

class QLineEdit;

class QCheckBox;

//
class GlobalPreferencePage : public AbstractPreferencesPage
{
Q_OBJECT
private:
    QCheckBox *qchbUseNativeDialogs;
public:
    GlobalPreferencePage (QWidget *parent = 0);

    ~GlobalPreferencePage ();

    void saveSettings ();

    void loadSettings ();

    void setDefaults ();

//	QString preferenceGroup() const
//	{ return tr("Global"); }
    static QString preferenceGroup ()
    {
        return tr ("Global");
    }

private:
    void createControls ();

    void setLayouts ();

    void setConnects ();
};

