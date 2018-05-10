#ifndef PLAINVIEW_H
#define PLAINVIEW_H

//
class QFile;

class QByteArray;

class QScrollArea;

class QTextCodec;
//
#include "abstractview.h"
#include <QFrame>

//
class Frame : public QFrame
{
Q_OBJECT

public:
    QStringList qslText;
    QRect qrRect;
public:
    Frame (QWidget *parent = 0);

    virtual ~Frame ()
    {
        ;
    }

    void setRect (const QRect &rect)
    {
        qrRect = rect;
    }

protected:
    void paintEvent (QPaintEvent * /* event */);
};

//
class PlainView : public AbstractView
{
Q_OBJECT
private:
    Frame *m_frame;
    QScrollArea *qsbScroll;
    QFile *qfFile;
    QTextCodec *qtcCodec;
    int stringCount;
    QString qsSplitSymbol;
public:
    PlainView (const QString &fileName, QWidget *parent = 0);

    ~PlainView ();

    static bool isOpen (const QString & /*fileName*/)
    {
        return true;
    }

    void setTextCodec (const QString &codecName);

    QString textCodec ();

private:
    void createControls ();

protected:
    bool event (QEvent *event);

private Q_SLOTS:

    void slotReadFile ();

    void slotScroll ();
};

#endif // PLAINVIEW_H
