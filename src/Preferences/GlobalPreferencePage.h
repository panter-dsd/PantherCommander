#pragma once

#include "AbstractPreferencesPage.h"

class GlobalPreferencePage : public AbstractPreferencesPage
{
Q_OBJECT
public:
    explicit GlobalPreferencePage (QWidget *parent = nullptr);

    virtual ~GlobalPreferencePage ();

    void saveSettings ();

    void loadSettings ();

    void setDefaults ();

    static QString preferenceGroup ();

private:

    void setConnects ();

private:
    class QCheckBox *useNativeDialogs_;
};

