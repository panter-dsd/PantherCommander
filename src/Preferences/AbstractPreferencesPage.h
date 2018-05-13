#pragma once

#include <QtWidgets/QWidget>

class AbstractPreferencesPage : public QWidget
{
Q_OBJECT

public:
    explicit AbstractPreferencesPage (QWidget *parent = nullptr);

    virtual ~AbstractPreferencesPage ();

    virtual void saveSettings () = 0;

    virtual void loadSettings () = 0;

    virtual void setDefaults () = 0;

    static QString preferenceGroup ();

Q_SIGNALS:

    void modified ();
};

