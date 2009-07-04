#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QFileIconProvider>
#include <QtGui/QToolButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMessageBox>
#include <QtCore/QTextStream>
#include "qdrivebar.h"
#include "flowlayout.h"
#include "appsettings.h"
#include "qfileoperationsthread.h"

#define TIMER_INTERVAL 2500

static bool isDrive(const QString& path)
{
	int length = path.length();
	return ((length == 1 && path.at(0) == QLatin1Char('/'))
			|| (length >= 2 && length <= 3 && path.at(0).isLetter() && path.at(1) == QLatin1Char(':')));
}

QDriveBar::QDriveBar(QWidget* parent) : QFrame(parent)
{
	qaLastChecked = 0;
	lastDrivesCount = 0;
	qagDrives=new QActionGroup(this);

	slotRefresh();
	timerID=this->startTimer(TIMER_INTERVAL);
}
//
void QDriveBar::slotRefresh()
{
	blockSignals(true);
	QSettings* settings = AppSettings::instance();
	QStringList qslIgnoreList = settings->value("Global/IgnoredDrives").toStringList();
	QList<QAction*> qalDrives=qagDrives->actions();
	QFileIconProvider provider;

	const QList<QFileInfo>& volumes = QFileOperationsThread::volumes();
	for(int i = 0; i < volumes.count(); ++i)
	{
		const QFileInfo& fi = volumes[i];
		QString path = fi.absoluteFilePath();

		if (!fi.isDir() || qslIgnoreList.contains(path))
			continue;

		if (qalDrives.count() > i && qalDrives.at(i)->data().toString() == path) //TODO: rework this sux
			continue;

		QAction* action = new QAction(this);
		action->setText(isDrive(path) ? path.left(1) : fi.fileName());
#ifdef Q_WS_WIN
		action->setIcon(provider.icon(fi));
#else
		action->setIcon(provider.icon(QFileIconProvider::Drive));
#endif
		action->setToolTip(QDir::toNativeSeparators(path));
		action->setData(path);
		action->setCheckable(true);
		connect(action, SIGNAL(triggered(bool)), this, SLOT(slotDiscChanged()));
		if (qalDrives.count() > i)
			qalDrives.insert(i, action);
		else
			qalDrives.append(action);
		qagDrives->addAction(action);
	}
	while(qalDrives.count() > volumes.count())
		delete qalDrives.takeLast();

//Remove toolButtons
	QList<QToolButton*> buttons = findChildren<QToolButton*>();
	qDeleteAll(buttons);
	delete layout();
//
	FlowLayout* flMainLayout=new FlowLayout();
	flMainLayout->setSpacing(0);
	flMainLayout->setMargin(0);
	flMainLayout->setContentsMargins(0,0,0,0);
	for (int i=0; i<qalDrives.count(); i++)
	{
		QToolButton* button=new QToolButton(this);
		button->setIconSize(QSize(16,16));
		button->setDefaultAction(qalDrives.at(i));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		button->setAutoRaise(true);
		button->setFocusPolicy(Qt::NoFocus);
		flMainLayout->addWidget(button);
	}
	this->setLayout(flMainLayout);
	slotSetDisc(qsCurrentPath);
	blockSignals(false);
}
//
void QDriveBar::slotDiscChanged()
{
	this->killTimer(timerID);
	QAction* action=qobject_cast<QAction*>(sender());
	if (action)
	{
		const QString path=action->data().toString();
		if (QFileInfo(path).isReadable())
		{
			qaLastChecked=action;
			emit discChanged(path);
			qsCurrentPath = path;
		}
		else
		{
			QMessageBox::critical(this,"",tr("Drive %1 is not ready!!!").arg(QDir::toNativeSeparators(path)));
			if (qaLastChecked)
				qaLastChecked->setChecked(true);
		}
	}
	timerID=this->startTimer(TIMER_INTERVAL);
}
//
void QDriveBar::slotSetDisc(const QString& path)
{
	qsCurrentPath=path;
	QList<QAction*> actionList=qagDrives->actions();
	for (int i=0; i<actionList.count(); i++)
	{
		if (QDir::toNativeSeparators(path).toUpper().startsWith(QDir::toNativeSeparators(actionList.at(i)->data().toString()).toUpper()))
		{
			actionList.at(i)->setChecked(true);
			qaLastChecked=actionList.at(i);
			break;
		}
		else
			actionList.at(i)->setChecked(false);
	}
}
//
void QDriveBar::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event);
	int count = QFileOperationsThread::volumes().count();
	if (lastDrivesCount != count) {
		slotRefresh();
		lastDrivesCount = count;
	}
}
