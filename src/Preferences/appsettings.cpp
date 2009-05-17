#include "appsettings.h"
//
AppSettings *AppSettings::instance = 0;
//
AppSettings *AppSettings::getInstance()
{
	if (instance == 0)
		instance = new AppSettings();
	return instance;
}
//
AppSettings::AppSettings()
{
}
//
AppSettings::~AppSettings()
{
	if (this == instance)
		instance = 0;
}
