#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>

class AppSettings : public QSettings
{
//	Q_OBJECT
public:
	static AppSettings *getInstance();
	virtual ~AppSettings();
	void setSettings(QSettings *settings) {appSettings = settings;}
	QSettings *settings() {return appSettings;}
	void settingsChange();

private:
	AppSettings ();
	static AppSettings *instance;

private:
	QSettings		*appSettings;

signals:
	void settingsChanged();
};

#endif // APPSETTINGS_H
