#pragma once

class PCCommandsPreference;

class QDialogButtonBox;

#include <QDialog>

class CommandsDialog : public QDialog
{
Q_OBJECT

private:
    PCCommandsPreference *commandReference;
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

