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

#ifndef QFULLVIEW_H
#define QFULLVIEW_H

#include <QtGui/QTreeView>

class QFullView : public QTreeView
{
	Q_OBJECT

private:
//	bool						isRightMouseButtonPressed;
//	bool						isMouseSelect;

//	bool						bFocused;

public:
	explicit QFullView(QWidget* parent = 0);
	virtual ~QFullView();

protected:
	void drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QRect visualRect(const QModelIndex& index) const;

#ifndef QT_NO_DRAGANDDROP
	void startDrag(Qt::DropActions supportedActions);
#endif

	QItemSelectionModel::SelectionFlags selectionBehaviorFlags() const;
	QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex& index, const QEvent* event = 0) const;

	void keyPressEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);

/*	void focusInEvent(QFocusEvent* event);
	void focusOutEvent(QFocusEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event)*/

private:
	QPersistentModelIndex pressedIndex;
};

#endif // QFULLVIEW_H
