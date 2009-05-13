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
//
class QLabel;
class QActionGroup;
class QTabBar;
class QAction;
class QPushButton;
class QToolButton;
//
#include <QWidget>

class FileWidget;
//
class QFilePanel : public QWidget
{
	Q_OBJECT

private:
	QTabBar*						qtabbTabs;
	FileWidget*						qflvCurrentFileList;
	QLabel*							qlDiscInformation;
	QToolButton*					qtbDriveButton;

	QAction*						actionAddTab;

	int m_currentIndex;
	int								timerID;

public:
	explicit QFilePanel(QWidget* parent = 0);
	virtual ~QFilePanel();

	void setDisc(const QString& name);
	QString path() const;
	void setPath(const QString& path);
	QString currentFileName() const;
	QStringList selectedFiles() const;
	void clearSelection();

	void saveSettings();
	void loadSettings();

protected:
	void timerEvent(QTimerEvent *event);
private:
	void createWidgets();
	void createActions();
	int addTab(const QString& path, bool bSetCurrent = true);
	void updateDirInformation();
	void updateDiscInformation();
	static QString size(qint64 bytes);
private slots:
	void slotPathChanged(const QString& path);
	void closeTab(int);
	void slotAddTab() {addTab(""); saveSettings();}
	void slotCurrentTabChange(int);
	void slotSetDisc();
	void slotInformationChanged();
	void slotSelectDisc();
signals:
	void pathChanged(const QString&);
};

#endif // QFILEPANEL_H
