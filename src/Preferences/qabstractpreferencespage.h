#ifndef QABSTRACTPREFERENCESPAGE_H
#define QABSTRACTPREFERENCESPAGE_H

#include <QtGui/QWidget>
#include <QtCore/QSettings>

class QSettings;
class QString;

class QAbstractPreferencesPage : public QWidget
{
	Q_OBJECT

public:
	QAbstractPreferencesPage(QSettings* qsetSettings, QWidget* parent = 0);

	virtual void saveSettings() = 0;
	virtual void loadSettings() = 0;
	virtual void setDefaults() = 0;

	//virtual QString preferenceGroup() const = 0;
	static QString preferenceGroup()
	{ return QString(); }

Q_SIGNALS:
	void modified();

protected:
	QSettings* qsetAppSettings;
};

#endif // QABSTRACTPREFERENCESPAGE_H
