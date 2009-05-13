#include "qabstractpreferencespage.h"

QAbstractPreferencesPage::QAbstractPreferencesPage(QSettings* qsetSettings, QWidget* parent) 
	: QWidget(parent), qsetAppSettings(qsetSettings)
{
}
