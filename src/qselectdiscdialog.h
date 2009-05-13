#ifndef QSELECTDISCDIALOG_H
#define QSELECTDISCDIALOG_H
//
class QListWidget;
class QListWidgetItem;
//
#include <QDialog>
#include "qfileoperationsthread.h"
//
class QSelectDiscDialog : public QDialog
{
Q_OBJECT
private:
	QListWidget*			qlwDiscList;
public:
	QSelectDiscDialog(QWidget* parent=0);
	QString discName();
};

#endif // QSELECTDISCDIALOG_H
