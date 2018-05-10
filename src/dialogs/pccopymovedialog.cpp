#include "pccopymovedialog.h"

#include <QtCore/QDir>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QStandardItemModel>

const QString oneFileString = "%1 \"%2\" to";
const QString manyFileString = "%1 \"%2\" file(s) to";

PCCopyMoveDialog::PCCopyMoveDialog (QWidget *parent)
    : QDialog (parent)
{
    createControls ();
    setLayouts ();
}

void PCCopyMoveDialog::createControls ()
{
    qlSources = new QLabel (this);
    qlSources->setWordWrap (true);

    qleDest = new QLineEdit (this);

    qlQueue = new QLabel (tr ("Queue"), this);

    qcbQueue = new QComboBox (this);

    qdbbButtons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                        Qt::Horizontal,
                                        this
    );
    connect (qdbbButtons,
             SIGNAL(accepted ()),
             this,
             SLOT(accept ()));
    connect (qdbbButtons,
             SIGNAL(rejected ()),
             this,
             SLOT(reject ()));
}

void PCCopyMoveDialog::setLayouts ()
{
    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (qlSources);
    qvblMainLayout->addWidget (qleDest);
    qvblMainLayout->addWidget (qlQueue);
    qvblMainLayout->addWidget (qcbQueue);
    qvblMainLayout->addWidget (qdbbButtons);

    this->setLayout (qvblMainLayout);
}

void PCCopyMoveDialog::setSource (const QStringList &fileList)
{
    qslSource = fileList;
    updateText ();
}

void PCCopyMoveDialog::setDest (const QString &dest)
{
    qsDest = dest;
    if (!qsDest.endsWith (QLatin1Char ('/'))) {
        qsDest += QLatin1String ("/");
    }
    updateText ();
}

QString PCCopyMoveDialog::dest ()
{
    return QDir::fromNativeSeparators (qleDest->text ());
}

void PCCopyMoveDialog::setOperation (const QString &operation)
{
    qsOperation = operation;
    updateText ();
}

void PCCopyMoveDialog::updateText ()
{
    if (qslSource.size () == 1) {
        qlSources->setText (oneFileString.arg (qsOperation).arg (QDir::toNativeSeparators (qslSource.at (0))));
        QFileInfo info (qslSource.at (0));
        if (info.isFile ()) {
            qleDest->setText (QDir::toNativeSeparators (qsDest + QFileInfo (qslSource.at (0)).fileName ()));
        } else {
            qleDest->setText (QDir::toNativeSeparators (qsDest));
        }
    } else {
        qlSources->setText (manyFileString.arg (qsOperation).arg (qslSource.size ()));
        qleDest->setText (QDir::toNativeSeparators (qsDest));
    }

    this->setWindowTitle (qsOperation);
}

void PCCopyMoveDialog::setQueueModel (QStandardItemModel *model)
{
    qcbQueue->setModel (model);
    qcbQueue->setModelColumn (0);
    qcbQueue->setCurrentIndex (-1);
}

int PCCopyMoveDialog::queueIndex () const
{
    return qcbQueue->currentIndex ();
}

