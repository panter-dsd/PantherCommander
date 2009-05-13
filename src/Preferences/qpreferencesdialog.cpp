#include <QtGui>
#include "qpreferencesdialog.h"
//
QPreferencesDialog::QPreferencesDialog(QSettings* qsSettings,QWidget * parent, Qt::WFlags f)
	: QDialog(parent,f)
{
	this->resize(640,480);
	this->setWindowTitle(tr("Preferences"));
	qsetAppSettings=qsSettings;
	createControls();
	setLayouts();
	setConnects();
	qdbbButtons->button(QDialogButtonBox::Apply)->setEnabled(false);
}
//
QPreferencesDialog::~QPreferencesDialog()
{

}
//
void QPreferencesDialog::createControls()
{
	qlwPreferencesList=new QListWidget(this);
	QStringList qslPreferenceItems;
	qslPreferenceItems << QPreferenceGlobal::preferenceGroup();
	qlwPreferencesList->addItems(qslPreferenceItems);
	qlwPreferencesList->setCurrentRow(0);
	setMaximumSizePreferencesList();

	qswPreferencesWidgets=new QStackedWidget();
	qswPreferencesWidgets->addWidget(new QPreferenceGlobal(qsetAppSettings,qswPreferencesWidgets));

	qdbbButtons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Apply|QDialogButtonBox::Cancel,
				Qt::Horizontal,
				this);

	qpbSetDefaults=new QPushButton(tr("Default"),qdbbButtons);
	qdbbButtons->addButton(qpbSetDefaults,QDialogButtonBox::ApplyRole);
}
//
void QPreferencesDialog::setLayouts()
{
	QWidget* qwPreferencesWidget=new QWidget();
	QHBoxLayout* qhblPreferencesLayout=new QHBoxLayout();
	qhblPreferencesLayout->addWidget(qlwPreferencesList);
	qhblPreferencesLayout->addWidget(qswPreferencesWidgets);
	qwPreferencesWidget->setLayout(qhblPreferencesLayout);

	QVBoxLayout* qvblMainLayout=new QVBoxLayout();
	qvblMainLayout->addWidget(qwPreferencesWidget);
	qvblMainLayout->addWidget(qdbbButtons);
	this->setLayout(qvblMainLayout);
}
//
void QPreferencesDialog::setConnects()
{
	connect(qdbbButtons,
				SIGNAL(accepted()),
				this,
				SLOT(slotSavePreferencesAndExit()));
	connect(qdbbButtons,
				SIGNAL(rejected()),
				this,
				SLOT(reject()));
	connect(qdbbButtons->button(QDialogButtonBox::Apply),
				SIGNAL(clicked()),
				this,
				SLOT(slotSavePreferences()));
	connect(qpbSetDefaults,
			SIGNAL(clicked()),
			this,
			SLOT(slotSetDefaults()));
	connect(qlwPreferencesList,
				SIGNAL(currentRowChanged (int)),
				qswPreferencesWidgets,
				SLOT(setCurrentIndex(int)));
	for (int i=0; i<qswPreferencesWidgets->count(); i++)
	{
		QAbstractPreferencesPage* widget=qobject_cast<QAbstractPreferencesPage*>(qswPreferencesWidgets->widget(i));
		if (widget)
			connect(widget,
						SIGNAL(modified()),
						this,
						SLOT(slotSetApplyEnabled()));
	}
}
//
void QPreferencesDialog::setMaximumSizePreferencesList()
{
	qlwPreferencesList->setMaximumWidth(50);
	for (int i=0; i<qlwPreferencesList->count(); i++)
	{
		int iWidth=QFontMetrics(qlwPreferencesList->font()).width(qlwPreferencesList->item(i)->text())+50;
		if (qlwPreferencesList->maximumWidth()<iWidth)
		{
			qlwPreferencesList->setMaximumWidth(iWidth);
		}
	}
}
//
void QPreferencesDialog::slotSavePreferences()
{
	for (int i=0; i<qswPreferencesWidgets->count(); i++)
	{
		QAbstractPreferencesPage* widget=qobject_cast<QAbstractPreferencesPage*>(qswPreferencesWidgets->widget(i));
		if (widget)
			widget->saveSettings();
	}
	qsetAppSettings->sync();
	qdbbButtons->button(QDialogButtonBox::Apply)->setEnabled(false);
}
//
void QPreferencesDialog::slotSetApplyEnabled()
{
	qdbbButtons->button(QDialogButtonBox::Apply)->setEnabled(true);
}
//
void QPreferencesDialog::slotSetDefaults()
{
	for (int i=0; i<qswPreferencesWidgets->count(); i++)
	{
		QAbstractPreferencesPage* widget=qobject_cast<QAbstractPreferencesPage*>(qswPreferencesWidgets->widget(i));
		if (widget)
			widget->setDefaults();
	}
}
//
