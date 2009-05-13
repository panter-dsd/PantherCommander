#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QVBoxLayout>
#include "qselectdiscdialog.h"

QSelectDiscDialog::QSelectDiscDialog(QWidget* parent)
	:QDialog(parent)
{
	//this->resize(this->width(),200);
	qlwDiscList=new QListWidget(this);
	qlwDiscList->addItems(QFileOperationsThread::getDrivesList());
	qlwDiscList->setCurrentRow(0);
	connect(qlwDiscList,
		SIGNAL(itemActivated(QListWidgetItem*)),
		this,
		SLOT(accept()));
	connect(qlwDiscList,
		SIGNAL(itemClicked(QListWidgetItem*)),
		this,
		SLOT(accept()));

	QVBoxLayout* layout=new QVBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(qlwDiscList);
	this->setLayout(layout);
}
//
QString QSelectDiscDialog::discName()
{
	return qlwDiscList->currentItem()->text();
}
//

