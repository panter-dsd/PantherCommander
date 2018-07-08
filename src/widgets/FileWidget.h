#pragma once

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtWidgets/QWidget>

class QAbstractItemDelegate;

class QFileIconProvider;

class QItemSelection;


class QLineEdit;

class FileWidgetPrivate;

class FileWidget : public QWidget
{
Q_OBJECT

public:
    explicit FileWidget (QWidget *parent = 0);

    virtual ~FileWidget ();

    QByteArray saveState () const;

    bool restoreState (const QByteArray &state);

    QDir directory () const;

    QStringList history () const;

    void setHistory (const QStringList &paths);

    QString currentFile () const;

    QStringList selectedFiles () const;

    void clearSelection ();

public Q_SLOTS:

    void setDirectory (const QString &directory);

protected:
    void changeEvent (QEvent *event);

    bool eventFilter (QObject *object, QEvent *event);

Q_SIGNALS:

    void directoryEntered (const QString &path);

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


