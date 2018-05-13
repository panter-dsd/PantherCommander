#include "AbstractPreferencesPage.h"

AbstractPreferencesPage::AbstractPreferencesPage (QWidget *parent)
    : QWidget (parent)
{
}

AbstractPreferencesPage::~AbstractPreferencesPage ()
{

}

QString AbstractPreferencesPage::preferenceGroup ()
{
    return QString ();
}
