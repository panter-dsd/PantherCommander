#ifndef PANTHERVIEWER_H
#define PANTHERVIEWER_H

//
class QTabWidget;

class QMenuBar;

class QToolBar;

class QAction;

class QActionGroup;
//
#include <QMainWindow>
#include "plainview.h"
#include "abstractview.h"

//
class PantherViewer : public QMainWindow
{
Q_OBJECT
private:
    QStringList qslFiles;
    QTabWidget *qtabwTabs;
    QMenuBar *qmbMainMenu;
    QToolBar *qtbarMainToolBar;
    QActionGroup *qagCodecs;

    QAction *actionExit;
    QAction *actionCloseCurrentTab;

public:
    PantherViewer (QWidget *parent = 0, Qt::WindowFlags f = 0);

    ~PantherViewer ();

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
};

#endif // PANTHERVIEWER_H
