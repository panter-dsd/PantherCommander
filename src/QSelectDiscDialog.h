#pragma once

class QListWidget;

class QListWidgetItem;

#include <QtWidgets/QDialog>

class QSelectDiscDialog : public QDialog
{
Q_OBJECT

public:
    QSelectDiscDialog (QWidget *parent = 0);

    void setPath (const QString &path);

private:
    QListWidget *discList;

Q_SIGNALS:

    void setectedDisc (const QString &disc);

private Q_SLOTS:

    void selectDisc (QListWidgetItem *item);
};

