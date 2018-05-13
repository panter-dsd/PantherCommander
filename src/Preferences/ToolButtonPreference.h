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
    QString command_;
    QString parameters_;
    QString workDir_;
    QString iconFile_;
    int iconNumber_;
    QIcon icon_;
    QString caption_;
};

class ToolButtonPreference : public QWidget
{
Q_OBJECT

public:
    explicit ToolButtonPreference (QWidget *parent = nullptr);

    virtual ~ToolButtonPreference ();

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

private:
    QLabel *commandLabel_;
    QLineEdit *commandEdit_;
    QToolButton *commandButton_;
    QToolButton *commandButton2_;

    QLabel *parametersLabel_;
    QLineEdit *parametersEdit_;

    QLabel *workDirLabel_;
    QLineEdit *workDirEdit_;
    QToolButton *workDirButton_;

    QLabel *iconFileLabel_;
    QLineEdit *iconFileEdit_;
    QToolButton *iconFileButton_;

    QLabel *iconLabel_;
    QListWidget *iconList_;

    QLabel *captionLabel_;
    QLineEdit *captionEdit_;

    ToolBarButton buttons_;
};

