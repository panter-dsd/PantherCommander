#include <QtWidgets/QListWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QHBoxLayout>

#include "AbstractPreferencesPage.h"
#include "GlobalPreferencePage.h"
#include "InterfacePreference.h"
#include "CommandsPreference.h"
#include "src/AppSettings.h"

#include "PreferencesDialog.h"

PreferencesDialog::PreferencesDialog (QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    setWindowTitle (tr ("Preferences"));
    preferencesList_ = new QListWidget (this);
    QStringList qslPreferenceItems;
    qslPreferenceItems << GlobalPreferencePage::preferenceGroup ()
                       << InterfacePreference::preferenceGroup ()
                       << CommandsPreference::preferenceGroup ();
    preferencesList_->addItems (qslPreferenceItems);
    preferencesList_->setCurrentRow (0);
    setMaximumSizePreferencesList ();

    QScrollArea *area;
    preferencesWidgets_ = new QStackedWidget ();

    area = new QScrollArea (this);
    area->setWidgetResizable (true);
    area->setWidget (new GlobalPreferencePage (preferencesWidgets_));
    preferencesWidgets_->addWidget (area);

    area = new QScrollArea (this);
    area->setWidgetResizable (true);
    area->setWidget (new InterfacePreference (preferencesWidgets_));
    preferencesWidgets_->addWidget (area);

    area = new QScrollArea (this);
    area->setWidgetResizable (true);
    area->setWidget (new CommandsPreference (preferencesWidgets_));
    preferencesWidgets_->addWidget (area);

    buttons_ = new QDialogButtonBox (QDialogButtonBox::Ok
                                     | QDialogButtonBox::Apply
                                     | QDialogButtonBox::Cancel,
                                     Qt::Horizontal,
                                     this
    );

    setDefaultsButton_ = new QPushButton (tr ("Default"), buttons_);
    buttons_->addButton (setDefaultsButton_, QDialogButtonBox::ApplyRole);
    buttons_->button (QDialogButtonBox::Apply)->setEnabled (false);

    QHBoxLayout *preferencesLayout = new QHBoxLayout ();
    preferencesLayout->addWidget (preferencesList_);
    preferencesLayout->addWidget (preferencesWidgets_);

    QVBoxLayout *mainLayout = new QVBoxLayout ();
    mainLayout->addLayout (preferencesLayout);
    mainLayout->addWidget (buttons_);
    setLayout (mainLayout);

    connect (buttons_, &QDialogButtonBox::accepted, this, &PreferencesDialog::slotSavePreferencesAndExit);
    connect (buttons_, &QDialogButtonBox::rejected, this, &PreferencesDialog::reject);
    connect (buttons_->button (QDialogButtonBox::Apply), &QPushButton::clicked,
             this, &PreferencesDialog::slotSavePreferences
    );
    connect (setDefaultsButton_, &QPushButton::clicked, this, &PreferencesDialog::slotSetDefaults);

    connect (preferencesList_, &QListWidget::currentRowChanged,
             preferencesWidgets_, &QStackedWidget::setCurrentIndex
    );

    for (int i = 0; i < preferencesWidgets_->count (); i++) {
        QScrollArea *area = qobject_cast<QScrollArea *> (preferencesWidgets_->widget (i));
        if (!area) {
            continue;
        }
        AbstractPreferencesPage *widget = qobject_cast<AbstractPreferencesPage *> (area->widget ());
        if (widget) {
            connect (widget, &AbstractPreferencesPage::modified, this, &PreferencesDialog::slotSetApplyEnabled);
        }
    }
    loadSettings ();
}

PreferencesDialog::~PreferencesDialog ()
{
    saveSettings ();
}

void PreferencesDialog::setMaximumSizePreferencesList ()
{
    preferencesList_->setMaximumWidth (50);
    for (int i = 0; i < preferencesList_->count (); i++) {
        int iWidth = QFontMetrics (preferencesList_->font ()).width (preferencesList_->item (i)->text ()) + 50;
        if (preferencesList_->maximumWidth () < iWidth) {
            preferencesList_->setMaximumWidth (iWidth);
        }
    }
}

void PreferencesDialog::slotSavePreferences ()
{
    for (int i = 0; i < preferencesWidgets_->count (); i++) {
        QScrollArea *area = qobject_cast<QScrollArea *> (preferencesWidgets_->widget (i));
        if (!area) {
            continue;
        }
        AbstractPreferencesPage *widget = qobject_cast<AbstractPreferencesPage *> (area->widget ());
        if (widget) {
            widget->saveSettings ();
        }
    }
    buttons_->button (QDialogButtonBox::Apply)->setEnabled (false);
}

void PreferencesDialog::slotSetApplyEnabled ()
{
    buttons_->button (QDialogButtonBox::Apply)->setEnabled (true);
}

void PreferencesDialog::slotSetDefaults ()
{
    for (int i = 0; i < preferencesWidgets_->count (); i++) {
        QScrollArea *area = qobject_cast<QScrollArea *> (preferencesWidgets_->widget (i));
        if (!area) {
            continue;
        }
        AbstractPreferencesPage *widget = qobject_cast<AbstractPreferencesPage *> (area->widget ());
        if (widget) {
            widget->setDefaults ();
        }
    }
}

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

void PreferencesDialog::saveSettings ()
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

void PreferencesDialog::slotSavePreferencesAndExit ()
{
    slotSavePreferences ();
    close ();
}

