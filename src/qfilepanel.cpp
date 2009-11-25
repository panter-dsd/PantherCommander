/********************************************************************
* Copyright (C) PanteR
*-------------------------------------------------------------------
*
* Panther Commander is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* Panther Commander is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Panther Commander; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301 USA
*-------------------------------------------------------------------
* Project:		Panther Commander
* Author:		PanteR
* Contact:	panter.dsd@gmail.com
*******************************************************************/

#include "qfilepanel.h"

#include <QtGui>
#include <QMap>
#include <QIcon>
#include <QFileIconProvider>

#include "appsettings.h"
#include "filewidget.h"
#include "tabbar.h"
#include "qselectdiscdialog.h"
#include "volumeinfoprovider.h"

#define TIMER_INTERVAL 5000

QFilePanel::QFilePanel(QWidget* parent) : QWidget(parent),
	m_currentIndex(-1)
{
	createWidgets();

	updateDiscInformation();
	const QString path=qflvCurrentFileList->directory().absolutePath();
	if(!path.isEmpty() && !path.startsWith("/"))
	{
		qtbDriveButton->setText(path[0]);
		qcbDriveComboBox->setCurrentIndex(qcbDriveComboBox->findText(path[0], Qt::MatchContains));
	}
	timerID=startTimer(TIMER_INTERVAL);
}

QFilePanel::~QFilePanel()
{
}

static bool isDrive(const QString& path)
{
	int length = path.length();
	return ((length == 1 && path.at(0) == QLatin1Char('/'))
			|| (length >= 2 && length <= 3 && path.at(0).isLetter() && path.at(1) == QLatin1Char(':')));
}

void QFilePanel::createWidgets()
{
	setContextMenuPolicy(Qt::PreventContextMenu);

	qtbDriveButton=new QToolButton(this);
	qtbDriveButton->setIcon(QFileIconProvider().icon(QFileIconProvider::Drive));
	qtbDriveButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	qtbDriveButton->setAutoRaise(true);
	qtbDriveButton->setFocusPolicy(Qt::NoFocus);
	connect(qtbDriveButton, SIGNAL(clicked()), this, SLOT(slotSelectDisc()));

	qcbDriveComboBox = new QComboBox(this);
	qcbDriveComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	qcbDriveComboBox->setFocusPolicy(Qt::NoFocus);
	foreach(const QFileInfo& fi, VolumeInfoProvider().volumes())
	{
		QString path = fi.absoluteFilePath();
		qcbDriveComboBox->addItem(QString("[-%1-]").arg(isDrive(path) ? path.left(1) : fi.fileName()));
	}

	qlDiscInformation = new QLabel(this);

	qtabbTabs = new TabBar(this);
	qtabbTabs->setExpanding(false);
	qtabbTabs->setFocusPolicy(Qt::NoFocus);
#if QT_VERSION >= 0x040500
	qtabbTabs->setDocumentMode(true);
	qtabbTabs->setMovable(true);
	qtabbTabs->setTabsClosable(true);
	qtabbTabs->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
#endif

	connect(qtabbTabs, SIGNAL(newTab()), this, SLOT(slotAddTab()));
	connect(qtabbTabs, SIGNAL(cloneTab(int)), this, SLOT(slotAddTab()));
	connect(qtabbTabs, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChange(int)));

	qflvCurrentFileList = new FileWidget(qtabbTabs);
	qflvCurrentFileList->setContextMenuPolicy(Qt::PreventContextMenu);
	connect(qflvCurrentFileList, SIGNAL(directoryEntered(const QString&)),
			this, SLOT(slotPathChanged(const QString&)));
	connect(qflvCurrentFileList, SIGNAL(directoryEntered(const QString&)),
			this, SIGNAL(pathChanged(const QString&)));

	QHBoxLayout* qhbDiscLayout = new QHBoxLayout;
	qhbDiscLayout->addWidget(qtbDriveButton);
	qhbDiscLayout->addWidget(qcbDriveComboBox);
	qhbDiscLayout->addWidget(qlDiscInformation);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addLayout(qhbDiscLayout);
	layout->addWidget(qtabbTabs);
	layout->addWidget(qflvCurrentFileList);
	setLayout(layout);

	setFocusProxy(qflvCurrentFileList);
}

void QFilePanel::slotPathChanged(const QString& path)
{
	updateDiscInformation();

	QString tabText;
	QDir dir(path);
#ifdef Q_WS_WIN
	tabText = path.startsWith(QLatin1Char('/')) ? QLatin1String("\\:") : dir.absolutePath().mid(0,2);
	tabText += dir.dirName();

	if (!path.isEmpty() && !path.startsWith(QLatin1Char('/')))
	{
		qtbDriveButton->setText(path[0]);
		qcbDriveComboBox->setCurrentIndex(qcbDriveComboBox->findText(path[0], Qt::MatchContains));
	}
#else
	if(!dir.isRoot())
		tabText = dir.dirName();
	else
		tabText = QLatin1String("/");
#endif
	qtabbTabs->setTabText(qtabbTabs->currentIndex(), tabText);
	qtabbTabs->setTabToolTip(qtabbTabs->currentIndex(), QDir::toNativeSeparators(path));
}
//
int QFilePanel::addTab(const QString& path, bool bSetCurrent)
{
	QString qsPath = path;
	if(qsPath.isEmpty())
		qsPath = this->path();
	if(qsPath.isEmpty())
		qsPath = QApplication::applicationDirPath();

	QString tabText;
	QDir dir(path);
#ifdef Q_WS_WIN
	tabText = dir.absolutePath().left(2) + dir.dirName();
#else
	if(!dir.isRoot())
		tabText = dir.dirName();
	else
		tabText = QLatin1String("/");
#endif

	int index = qtabbTabs->addTab(tabText);
	qtabbTabs->setTabToolTip(index, QDir::toNativeSeparators(path));
	if(bSetCurrent)
	{
		qtabbTabs->setCurrentIndex(index);
		qflvCurrentFileList->setDirectory(qsPath);
	}

	return index;
}

void QFilePanel::slotAddTab()
{
	addTab(QString());

	saveSettings();
}

void QFilePanel::slotCurrentTabChange(int index)
{
	if(m_currentIndex == index)
		return;

	if(m_currentIndex >= 0 && m_currentIndex < qtabbTabs->count())
		qtabbTabs->setTabData(m_currentIndex, qflvCurrentFileList->saveState());

	if(index >= 0 && index < qtabbTabs->count())
	{
		if(!qflvCurrentFileList->restoreState(qtabbTabs->tabData(index).toByteArray()))
			qflvCurrentFileList->setDirectory(QDir::rootPath());
		m_currentIndex = index;
	}
	qflvCurrentFileList->setFocus();
}

void QFilePanel::saveSettings()
{
	int count = qtabbTabs->count();
	int currentIndex = qtabbTabs->currentIndex();

	/*	NavigationHistoryType
		0 - for all tabs
		1 - for every tab
		2 - no history at all
	*/
	QSettings* settings = AppSettings::instance();
	int historyType = settings->value("NavigationHistoryType", 0).toInt();
	settings->remove(objectName());
	settings->beginGroup(objectName());
	settings->setValue("TabsCount", count);
	settings->setValue("CurrentTab", currentIndex);
	for(int i = 0; i < count; ++i)
	{
		// flush state to the current tab
		if(i == currentIndex)
			qtabbTabs->setTabData(i, qflvCurrentFileList->saveState());

		qint32 magic;
		qint32 version;
		QStringList history;
		QString lastVisitedDir;
		QByteArray headerData;

		{
			QByteArray data = qtabbTabs->tabData(i).toByteArray();
			QDataStream stream(&data, QIODevice::ReadOnly);
			if(!stream.atEnd())
			{
				stream >> magic;
				stream >> version;

				stream >> history;
				stream >> lastVisitedDir;
				stream >> headerData;
			}
		}
/*? //don't touch me
		QByteArray data;
		{
			QDataStream stream(&data, QIODevice::WriteOnly);
			stream << qint32(magic);
			stream << qint32(version);

			stream << history;
			stream << lastVisitedDir;
			stream << headerData;
		}
		settings->setValue(QString("Tab_%1").arg(i), data);*/

		if(historyType == 1)
			settings->setValue(QString("History_%1").arg(i), history);
		settings->setValue(QString("Path_%1").arg(i), lastVisitedDir);
		settings->setValue(QString("State_%1").arg(i), headerData);
	}
	if(historyType <= 0 || historyType > 2)
		settings->setValue("History", qflvCurrentFileList->history());
	settings->endGroup();
}

void QFilePanel::loadSettings()
{
	/*	NavigationHistoryType
		0 - for all tabs
		1 - for every tab
		2 - no history at all
	*/
	QSettings* settings = AppSettings::instance();
	int historyType = settings->value("NavigationHistoryType", 0).toInt();
	settings->beginGroup(objectName());
	int count = settings->value("TabsCount", 1).toInt();
	int currentIndex = settings->value("CurrentTab", 0).toInt();
	for(int i = 0; i < count; ++i)
	{
		qint32 magic;
		qint32 version;
		QStringList history;
		QString lastVisitedDir;
		QByteArray headerData;

		{
			QByteArray data = qflvCurrentFileList->saveState();
			QDataStream stream(&data, QIODevice::ReadOnly);
			if(!stream.atEnd())
			{
				stream >> magic;
				stream >> version;

				stream >> history;
				stream >> lastVisitedDir;
				stream >> headerData;
			}
		}

		history.clear();
		if(historyType == 1)
			history = settings->value(QString("History_%1").arg(i)).toStringList();
		lastVisitedDir = settings->value(QString("Path_%1").arg(i)).toString();
		headerData = settings->value(QString("State_%1").arg(i)).toByteArray();

		QByteArray data;
		{
			QDataStream stream(&data, QIODevice::WriteOnly);
			stream << qint32(magic);
			stream << qint32(version);

			stream << history;
			stream << lastVisitedDir;
			stream << headerData;
		}

		bool bFirstTab = (qtabbTabs->count() == 0);
		if(bFirstTab)
		{
			disconnect(qtabbTabs, SIGNAL(currentChanged(int)),
						this, SLOT(slotCurrentTabChange(int)));
		}
		int index = addTab(lastVisitedDir, false);
		qtabbTabs->setTabData(index, data);
		if(bFirstTab)
		{
			connect(qtabbTabs, SIGNAL(currentChanged(int)),
						this, SLOT(slotCurrentTabChange(int)));
			if(index == currentIndex)
				slotCurrentTabChange(index);
		}
		else
		{
			if(index == currentIndex)
				qtabbTabs->setCurrentIndex(index);
		}
	}
	if(historyType <= 0 || historyType > 2)
		qflvCurrentFileList->setHistory(settings->value("History").toStringList());
	settings->endGroup();

	qtbDriveButton->setVisible(settings->value("Interface/ShowDriveButton", true).toBool());
	qcbDriveComboBox->setVisible(settings->value("Interface/ShowDriveComboBox", true).toBool());
	qtabbTabs->setVisible(settings->value("Interface/ShowTabs", true).toBool());
}
//
void QFilePanel::setDisc(const QString& name)
{
	setPath(name);
}
//
void QFilePanel::slotSetDisc()
{
	killTimer(timerID);

	QAction* action = qobject_cast<QAction*>(sender());
	if(!action)
		return;

	QString path = action->data().toString();
	if(QDir::setCurrent(path))
	{
		setDisc(path);
	}
	else
	{
#ifndef Q_CC_MSVC
	#warning "TODO: must be configurable (message, beep, or something else)"
#endif
		QMessageBox::critical(this, "", tr("Drive is not ready."));
	}

	timerID = startTimer(TIMER_INTERVAL);
}
//
void QFilePanel::clearSelection()
{
	qflvCurrentFileList->clearSelection();
}
//
QString QFilePanel::path() const
{
	return qflvCurrentFileList->directory().absolutePath();
}
//
QString QFilePanel::currentFileName() const
{
	return qflvCurrentFileList->currentFile();
}
//
void QFilePanel::setPath(const QString& path)
{
	qflvCurrentFileList->setDirectory(path);
}
//
QStringList QFilePanel::selectedFiles() const
{
	return qflvCurrentFileList->selectedFiles();
}
//
void QFilePanel::slotInformationChanged()
{
	updateDiscInformation();
}
//
QString QFilePanel::size(qint64 bytes)
{
	// According to the Si standard KB is 1000 bytes, KiB is 1024
	// but on windows sizes are calculated by dividing by 1024 so we do what they do.
	const qint64 kb = 1024;
	const qint64 mb = 1024 * kb;
	const qint64 gb = 1024 * mb;
	const qint64 tb = 1024 * gb;
	if (bytes >= tb)
		return QFileSystemModel::tr("%1 TB").arg(QLocale().toString(qreal(bytes) / tb, 'f', 3));
	if (bytes >= gb)
		return QFileSystemModel::tr("%1 GB").arg(QLocale().toString(qreal(bytes) / gb, 'f', 2));
	if (bytes >= mb)
		return QFileSystemModel::tr("%1 MB").arg(QLocale().toString(qreal(bytes) / mb, 'f', 1));
	if (bytes >= kb)
		return QFileSystemModel::tr("%1 KB").arg(QLocale().toString(bytes / kb));
	return QFileSystemModel::tr("%1 bytes").arg(QLocale().toString(bytes));
}
//
void QFilePanel::updateDiscInformation()
{
	QString text;

	VolumeInfoProvider provider;

	QString label = provider.volumeLabel(path());
	if(!label.isEmpty())
		text.append(QString("[<i>%1</i>]").arg(label));

	qint64 totalBytes, freeBytes, availableBytes;
	if(provider.getDiskFreeSpace(path(), &totalBytes, &freeBytes, &availableBytes))
	{
		if(!text.isEmpty())
			text.append(QLatin1Char(' '));
		text.append(tr("<b>%1</b> of <b>%2</b> free").arg(size(availableBytes)).arg(size(totalBytes)));
	}

	qlDiscInformation->setText(text);
}
//
void QFilePanel::timerEvent(QTimerEvent */*event*/)
{
	updateDiscInformation();
}
//
void QFilePanel::slotSelectDisc()
{
	QSelectDiscDialog *dialog = new QSelectDiscDialog(this);
	dialog->setPath(path());
	connect(dialog, SIGNAL(setectedDisc(QString)), this, SLOT(setPath(QString)));

	dialog->setWindowFlags(Qt::Popup);
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	QPoint point(qtbDriveButton->x(), qtbDriveButton->y() + qtbDriveButton->height());
	dialog->move(qtbDriveButton->mapToGlobal(point));

	dialog->show();
}
