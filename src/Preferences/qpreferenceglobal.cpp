#include "qpreferenceglobal.h"

#include <QtGui>
//
QPreferenceGlobal::QPreferenceGlobal(QWidget* parent)
	: QAbstractPreferencesPage(parent)
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
