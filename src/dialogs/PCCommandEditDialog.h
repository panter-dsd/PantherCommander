#ifndef PCCOMMANDEDITDIALOG_H
#define PCCOMMANDEDITDIALOG_H

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
private:
    QGroupBox *qgbMainBox;
    QLabel *qlObjectName;
    QLineEdit *qleObjectName;
    QLabel *qlText;
    QLineEdit *qleText;
    QLabel *qlToolTip;
    QLineEdit *qleToolTip;
    QGroupBox *qgbShortcutBox;
    QList<PCShortcutWidget *> qlShortcutWidgets;
    QPushButton *qpbAddShortcut;
    QPushButton *qpbRemoveShortcut;
    QHBoxLayout *qhblButtonsLayout;
    QDialogButtonBox *qdbbButtons;

    QVBoxLayout *m_shortcutsLayout;

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
};

#endif // PCCOMMANDEDITDIALOG_H
