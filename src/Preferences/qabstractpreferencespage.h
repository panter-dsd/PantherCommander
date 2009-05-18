#ifndef QABSTRACTPREFERENCESPAGE_H
#define QABSTRACTPREFERENCESPAGE_H

#include <QtCore/QSettings>

#include <QtGui/QWidget>

class QString;

class QAbstractPreferencesPage : public QWidget
{
	Q_OBJECT

public:
	QAbstractPreferencesPage(QWidget* parent = 0);

	virtual void saveSettings() = 0;
	virtual void loadSettings() = 0;
	virtual void setDefaults() = 0;

	//virtual QString preferenceGroup() const = 0;
	static QString preferenceGroup()
	{ return QString(); }

Q_SIGNALS:
	void modified();
};

#endif // QABSTRACTPREFERENCESPAGE_H
