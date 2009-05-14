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
* Author:		Ritt K.
* Contact:		ritt.ks@gmail.com
*******************************************************************/

#ifndef FINDFILESDIALOG_H
#define FINDFILESDIALOG_H

#include <QtCore/QList>

#include <QtGui/QDialog>

class QFileInfo;
typedef QList<QFileInfo> QFileInfoList;

class QComboBox;
class QLabel;
class QPushButton;
class QTabWidget;
class QTableWidget;

class FindFilesDialog : public QDialog
{
	Q_OBJECT

public:
	explicit FindFilesDialog(QWidget* parent = 0);
	virtual ~FindFilesDialog();

private slots:
	void browse();
	void find();
	void _slot_cellActivated(int row, int column);

private:
	QFileInfoList findFiles(const QFileInfoList& files, const QString& text);
	void showFiles(const QFileInfoList& files);

	QComboBox* fileComboBox;
	QComboBox* textComboBox;
	QComboBox* directoryComboBox;
	QLabel* fileLabel;
	QLabel* textLabel;
	QLabel* directoryLabel;
	QLabel* filesFoundLabel;
	QPushButton* browseButton;
	QPushButton* findButton;
	QTableWidget* filesTable;
	QTabWidget* tabber;
};

#endif // FINDFILESDIALOG_H
