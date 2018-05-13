#pragma once

class QTabWidget;

class QMenuBar;

class QToolBar;

class QAction;

class QActionGroup;

#include <QtWidgets/QMainWindow>

class PantherViewer : public QMainWindow
{
Q_OBJECT

public:
    PantherViewer (QWidget *parent = nullptr, Qt::WindowFlags f = 0);

    virtual ~PantherViewer ();

    void viewFile (const QString &fileName);

private:
    void createControls ();

    void setLayouts ();

    void setConnects ();

    void createActions ();

    void createMenu ();

    void createToolBar ();

    void loadSettings ();

    void saveSettings ();

    QStringList codecsList ();

private Q_SLOTS:

    void slotSetEncoding ();

    void slotCurrentTabChanged (int index);

    void slotCloseCurrentTab ();

private:
    QStringList files_;
    QTabWidget *tabs_;
    QMenuBar *mainMenu_;
    QToolBar *mainToolBar_;
    QActionGroup *codecs_;

    QAction *actionExit_;
    QAction *actionCloseCurrentTab_;
};

