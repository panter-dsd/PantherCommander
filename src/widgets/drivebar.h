#ifndef DRIVEBAR_H
#define DRIVEBAR_H

class QAction;

class QActionGroup;

class VolumeInfoProvider;

class QFileIconProvider;

#include <QtWidgets/QFrame>

class DriveBar : public QFrame
{
Q_OBJECT

private:
    QAction *lastChecked;
    QActionGroup *actionGroup;
    VolumeInfoProvider *provider;
    QFileIconProvider *iconProvider;

public:
    explicit DriveBar (QWidget *parent = 0);

    virtual ~DriveBar ();

private:
    void loadDrivesList ();

public Q_SLOTS:

    void slotSetDisc (const QString &path);

Q_SIGNALS:

    void discChanged (const QString &disc);

private Q_SLOTS:

    void _q_actionTriggered (QAction *action);

    void _q_showContextMenu (const QPoint &position);

    void volumeAdd (const QString &);

    void volumeRemove (const QString &);
};

#endif // DRIVEBAR_H
