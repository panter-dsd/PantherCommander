#pragma once

#include <QtWidgets/QWidget>

class QLabel;

class QActionGroup;

class QPushButton;

class QToolButton;

class QComboBox;

class FileWidget;

class TabBar;

class FilePanel : public QWidget
{
Q_OBJECT

private:
    TabBar *qtabbTabs;
    FileWidget *qflvCurrentFileList;
    QLabel *qlDiscInformation;
    QToolButton *qtbDriveButton;
    QComboBox *qcbDriveComboBox;

    int m_currentIndex;
    int timerID;

public:
    explicit FilePanel (QWidget *parent = 0);

    virtual ~FilePanel ();

    void setDisc (const QString &name);

    QString path () const;

    QString currentFileName () const;

    QStringList selectedFiles () const;

    void clearSelection ();

    void saveSettings ();

    void loadSettings ();

protected:
    void timerEvent (QTimerEvent *event);

Q_SIGNALS:

    void pathChanged (const QString &);

private Q_SLOTS:

    void slotPathChanged (const QString &path);

    void slotAddTab ();

    void slotCurrentTabChange (int);

    void slotSetDisc ();

    void slotInformationChanged ();

    void slotSelectDisc ();

public Q_SLOTS:

    void setPath (const QString &path);

private:
    void createWidgets ();

    int addTab (const QString &tabPath, bool bSetCurrent = true);

    void updateDirInformation ();

    void updateDiscInformation ();

    static QString size (qint64 bytes);
};

