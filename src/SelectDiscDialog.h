#pragma once

class QListWidget;

class QListWidgetItem;

#include <QtWidgets/QDialog>

class SelectDiscDialog : public QDialog
{
Q_OBJECT

public:
    SelectDiscDialog (QWidget *parent = 0);

    void setPath (const QString &path);

private:
    QListWidget *discList;

Q_SIGNALS:

    void setectedDisc (const QString &disc);

private Q_SLOTS:

    void selectDisc (QListWidgetItem *item);
};

