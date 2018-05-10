#pragma once

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtWidgets/QWidget>

class QAbstractItemDelegate;

class QFileIconProvider;

class QItemSelection;

//
class QLineEdit;

class FileWidgetPrivate;

class FileWidget : public QWidget
{
Q_OBJECT

public:
    explicit FileWidget (QWidget *parent = 0);

    explicit FileWidget (const QString &path, QWidget *parent = 0);

    virtual ~FileWidget ();

    QByteArray saveState () const;

    bool restoreState (const QByteArray &state);

    QDir directory () const;

    inline void setDirectory (const QDir &directory);

    QDir::Filters filter () const;

    void setFilter (QDir::Filters filters);

    QStringList history () const;

    void setHistory (const QStringList &paths);

    QFileIconProvider *iconProvider () const;

    void setIconProvider (QFileIconProvider *provider);

    QAbstractItemDelegate *itemDelegate () const;

    void setItemDelegate (QAbstractItemDelegate *delegate);

    QString currentFile () const;

    void setCurrentFile (const QString &fileName);

    QStringList selectedFiles () const;

    inline void selectFile (const QString &fileName);

    void selectFile (const QString &fileName, bool clearSelection);

    void clearSelection ();

public Q_SLOTS:

    void setDirectory (const QString &directory);

protected:
    void changeEvent (QEvent *event);

    bool eventFilter (QObject *object, QEvent *event);

Q_SIGNALS:

    void directoryEntered (const QString &path);

//**//
public Q_SLOTS:

    void gotoHome ();

    void gotoRoot ();

    void cdUP ();

    void slotRename ();

/*1private Q_SLOTS:
	void slotCloseEditor();

private:
	QLineEdit*						qleRenameEditor;
	bool							selectOnlyFileName;*/
//**//

private:
    Q_DISABLE_COPY(FileWidget)

    Q_DECLARE_PRIVATE(FileWidget)

    FileWidgetPrivate *const d_ptr;
    Q_PRIVATE_SLOT(d_func (), void _q_rowsInserted (const QModelIndex &parent, int start, int end))
    Q_PRIVATE_SLOT(d_func (), void _q_rowsAboutToBeRemoved (const QModelIndex &parent, int start, int end))
    Q_PRIVATE_SLOT(d_func (),
                   void _q_selectionChanged (const QItemSelection &selected, const QItemSelection &deselected))
    Q_PRIVATE_SLOT(d_func (), void _q_showHeader (QAction *action))
    Q_PRIVATE_SLOT(d_func (), void _q_showContextMenu (const QPoint &position))
    Q_PRIVATE_SLOT(d_func (), void _q_addressChanged ())
    Q_PRIVATE_SLOT(d_func (), void _q_activate (const QModelIndex &index))
    Q_PRIVATE_SLOT(d_func (), void _q_pathChanged (const QString &newPath))
    Q_PRIVATE_SLOT(d_func (), void _q_navigateBackward ())
    Q_PRIVATE_SLOT(d_func (), void _q_navigateForward ())
    Q_PRIVATE_SLOT(d_func (), void _q_navigateToParent ())
    Q_PRIVATE_SLOT(d_func (), void _q_navigateToHome ())
    Q_PRIVATE_SLOT(d_func (), void _q_navigateToRoot ())
};

inline void FileWidget::setDirectory (const QDir &directory)
{
    setDirectory (directory.absolutePath ());
}

inline void FileWidget::selectFile (const QString &fileName)
{
    selectFile (fileName, true);
}

