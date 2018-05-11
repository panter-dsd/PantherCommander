#pragma once

class CommandsPreference;

class QDialogButtonBox;

#include <QDialog>

class CommandsDialog : public QDialog
{
Q_OBJECT

private:
    CommandsPreference *commandReference;
    QDialogButtonBox *qdbbButtons;

public:
    CommandsDialog (QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint);

    ~CommandsDialog ();

    QAction *getCurrentAction ();

    QString getCurrentActionName ();

    void loadSettings ();

    void saveSetings ();

private Q_SLOTS:

    void saveAndAccept ();
};

