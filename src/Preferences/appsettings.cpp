#include "appsettings.h"

AppSettings *AppSettings::pInstance = 0;

AppSettings *AppSettings::instance()
{
	if (!pInstance)
		pInstance = new AppSettings();
	return pInstance;
}

AppSettings::AppSettings()
{
}

AppSettings::~AppSettings()
{
	if (this == pInstance)
		pInstance = 0;
}

void AppSettings::settingsChange()
{
}
