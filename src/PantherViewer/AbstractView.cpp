#include <QtGui>
#include "AbstractView.h"

AbstractView::AbstractView (const QString &fileName, QWidget *parent)
    : QWidget (parent)
{
    qsFileName = fileName;
}
