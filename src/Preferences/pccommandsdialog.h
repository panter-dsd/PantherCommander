#ifndef PCCOMMANDSDIALOG_H
#define PCCOMMANDSDIALOG_H

class QListWidget;
class QTableWidget;
class QDialogButtonBox;
class QAction;

#include <QDialog>

class PCCommandsDialog : public QDialog
{
	Q_OBJECT

private:
	QListWidget *qlwCategoryList;
	QTableWidget *qtwActionsTable;
	QDialogButtonBox *qdbbButtons;

public:
	PCCommandsDialog(QWidget* parent = 0);
	QAction* getCurrentAction();
	QString getCurrentActionName();

private:
	void createControls();
	void setLayouts();
	void loadCategories();

private Q_SLOTS:
	void loadActions(const QString &category = 0);
};

#endif // PCCOMMANDSDIALOG_H
