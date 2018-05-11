#include <QtWidgets/QAction>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QHeaderView>

#include "PCCommandsPreference.h"
#include "src/PCCommands.h"
#include "src/dialogs/CommandEditDialog.h"

const QString allCategoryName = QObject::tr ("All");

PCCommandsPreference::PCCommandsPreference (QWidget *parent)
    : QAbstractPreferencesPage (parent)
{
    qgbCommands = new QGroupBox (tr ("Commands"), this);

    qlwCategoryList = new QListWidget (this);
    connect (qlwCategoryList, SIGNAL(currentTextChanged (QString)),
             this, SLOT(loadActions (QString)));

    qtwActionsTable = new QTableWidget (this);

    qtwActionsTable->setColumnCount (COLUMN_COUNT);
    QStringList header;
    header << tr ("Command")
           << tr ("Name")
           << tr ("Shortcut")
           << tr ("ToolTip");
    qtwActionsTable->setHorizontalHeaderLabels (header);

    qtwActionsTable->setSelectionBehavior (QAbstractItemView::SelectRows);
    qtwActionsTable->setHorizontalScrollMode (QAbstractItemView::ScrollPerPixel);
    qtwActionsTable->verticalHeader ()->hide ();
    qtwActionsTable->setContextMenuPolicy (Qt::ActionsContextMenu);
    connect (qtwActionsTable, SIGNAL(activated (QModelIndex)),
             this, SIGNAL(itemActivated ()));

    QHBoxLayout *qhblCommandsLayout = new QHBoxLayout ();
    qhblCommandsLayout->addWidget (qlwCategoryList);
    qhblCommandsLayout->addWidget (qtwActionsTable);
    qgbCommands->setLayout (qhblCommandsLayout);

    qgbFilter = new QGroupBox (tr ("Filter"), this);

    qleFilter = new QLineEdit (this);
    connect (qleFilter, SIGNAL(textChanged (QString)),
             this, SLOT(filterChange (QString)));

    qtbClearFilter = new QToolButton (this);
    connect (qtbClearFilter, SIGNAL(clicked ()),
             qleFilter, SLOT(clear ()));

    QHBoxLayout *qhblFilterLayout = new QHBoxLayout ();
    qhblFilterLayout->addWidget (qleFilter);
    qhblFilterLayout->addWidget (qtbClearFilter);
    qgbFilter->setLayout (qhblFilterLayout);

    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (qgbFilter);
    qvblMainLayout->addWidget (qgbCommands);

    this->setLayout (qvblMainLayout);

    loadSettings ();

    qaEditCommand = new QAction (tr ("Edit command"), this);
    connect (qaEditCommand, SIGNAL(triggered ()),
             this, SLOT(editCommand ()));
    qtwActionsTable->addAction (qaEditCommand);
}

void PCCommandsPreference::saveSettings ()
{
        foreach(QAction *newAction, editingActions.values ()) {
            const QString name = newAction->objectName ();
            QAction *action = PCCommands::instance ()->action (name);
            action->setText (newAction->text ());
            action->setToolTip (newAction->toolTip ());
            action->setShortcuts (newAction->shortcuts ());
            PCCommands::instance ()->saveAction (name);
        }
}

void PCCommandsPreference::loadSettings ()
{
    loadCategories ();
}

void PCCommandsPreference::setDefaults ()
{

}

void PCCommandsPreference::loadCategories ()
{
    qlwCategoryList->addItems (PCCommands::instance ()->categories ());
    qlwCategoryList->insertItem (0, allCategoryName);
    setMaximumSizeCategoriesList ();
}

void PCCommandsPreference::setMaximumSizeCategoriesList ()
{
    qlwCategoryList->setMaximumWidth (40);
    for (int i = 0; i < qlwCategoryList->count (); i++) {
        int iWidth = QFontMetrics (qlwCategoryList->font ()).width (qlwCategoryList->item (i)->text ()) + 40;
        if (qlwCategoryList->maximumWidth () < iWidth) {
            qlwCategoryList->setMaximumWidth (iWidth);
        }
    }
}

void PCCommandsPreference::loadActions (const QString &category)
{
    QString qsCategory = (category == allCategoryName) ? QString () : category;
    QTableWidgetItem *item;

    qtwActionsTable->setRowCount (0);
    QList<QAction *> l = PCCommands::instance ()->actions (qsCategory);
    qtwActionsTable->setRowCount (l.count ());

    int i = 0;
        foreach(QAction *action, l) {
            if (editingActions.contains (action->objectName ())) {
                action = editingActions.value (action->objectName ());
            }
            item = new QTableWidgetItem (action->objectName ());
            item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            qtwActionsTable->setItem (i, COMMAND, item);

            item = new QTableWidgetItem (action->text ());
            item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            qtwActionsTable->setItem (i, NAME, item);

            item = new QTableWidgetItem (action->toolTip ());
            item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            qtwActionsTable->setItem (i, TOOLTTIP, item);

            QStringList qslShortcuts;
                foreach(const QKeySequence &ks, action->shortcuts ()) {
                    qslShortcuts << ks.toString ();
                }
            item = new QTableWidgetItem (qslShortcuts.join (QLatin1String ("\n")));
            item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            qtwActionsTable->setItem (i++, SHORTCUT, item);
        }
    emit filterChange (qleFilter->text ());
    qtwActionsTable->resizeRowsToContents ();
    qtwActionsTable->resizeColumnsToContents ();
//	qtwActionsTable->resizeRowsToContents();
}

void PCCommandsPreference::filterChange (const QString &filter)
{
    for (int i = 0; i < qtwActionsTable->rowCount (); i++) {
        qtwActionsTable->showRow (i);
        bool isInFilter = false;
        for (int j = 0; j < qtwActionsTable->columnCount (); j++) {
            if (qtwActionsTable->item (i, j)->text ().contains (filter, Qt::CaseInsensitive)) {
                isInFilter = true;
                break;
            }
        }
        if (!isInFilter) {
            qtwActionsTable->hideRow (i);
        }
    }
}

void PCCommandsPreference::editCommand ()
{
    CommandEditDialog *dialog = new CommandEditDialog (this);

    QString actionName = qtwActionsTable->item (qtwActionsTable->currentRow (), COMMAND)->text ();
    QAction *action = PCCommands::instance ()->action (actionName);
    if (editingActions.contains (action->objectName ())) {
        action = editingActions.value (action->objectName ());
    }

    dialog->setCommandObjectName (actionName);
    dialog->setCommandText (action->text ());
    dialog->setCommandToolTip (action->toolTip ());
    dialog->setCommandShortcuts (action->shortcuts ());

    if (dialog->exec ()) {
        QList<QKeySequence> l = dialog->commandShortcuts ();
        QAction *editAction = new QAction (action->parent ());
        editAction->setObjectName (action->objectName ());
        editAction->setText (dialog->commandText ());
        editAction->setToolTip (dialog->commandToolTip ());
        editAction->setShortcuts (l);
        editingActions.remove (editAction->objectName ());
        editingActions.insert (editAction->objectName (), editAction);
        loadActions (qlwCategoryList->currentItem ()->text ());
        emit modified ();
    }
    delete dialog;
}

QAction *PCCommandsPreference::getCurrentAction ()
{
    QTableWidgetItem *item;
    item = qtwActionsTable->item (qtwActionsTable->currentRow (), COMMAND);
    return item ? PCCommands::instance ()->action (item->text ()) : 0;
}

QString PCCommandsPreference::getCurrentActionName ()
{
    QAction *action = getCurrentAction ();
    return action ? action->objectName () : QString ();
}
