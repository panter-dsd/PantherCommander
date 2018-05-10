#pragma once

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

private:
    QComboBox *fileComboBox_;
    QComboBox *textComboBox_;
    QComboBox *directoryComboBox_;
    QLabel *fileLabel_;
    QLabel *textLabel_;
    QLabel *directoryLabel_;
    QLabel *filesFoundLabel_;
    QPushButton *browseButton_;
    QPushButton *findButton_;
    QTableWidget *filesTable_;
    QTabWidget *tabber_;
};

