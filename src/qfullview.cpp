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
//
QFullView::QFullView(QWidget * parent)
		:QTreeView(parent)
{
	doubleClickTime=QTime::currentTime();
	isRightMouseButtonPressed=false;
	bFocused=false;
	setAcceptDrops(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	setItemsExpandable(false);
	selectedFilesCount=selectedDirsCount=selectedFilesSize;
}
//
QFullView::~QFullView()
{

}
//
void QFullView::drawRow ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QTreeView::drawRow(painter,option,index);
	if ((index.row()==currentIndex().row()) && bFocused)
	{
		painter->setPen(Qt::green);
		painter->drawRect(0,
						option.rect.top(),
						this->width()-1,
						option.rect.height()-1);
	}
}
//
void QFullView::mousePressEvent ( QMouseEvent * ev)
{
	if (ev->button()==Qt::LeftButton)
	{
		QModelIndex index=indexAt(ev->pos());
		if (!index.isValid())
			return;
		if (index!=selectionModel()->currentIndex())
		{
			selectionModel()->setCurrentIndex(index,QItemSelectionModel::NoUpdate);
		}
		else
		{
			if (doubleClickTime.msecsTo(QTime::currentTime())<=qApp->doubleClickInterval())
			{
				emit activated(index);
			}
		}
		doubleClickTime=QTime::currentTime();
//Drag
		dragStartPosition = ev->pos();
//
	}
	if (ev->button()==Qt::RightButton)
	{
		isRightMouseButtonPressed=true;
		QModelIndex index=indexAt(ev->pos());
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
}
//
void QFullView::mouseMoveEvent (QMouseEvent* ev)
{
	if (isRightMouseButtonPressed)
	{
		QModelIndex index=indexAt(ev->pos());
		if (!index.isValid())
			return;
		if (index!=selectionModel()->currentIndex())
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
	if ((ev->buttons() & Qt::LeftButton))
	{
		if ((ev->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
			return;
		if (currentIndex().data(Qt::DisplayRole).toString()=="..")
			return;
		QDrag *drag = new QDrag(this);

		QMimeData *mimeData = new QMimeData;

		QList<QUrl> urlList;
		//Если нет селекта, выделяем текущую строку
		if (selectionModel()->selectedIndexes().isEmpty() && !selectionModel()->isSelected(currentIndex()))
		{
			QItemSelection selection(model()->index(currentIndex().row(),0),
						model()->index(currentIndex().row(),model()->columnCount()-1));
			selectionModel()->select(selection,QItemSelectionModel::Select);
			connect(drag,
					SIGNAL(destroyed()),
					this,
					SLOT(clearSelection()));
		}
		QList<QModelIndex> indexList=selectedIndexes();

		QString dir=QDir::toNativeSeparators(QDir::currentPath());
		if (dir.at(dir.length()-1)!=QDir::separator())
			dir+=QDir::separator();

		for (int i=0; i<indexList.count(); i++)
		{
			if (indexList.at(i).column()!=0)
				continue;
			urlList << QUrl::fromLocalFile(dir+indexList.at(i).data(Qt::EditRole).toString());
		}
		mimeData->setUrls(urlList);
		drag->setMimeData(mimeData);

		drag->start(Qt::CopyAction);
	}
	QWidget::mouseMoveEvent(ev);
}
//
void QFullView::mouseReleaseEvent (QMouseEvent* ev)
{
	if (ev->button()==Qt::RightButton)
	{
		isRightMouseButtonPressed=false;
	}
}
//
void QFullView::focusInEvent (QFocusEvent* event)
{
	QPalette palette=this->palette();
	palette.setColor(QPalette::Active,
					QPalette::Highlight,
					qApp->palette().color(QPalette::Active,QPalette::Highlight));
	setPalette(palette);
	bFocused=true;
	for (int i=0; i<model()->columnCount(); i++)
	{
		update(model()->index(currentIndex().row(),i));
	}
	emit focusIn();
}
//
void QFullView::focusOutEvent (QFocusEvent* event)
{
	QPalette palette=this->palette();
	palette.setColor(QPalette::Active,
					QPalette::Highlight,
					qApp->palette().color(QPalette::Inactive,QPalette::Highlight));
	setPalette(palette);
	bFocused=false;
	for (int i=0; i<model()->columnCount(); i++)
	{
		update(model()->index(currentIndex().row(),i));
	}
	emit focusOut();
}
//
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
	if (event->text()=="*")
	{
		selectionModel()->select(QItemSelection(model()->index(0,0),
						model()->index(model()->rowCount()-1,model()->columnCount()-1)),
						QItemSelectionModel::Toggle);
		if (model()->index(0,0).data(Qt::DisplayRole).toString()=="..")
			selectionModel()->select(QItemSelection(model()->index(0,0),
						model()->index(0,model()->columnCount()-1)),
						QItemSelectionModel::Deselect);
	}
	if (event->key()==Qt::Key_Down && (event->modifiers() & Qt::SHIFT))
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
		return;
	}
	if (event->key()==Qt::Key_Up && (event->modifiers() & Qt::SHIFT))
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
		return;
	}
	QTreeView::keyPressEvent(event);
}
//
