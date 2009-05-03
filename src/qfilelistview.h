#ifndef QFILELISTVIEW_H
#define QFILELISTVIEW_H
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

//
class QTreeView;
class QAbstractItemDelegate;
class QTime;
class QLineEdit;
//
#include <QWidget>
#include "qfilelistmodel.h"
#include "qfilelistsortfilterproxymodel.h"
#include "qfullview.h"
//
class QFileListView : public QWidget
{
Q_OBJECT
private:
	struct SDirInformation
	{
		int dirsCount;
		int dirsSelectedCount;
		int filesCount;
		int filesSelectedCount;
		qint64 filesSize;
		qint64 filesSelectedSize;
	};
private:
	QFileListModel*								flmModel;
	QFileListSortFilterProxyModel*		qflsfpmProxyModel;
	QFullView*										qtrvView;
	QLineEdit*										qleRenameEditor;
	QString											qsPath;
	QTime												qtLastRefresh;
	SDirInformation								dirInformation;
	int													iRefreshInterval;
	bool													selectOnlyFileName;
public:
	QFileListView(QWidget* parent=0,const QString& path="");
	~QFileListView();
	QString path();
	void setPath(const QString& path);
	void headerSectionResize ( int logicalIndex, int newSize );
	int sortColumn() {return qflsfpmProxyModel->sortColumn();}
	Qt::SortOrder sortOrder() {return qflsfpmProxyModel->sortOrder();}
	void sort(int column, Qt::SortOrder order);
	void setFocus();
	QString currentFileName();
	QStringList selectedFileNames();
	void clearSelection();
	int dirsCount() {return dirInformation.dirsCount;}
	int dirsSelectedCount() {return dirInformation.dirsSelectedCount;}
	int filesCount() {return dirInformation.filesCount;}
	int filesSelectedCount() {return dirInformation.filesSelectedCount;}
	qint64 filesSize() {return dirInformation.filesSize;}
	qint64 filesSelectedSize() {return dirInformation.filesSelectedSize;}
private:
	void createControls();
	void setLayouts();
	void setConnects();
	void loadSettings();
	void saveSettings();
	void selectItem(const QString& name="");
	void selectItem(int row);
protected:
	bool eventFilter(QObject *obj, QEvent *event);
private slots:
	void slotDoubleClick( const QModelIndex& index);
	void slotCloseEditor();
	void slotUpdateDirInformation();
	void slotSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
public slots:
	void gotoRoot();
	void cdUP();
	void gotoHome();
	void slotRefresh();
	void slotRename();
	void slotSetPath(const QString& path) {setPath(path);}
	void slotFocusIn();
signals:
	void changedPath(const QString&);
	void headerSectionResized ( int logicalIndex, int oldSize, int newSize );
	void focusIn();
	void focusOut();
	void dirInformationChanged();
};

#endif // QFILELISTVIEW_H
