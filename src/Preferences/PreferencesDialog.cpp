#include <QtWidgets/QListWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QHBoxLayout>

#include "PreferencesDialog.h"

#include "AbstractPreferencesPage.h"
#include "GlobalPreferencePage.h"
#include "InterfacePreference.h"
#include "CommandsPreference.h"
#include "src/AppSettings.h"

PreferencesDialog::PreferencesDialog (QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    setWindowTitle (tr ("Preferences"));
    createControls ();
    setLayouts ();
    setConnects ();
    loadSettings ();
}

//
PreferencesDialog::~PreferencesDialog ()
{
    saveSattings ();
}

//
void PreferencesDialog::createControls ()
{
    qlwPreferencesList = new QListWidget (this);
    QStringList qslPreferenceItems;
    qslPreferenceItems << GlobalPreferencePage::preferenceGroup ()
                       << InterfacePreference::preferenceGroup ()
                       << CommandsPreference::preferenceGroup ();
    qlwPreferencesList->addItems (qslPreferenceItems);
    qlwPreferencesList->setCurrentRow (0);
    setMaximumSizePreferencesList ();

    QScrollArea *area;
    qswPreferencesWidgets = new QStackedWidget ();

    area = new QScrollArea (this);
    area->setWidgetResizable (true);
    area->setWidget (new GlobalPreferencePage (qswPreferencesWidgets));
    qswPreferencesWidgets->addWidget (area);

    area = new QScrollArea (this);
    area->setWidgetResizable (true);
    area->setWidget (new InterfacePreference (qswPreferencesWidgets));
    qswPreferencesWidgets->addWidget (area);

    area = new QScrollArea (this);
    area->setWidgetResizable (true);
    area->setWidget (new CommandsPreference (qswPreferencesWidgets));
    qswPreferencesWidgets->addWidget (area);

    qdbbButtons = new QDialogButtonBox (QDialogButtonBox::Ok
                                        | QDialogButtonBox::Apply
                                        | QDialogButtonBox::Cancel,
                                        Qt::Horizontal,
                                        this
    );

    qpbSetDefaults = new QPushButton (tr ("Default"), qdbbButtons);
    qdbbButtons->addButton (qpbSetDefaults, QDialogButtonBox::ApplyRole);
    qdbbButtons->button (QDialogButtonBox::Apply)->setEnabled (false);
}

//
void PreferencesDialog::setLayouts ()
{
    QHBoxLayout *qhblPreferencesLayout = new QHBoxLayout ();
    qhblPreferencesLayout->addWidget (qlwPreferencesList);
    qhblPreferencesLayout->addWidget (qswPreferencesWidgets);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addLayout (qhblPreferencesLayout);
    qvblMainLayout->addWidget (qdbbButtons);

    setLayout (qvblMainLayout);
}

//
void PreferencesDialog::setConnects ()
{
    connect (qdbbButtons, SIGNAL(accepted ()),
             this, SLOT(slotSavePreferencesAndExit ()));
    connect (qdbbButtons, SIGNAL(rejected ()),
             this, SLOT(reject ()));
    connect (qdbbButtons->button (QDialogButtonBox::Apply), SIGNAL(clicked ()),
             this, SLOT(slotSavePreferences ()));
    connect (qpbSetDefaults, SIGNAL(clicked ()),
             this, SLOT(slotSetDefaults ()));

    connect (qlwPreferencesList, SIGNAL(currentRowChanged (int)),
             qswPreferencesWidgets, SLOT(setCurrentIndex (int)));

    for (int i = 0; i < qswPreferencesWidgets->count (); i++) {
        QScrollArea *area = qobject_cast<QScrollArea *> (qswPreferencesWidgets->widget (i));
        if (!area) {
            continue;
        }
        AbstractPreferencesPage *widget = qobject_cast<AbstractPreferencesPage *> (area->widget ());
        if (widget) {
            connect (widget, SIGNAL(modified ()), this, SLOT(slotSetApplyEnabled ()));
        }
    }
}

//
void PreferencesDialog::setMaximumSizePreferencesList ()
{
    qlwPreferencesList->setMaximumWidth (50);
    for (int i = 0; i < qlwPreferencesList->count (); i++) {
        int iWidth = QFontMetrics (qlwPreferencesList->font ()).width (qlwPreferencesList->item (i)->text ()) + 50;
        if (qlwPreferencesList->maximumWidth () < iWidth) {
            qlwPreferencesList->setMaximumWidth (iWidth);
        }
    }
}

//
void PreferencesDialog::slotSavePreferences ()
{
    for (int i = 0; i < qswPreferencesWidgets->count (); i++) {
        QScrollArea *area = qobject_cast<QScrollArea *> (qswPreferencesWidgets->widget (i));
        if (!area) {
            continue;
        }
        AbstractPreferencesPage *widget = qobject_cast<AbstractPreferencesPage *> (area->widget ());
        if (widget) {
            widget->saveSettings ();
        }
    }
    qdbbButtons->button (QDialogButtonBox::Apply)->setEnabled (false);
}

//
void PreferencesDialog::slotSetApplyEnabled ()
{
    qdbbButtons->button (QDialogButtonBox::Apply)->setEnabled (true);
}

//
void PreferencesDialog::slotSetDefaults ()
{
    for (int i = 0; i < qswPreferencesWidgets->count (); i++) {
        QScrollArea *area = qobject_cast<QScrollArea *> (qswPreferencesWidgets->widget (i));
        if (!area) {
            continue;
        }
        AbstractPreferencesPage *widget = qobject_cast<AbstractPreferencesPage *> (area->widget ());
        if (widget) {
            widget->setDefaults ();
        }
    }
}
//

void PreferencesDialog::loadSettings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("PreferencesDialog");
    move (settings->value ("pos", QPoint (0, 0)).toPoint ());
    resize (settings->value ("size", QSize (640, 480)).toSize ());
    if (settings->value ("IsMaximized", false).toBool ()) {
        showMaximized ();
    }
    settings->endGroup ();
}

void PreferencesDialog::saveSattings ()
{
    QSettings *settings = AppSettings::instance ();
    settings->beginGroup ("PreferencesDialog");
    settings->setValue ("IsMaximized", isMaximized ());
    if (!isMaximized ()) {
        settings->setValue ("pos", pos ());
        settings->setValue ("size", size ());
    }
    settings->endGroup ();
    settings->sync ();
}

