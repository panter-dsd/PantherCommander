#include "appsettings.h"

#include <QtCore/QCoreApplication>

AppSettings* AppSettings::pInstance = 0;

AppSettings* AppSettings::instance()
{
	if(!pInstance)
		pInstance = new AppSettings(qApp);
	return pInstance;
}

AppSettings::AppSettings(QObject* parent) : QSettings(parent)
{
}

AppSettings::~AppSettings()
{
	if(pInstance == this)
		pInstance = 0;
}

void AppSettings::settingsChange()
{
}
