#ifndef QPREFERENCESDIALOG_H
#define QPREFERENCESDIALOG_H
//
class QListWidget;
class QStackedWidget;
class QDialogButtonBox;
class QPushButton;
//
#include <QDialog>
//
#include "qabstractpreferencespage.h"
//
class QPreferencesDialog : public QDialog
{
Q_OBJECT
private:
	QListWidget*					qlwPreferencesList;
	QStackedWidget*			qswPreferencesWidgets;
	QDialogButtonBox*		qdbbButtons;
	QPushButton*					qpbSetDefaults;
private:
	void createControls();
	void setLayouts();
	void setConnects();
	void setMaximumSizePreferencesList();
public:
	QPreferencesDialog(QWidget * parent = 0, Qt::WFlags f = Qt::WindowSystemMenuHint);
	~QPreferencesDialog();
private Q_SLOTS:
	void slotSavePreferences();
	void slotSavePreferencesAndExit() {slotSavePreferences(); close();}
	void slotSetApplyEnabled();
	void slotSetDefaults();
};
#endif
