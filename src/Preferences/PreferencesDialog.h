#pragma once

class QListWidget;

class QStackedWidget;

class QDialogButtonBox;

class QPushButton;

#include <QtWidgets/QDialog>

class PreferencesDialog : public QDialog
{
Q_OBJECT

public:
    explicit PreferencesDialog (QWidget *parent = nullptr,
                                Qt::WindowFlags f = Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint);

    virtual ~PreferencesDialog ();

private:
    void createControls ();

    void setLayouts ();

    void setConnects ();

    void setMaximumSizePreferencesList ();

    void loadSettings ();

    void saveSettings ();

private Q_SLOTS:

    void slotSavePreferences ();

    void slotSavePreferencesAndExit ();

    void slotSetApplyEnabled ();

    void slotSetDefaults ();

private:
    QListWidget *preferencesList_;
    QStackedWidget *preferencesWidgets_;
    QDialogButtonBox *buttons_;
    QPushButton *setDefaultsButton_;
};

