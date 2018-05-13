#pragma once

#include <QtWidgets/QWidget>

class AbstractView : public QWidget
{
Q_OBJECT

public:
    AbstractView (const QString &fileName, QWidget *parent = nullptr);

    virtual ~AbstractView ();

    static bool isOpen (const QString &fileName);

    virtual void setTextCodec (const QString &codecName);

    virtual QString textCodec ();

private:
    QString fileName_;
};

