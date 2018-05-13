#include <QtCore/QTextCodec>

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QVBoxLayout>

#include "PlainView.h"

Frame::Frame (QWidget *parent)
    : QFrame (parent)
{
}

Frame::~Frame ()
{
}

void Frame::paintEvent (QPaintEvent *event)
{
    QPainter painter (this);
#ifndef Q_CC_MSVC
#warning "This is only white, but if other color?"
#endif
    painter.fillRect (this->rect (), Qt::white);
    for (int i = 0; i < text_.count (); i++) {
        painter.drawText (rect_.left (), this->fontMetrics ().height () * (i + 1) + rect_.top (), text_.at (i));
    }
    event->accept ();
}

void Frame::setRect (const QRect &rect)
{
    rect_ = rect;
}
//
PlainView::PlainView (const QString &fileName, QWidget *parent)
    : AbstractView (fileName, parent)
{
    splitSymbol_ = QChar (0x0D);
    splitSymbol_ += QChar (0x0A);
    createControls ();
    textCodec_ = QTextCodec::codecForLocale ();
    file_ = new QFile (fileName);
    if (!file_->open (QFile::ReadOnly)) {
        QMessageBox::critical (this, tr ("Error open"), file_->errorString ());
    }
    stringCount_ = file_->size ();
}

//
PlainView::~PlainView ()
{
    file_->close ();
    delete file_;
}

//
void PlainView::createControls ()
{
    frame_ = new Frame (this);
    frame_->setFrameShape (QFrame::Panel);
    frame_->setFrameShadow (QFrame::Sunken);
    frame_->setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Ignored);
    scrollArea_ = new QScrollArea (this);
    scrollArea_->setWidget (frame_);
#ifndef Q_CC_MSVC
#warning "This is only white, but if other color?"
#endif
    scrollArea_->setBackgroundRole (QPalette::Light);
    scrollArea_->verticalScrollBar ()->setSingleStep (1);

    QVBoxLayout *layout = new QVBoxLayout ();
    layout->addWidget (scrollArea_);
    this->setLayout (layout);
    connect (scrollArea_->verticalScrollBar (), &QScrollBar::valueChanged, this, &PlainView::slotScroll);
}

//
void PlainView::slotScroll ()
{
    file_->seek (scrollArea_->verticalScrollBar ()->value ());

    QByteArray buf = file_->read (256);
    int pos = buf.indexOf (splitSymbol_, 0);
    scrollArea_->verticalScrollBar ()->blockSignals (true);
    if (pos > 0) {
        scrollArea_->verticalScrollBar ()->setValue (scrollArea_->verticalScrollBar ()->value () + pos);
    } else {
        scrollArea_->verticalScrollBar ()->setValue (scrollArea_->verticalScrollBar ()->value () + 256);
    }
    scrollArea_->verticalScrollBar ()->blockSignals (false);
    slotReadFile ();
}

//
bool PlainView::event (QEvent *event)
{
    if (event->type () == QEvent::Resize) {
        frame_->resize (frame_->width (),
                         stringCount_ * frame_->fontMetrics ().height () + frame_->fontMetrics ().height ());
        if (frame_->height () < scrollArea_->height ()) {
            frame_->resize (frame_->width (), scrollArea_->heightMM ());
        }
        slotReadFile ();
    }
    return QWidget::event (event);
}

//
void PlainView::slotReadFile ()
{
    file_->seek (scrollArea_->verticalScrollBar ()->value ());
    frame_->text_.clear ();
    frame_->setRect (QRect (10, scrollArea_->verticalScrollBar ()->value (), 0, 0));
    frame_->resize (frame_->fontMetrics ().width ("W") * 256, frame_->height ());
    while (frame_->text_.count () < scrollArea_->height () / frame_->fontMetrics ().height ()) {
        QByteArray buf = file_->read (256);//.simplified();
        for (int i = 0; i < buf.count (); i++) {
            if ((unsigned char) (buf[i]) < 32) {
                if (!splitSymbol_.contains (QChar (buf[i]))) {
                    buf[i] = '*';
                }
            }
        }
        int pos = buf.lastIndexOf (splitSymbol_);
        if (pos > 0) {
            file_->seek (file_->pos () - buf.size () + pos + splitSymbol_.count ());
            buf.resize (pos);
        }
        QString str = textCodec_->toUnicode (buf);
        frame_->text_.append (str.split (splitSymbol_, QString::KeepEmptyParts));
    }
    frame_->update ();
}

//
void PlainView::setTextCodec (const QString &codecName)
{
    textCodec_ = QTextCodec::codecForName (codecName.toLatin1 ());
    slotReadFile ();
}

//
QString PlainView::textCodec ()
{
    return QString (textCodec_->name ());
}

//
