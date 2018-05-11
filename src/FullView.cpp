#include "FullView.h"

#include <QtWidgets/QApplication>
#include <QtGui/QDrag>
#include <QtWidgets/QHeaderView>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

FullView::FullView (QWidget *parent)
    : QTreeView (parent)
{
//	isRightMouseButtonPressed=false;
//	isMouseSelect=false;
}

FullView::~FullView ()
{
}

void FullView::drawRow (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QTreeView::drawRow (painter, option, index);

    if (index.row () == currentIndex ().row () && hasFocus ()) {
        QRect rect (visualRect (index));
        rect.adjust (0, 0, -1, -1);
        painter->setPen (Qt::green);
        painter->drawRect (rect);
    }
}

QRect FullView::visualRect (const QModelIndex &index) const
{
    if (index.row () < 0 || index.column () < 0 || index.model () != model ()) {
        return QRect ();
    }

    QModelIndex left = index.sibling (index.row (), header ()->logicalIndex (0));
    QModelIndex right = index.sibling (index.row (),
                                       header ()->logicalIndex (index.model ()->columnCount (index.parent ()) - 1));
    return QRect (QTreeView::visualRect (left).topLeft (), QTreeView::visualRect (right).bottomRight ());
}

#ifndef QT_NO_DRAGANDDROP

/*!
	Starts a drag by calling drag->exec() using the given \a supportedActions.
*/
void FullView::startDrag (Qt::DropActions supportedActions)
{
    QModelIndexList indexes = selectedIndexes ();
/*	if(indexes.isEmpty())
	{
		//QModelIndex tl = model()->index(currentIndex().row(), 0, currentIndex().parent());
		//QModelIndex br = model()->index(currentIndex().row(), model()->columnCount() - 1, currentIndex().parent());
		QItemSelection selection(currentIndex(), currentIndex());
		selectionModel()->select(selection, QItemSelectionModel::Select);
		indexes = selectedIndexes();
	}*/
    for (int i = indexes.size () - 1; i >= 0; --i) {
        if (!(model ()->flags (indexes.at (i)) & Qt::ItemIsDragEnabled)) {
            indexes.removeAt (i);
        }
    }
    QMimeData *data = model ()->mimeData (indexes);
    if (data) {
        QDrag *drag = new QDrag (this);
        drag->setMimeData (data);
        //drag->setPixmap(pixmap);
        //drag->setHotSpot(d->pressedPosition - rect.topLeft());
        Qt::DropAction defaultDropAction = Qt::IgnoreAction;
        if ((supportedActions & Qt::CopyAction) && dragDropMode () != QAbstractItemView::InternalMove) {
            defaultDropAction = Qt::CopyAction;
        }
        if (drag->exec (supportedActions, defaultDropAction) == Qt::MoveAction) {
        }//clearSelection();
    }
}

#endif // QT_NO_DRAGANDDROP

/*
void FullView::mousePressEvent ( QMouseEvent * event)
{
	if (event->button()==Qt::RightButton)
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

	QTreeView::mousePressEvent(event);
}
//
void FullView::mouseMoveEvent (QMouseEvent* event)
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

	QTreeView::mouseMoveEvent(event);
}
//
void FullView::mouseReleaseEvent (QMouseEvent* event)
{
	if (event->button()==Qt::RightButton)
	{
		isRightMouseButtonPressed=false;
	}
	QTreeView::mouseReleaseEvent(event);
}

void FullView::focusInEvent (QFocusEvent* event)
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

void FullView::focusOutEvent (QFocusEvent* event)
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
}*/

QItemSelectionModel::SelectionFlags FullView::selectionBehaviorFlags () const
{
    if (selectionBehavior () == QAbstractItemView::SelectRows) {
        return QItemSelectionModel::Rows;
    }
    if (selectionBehavior () == QAbstractItemView::SelectColumns) {
        return QItemSelectionModel::Columns;
    }
    return QItemSelectionModel::NoUpdate;
}

QItemSelectionModel::SelectionFlags FullView::selectionCommand (const QModelIndex &index, const QEvent *event) const
{
    if (selectionMode () != ExtendedSelection) {
        return QItemSelectionModel::NoUpdate;
    }

    Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers ();
    if (event) {
        switch (event->type ()) {
            case QEvent::MouseMove: {
                // Toggle on MouseMove
                modifiers = static_cast<const QMouseEvent *>(event)->modifiers ();
                if (modifiers & Qt::ControlModifier) {
                    return QItemSelectionModel::ToggleCurrent | selectionBehaviorFlags ();
                }
            }
                break;
            case QEvent::MouseButtonPress: {
                modifiers = static_cast<const QMouseEvent *>(event)->modifiers ();
                const Qt::MouseButton button = static_cast<const QMouseEvent *>(event)->button ();
                const bool rightButtonPressed = (button & Qt::RightButton);
                const bool shiftKeyPressed = (modifiers & Qt::ShiftModifier);
                const bool controlKeyPressed = (modifiers & Qt::ControlModifier);
                const bool indexIsSelected = selectionModel ()->isSelected (index);
                if ((shiftKeyPressed || controlKeyPressed) && rightButtonPressed) {
                    return QItemSelectionModel::NoUpdate;
                }
                if (!shiftKeyPressed && !controlKeyPressed && indexIsSelected) {
                    return QItemSelectionModel::NoUpdate;
                }
                if (!index.isValid () && !rightButtonPressed && !shiftKeyPressed && !controlKeyPressed) {
                    return QItemSelectionModel::Clear;
                }
                if (!index.isValid ()) {
                    return QItemSelectionModel::NoUpdate;
                }
            }
                break;
            case QEvent::MouseButtonRelease: {
                // Clear on MouseButtonRelease if MouseButtonPress on selected item or empty area
                modifiers = static_cast<const QMouseEvent *>(event)->modifiers ();
                const Qt::MouseButton button = static_cast<const QMouseEvent *>(event)->button ();
                const bool rightButtonPressed = (button & Qt::RightButton);
                const bool shiftKeyPressed = (modifiers & Qt::ShiftModifier);
                const bool controlKeyPressed = (modifiers & Qt::ControlModifier);
                if (((index == pressedIndex && selectionModel ()->isSelected (index)) || !index.isValid ())
                    && state () != QAbstractItemView::DragSelectingState
                    && !shiftKeyPressed && !controlKeyPressed && !rightButtonPressed) {
                    return QItemSelectionModel::Clear | selectionBehaviorFlags ();
                }
                return QItemSelectionModel::NoUpdate;
            }
            case QEvent::KeyPress: {
                // NoUpdate on Key movement and Ctrl
                modifiers = static_cast<const QKeyEvent *>(event)->modifiers ();
                switch (static_cast<const QKeyEvent *>(event)->key ()) {
                    case Qt::Key_Backtab:
                        modifiers = modifiers & ~Qt::ShiftModifier; // special case for backtab
                    case Qt::Key_Down:
                    case Qt::Key_Up:
                    case Qt::Key_Left:
                    case Qt::Key_Right:
                    case Qt::Key_Home:
                    case Qt::Key_End:
                    case Qt::Key_PageUp:
                    case Qt::Key_PageDown:
                    case Qt::Key_Tab:
#ifdef QT_KEYPAD_NAVIGATION
                        return QItemSelectionModel::NoUpdate;
#else
                        if (modifiers & Qt::ShiftModifier) {
                            return QItemSelectionModel::Toggle | selectionBehaviorFlags ();
                        }
                        return QItemSelectionModel::NoUpdate;
#endif
                    case Qt::Key_Insert:
                    case Qt::Key_Select:
                    case Qt::Key_Space:
                        return QItemSelectionModel::Toggle | selectionBehaviorFlags ();
                    default:
                        break;
                }
            }
            default:
                break;
        }
    }

    if (modifiers & Qt::ShiftModifier) {
        return QItemSelectionModel::SelectCurrent | selectionBehaviorFlags ();
    }
    if (modifiers & Qt::ControlModifier) {
        return QItemSelectionModel::Toggle | selectionBehaviorFlags ();
    }
    if (state () == QAbstractItemView::DragSelectingState) {
        //when drag-selecting we need to clear any previous selection and select the current one
        return QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent | selectionBehaviorFlags ();
    }

    return QItemSelectionModel::Clear;
}

void FullView::keyPressEvent (QKeyEvent *event)
{
/*	QModelIndex index = currentIndex();
	switch(event->key())
	{
		case Qt::Key_Down:
			if (keyEvent->modifiers() & Qt::SHIFT)
			{
				if(index.flags() & Qt::ItemIsSelectable)
				{
					d->treeView->selectionModel()->select(QItemSelection(d->treeView->model()->index(index.row(), 0),
															d->treeView->model()->index(index.row(), d->treeView->model()->columnCount()-1)),
															QItemSelectionModel::Toggle);
				}
				if (index.row() < d->treeView->model()->rowCount()-1)
				{
					d->treeView->selectionModel()->setCurrentIndex(d->treeView->model()->index(index.row()+1, 0),
																	QItemSelectionModel::NoUpdate);
				}
			}
			break;
		case Qt::Key_Up:
			if (keyEvent->modifiers() & Qt::SHIFT)
			{
				if(index.flags() & Qt::ItemIsSelectable)
				{
					d->treeView->selectionModel()->select(QItemSelection(d->treeView->model()->index(index.row(), 0),
															d->treeView->model()->index(index.row(), d->treeView->model()->columnCount()-1)),
															QItemSelectionModel::Toggle);
				}
				if (index.row()>0)
				{
					d->treeView->selectionModel()->setCurrentIndex(d->treeView->model()->index(index.row()-1, 0),
																	QItemSelectionModel::NoUpdate);
				}
			}
			break;
		default:
			break;
	}
*/
    QTreeView::keyPressEvent (event);
}

void FullView::mousePressEvent (QMouseEvent *event)
{
    QTreeView::mousePressEvent (event);

    pressedIndex = indexAt (event->pos ());
}
