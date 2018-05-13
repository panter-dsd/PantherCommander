#pragma once

class QFile;

class QByteArray;

class QScrollArea;

class QTextCodec;

#include <QtWidgets/QFrame>

#include "AbstractView.h"

class Frame : public QFrame
{
Q_OBJECT

public:
    explicit Frame (QWidget *parent = nullptr);

    virtual ~Frame ();

    void setRect (const QRect &rect);

protected:
    virtual void paintEvent (QPaintEvent *event) override;

public:
    QStringList text_;
    QRect rect_;
};

class PlainView : public AbstractView
{
Q_OBJECT
public:
    explicit PlainView (const QString &fileName, QWidget *parent = nullptr);

    virtual ~PlainView ();

    void setTextCodec (const QString &codecName);

    QString textCodec ();

private:
    void createControls ();

protected:
    virtual bool event (QEvent *event) override ;

private Q_SLOTS:

    void slotReadFile ();

    void slotScroll ();

private:
    Frame *frame_;
    QScrollArea *scrollArea_;
    QFile *file_;
    QTextCodec *textCodec_;
    int stringCount_;
    QString splitSymbol_;
};

