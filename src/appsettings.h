#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QtCore/QSettings>

class AppSettings : public QSettings
{
	Q_OBJECT

public:
	static AppSettings* instance();
	virtual ~AppSettings();

	void settingsChange();

protected:
	explicit AppSettings(QObject* parent = 0);

signals:
	void settingsChanged();

private:
	static AppSettings* pInstance;
};

#endif // APPSETTINGS_H
