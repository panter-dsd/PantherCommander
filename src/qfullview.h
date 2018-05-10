#ifndef QFULLVIEW_H
#define QFULLVIEW_H

#include <QtWidgets/QTreeView>

class QFullView : public QTreeView
{
Q_OBJECT

private:
//	bool						isRightMouseButtonPressed;
//	bool						isMouseSelect;

//	bool						bFocused;

public:
    explicit QFullView (QWidget *parent = 0);

    virtual ~QFullView ();

protected:
    void drawRow (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QRect visualRect (const QModelIndex &index) const;

#ifndef QT_NO_DRAGANDDROP

    void startDrag (Qt::DropActions supportedActions);

#endif

    QItemSelectionModel::SelectionFlags selectionBehaviorFlags () const;

    QItemSelectionModel::SelectionFlags selectionCommand (const QModelIndex &index, const QEvent *event = 0) const;

    void keyPressEvent (QKeyEvent *event);

    void mousePressEvent (QMouseEvent *event);

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
