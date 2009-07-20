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

#include "qdrivebar.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QList>

#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QFileIconProvider>
#include <QtGui/QLayout>
#include <QtGui/QMessageBox>
#include <QtGui/QToolButton>

#include "appsettings.h"
#ifdef Q_WS_WIN
#  include "filecontextmenu.h"
#endif
#include "flowlayout.h"
#include "volumeinfoprovider.h"

static bool isDrive(const QString& path)
{
	int length = path.length();
	return ((length == 1 && path.at(0) == QLatin1Char('/'))
			|| (length >= 2 && length <= 3 && path.at(0).isLetter() && path.at(1) == QLatin1Char(':')));
}


QDriveBar::QDriveBar(QWidget* parent) : QFrame(parent),
	m_inRefresh(false)
{
	setFocusPolicy(Qt::NoFocus);

	qaLastChecked = 0;

	qagDrives = new QActionGroup(this);
	connect(qagDrives, SIGNAL(triggered(QAction*)), this, SLOT(_q_actionTriggered(QAction*)));

	slotRefresh();

	VolumeInfoProvider* provider = new VolumeInfoProvider(this);
	connect(provider, SIGNAL(volumeAdded(const QString&)), this, SLOT(slotRefresh()));
	connect(provider, SIGNAL(volumeChanged(const QString&)), this, SLOT(slotRefresh()));
	connect(provider, SIGNAL(volumeRemoved(const QString&)), this, SLOT(slotRefresh()));
}

QDriveBar::~QDriveBar()
{
}

void QDriveBar::slotRefresh()
{
	if(m_inRefresh)
		return;

	m_inRefresh = true;

	QSettings* settings = AppSettings::instance();
	QStringList qslIgnoreList = settings->value("Global/IgnoredDrives").toStringList();

	QList<QAction*> qalDrives = qagDrives->actions();
	QFileIconProvider provider;

	const QList<QFileInfo>& volumes = VolumeInfoProvider().volumes();
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
	FlowLayout* layout = new FlowLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	for(int i = 0; i < qalDrives.size(); ++i)
	{
		QToolButton* button = new QToolButton(this);
		button->setIconSize(QSize(16, 16));
		button->setDefaultAction(qalDrives.at(i));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		button->setAutoRaise(true);
		button->setFocusPolicy(Qt::NoFocus);
		button->setContextMenuPolicy(Qt::CustomContextMenu);
		connect (button, SIGNAL(customContextMenuRequested(QPoint)),
				 this, SLOT(_q_showContextMenu(QPoint)));

		layout->addWidget(button);
	}
	setLayout(layout);

	slotSetDisc(qsCurrentPath);

	m_inRefresh = false;
}

void QDriveBar::_q_actionTriggered(QAction* action)
{
	if(!action)
		return;

	const QString path = action->data().toString();
	if(QFileInfo(path).isReadable())
	{
		qaLastChecked = action;
		qsCurrentPath = path;
		emit discChanged(path);
	}
	else
	{
		QMessageBox::critical(this, "", tr("Drive %1 is not ready.").arg(QDir::toNativeSeparators(path)));
		if(qaLastChecked)
			qaLastChecked->setChecked(true);
	}
}

void QDriveBar::slotSetDisc(const QString& volume)
{
	QString path = QDir::fromNativeSeparators(volume);
	const QList<QAction*>& actionList = qagDrives->actions();
	for(int i = 0; i < actionList.size(); ++i)
	{
		if(path.toUpper() == actionList.at(i)->data().toString().toUpper())
		{
			qsCurrentPath = path;
			actionList.at(i)->setChecked(true);
			qaLastChecked = actionList.at(i);
			break;
		}
	}
}

void QDriveBar::_q_showContextMenu(const QPoint& position)
{
#ifdef QT_NO_MENU
	Q_UNUSED(position);
#else
	QToolButton* button = qobject_cast<QToolButton*> (sender());
	if (!button)
		return;
	QString path = button->defaultAction()->data().toString();

	QPoint globalPos = button->mapToGlobal(position);
#ifdef Q_WS_WIN
	FileContextMenu menu(this);
	menu.setPath(path);
	menu.executeNativeMenu(globalPos);
#endif // Q_WS_WIN
#endif // QT_NO_MENU
}
