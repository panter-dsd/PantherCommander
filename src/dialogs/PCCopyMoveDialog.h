#pragma once

#include <QDialog>

class QLabel;

class QLineEdit;

class QComboBox;

class QDialogButtonBox;

class QStandardItemModel;

class PCCopyMoveDialog : public QDialog
{
Q_OBJECT
private:
    QLabel *qlSources;
    QLineEdit *qleDest;
    QLabel *qlQueue;
    QComboBox *qcbQueue;
    QDialogButtonBox *qdbbButtons;

    QStringList qslSource;
    QString qsDest;
    QString qsOperation;
public:
    explicit PCCopyMoveDialog (QWidget *parent = 0);

    virtual ~PCCopyMoveDialog ()
    {
    }

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
};

