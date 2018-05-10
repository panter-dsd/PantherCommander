#ifndef ABSTRACTVIEW_H
#define ABSTRACTVIEW_H

#include <QWidget>

class AbstractView : public QWidget
{
Q_OBJECT

private:
    QString qsFileName;
public:
    AbstractView (const QString &fileName, QWidget *parent = 0);

    static bool isOpen (const QString & /*fileName*/)
    {
        return false;
    }

    virtual void setTextCodec (const QString & /*codecName*/)
    {
        ;
    }

    virtual QString textCodec ()
    {
        return "System";
    }
};

#endif // ABSTRACTVIEW_H
