#pragma once

class QLabel;

class QLineEdit;

class QToolButton;

class QPushButton;

class QGroupBox;

class QDialogButtonBox;

class QHBoxLayout;

class QVBoxLayout;

class PCShortcutEdit;

class PCShortcutWidget;

#include <QDialog>

class PCCommandEditDialog : public QDialog
{
Q_OBJECT

public:
    PCCommandEditDialog (QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint);

    virtual ~PCCommandEditDialog ()
    {
    }

    void setCommandObjectName (const QString &objectName);

    void setCommandText (const QString &text);

    QString commandText ();

    void setCommandToolTip (const QString &text);

    QString commandToolTip ();

    void setCommandShortcuts (QList<QKeySequence> shortcuts);

    QList<QKeySequence> commandShortcuts ();

private Q_SLOTS:

    void addShortcut ();

    void removeShortcut ();

private:
    QGroupBox *mainBox_;
    QLabel *objectNameLabel_;
    QLineEdit *objectNameEdit_;
    QLabel *textLabel_;
    QLineEdit *textEdit_;
    QLabel *toolTipLabel_;
    QLineEdit *toolTipEdit_;
    QGroupBox *shortcutBox_;
    QList<PCShortcutWidget *> shortcutWidgets_;
    QPushButton *addShortcutButton_;
    QPushButton *removeShortcutButton_;
    QHBoxLayout *buttonsLayout_;
    QDialogButtonBox *buttons_;

    QVBoxLayout *shortcutsLayout_;
};
