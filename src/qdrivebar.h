#ifndef QDRIVEBAR_H
#define QDRIVEBAR_H
//
#include <QFrame>
#include "appsettings.h"
//
class QActionGroup;
//
class QDriveBar : public QFrame
{
Q_OBJECT
private:
	QAction*					qaLastChecked;
	QActionGroup*		qagDrives;
	QString					qsCurrentPath;
	int				timerID;
	AppSettings					*appSettings;
public:
	QDriveBar(QWidget* parent = 0);

private slots:
	void slotDiscChanged();

protected:
	void timerEvent(QTimerEvent *event);

public slots:
	void slotRefresh();
	void slotSetDisc(const QString& path);

signals:
	void discChanged(const QString& disc);
};

#endif // QDRIVEBAR_H
