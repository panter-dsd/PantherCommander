#include <QtWidgets/QAction>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QHeaderView>

#include "src/Commands.h"
#include "src/dialogs/CommandEditDialog.h"

#include "CommandsPreference.h"

const QString allCategoryName = QObject::tr ("All");

CommandsPreference::CommandsPreference (QWidget *parent)
    : AbstractPreferencesPage (parent)
{
    commandsGroupBox_ = new QGroupBox (tr ("Commands"), this);

    categoryList_ = new QListWidget (this);
    connect (categoryList_, &QListWidget::currentTextChanged, this, &CommandsPreference::loadActions);

    actionsTable_ = new QTableWidget (this);

    actionsTable_->setColumnCount (COLUMN_COUNT);
    const QStringList header {tr ("Command"), tr ("Name"), tr ("Shortcut"), tr ("ToolTip")};
    actionsTable_->setHorizontalHeaderLabels (header);

    actionsTable_->setSelectionBehavior (QAbstractItemView::SelectRows);
    actionsTable_->setHorizontalScrollMode (QAbstractItemView::ScrollPerPixel);
    actionsTable_->verticalHeader ()->hide ();
    actionsTable_->setContextMenuPolicy (Qt::ActionsContextMenu);
    connect (actionsTable_, &QTableWidget::activated, this, &CommandsPreference::itemActivated);

    QHBoxLayout *qhblCommandsLayout = new QHBoxLayout ();
    qhblCommandsLayout->addWidget (categoryList_);
    qhblCommandsLayout->addWidget (actionsTable_);
    commandsGroupBox_->setLayout (qhblCommandsLayout);

    filterGroupBox_ = new QGroupBox (tr ("Filter"), this);

    filterEdit_ = new QLineEdit (this);
    connect (filterEdit_, &QLineEdit::textChanged, this, &CommandsPreference::filterChange);

    clearFilterButton_ = new QToolButton (this);
    connect (clearFilterButton_, &QToolButton::clicked, filterEdit_, &QLineEdit::clear);

    QHBoxLayout *qhblFilterLayout = new QHBoxLayout ();
    qhblFilterLayout->addWidget (filterEdit_);
    qhblFilterLayout->addWidget (clearFilterButton_);
    filterGroupBox_->setLayout (qhblFilterLayout);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (filterGroupBox_);
    qvblMainLayout->addWidget (commandsGroupBox_);

    setLayout (qvblMainLayout);

    loadSettings ();

    editCommandAction_ = new QAction (tr ("Edit command"), this);
    connect (editCommandAction_, &QAction::triggered, this, &CommandsPreference::editCommand);
    actionsTable_->addAction (editCommandAction_);
}

CommandsPreference::~CommandsPreference ()
{
}

void CommandsPreference::saveSettings ()
{
    for (QAction *newAction : actions_.values ()) {
        const QString name = newAction->objectName ();
        QAction *action = Commands::instance ()->action (name);
        action->setText (newAction->text ());
        action->setToolTip (newAction->toolTip ());
        action->setShortcuts (newAction->shortcuts ());
        Commands::instance ()->saveAction (name);
    }
}

void CommandsPreference::loadSettings ()
{
    loadCategories ();
}

void CommandsPreference::setDefaults ()
{

}

void CommandsPreference::loadCategories ()
{
    categoryList_->addItems (Commands::instance ()->categories ());
    categoryList_->insertItem (0, allCategoryName);
    setMaximumSizeCategoriesList ();
}

void CommandsPreference::setMaximumSizeCategoriesList ()
{
    categoryList_->setMaximumWidth (40);
    for (int i = 0; i < categoryList_->count (); i++) {
        int iWidth = QFontMetrics (categoryList_->font ()).width (categoryList_->item (i)->text ()) + 40;
        if (categoryList_->maximumWidth () < iWidth) {
            categoryList_->setMaximumWidth (iWidth);
        }
    }
}

void CommandsPreference::loadActions (const QString &category)
{
    actionsTable_->setRowCount (0);
    const QList<QAction *> &actions = Commands::instance ()->actions (
        (category == allCategoryName) ? QString () : category
    );
    actionsTable_->setRowCount (actions.count ());

    int i = 0;
    for (QAction *action: actions) {
        if (actions_.contains (action->objectName ())) {
            action = actions_.value (action->objectName ());
        }
        QTableWidgetItem *item = new QTableWidgetItem (action->objectName ());
        item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        actionsTable_->setItem (i, COMMAND, item);

        item = new QTableWidgetItem (action->text ());
        item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        actionsTable_->setItem (i, NAME, item);

        item = new QTableWidgetItem (action->toolTip ());
        item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        actionsTable_->setItem (i, TOOLTTIP, item);

        QStringList qslShortcuts;
            foreach(const QKeySequence &ks, action->shortcuts ()) {
                qslShortcuts << ks.toString ();
            }
        item = new QTableWidgetItem (qslShortcuts.join (QLatin1String ("\n")));
        item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        actionsTable_->setItem (i++, SHORTCUT, item);
    }
    emit filterChange (filterEdit_->text ());
    actionsTable_->resizeRowsToContents ();
    actionsTable_->resizeColumnsToContents ();
}

void CommandsPreference::filterChange (const QString &filter)
{
    for (int i = 0; i < actionsTable_->rowCount (); i++) {
        actionsTable_->showRow (i);
        bool isInFilter = false;
        for (int j = 0; j < actionsTable_->columnCount (); j++) {
            if (actionsTable_->item (i, j)->text ().contains (filter, Qt::CaseInsensitive)) {
                isInFilter = true;
                break;
            }
        }
        if (!isInFilter) {
            actionsTable_->hideRow (i);
        }
    }
}

void CommandsPreference::editCommand ()
{
    CommandEditDialog dialog (this);

    QString actionName = actionsTable_->item (actionsTable_->currentRow (), COMMAND)->text ();
    QAction *action = Commands::instance ()->action (actionName);
    if (actions_.contains (action->objectName ())) {
        action = actions_.value (action->objectName ());
    }

    dialog.setCommandObjectName (actionName);
    dialog.setCommandText (action->text ());
    dialog.setCommandToolTip (action->toolTip ());
    dialog.setCommandShortcuts (action->shortcuts ());

    if (dialog.exec ()) {
        QList<QKeySequence> l = dialog.commandShortcuts ();
        QAction *editAction = new QAction (action->parent ());
        editAction->setObjectName (action->objectName ());
        editAction->setText (dialog.commandText ());
        editAction->setToolTip (dialog.commandToolTip ());
        editAction->setShortcuts (l);
        actions_.remove (editAction->objectName ());
        actions_.insert (editAction->objectName (), editAction);
        loadActions (categoryList_->currentItem ()->text ());
        emit modified ();
    }
}

QAction *CommandsPreference::getCurrentAction ()
{
    QTableWidgetItem *item = actionsTable_->item (actionsTable_->currentRow (), COMMAND);
    return item ? Commands::instance ()->action (item->text ()) : 0;
}

QString CommandsPreference::getCurrentActionName ()
{
    QAction *action = getCurrentAction ();
    return action ? action->objectName () : QString ();
}

QString CommandsPreference::preferenceGroup ()
{
    return tr ("Commands");
}

