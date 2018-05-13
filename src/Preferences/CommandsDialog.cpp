#include <QtWidgets/QAction>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>

#include "src/AppSettings.h"
#include "CommandsPreference.h"

#include "CommandsDialog.h"

CommandsDialog::CommandsDialog (QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
    , commandReference_ (new CommandsPreference (this))
    , buttonBox_ (new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this))
{
    connect (commandReference_, &CommandsPreference::itemActivated, this, &CommandsDialog::accept);

    connect (buttonBox_, &QDialogButtonBox::accepted, this, &CommandsDialog::saveAndAccept);
    connect (buttonBox_, &QDialogButtonBox::rejected, this, &CommandsDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout ();
    layout->addWidget (commandReference_);
    layout->addWidget (buttonBox_);
    this->setLayout (layout);

    loadSettings ();
}

CommandsDialog::~CommandsDialog ()
{
    saveSetings ();
}

QAction *CommandsDialog::getCurrentAction ()
{
    return commandReference_->getCurrentAction ();
}

QString CommandsDialog::getCurrentActionName ()
{
    return commandReference_->getCurrentActionName ();
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
    commandReference_->saveSettings ();
}
