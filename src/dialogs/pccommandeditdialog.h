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

#ifndef PCCOMMANDEDITDIALOG_H
#define PCCOMMANDEDITDIALOG_H

class QLabel;
class QLineEdit;
class QToolButton;
class QPushButton;
class QGroupBox;
class QDialogButtonBox;
class QHBoxLayout;
class QVBoxLayout;
class PCShortcutEdit;
class PCShortcutWidget;

#include <QDialog>

class PCCommandEditDialog : public QDialog
{
	Q_OBJECT
private:
	QGroupBox *qgbMainBox;
	QLabel *qlObjectName;
	QLineEdit *qleObjectName;
	QLabel *qlText;
	QLineEdit *qleText;
	QLabel *qlToolTip;
	QLineEdit *qleToolTip;
	QGroupBox *qgbShortcutBox;
	QList<PCShortcutWidget*> qlShortcutWidgets;
	QPushButton *qpbAddShortcut;
	QPushButton *qpbRemoveShortcut;
	QHBoxLayout *qhblButtonsLayout;
	QDialogButtonBox *qdbbButtons;

	QVBoxLayout* m_shortcutsLayout;

public:
	PCCommandEditDialog(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint);
	virtual ~PCCommandEditDialog() {}

	void setCommandObjectName(const QString& objectName);

	void setCommandText(const QString& text);
	QString commandText();

	void setCommandToolTip(const QString& text);
	QString commandToolTip();

	void setCommandShortcuts(QList<QKeySequence> shortcuts);
	QList<QKeySequence> commandShortcuts();

private Q_SLOTS:
	void addShortcut();
	void removeShortcut();
};

#endif // PCCOMMANDEDITDIALOG_H
