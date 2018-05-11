#pragma once

#include <QMainWindow>
#include <QMap>
#include <QPointer>

class QLabel;

class QComboBox;

class QAction;

class QFrame;

class QStandardItemModel;

class QPushButton;

class QSplitter;

class QToolBar;

class DriveBar;

class QFilePanel;

class QFileOperationsDialog;

class QFileOperationsDialog;

class PantherViewer;

class PCToolBar;

class ToolBarButton;

#include "QFileOperationsThread.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
    Q_PROPERTY(QStringList commandHistory
                   READ
                       commandHistory
                   WRITE
                   setCommandHistory)

public:
    explicit MainWindow (QWidget *parent = 0);

    virtual ~MainWindow ();

    QStringList commandHistory () const;

    void setCommandHistory (const QStringList &commandHistory);

private:
    void createWidgets ();

    void createActions ();

    void createMenus ();

    void createCommandButtons ();

    void saveSettings ();

    void loadSettings ();

    inline void connectToolBar (PCToolBar *toolBar);

    QMenu *createToolBarsMenu (PCToolBar *currentToolBar);

protected:
    void resizeEvent (QResizeEvent *event);

    void dropEvent (QDropEvent *event);

    void dragMoveEvent (QDragMoveEvent *event);

    void dragEnterEvent (QDragEnterEvent *event);

    QFileOperationsDialog *addJob (QFileOperationsDialog *queue, QFileOperationsThread::FileOperation operation,
                                   const QStringList &parameters);

private Q_SLOTS:

    void showSplitterContextMenu (const QPoint &pos);

    void slotResizeSplitter ();

    void slotChangedFocus ();

    void slotRunCommand ();

    void slotSetDisc (const QString &path);

    void slotView (const QString &fileName = QString ());

    void slotCopy (const QString &destDir = QString (), const QStringList &fileList = QStringList ());

    void slotMove (const QString &destDir = QString (), const QStringList &fileList = QStringList ());

    void slotRemove (const QStringList &fileList = QStringList ());

    void slotMkDir ();

    void slotPathChanged (const QString &path);

    void slotCpCurFileName2Cmd ();

    void slotCpCurFileNameWhithPath2Cmd ();

    void slotRunConsole ();

    void slotRename ();

    void slotQueueFinished ();

    void slotQueueChanged ();

    void slotAddToolBar ();

    void slotRemoveToolBar ();

    void slotRenameToolBar ();

    void slotFindFiles ();

    void slotPreferences ();

    void toolBarActionExecute (const ToolBarButton &actionButton);

    void cdExecute (const QString &path);

    void slotToolBarContextMenu (const QPoint &pos);

    /* ** TESTING PURPOSES ONLY ** */
    void slotTestingFileDialog ();

#ifdef Q_WS_WIN
    void slotTestingEnableNTFSPermissionLookup(bool enable);
#endif

private:
    DriveBar *leftDriveBar_;
    DriveBar *rightDriveBar_;
    QFilePanel *leftPanel_;
    QFilePanel *rightPanel_;
    QFilePanel *focusedFilePanel_;
    QSplitter *panelsSplitter_;
    QLabel *consolePath_;
    QComboBox *consoleCommandComboBox_;
    QFrame *commandButtonsFrame_;
    QPointer<PantherViewer> viewer_;
    QStandardItemModel *qeueuModel_;
    QList<QFileOperationsDialog *> queueList_;
    QList<PCToolBar *> toolBars_;

    QPushButton *runConsoleButton_;
    QPushButton *viewButton_;
    QPushButton *editButton_;
    QPushButton *copyButton_;
    QPushButton *moveButton_;
    QPushButton *mkDirButton_;
    QPushButton *removeButton_;
    QPushButton *exitButton_;

    QAction *actionCpCurFileName2Cmd_;
    QAction *actionCpCurFileNameWhithPath2Cmd_;
    QAction *actionClearCmd_;

    QAction *actionRunConsole_;
    QAction *actionView_;
    QAction *actionEdit_;
    QAction *actionCopy_;
    QAction *actionMove_;
    QAction *actionRename_;
    QAction *actionMkDir_;
    QAction *actionRemove_;
    QAction *actionExit_;
    QAction *actionFindFiles_;
    QAction *actionPreferences_;
};

