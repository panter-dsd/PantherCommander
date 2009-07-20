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

#ifndef PCCOPYMOVEDIALOG_H
#define PCCOPYMOVEDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QComboBox;
class QDialogButtonBox;
class QStandardItemModel;

class PCCopyMoveDialog : public QDialog
{
Q_OBJECT
private:
	QLabel *qlSources;
	QLineEdit *qleDest;
	QLabel *qlQueue;
	QComboBox *qcbQueue;
	QDialogButtonBox *qdbbButtons;

	QStringList qslSource;
	QString qsDest;
	QString qsOperation;
public:
	explicit PCCopyMoveDialog(QWidget *parent = 0);
	virtual ~PCCopyMoveDialog()
	{}
	void setSource(const QStringList &fileList);
	void setDest(const QString &dest);
	QString dest();
	void setOperation(const QString &operation);
	void setQueueModel(QStandardItemModel *model);

	int queueIndex() const;
private:
	void createControls();
	void setLayouts();
	void updateText();
};

#endif // PCCOPYMOVEDIALOG_H
