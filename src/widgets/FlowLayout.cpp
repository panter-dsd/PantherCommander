#include <QtWidgets>

#include "FlowLayout.h"

FlowLayout::FlowLayout (QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout (parent)
    , horisontalSpace_ (hSpacing)
    , verticalSpace_ (vSpacing)
{
    setMargin (margin);
}

FlowLayout::FlowLayout (int margin, int hSpacing, int vSpacing)
    : FlowLayout (nullptr, margin, hSpacing, vSpacing)
{
}

FlowLayout::~FlowLayout ()
{
    qDeleteAll (itemList_);
}

void FlowLayout::addItem (QLayoutItem *item)
{
    itemList_.append (item);
}

int FlowLayout::horizontalSpacing () const
{
    return horisontalSpace_ >= 0 ? horisontalSpace_ : smartSpacing (QStyle::PM_LayoutHorizontalSpacing);
}

int FlowLayout::verticalSpacing () const
{
    return verticalSpace_ >= 0 ? verticalSpace_ : smartSpacing (QStyle::PM_LayoutVerticalSpacing);
}

int FlowLayout::count () const
{
    return itemList_.size ();
}

QLayoutItem *FlowLayout::itemAt (int index) const
{
    return itemList_.value (index);
}

QLayoutItem *FlowLayout::takeAt (int index)
{
    return index >= 0 && index < itemList_.size () ? itemList_.takeAt (index) : nullptr;
}

Qt::Orientations FlowLayout::expandingDirections () const
{
    return 0;
}

bool FlowLayout::hasHeightForWidth () const
{
    return true;
}

int FlowLayout::heightForWidth (int width) const
{
    return doLayout (QRect (0, 0, width, 0), true);
}

void FlowLayout::setGeometry (const QRect &rect)
{
    QLayout::setGeometry (rect);
    doLayout (rect, false);
}

QSize FlowLayout::sizeHint () const
{
    return minimumSize ();
}

QSize FlowLayout::minimumSize () const
{
    QSize size {};
    for (QLayoutItem *item : itemList_) {
        size = size.expandedTo (item->minimumSize ());
    }

    return size + QSize (2 * margin (), 2 * margin ());
}

int FlowLayout::doLayout (const QRect &rect, bool testOnly) const
{
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    getContentsMargins (&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted (+left, +top, -right, -bottom);
    int x = effectiveRect.x ();
    int y = effectiveRect.y ();
    int lineHeight = 0;

    for (QLayoutItem *item : itemList_) {
        QWidget *w = item->widget ();
        int spaceX = horizontalSpacing ();
        if (spaceX == -1) {
            spaceX = w->style ()->layoutSpacing (QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        }
        int spaceY = verticalSpacing ();
        if (spaceY == -1) {
            spaceY = w->style ()->layoutSpacing (QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        }
        int nextX = x + item->sizeHint ().width () + spaceX;
        if (nextX - spaceX > effectiveRect.right () && lineHeight > 0) {
            x = effectiveRect.x ();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint ().width () + spaceX;
            lineHeight = 0;
        }

        if (!testOnly) {
            item->setGeometry (QRect (QPoint (x, y), item->sizeHint ()));
        }

        x = nextX;
        lineHeight = qMax (lineHeight, item->sizeHint ().height ());
    }

    return y + lineHeight - rect.y () + bottom;
}

int FlowLayout::smartSpacing (QStyle::PixelMetric pm) const
{
    QObject *p = parent ();
    if (!p) {
        return -1;
    } else if (p->isWidgetType ()) {
        return (static_cast<QWidget *>(p))->style ()->pixelMetric (pm, 0, static_cast<QWidget *>(p));
    } else {
        return static_cast<QLayout *>(p)->spacing ();
    }
}

