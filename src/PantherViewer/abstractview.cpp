#include <QtGui>
#include "abstractview.h"

AbstractView::AbstractView (const QString &fileName, QWidget *parent)
    : QWidget (parent)
{
    qsFileName = fileName;
}
