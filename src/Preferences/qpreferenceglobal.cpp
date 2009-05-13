#include "qpreferenceglobal.h"

#include <QtGui>
//
QPreferenceGlobal::QPreferenceGlobal(QSettings* qsetSettings, QWidget* parent)
	: QAbstractPreferencesPage(qsetSettings, parent)
{
	createControls();
	setConnects();
	setLayouts();
	loadSettings();
}
//
QPreferenceGlobal::~QPreferenceGlobal()
{
}
//
void QPreferenceGlobal::saveSettings()
{
}
//
void QPreferenceGlobal::loadSettings()
{
}
//
void QPreferenceGlobal::setDefaults()
{
}
//
void QPreferenceGlobal::createControls()
{
}
//
void QPreferenceGlobal::setLayouts()
{
}
//
void QPreferenceGlobal::setConnects()
{
}
//
