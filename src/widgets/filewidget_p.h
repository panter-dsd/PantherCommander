/********************************************************************
* Copyright (C) PanteR
*-------------------------------------------------------------------
*
* Panther Commander is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* Panther Commander is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Panther Commander; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301 USA
*-------------------------------------------------------------------
* Project:		Panther Commander
* Author:		Ritt K.
* Contact:		ritt.ks@gmail.com
*******************************************************************/

#ifndef FILEWIDGET_P_H
#define FILEWIDGET_P_H

#include "filewidget.h"

#include <QtWidgets/QAction>
#include <QtCore/QAbstractProxyModel>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeView>

#include <QtGui/QFocusEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>

#include "qfilelistmodel.h"

class AddressLineEdit : public QLineEdit
{
//	Q_OBJECT

public:
    explicit AddressLineEdit (QWidget *parent = 0)
        : QLineEdit (parent)
    {
        setFocusPolicy (Qt::ClickFocus);
        setCursor (Qt::ArrowCursor);
    }

    virtual ~AddressLineEdit ()
    {
    }

protected:
    void focusInEvent (QFocusEvent *event)
    {
        setCursor (Qt::IBeamCursor);
        QLineEdit::focusInEvent (event);
    }

    void focusOutEvent (QFocusEvent *event)
    {
//		setCursor(Qt::PointingHandCursor);
        setCursor (Qt::ArrowCursor);
        QLineEdit::focusOutEvent (event);
    }

    void mousePressEvent (QMouseEvent *event)
    {
        if (!hasFocus ()) {
            setText ("123");
            event->ignore ();
        }

        QLineEdit::mousePressEvent (event);
    }

    void paintEvent (QPaintEvent *event)
    {
        QLineEdit::paintEvent (event);
    }
};

class FileWidgetPrivate
{
    Q_DECLARE_PUBLIC(FileWidget)

public:
    struct SDirInformation
    {
        SDirInformation ()
        {
            clear ();
        }

        void clear ()
        {
            dirsCount = 0;
            filesCount = 0;
            filesSize = 0;
            dirsSelectedCount = 0;
            filesSelectedCount = 0;
            filesSelectedSize = 0;
        }

        int dirsCount;
        int dirsSelectedCount;
        int filesCount;
        int filesSelectedCount;
        qint64 filesSize;
        qint64 filesSelectedSize;
    };

    FileWidgetPrivate ()
        : q_ptr (0)
        , model (0)
        ,
#ifndef QT_NO_PROXYMODEL
        proxyModel (0)
        ,
#endif
        treeView (0)
        , pathLineEdit (0)
        , infoLabel (0)
        , navigateBackwardAction (0)
        , navigateForwardAction (0)
        , navigateToParentAction (0)
        , navigateToHomeAction (0)
        , navigateToRootAction (0)
        , navigateHistoryAction (0)
        , newFolderAction (0)
        , renameAction (0)
        , deleteAction (0)
        , historyLocation (-1)
    {
    }

    ~FileWidgetPrivate ()
    {
    }

    void init (const QString &directory);

    void createWidgets ();

    void createActions ();

    void retranslateStrings ();

    QString workingDirectory (const QString &path) const;

    inline QModelIndex mapFromSource (const QModelIndex &sourceIndex) const;

    inline QModelIndex mapToSource (const QModelIndex &index) const;

    inline QString rootPath () const;

    inline QModelIndex rootIndex () const;

    inline void setRootIndex (const QModelIndex &sourceIndex) const;

    inline QModelIndex setCurrentIndex (const QModelIndex &sourceIndex) const;

    inline QModelIndex select (const QModelIndex &sourceIndex) const;

    QString size (qint64 bytes) const;

    void updateDirInfo ();

    void _q_rowsInserted (const QModelIndex &parent, int start, int end);

    void _q_rowsAboutToBeRemoved (const QModelIndex &parent, int start, int end);

    void _q_selectionChanged (const QItemSelection &selected, const QItemSelection &deselected);

    void _q_showHeader (QAction *action);

    void _q_showContextMenu (const QPoint &position);

    void _q_addressChanged ();

    void _q_activate (const QModelIndex &index);

    void _q_pathChanged (const QString &newPath);

    void _q_navigateBackward ();

    void _q_navigateForward ();

    void _q_navigateToParent ();

    void _q_navigateToHome ();

    void _q_navigateToRoot ();

    FileWidget *q_ptr;

    QFileListModel *model;
    QAbstractProxyModel *proxyModel;
    QTreeView *treeView;

    QLineEdit *pathLineEdit;
    QLabel *infoLabel;

    QAction *navigateBackwardAction;
    QAction *navigateForwardAction;
    QAction *navigateToParentAction;
    QAction *navigateToHomeAction;
    QAction *navigateToRootAction;
    QAction *navigateHistoryAction;
    QAction *refreshListAction;
    QAction *newFolderAction;
    QAction *renameAction;
    QAction *deleteAction;

    QString lastVisitedDir;
    QStringList history;
    int historyLocation;

    SDirInformation dirInfo;
};

inline QModelIndex FileWidgetPrivate::mapFromSource (const QModelIndex &sourceIndex) const
{
    return proxyModel ? proxyModel->mapFromSource (sourceIndex) : sourceIndex;
}

inline QModelIndex FileWidgetPrivate::mapToSource (const QModelIndex &index) const
{
    return proxyModel ? proxyModel->mapToSource (index) : index;
}

inline QString FileWidgetPrivate::rootPath () const
{
    return model->rootPath ();
}

#endif // FILEWIDGET_P_H
