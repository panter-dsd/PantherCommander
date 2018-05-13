#pragma once

#include <QDialog>

class QLabel;

class QLineEdit;

class QComboBox;

class QDialogButtonBox;

class QStandardItemModel;

class CopyMoveDialog : public QDialog
{
Q_OBJECT
public:
    explicit CopyMoveDialog (QWidget *parent = 0);

    virtual ~CopyMoveDialog ();

    void setSource (const QStringList &fileList);

    void setDest (const QString &dest);

    QString dest ();

    void setOperation (const QString &operation);

    void setQueueModel (QStandardItemModel *model);

    int queueIndex () const;

private:
    void createControls ();

    void setLayouts ();

    void updateText ();

private:
    QLabel *sourcesLabel_;
    QLineEdit *destEdit_;
    QLabel *queueLabel_;
    QComboBox *queueComboBox_;
    QDialogButtonBox *buttons_;

    QStringList source_;
    QString dest_;
    QString operation_;
};

