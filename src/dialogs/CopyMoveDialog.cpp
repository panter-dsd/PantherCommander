#include "CopyMoveDialog.h"

#include <QtCore/QDir>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QStandardItemModel>

const QString oneFileString = "%1 \"%2\" to";
const QString manyFileString = "%1 \"%2\" file(s) to";

CopyMoveDialog::CopyMoveDialog (QWidget *parent)
    : QDialog (parent)
{
    createControls ();
    setLayouts ();
}

void CopyMoveDialog::createControls ()
{
    sourcesLabel_ = new QLabel (this);
    sourcesLabel_->setWordWrap (true);

    destEdit_ = new QLineEdit (this);

    queueLabel_ = new QLabel (tr ("Queue"), this);

    queueComboBox_ = new QComboBox (this);

    buttons_ = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                     Qt::Horizontal,
                                     this
    );
    connect (buttons_, &QDialogButtonBox::accepted, this, &CopyMoveDialog::accept);
    connect (buttons_, &QDialogButtonBox::rejected, this, &CopyMoveDialog::reject);
}

void CopyMoveDialog::setLayouts ()
{
    QVBoxLayout *qvblMainLayout = new QVBoxLayout ();
    qvblMainLayout->addWidget (sourcesLabel_);
    qvblMainLayout->addWidget (destEdit_);
    qvblMainLayout->addWidget (queueLabel_);
    qvblMainLayout->addWidget (queueComboBox_);
    qvblMainLayout->addWidget (buttons_);

    this->setLayout (qvblMainLayout);
}

void CopyMoveDialog::setSource (const QStringList &fileList)
{
    source_ = fileList;
    updateText ();
}

void CopyMoveDialog::setDest (const QString &dest)
{
    dest_ = dest;
    if (!dest_.endsWith (QLatin1Char ('/'))) {
        dest_ += QLatin1String ("/");
    }
    updateText ();
}

QString CopyMoveDialog::dest ()
{
    return QDir::fromNativeSeparators (destEdit_->text ());
}

void CopyMoveDialog::setOperation (const QString &operation)
{
    operation_ = operation;
    updateText ();
}

void CopyMoveDialog::updateText ()
{
    if (source_.size () == 1) {
        sourcesLabel_->setText (oneFileString.arg (operation_).arg (QDir::toNativeSeparators (source_.at (0))));
        QFileInfo info (source_.at (0));
        if (info.isFile ()) {
            destEdit_->setText (QDir::toNativeSeparators (dest_ + QFileInfo (source_.at (0)).fileName ()));
        } else {
            destEdit_->setText (QDir::toNativeSeparators (dest_));
        }
    } else {
        sourcesLabel_->setText (manyFileString.arg (operation_).arg (source_.size ()));
        destEdit_->setText (QDir::toNativeSeparators (dest_));
    }

    this->setWindowTitle (operation_);
}

void CopyMoveDialog::setQueueModel (QStandardItemModel *model)
{
    queueComboBox_->setModel (model);
    queueComboBox_->setModelColumn (0);
    queueComboBox_->setCurrentIndex (-1);
}

int CopyMoveDialog::queueIndex () const
{
    return queueComboBox_->currentIndex ();
}

CopyMoveDialog::~CopyMoveDialog ()
{
}

