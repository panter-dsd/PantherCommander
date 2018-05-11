#pragma once

#include <QtCore/QSettings>

#include <QtWidgets/QWidget>

class QString;

class AbstractPreferencesPage : public QWidget
{
Q_OBJECT

public:
    AbstractPreferencesPage (QWidget *parent = 0);

    virtual void saveSettings () = 0;

    virtual void loadSettings () = 0;

    virtual void setDefaults () = 0;

    //virtual QString preferenceGroup() const = 0;
    static QString preferenceGroup ()
    {
        return QString ();
    }

Q_SIGNALS:

    void modified ();
};

