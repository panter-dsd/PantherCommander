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

#ifndef QFILEOPERATIONSCONFIRMATIONDIALOG_H
#define QFILEOPERATIONSCONFIRMATIONDIALOG_H
//
class QVBoxLayout;
//
#include <QDialog>
//
class QFileOperationsConfirmationDialog : public QDialog
{
Q_OBJECT
public:
	enum StandardButton
	{
		Ovewrite=0x1,
		OvewriteAll=0x2,
		Skip=0x4,
		SkipAll=0x8,
		Cancel=0x10,
		OvewriteAllOlder=0x20,
		OvewriteAllNew=0x40,
		OvewriteAllSmallest=0x80,
		OvewriteAllLager=0x100,
		Rename=0x200,
		Delete=0x400,
		All=0x800,
		Retry=0x1000
	};
	Q_DECLARE_FLAGS(StandardButtons, StandardButton)
private:
	int				result;
	QVBoxLayout* qvblMainLayout;
public:
	QFileOperationsConfirmationDialog(QWidget* parent=0);
	void setButtons(QFlags<StandardButtons> buttons);
	void setFiles(const QString& source,const QString& dest);
	void setText(const QString& text);
	int getResult() {return result;}
private Q_SLOTS:
	void slotButtonClick();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QFileOperationsConfirmationDialog::StandardButtons)
#endif // QFILEOPERATIONSCONFIRMATIONDIALOG_H
