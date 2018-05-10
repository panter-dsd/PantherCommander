#include "AppSettings.h"

#include <QtCore/QCoreApplication>

AppSettings *AppSettings::instance_ = 0;

AppSettings *AppSettings::instance ()
{
    if (!AppSettings::instance_) {
        instance_ = new AppSettings (qApp);
    }
    return instance_;
}

AppSettings::AppSettings (QObject *parent)
    : QSettings (parent)
{
}

AppSettings::~AppSettings ()
{
    if (instance_ == this) {
        instance_ = 0;
    }
}

void AppSettings::settingsChange ()
{
}

bool AppSettings::useNativeDialogs ()
{
    return value ("Global/UseNativeDialogs", true).toBool ();
}
