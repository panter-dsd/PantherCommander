#include <QtWidgets/QAction>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>

#include "PCCommandsDialog.h"
#include "AppSettings.h"
#include "PCCommandsPreference.h"

PCCommandsDialog::PCCommandsDialog (QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    commandReference = new PCCommandsPreference (this);
    connect (commandReference, SIGNAL(itemActivated ()),
             this, SLOT(accept ()));

    qdbbButtons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                        Qt::Horizontal,
                                        this
    );
    connect (qdbbButtons, SIGNAL(accepted ()),
             this, SLOT(saveAndAccept ()));
    connect (qdbbButtons, SIGNAL(rejected ()),
             this, SLOT(reject ()));

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (commandReference);
    qvblMainLayout->addWidget (qdbbButtons);
    this->setLayout (qvblMainLayout);

    loadSettings ();
}

PCCommandsDialog::~PCCommandsDialog ()
{
    saveSetings ();
}

QAction *PCCommandsDialog::getCurrentAction ()
{
    return commandReference->getCurrentAction ();
}

QString PCCommandsDialog::getCurrentActionName ()
{
    return commandReference->getCurrentActionName ();
}

void PCCommandsDialog::loadSettings ()
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

void PCCommandsDialog::saveSetings ()
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

void PCCommandsDialog::saveAndAccept ()
{
    commandReference->saveSettings ();
}
