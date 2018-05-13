#pragma once

#include <QtWidgets/QFrame>

#include <memory>

class DriveBar : public QFrame
{
Q_OBJECT

public:
    explicit DriveBar (QWidget *parent = nullptr);

    virtual ~DriveBar ();

public Q_SLOTS:

    void slotSetDisc (const QString &path);

Q_SIGNALS:

    void discChanged (const QString &disc);

private Q_SLOTS:

    void actionTriggered (QAction *action);

    void showContextMenu (const QPoint &position);

private:
    void loadDrivesList ();

    void refresh ();

private:
    class QAction *lastCheckedAction_;

    class QActionGroup *actionGroup_;

    class VolumeInfoProvider *volumeInfoProvider_;

    std::unique_ptr<class QFileIconProvider> iconProvider_;
};

