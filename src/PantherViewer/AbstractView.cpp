#include "AbstractView.h"

AbstractView::AbstractView (const QString &fileName, QWidget *parent)
    : QWidget (parent)
    , fileName_ (fileName)
{
}

AbstractView::~AbstractView ()
{

}

bool AbstractView::isOpen (const QString &fileName)
{
    Q_UNUSED (fileName)
    return false;
}

void AbstractView::setTextCodec (const QString &codecName)
{
    Q_UNUSED (codecName)
}

QString AbstractView::textCodec ()
{
    return "System";
}
