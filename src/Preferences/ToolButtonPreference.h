#pragma once

class QLabel;

class QLineEdit;

class QListWidget;

class QToolButton;

class QAction;

#include <QtWidgets/QWidget>
#include <QtGui/QIcon>

struct ToolBarButton
{
    QString qsCommand;
    QString qsParams;
    QString qsWorkDir;
    QString qsIconFile;
    int iconNumber;
    QIcon qiIcon;
    QString qsCaption;
};

class ToolButtonPreference : public QWidget
{
Q_OBJECT
private:
    QLabel *qlCommand;
    QLineEdit *qleCommand;
    QToolButton *qtbPCCommand;
    QToolButton *qtbCommand;

    QLabel *qlParams;
    QLineEdit *qleParams;

    QLabel *qlWorkDir;
    QLineEdit *qleWorkDir;
    QToolButton *qtbWorkDir;

    QLabel *qlIconFile;
    QLineEdit *qleIconFile;
    QToolButton *qtbIconFile;

    QLabel *qlIcon;
    QListWidget *qlwIcons;

    QLabel *qlCaption;
    QLineEdit *qleCaption;

    ToolBarButton stbbButton;

public:
    ToolButtonPreference (QWidget *parent = 0);

    void setButton (const ToolBarButton &button);

    ToolBarButton getButton ();

    static ToolBarButton getButton (const QString &command);

private:
    void createControls ();

    void setLayouts ();

    void setConnects ();

    static QIcon getIcon (const QString &fileName, int number);

private Q_SLOTS:

    void slotChooseCommandFile ();

    void slotChoosePCCommand ();

    void slotChooseWorkDir ();

    void slotChooseIconFile ();

    void slotGetIconList (const QString &iconFileName);
};
