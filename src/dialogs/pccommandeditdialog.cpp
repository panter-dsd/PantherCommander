#include "pccommandeditdialog.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>

#include "pcshortcutedit.h"

class PCShortcutWidget : public QWidget
{
public:
    PCShortcutWidget (QWidget *parent);

    void setShortcut (const QKeySequence &ks);

    QKeySequence shortcut ();

    void clear ();

private:
    PCShortcutEdit *qcseEdit;
    QToolButton *qtbDefault;
    QToolButton *qtbClear;
};

PCShortcutWidget::PCShortcutWidget (QWidget *parent)
    : QWidget (parent)
{
    qcseEdit = new PCShortcutEdit (this);

    qtbDefault = new QToolButton (this);

    qtbClear = new QToolButton (this);
    connect (qtbClear, SIGNAL(clicked ()), qcseEdit, SLOT(clear ()));

    QHBoxLayout *qhblMainLayout = new QHBoxLayout ();
    qhblMainLayout->addWidget (qcseEdit, 10);
    qhblMainLayout->addWidget (qtbDefault, 0);
    qhblMainLayout->addWidget (qtbClear, 0);
    setLayout (qhblMainLayout);
}

void PCShortcutWidget::setShortcut (const QKeySequence &ks)
{
    qcseEdit->setShortcut (ks);
}

QKeySequence PCShortcutWidget::shortcut ()
{
    return qcseEdit->shortcut ();
}

void PCShortcutWidget::clear ()
{
    qcseEdit->clear ();
}

PCCommandEditDialog::PCCommandEditDialog (QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    qgbMainBox = new QGroupBox (this);

    qlObjectName = new QLabel (tr ("Command name"), this);

    qleObjectName = new QLineEdit (this);
    qleObjectName->setEnabled (false);

    qlText = new QLabel (tr ("Command text"), this);

    qleText = new QLineEdit (this);

    qlToolTip = new QLabel (tr ("Command tooltip"), this);

    qleToolTip = new QLineEdit (this);

    qgbShortcutBox = new QGroupBox (tr ("Shortcuts"), this);

    qpbAddShortcut = new QPushButton (tr ("Add shortcut"), this);
    connect (qpbAddShortcut, SIGNAL(clicked ()),
             this, SLOT(addShortcut ()));

    qpbRemoveShortcut = new QPushButton (tr ("Remove shortcut"), this);
    connect (qpbRemoveShortcut, SIGNAL(clicked ()),
             this, SLOT(removeShortcut ()));

    qdbbButtons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                        Qt::Horizontal, this
    );

    connect (qdbbButtons, SIGNAL(accepted ()), this, SLOT(accept ()));
    connect (qdbbButtons, SIGNAL(rejected ()), this, SLOT(reject ()));

    qhblButtonsLayout = new QHBoxLayout ();
    qhblButtonsLayout->addWidget (qpbAddShortcut);
    qhblButtonsLayout->addWidget (qpbRemoveShortcut);

    m_shortcutsLayout = new QVBoxLayout;
    m_shortcutsLayout->addLayout (qhblButtonsLayout);
    qgbShortcutBox->setLayout (m_shortcutsLayout);

    QVBoxLayout *qglFirstLayout = new QVBoxLayout ();
    qglFirstLayout->addWidget (qlObjectName);
    qglFirstLayout->addWidget (qleObjectName);
    qglFirstLayout->addWidget (qlText);
    qglFirstLayout->addWidget (qleText);
    qglFirstLayout->addWidget (qlToolTip);
    qglFirstLayout->addWidget (qleToolTip);
    qgbMainBox->setLayout (qglFirstLayout);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->setSizeConstraint (QLayout::SetFixedSize);
    qvblMainLayout->addWidget (qgbMainBox);
    qvblMainLayout->addWidget (qgbShortcutBox);
    qvblMainLayout->addWidget (qdbbButtons);
    setLayout (qvblMainLayout);
}

void PCCommandEditDialog::setCommandObjectName (const QString &objectName)
{
    qleObjectName->setText (objectName);
}

void PCCommandEditDialog::setCommandText (const QString &text)
{
    qleText->setText (text);
}

QString PCCommandEditDialog::commandText ()
{
    return qleText->text ();
}

void PCCommandEditDialog::setCommandToolTip (const QString &text)
{
    qleToolTip->setText (text);
}

QString PCCommandEditDialog::commandToolTip ()
{
    return qleToolTip->text ();
}

void PCCommandEditDialog::setCommandShortcuts (QList<QKeySequence> shortcuts)
{
    qDeleteAll (qlShortcutWidgets);
    qlShortcutWidgets.clear ();

        foreach(const QKeySequence &ks, shortcuts) {
            PCShortcutWidget *shortcut = new PCShortcutWidget (this);
            shortcut->setShortcut (ks);
            qlShortcutWidgets.append (shortcut);
            m_shortcutsLayout->insertWidget (m_shortcutsLayout->count () - 1, shortcut);
        }

    if (qlShortcutWidgets.isEmpty ()) {
        PCShortcutWidget *shortcut = new PCShortcutWidget (this);
        qlShortcutWidgets.append (shortcut);
        m_shortcutsLayout->insertWidget (m_shortcutsLayout->count () - 1, shortcut);
    }
}

QList<QKeySequence> PCCommandEditDialog::commandShortcuts ()
{
    QList<QKeySequence> l;
        foreach(PCShortcutWidget *shortcut, qlShortcutWidgets) {
            QKeySequence ks = shortcut->shortcut ();
            if (!ks.isEmpty ()) {
                l << ks;
            }
        }
    return l;
}

void PCCommandEditDialog::addShortcut ()
{
    PCShortcutWidget *shortcut = new PCShortcutWidget (this);
    qlShortcutWidgets.append (shortcut);
    m_shortcutsLayout->insertWidget (m_shortcutsLayout->count () - 1, shortcut);
}

void PCCommandEditDialog::removeShortcut ()
{
    if (qlShortcutWidgets.count () == 1) {
        qlShortcutWidgets.first ()->clear ();
        return;
    }

    delete qlShortcutWidgets.takeLast ();
}
