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

#include <QtGui>
#include "qfullview.h"

QFullView::QFullView(QWidget* parent) : QTreeView(parent)
{
	isRightMouseButtonPressed=false;
}

QFullView::~QFullView()
{
}

void QFullView::drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QTreeView::drawRow(painter, option, index);

	if(index.row() == currentIndex().row() && hasFocus())
	{
		QRect rect(visualRect(index));
		rect.adjust(0, 0, -1, -1);
		painter->setPen(Qt::green);
		painter->drawRect(rect);
	}
}

QRect QFullView::visualRect(const QModelIndex& index) const
{
	if(index.row() < 0 || index.column() < 0 || index.model() != model())
		return QRect();

	QModelIndex left = index.sibling(index.row(), 0);
	QModelIndex right = index.sibling(index.row(), index.model()->columnCount(index.parent()) - 1);
	return QRect(QTreeView::visualRect(left).topLeft(), QTreeView::visualRect(right).bottomRight());
}

#ifndef QT_NO_DRAGANDDROP
/*!
	Starts a drag by calling drag->exec() using the given \a supportedActions.
*/
void QFullView::startDrag(Qt::DropActions supportedActions)
{
	QModelIndexList indexes = selectedIndexes();
/*	if(indexes.isEmpty())
	{
		//QModelIndex tl = model()->index(currentIndex().row(), 0, currentIndex().parent());
		//QModelIndex br = model()->index(currentIndex().row(), model()->columnCount() - 1, currentIndex().parent());
		QItemSelection selection(currentIndex(), currentIndex());
		selectionModel()->select(selection, QItemSelectionModel::Select);
		indexes = selectedIndexes();
	}*/
	for(int i = indexes.size() - 1 ; i >= 0; --i)
	{
		if(!(model()->flags(indexes.at(i)) & Qt::ItemIsDragEnabled))
			indexes.removeAt(i);
	}
	QMimeData* data = model()->mimeData(indexes);
	if(data)
	{
		QDrag* drag = new QDrag(this);
		drag->setMimeData(data);
		//drag->setPixmap(pixmap);
		//drag->setHotSpot(d->pressedPosition - rect.topLeft());
		Qt::DropAction defaultDropAction = Qt::IgnoreAction;
		if((supportedActions & Qt::CopyAction) && dragDropMode() != QAbstractItemView::InternalMove)
			defaultDropAction = Qt::CopyAction;
		if(drag->exec(supportedActions, defaultDropAction) == Qt::MoveAction)
			;//clearSelection();
	}
}
#endif // QT_NO_DRAGANDDROP
/*
void QFullView::mousePressEvent ( QMouseEvent * event)
{
	if (ev->button()==Qt::LeftButton)
	{
		QModelIndex index=indexAt(event->pos());
		if (index.isValid() && index!=selectionModel()->currentIndex())
			selectionModel()->setCurrentIndex(index,QItemSelectionModel::NoUpdate);
	}
	else if (event->button()==Qt::RightButton)
	{
		isRightMouseButtonPressed=true;
		QModelIndex index=indexAt(event->pos());
		selectionModel()->setCurrentIndex(index,QItemSelectionModel::NoUpdate);
		if (index.data(Qt::DisplayRole).toString()!="..")
		{
			QItemSelection selection(model()->index(index.row(),0),
									model()->index(index.row(),model()->columnCount()-1));
			if (selectionModel()->selectedIndexes().contains(index))
				isMouseSelect=false;
			else
				isMouseSelect=true;
			selectionModel()->select(selection,QItemSelectionModel::Toggle);
		}
		else
			isMouseSelect=false;
	}

	QWidget::mousePressEvent(event);
}
//
void QFullView::mouseMoveEvent (QMouseEvent* event)
{
	if (isRightMouseButtonPressed)
	{
		QModelIndex index=indexAt(event->pos());
		if (index.isValid() && index!=selectionModel()->currentIndex())
		{
			selectionModel()->setCurrentIndex(index,QItemSelectionModel::NoUpdate);
			if (index.data(Qt::DisplayRole).toString()!="..")
			{
				QItemSelection selection(model()->index(index.row(),0),
							model()->index(index.row(),model()->columnCount()-1));
				if (isMouseSelect)
					selectionModel()->select(selection,QItemSelectionModel::Select);
				else
					selectionModel()->select(selection,QItemSelectionModel::Deselect);
			}
		}
	}

	QWidget::mouseMoveEvent(event);
}
//
void QFullView::mouseReleaseEvent (QMouseEvent* event)
{
	if (event->button()==Qt::RightButton)
	{
		isRightMouseButtonPressed=false;
	}
	QTreeView::mouseReleaseEvent(event);
}

void QFullView::focusInEvent (QFocusEvent* event)
{
	QTreeView::focusInEvent(event);

	QPalette palette=this->palette();
	palette.setColor(QPalette::Active,
					QPalette::Highlight,
					qApp->palette().color(QPalette::Active,QPalette::Highlight));
	setPalette(palette);
	for (int i=0; i<model()->columnCount(); i++)
	{
		update(model()->index(currentIndex().row(),i));
	}
}

void QFullView::focusOutEvent (QFocusEvent* event)
{
	QTreeView::focusOutEvent(event);

	QPalette palette=this->palette();
	palette.setColor(QPalette::Active,
					QPalette::Highlight,
					qApp->palette().color(QPalette::Inactive,QPalette::Highlight));
	setPalette(palette);
	for (int i=0; i<model()->columnCount(); i++)
	{
		update(model()->index(currentIndex().row(),i));
	}
}

void QFullView::keyPressEvent (QKeyEvent* event)
{
	if (event->key()==Qt::Key_Insert)
	{
		int row=currentIndex().row();
		if (currentIndex().data(Qt::DisplayRole).toString()!="..")
		{
			selectionModel()->select(QItemSelection(model()->index(row,0),
								model()->index(row,model()->columnCount()-1)),
								QItemSelectionModel::Toggle);
		}
		if (row<model()->rowCount()-1)
			selectionModel()->setCurrentIndex(model()->index(row+1,0),
														QItemSelectionModel::NoUpdate);
	}
	else if (event->text()=="*")
	{
		selectionModel()->select(QItemSelection(model()->index(0,0),
						model()->index(model()->rowCount()-1,model()->columnCount()-1)),
						QItemSelectionModel::Toggle);
		if (model()->index(0,0).data(Qt::DisplayRole).toString()=="..")
			selectionModel()->select(QItemSelection(model()->index(0,0),
						model()->index(0,model()->columnCount()-1)),
						QItemSelectionModel::Deselect);
	}
	else if (event->key()==Qt::Key_Down && (event->modifiers() & Qt::SHIFT))
	{
		int row=currentIndex().row();
		if (currentIndex().data(Qt::DisplayRole).toString()!="..")
		{
			selectionModel()->select(QItemSelection(model()->index(row,0),
								model()->index(row,model()->columnCount()-1)),
								QItemSelectionModel::Toggle);
		}
		if (row<model()->rowCount()-1)
			selectionModel()->setCurrentIndex(model()->index(row+1,0),
														QItemSelectionModel::NoUpdate);
	}
	else if (event->key()==Qt::Key_Up && (event->modifiers() & Qt::SHIFT))
	{
		int row=currentIndex().row();
		if (currentIndex().data(Qt::DisplayRole).toString()!="..")
		{
			selectionModel()->select(QItemSelection(model()->index(row,0),
								model()->index(row,model()->columnCount()-1)),
								QItemSelectionModel::Toggle);
		}
		if (row>0)
			selectionModel()->setCurrentIndex(model()->index(row-1,0),
														QItemSelectionModel::NoUpdate);
	}

	QTreeView::keyPressEvent(event);
}
*/
