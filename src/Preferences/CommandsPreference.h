#pragma once

class QListWidget;

class QTableWidget;

class QDialogButtonBox;

class QAction;

class QGroupBox;

class QLabel;

class QLineEdit;

class QToolButton;

class QTableWidgetItem;

#include <QtCore/QMap>

#include "AbstractPreferencesPage.h"

class CommandsPreference : public AbstractPreferencesPage
{
Q_OBJECT
    enum Columns
    {
        COMMAND = 0,
        NAME,
        SHORTCUT,
        TOOLTTIP,
        COLUMN_COUNT
    };

private:
    QGroupBox *qgbCommands;
    QListWidget *qlwCategoryList;
    QTableWidget *qtwActionsTable;
    QGroupBox *qgbFilter;
    QLineEdit *qleFilter;
    QToolButton *qtbClearFilter;

    QAction *qaEditCommand;

    QMap<QString, QAction *> editingActions;
public:
    CommandsPreference (QWidget *parent = 0);

    virtual ~CommandsPreference ()
    {
    }

    void saveSettings ();

    void loadSettings ();

    void setDefaults ();

    static QString preferenceGroup ()
    {
        return tr ("Commands");
    }

    QAction *getCurrentAction ();

    QString getCurrentActionName ();

private:
    void loadCategories ();

    void setMaximumSizeCategoriesList ();

private Q_SLOTS:

    void loadActions (const QString &category = 0);

    void filterChange (const QString &filter);

    void editCommand ();

Q_SIGNALS:

    void itemActivated ();
};

