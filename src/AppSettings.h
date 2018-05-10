#pragma once

#include <QtCore/QSettings>

class AppSettings : public QSettings
{
Q_OBJECT

public:
    static AppSettings *instance ();

    virtual ~AppSettings ();

    void settingsChange ();

    bool useNativeDialogs ();

protected:
    explicit AppSettings (QObject *parent = 0);

Q_SIGNALS:

    void settingsChanged ();

private:
    static AppSettings *instance_;
};

