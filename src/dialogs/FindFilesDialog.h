#ifndef FINDFILESDIALOG_H
#define FINDFILESDIALOG_H

#include <QtCore/QList>

#include <QtWidgets/QDialog>

class QFileInfo;

typedef QList<QFileInfo> QFileInfoList;

class QComboBox;

class QLabel;

class QPushButton;

class QTabWidget;

class QTableWidget;

class FindFilesDialog : public QDialog
{
Q_OBJECT

public:
    explicit FindFilesDialog (QWidget *parent = 0);

    virtual ~FindFilesDialog ();

private Q_SLOTS:

    void browse ();

    void find ();

    void _slot_cellActivated (int row, int column);

private:
    QFileInfoList findFiles (const QFileInfoList &files, const QString &text);

    void showFiles (const QFileInfoList &files);

    QComboBox *fileComboBox;
    QComboBox *textComboBox;
    QComboBox *directoryComboBox;
    QLabel *fileLabel;
    QLabel *textLabel;
    QLabel *directoryLabel;
    QLabel *filesFoundLabel;
    QPushButton *browseButton;
    QPushButton *findButton;
    QTableWidget *filesTable;
    QTabWidget *tabber;
};

#endif // FINDFILESDIALOG_H
