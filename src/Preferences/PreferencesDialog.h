#pragma once

class QListWidget;

class QStackedWidget;

class QDialogButtonBox;

class QPushButton;

#include <QDialog>

class PreferencesDialog : public QDialog
{
Q_OBJECT

private:
    QListWidget *qlwPreferencesList;
    QStackedWidget *qswPreferencesWidgets;
    QDialogButtonBox *qdbbButtons;
    QPushButton *qpbSetDefaults;

private:
    void createControls ();

    void setLayouts ();

    void setConnects ();

    void setMaximumSizePreferencesList ();

    void loadSettings ();

    void saveSattings ();

public:
    PreferencesDialog (QWidget *parent = 0,
                        Qt::WindowFlags f = Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint);

    ~PreferencesDialog ();

private Q_SLOTS:

    void slotSavePreferences ();

    void slotSavePreferencesAndExit ()
    {
        slotSavePreferences ();
        close ();
    }

    void slotSetApplyEnabled ();

    void slotSetDefaults ();
};

