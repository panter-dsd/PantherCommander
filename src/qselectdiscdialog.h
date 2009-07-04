#ifndef QSELECTDISCDIALOG_H
#define QSELECTDISCDIALOG_H

#include <QtGui/QDialog>

class QListWidget;

class QSelectDiscDialog : public QDialog
{
	Q_OBJECT

public:
	QSelectDiscDialog(QWidget* parent = 0);

	QString discName() const;

private:
	QListWidget* qlwDiscList;
};

#endif // QSELECTDISCDIALOG_H
