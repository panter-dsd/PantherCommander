#include "qabstractpreferencespage.h"

QAbstractPreferencesPage::QAbstractPreferencesPage(QWidget* parent)
	: QWidget(parent)
{
	appSettings=AppSettings::getInstance();
}
