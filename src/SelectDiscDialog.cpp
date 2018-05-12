#include "SelectDiscDialog.h"

#include <QtCore/QDir>

#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>

#include "io/VolumeInfoProvider.h"

SelectDiscDialog::SelectDiscDialog (QWidget *parent)
    : QDialog (parent)
{
    discList = new QListWidget (this);

    QListWidgetItem *item;
    for (const QStorageInfo &info : VolumeInfoProvider ().volumes ()) {
        item = new QListWidgetItem (QDir::toNativeSeparators (info.rootPath ()), discList);
        item->setData (Qt::EditRole, info.rootPath ());
        discList->addItem (item);
    }

    discList->setCurrentRow (0);

    connect (discList, SIGNAL(itemActivated (QListWidgetItem * )),
             this, SLOT(selectDisc (QListWidgetItem * )));
    connect (discList, SIGNAL(itemClicked (QListWidgetItem * )),
             this, SLOT(selectDisc (QListWidgetItem * )));

    QVBoxLayout *layout = new QVBoxLayout ();
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (discList);
    setLayout (layout);

    discList->setFocus ();
}

void SelectDiscDialog::selectDisc (QListWidgetItem *item)
{
    emit setectedDisc (item->text ());
    accept ();
}

void SelectDiscDialog::setPath (const QString &path)
{
    for (int i = 0; i < discList->count (); i++) {
        if (path.startsWith (discList->item (i)->data (Qt::EditRole).toString ())) {
            discList->setCurrentItem (discList->item (i));
            break;
        }
    }
}
