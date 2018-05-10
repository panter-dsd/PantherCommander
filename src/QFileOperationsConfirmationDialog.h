#ifndef QFILEOPERATIONSCONFIRMATIONDIALOG_H
#define QFILEOPERATIONSCONFIRMATIONDIALOG_H

//
class QVBoxLayout;
//

#include <QtCore/QFlags>
#include <QDialog>

//
class QFileOperationsConfirmationDialog : public QDialog
{
Q_OBJECT
public:
    enum StandardButton
    {
        Ovewrite = 0x1,
        OvewriteAll = 0x2,
        Skip = 0x4,
        SkipAll = 0x8,
        Cancel = 0x10,
        OvewriteAllOlder = 0x20,
        OvewriteAllNew = 0x40,
        OvewriteAllSmallest = 0x80,
        OvewriteAllLager = 0x100,
        Rename = 0x200,
        Delete = 0x400,
        All = 0x800,
        Retry = 0x1000
    };
private:
    int result;
    QVBoxLayout *qvblMainLayout;
public:
    QFileOperationsConfirmationDialog (QWidget *parent = 0);

    void setButtons (int buttons);

    void setFiles (const QString &source, const QString &dest);

    void setText (const QString &text);

    int getResult ()
    {
        return result;
    }

private Q_SLOTS:

    void slotButtonClick ();
};

#endif // QFILEOPERATIONSCONFIRMATIONDIALOG_H
