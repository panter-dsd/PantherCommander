#pragma once

#include <QtWidgets/QDialog>

class CommandsDialog : public QDialog
{
Q_OBJECT

public:
    CommandsDialog (QWidget *parent = nullptr,
                    Qt::WindowFlags f = Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint);

    virtual ~CommandsDialog ();

    QAction *getCurrentAction ();

    QString getCurrentActionName ();

    void loadSettings ();

    void saveSetings ();

private Q_SLOTS:

    void saveAndAccept ();

private:
    class CommandsPreference *commandReference_;

    class QDialogButtonBox *buttonBox_;
};

