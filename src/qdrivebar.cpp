#include <QtCore/QSettings>
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

#define TIMER_INTERVAL 2500

QDriveBar::QDriveBar(QWidget* parent)
		:QFrame(parent)
{
	qaLastChecked=0;
	qagDrives=new QActionGroup(this);

	slotRefresh();
	timerID=this->startTimer(TIMER_INTERVAL);
}
//
void QDriveBar::slotRefresh()
{
	blockSignals(true);
	QSettings* settings=new QSettings(this);
	const QStringList qslIgnoreList = settings->value("Global/IgnoredDrives").toStringList();
	delete settings;
	QList<QAction*> qalDrives=qagDrives->actions();
	QFileIconProvider provider;
#ifdef Q_WS_WIN
	const QList<QFileInfo>& fileInfoList = QDir::drives();
	for (int i=0; i<fileInfoList.count(); i++)
	{
		QString path(fileInfoList[i].absolutePath());
		if (qslIgnoreList.contains(path))
			continue;

		if (qalDrives.count()>i && qalDrives.at(i)->data().toString()==path) // weird too
			continue;
		QAction* action=new QAction(path.mid(0,1),this);
		action->setIcon(provider.icon(fileInfoList[i]));
		action->setData(path);
		action->setCheckable(true);
		connect(action, SIGNAL(triggered(bool)), this, SLOT(slotDiscChanged()));
		if (qalDrives.count() > i)
			qalDrives.insert(i,action);
		else
			qalDrives.append(action);
		qagDrives->addAction(action);
	}
	while(qalDrives.count()>fileInfoList.count())
	{
		QAction* action=qalDrives.last();
		qalDrives.removeLast();
		delete action;
	}
#endif
#ifdef Q_WS_X11
	QFile file;
	file.setFileName("/etc/mtab");
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);
		QString buffer;
		int i=-1;
		while(!stream.atEnd())
		{
			buffer=stream.readLine();
			QFileInfo fileInfo(buffer.split(" ").at(1));
			if (!fileInfo.isDir() || fileInfo.isRoot() || qslIgnoreList.contains(fileInfo.absoluteFilePath()))
				continue;
			i++;
			if (qalDrives.count()>i && qalDrives.at(i)->text()==fileInfo.fileName())
				continue;
			QAction* action=new QAction(fileInfo.fileName(),this);
			action->setCheckable(true);
			action->setToolTip(fileInfo.absoluteFilePath());
			action->setIcon(provider.icon(QFileIconProvider::Drive));
			action->setData(fileInfo.absoluteFilePath());
			connect(action, SIGNAL(triggered(bool)), this, SLOT(slotDiscChanged()));
			qagDrives->addAction(action);
		}
		file.close();
		while(qalDrives.count()>(i+1))
		{
			QAction* action=qalDrives.last();
			qalDrives.removeLast();
			delete action;
		}
	}
#endif
//Remove toolButtons
	QToolButton* button;
	while(button=this->findChild<QToolButton*>())
		delete button;
	delete this->layout();
//
	FlowLayout* flMainLayout=new FlowLayout();
	flMainLayout->setSpacing(0);
	flMainLayout->setMargin(0);
	flMainLayout->setContentsMargins(0,0,0,0);
	for (int i=0; i<qalDrives.count(); i++)
	{
		button=new QToolButton(this);
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
	slotRefresh();
}
//

