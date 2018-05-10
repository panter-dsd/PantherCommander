#pragma once

class PCCommandsPreference;

class QDialogButtonBox;

#include <QDialog>

class PCCommandsDialog : public QDialog
{
Q_OBJECT

private:
    PCCommandsPreference *commandReference;
    QDialogButtonBox *qdbbButtons;

public:
    PCCommandsDialog (QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint);

    ~PCCommandsDialog ();

    QAction *getCurrentAction ();

    QString getCurrentActionName ();

    void loadSettings ();

    void saveSetings ();

private Q_SLOTS:

    void saveAndAccept ();
};

