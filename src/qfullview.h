#ifndef QFULLVIEW_H
#define QFULLVIEW_H
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
#include <QTreeView>
#include <QTime>
#include <QPoint>
//
class QFullView : public QTreeView
{
Q_OBJECT
private:
	QTime					doubleClickTime;
	bool						isRightMouseButtonPressed;
	bool						isMouseSelect;
	QPoint					dragStartPosition;

	bool						bFocused;
	int						selectedFilesCount;
	int						selectedDirsCount;
	int						selectedFilesSize;
public:
	QFullView(QWidget * parent = 0);
	~QFullView();
	bool isFocused() {return bFocused;}
protected:
	void drawRow ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	void mousePressEvent (QMouseEvent* ev);
	void mouseMoveEvent (QMouseEvent* ev);
	void mouseReleaseEvent (QMouseEvent* ev);
	//void dropEvent (QDropEvent* ev);
	//void dragEnterEvent(QDragEnterEvent* event);
	//void dragMoveEvent (QDragMoveEvent* event);
	void focusInEvent (QFocusEvent* event);
	void focusOutEvent (QFocusEvent* event);
	void keyPressEvent (QKeyEvent* event);
signals:
	void focusIn();
	void focusOut();
};

#endif // QFULLVIEW_H
