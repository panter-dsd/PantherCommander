#pragma once

class QGroupBox;

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

public:
    explicit CommandsPreference (QWidget *parent = nullptr);

    virtual ~CommandsPreference ();

    void saveSettings ();

    void loadSettings ();

    void setDefaults ();

    static QString preferenceGroup ();

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

private:
    QGroupBox *commandsGroupBox_;
    class QListWidget *categoryList_;
    class QTableWidget *actionsTable_;
    QGroupBox *filterGroupBox_;
    class QLineEdit *filterEdit_;
    class QToolButton *clearFilterButton_;

    QAction *editCommandAction_;

    QMap<QString, QAction *> actions_;
};

