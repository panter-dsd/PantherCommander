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

#ifndef QFILEPANEL_H
#define QFILEPANEL_H

#include <QtWidgets/QWidget>

class QLabel;
class QActionGroup;
class QPushButton;
class QToolButton;
class QComboBox;

class FileWidget;
class TabBar;

class QFilePanel : public QWidget
{
	Q_OBJECT

private:
	TabBar*							qtabbTabs;
	FileWidget*						qflvCurrentFileList;
	QLabel*							qlDiscInformation;
	QToolButton*					qtbDriveButton;
	QComboBox* qcbDriveComboBox;

	int m_currentIndex;
	int								timerID;

public:
	explicit QFilePanel(QWidget* parent = 0);
	virtual ~QFilePanel();

	void setDisc(const QString& name);
	QString path() const;
	QString currentFileName() const;
	QStringList selectedFiles() const;
	void clearSelection();

	void saveSettings();
	void loadSettings();

protected:
	void timerEvent(QTimerEvent *event);

Q_SIGNALS:
	void pathChanged(const QString&);

private Q_SLOTS:
	void slotPathChanged(const QString& path);
	void slotAddTab();
	void slotCurrentTabChange(int);
	void slotSetDisc();
	void slotInformationChanged();
	void slotSelectDisc();

public Q_SLOTS:
	void setPath(const QString& path);

private:
	void createWidgets();
	int addTab(const QString& path, bool bSetCurrent = true);
	void updateDirInformation();
	void updateDiscInformation();

	static QString size(qint64 bytes);
};

#endif // QFILEPANEL_H
