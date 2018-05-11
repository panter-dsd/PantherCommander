#include "CommandEditDialog.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>

#include "src/widgets/PCShortcutEdit.h"

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

CommandEditDialog::CommandEditDialog (QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    mainBox_ = new QGroupBox (this);

    objectNameLabel_ = new QLabel (tr ("Command name"), this);

    objectNameEdit_ = new QLineEdit (this);
    objectNameEdit_->setEnabled (false);

    textLabel_ = new QLabel (tr ("Command text"), this);

    textEdit_ = new QLineEdit (this);

    toolTipLabel_ = new QLabel (tr ("Command tooltip"), this);

    toolTipEdit_ = new QLineEdit (this);

    shortcutBox_ = new QGroupBox (tr ("Shortcuts"), this);

    addShortcutButton_ = new QPushButton (tr ("Add shortcut"), this);
    connect (addShortcutButton_, SIGNAL(clicked ()),
             this, SLOT(addShortcut ()));

    removeShortcutButton_ = new QPushButton (tr ("Remove shortcut"), this);
    connect (removeShortcutButton_, SIGNAL(clicked ()),
             this, SLOT(removeShortcut ()));

    buttons_ = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                     Qt::Horizontal, this
    );

    connect (buttons_, SIGNAL(accepted ()), this, SLOT(accept ()));
    connect (buttons_, SIGNAL(rejected ()), this, SLOT(reject ()));

    buttonsLayout_ = new QHBoxLayout ();
    buttonsLayout_->addWidget (addShortcutButton_);
    buttonsLayout_->addWidget (removeShortcutButton_);

    shortcutsLayout_ = new QVBoxLayout;
    shortcutsLayout_->addLayout (buttonsLayout_);
    shortcutBox_->setLayout (shortcutsLayout_);

    QVBoxLayout *qglFirstLayout = new QVBoxLayout ();
    qglFirstLayout->addWidget (objectNameLabel_);
    qglFirstLayout->addWidget (objectNameEdit_);
    qglFirstLayout->addWidget (textLabel_);
    qglFirstLayout->addWidget (textEdit_);
    qglFirstLayout->addWidget (toolTipLabel_);
    qglFirstLayout->addWidget (toolTipEdit_);
    mainBox_->setLayout (qglFirstLayout);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->setSizeConstraint (QLayout::SetFixedSize);
    qvblMainLayout->addWidget (mainBox_);
    qvblMainLayout->addWidget (shortcutBox_);
    qvblMainLayout->addWidget (buttons_);
    setLayout (qvblMainLayout);
}

void CommandEditDialog::setCommandObjectName (const QString &objectName)
{
    objectNameEdit_->setText (objectName);
}

void CommandEditDialog::setCommandText (const QString &text)
{
    textEdit_->setText (text);
}

QString CommandEditDialog::commandText ()
{
    return textEdit_->text ();
}

void CommandEditDialog::setCommandToolTip (const QString &text)
{
    toolTipEdit_->setText (text);
}

QString CommandEditDialog::commandToolTip ()
{
    return toolTipEdit_->text ();
}

void CommandEditDialog::setCommandShortcuts (QList<QKeySequence> shortcuts)
{
    qDeleteAll (shortcutWidgets_);
    shortcutWidgets_.clear ();

        foreach(const QKeySequence &ks, shortcuts) {
            PCShortcutWidget *shortcut = new PCShortcutWidget (this);
            shortcut->setShortcut (ks);
            shortcutWidgets_.append (shortcut);
            shortcutsLayout_->insertWidget (shortcutsLayout_->count () - 1, shortcut);
        }

    if (shortcutWidgets_.isEmpty ()) {
        PCShortcutWidget *shortcut = new PCShortcutWidget (this);
        shortcutWidgets_.append (shortcut);
        shortcutsLayout_->insertWidget (shortcutsLayout_->count () - 1, shortcut);
    }
}

QList<QKeySequence> CommandEditDialog::commandShortcuts ()
{
    QList<QKeySequence> l;
        foreach(PCShortcutWidget *shortcut, shortcutWidgets_) {
            QKeySequence ks = shortcut->shortcut ();
            if (!ks.isEmpty ()) {
                l << ks;
            }
        }
    return l;
}

void CommandEditDialog::addShortcut ()
{
    PCShortcutWidget *shortcut = new PCShortcutWidget (this);
    shortcutWidgets_.append (shortcut);
    shortcutsLayout_->insertWidget (shortcutsLayout_->count () - 1, shortcut);
}

void CommandEditDialog::removeShortcut ()
{
    if (shortcutWidgets_.count () == 1) {
        shortcutWidgets_.first ()->clear ();
        return;
    }

    delete shortcutWidgets_.takeLast ();
}
