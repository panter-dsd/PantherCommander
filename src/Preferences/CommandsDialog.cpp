#include <QtWidgets/QAction>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>

#include "CommandsDialog.h"
#include "src/AppSettings.h"
#include "CommandsPreference.h"

CommandsDialog::CommandsDialog (QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    commandReference = new CommandsPreference (this);
    connect (commandReference, &CommandsPreference::itemActivated, this, &CommandsDialog::accept);

    qdbbButtons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                        Qt::Horizontal,
                                        this
    );
    connect (qdbbButtons, &QDialogButtonBox::accepted, this, &CommandsDialog::saveAndAccept);
    connect (qdbbButtons, &QDialogButtonBox::rejected, this, &CommandsDialog::reject);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (commandReference);
    qvblMainLayout->addWidget (qdbbButtons);
    this->setLayout (qvblMainLayout);

    loadSettings ();
}

CommandsDialog::~CommandsDialog ()
{
    saveSetings ();
}

QAction *CommandsDialog::getCurrentAction ()
{
    return commandReference->getCurrentAction ();
}

QString CommandsDialog::getCurrentActionName ()
{
    return commandReference->getCurrentActionName ();
}

void CommandsDialog::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("CommandsDialog");
    move (settings->value ("pos", QPoint (0, 0)).toPoint ());
    resize (settings->value ("size", QSize (640, 480)).toSize ());
    if (settings->value ("IsMaximized", false).toBool ()) {
        showMaximized ();
    }

    settings->endGroup ();
}

void CommandsDialog::saveSetings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("CommandsDialog");
    settings->setValue ("IsMaximized", isMaximized ());
    if (!isMaximized ()) {
        settings->setValue ("pos", pos ());
        settings->setValue ("size", size ());
    }

    settings->endGroup ();
    settings->sync ();
}

void CommandsDialog::saveAndAccept ()
{
    commandReference->saveSettings ();
}
