#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

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

class SToolBarButton;

#include "QFileOperationsThread.h"

class MainWindowImpl : public QMainWindow
{
Q_OBJECT
    Q_PROPERTY(QStringList commandHistory
                   READ
                   commandHistory
                   WRITE
                   setCommandHistory)

private:
    DriveBar *qdbDriveBarLeft;
    DriveBar *qdbDriveBarRight;
    QFilePanel *qflvLeftPanel;
    QFilePanel *qflvRightPanel;
    QFilePanel *qfpFocusedFilePanel;
    QSplitter *qsplitSplitter;
    QLabel *qlConsolePath;
    QComboBox *qcbConsoleCommand;
    QFrame *qfCommandButtons;
    QPointer<PantherViewer> pvViewer;
    QStandardItemModel *qsimQeueuModel;
    QList<QFileOperationsDialog *> qlQueueList;
    QList<PCToolBar *> qlpcToolBars;

    QPushButton *qpbRunConsole;
    QPushButton *qpbView;
    QPushButton *qpbEdit;
    QPushButton *qpbCopy;
    QPushButton *qpbMove;
    QPushButton *qpbMkDir;
    QPushButton *qpbRemove;
    QPushButton *qpbExit;

    QAction *actionCpCurFileName2Cmd;
    QAction *actionCpCurFileNameWhithPath2Cmd;
    QAction *actionClearCmd;

    QAction *actionRunConsole;
    QAction *actionView;
    QAction *actionEdit;
    QAction *actionCopy;
    QAction *actionMove;
    QAction *actionRename;
    QAction *actionMkDir;
    QAction *actionRemove;
    QAction *actionExit;
    QAction *actionFindFiles;
    QAction *actionPreferences;
public:
    explicit MainWindowImpl (QWidget *parent = 0);

    virtual ~MainWindowImpl ();

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

    void toolBarActionExecute (const SToolBarButton &actionButton);

    void cdExecute (const QString &path);

    void slotToolBarContextMenu (const QPoint &pos);

    /* ** TESTING PURPOSES ONLY ** */
    void slotTestingFileDialog ();

#ifdef Q_WS_WIN
    void slotTestingEnableNTFSPermissionLookup(bool enable);
#endif
};

#endif // MAINWINDOWIMPL_H
