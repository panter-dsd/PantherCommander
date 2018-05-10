#ifndef PCCOMMANDSPREFERENCE_H
#define PCCOMMANDSPREFERENCE_H

class QListWidget;

class QTableWidget;

class QDialogButtonBox;

class QAction;

class QGroupBox;

class QLabel;

class QLineEdit;

class QToolButton;

class QTableWidgetItem;

#include "qabstractpreferencespage.h"

class PCCommandsPreference : public QAbstractPreferencesPage
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
    PCCommandsPreference (QWidget *parent = 0);

    virtual ~PCCommandsPreference ()
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

#endif // PCCommandsPreference_H
