#include "qselectdiscdialog.h"

#include <QtCore/QDir>

#include <QtGui/QLayout>
#include <QtGui/QListWidget>

#include "qfileoperationsthread.h"

QSelectDiscDialog::QSelectDiscDialog(QWidget* parent) : QDialog(parent)
{
	//this->resize(this->width(),200);
	qlwDiscList=new QListWidget(this);
	foreach(const QFileInfo& fi, QFileOperationsThread::volumes())
	{
		QString path = fi.absoluteFilePath();
		qlwDiscList->addItem(QDir::toNativeSeparators(path));
	}
	qlwDiscList->setCurrentRow(0);
	connect(qlwDiscList, SIGNAL(itemActivated(QListWidgetItem*)),
			this, SLOT(accept()));
	connect(qlwDiscList, SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(accept()));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(qlwDiscList);
	setLayout(layout);
}

QString QSelectDiscDialog::discName() const
{
	return qlwDiscList->currentItem()->text();
}
