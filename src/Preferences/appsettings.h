#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QtCore/QSettings>

class AppSettings : public QSettings
{
	Q_OBJECT

public:
	static AppSettings *instance();
	virtual ~AppSettings();

	void settingsChange();

protected:
	AppSettings();

signals:
	void settingsChanged();

private:
	static AppSettings *pInstance;
};

#endif // APPSETTINGS_H
