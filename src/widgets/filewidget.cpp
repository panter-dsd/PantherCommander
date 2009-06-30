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

#include "filewidget.h"
#include "filewidget_p.h"

#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QFileInfo>

#include <QtGui/QActionGroup>
#include <QtGui/QApplication>
#include <QtGui/QFontMetrics>
#include <QtGui/QHeaderView>
#include <QtGui/QKeyEvent>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QStyle>
#include <QtGui/QToolButton>

#include "qfileoperationsthread.h"
#include "qfilelistsortfilterproxymodel.h"
#include "qfullview.h"
#include <QDebug>
#include <QtGui/QMessageBox>

#ifndef Q_CC_MSVC
	#warning "TODO: rename to Q?FileSystem.*View"
#endif
#ifdef Q_WS_WIN
/*3*/extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

inline static QString _helper_get_directory(const QString& path)
{
	QFileInfo info = QFileInfo(QDir::current(), path);
	if(info.exists() && info.isDir())
		return QDir::cleanPath(info.absoluteFilePath());

	info.setFile(info.absolutePath());
	if(info.exists() && info.isDir())
		return info.absoluteFilePath();

	return QString();
}


void FileWidgetPrivate::init(const QString& directory)
{
	Q_Q(FileWidget);

	createWidgets();
//	createActions();
	retranslateStrings();

	lastVisitedDir = workingDirectory(directory);
	q->setDirectory(lastVisitedDir);

	q->setFocusProxy(treeView);
	q->resize(q->sizeHint());
}

void FileWidgetPrivate::createWidgets()
{
	Q_Q(FileWidget);

//	model = new QFileSystemModel(q);
	model = new QFileListModel(q);
	model->setObjectName(QLatin1String("_filesystem_model"));
	QDir::Filters filters = QDir::AllDirs | QDir::Files | QDir::Drives;
	filters |= QDir::Readable | QDir::Writable | QDir::Executable;
	filters |= QDir::Modified | QDir::Hidden | QDir::System;
	model->setFilter(filters);
	model->setReadOnly(false);

	QObject::connect(model, SIGNAL(rootPathChanged(const QString&)),
						q, SLOT(_q_pathChanged(const QString&)));

/*
	{
		QFileListSortFilterProxyModel* prx = new QFileListSortFilterProxyModel(q);
//		prx->setDynamicSortFilter(true);
//		prx->setSortRole(Qt::EditRole);
		proxyModel = prx;
	}
	proxyModel->setSourceModel(model);
*/

//	treeView = new QTreeView(q);
	treeView = new QFullView(q);
	treeView->installEventFilter(q);
	treeView->setRootIsDecorated(false);
	treeView->setItemsExpandable(false);
	treeView->setSortingEnabled(true);
	treeView->setUniformRowHeights(true);
	treeView->setAllColumnsShowFocus(true);
	treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	//treeView->setEditTriggers(QAbstractItemView::SelectedClicked);
	treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	treeView->setContextMenuPolicy(Qt::CustomContextMenu);
#ifndef QT_NO_DRAGANDDROP
	treeView->setAcceptDrops(true);
	treeView->setDragEnabled(true);
	treeView->setDragDropMode(QAbstractItemView::DragDrop);
//	treeView->setDragDropMode(QAbstractItemView::InternalMove);
#endif
//	treeView->setModel(proxyModel);
	treeView->setModel(model);

	QObject::connect(treeView, SIGNAL(activated(const QModelIndex&)),
						q, SLOT(_q_activate(const QModelIndex&)));
	QObject::connect(treeView, SIGNAL(customContextMenuRequested(const QPoint&)),
						q, SLOT(_q_showContextMenu(const QPoint&)));
	QObject::connect(treeView->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
						q, SLOT(_q_rowsInserted(const QModelIndex&, int, int)));
	QObject::connect(treeView->model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
						q, SLOT(_q_rowsAboutToBeRemoved(const QModelIndex&, int, int)));

	QHeaderView* headerView = treeView->header();
	headerView->setSortIndicator(0, Qt::AscendingOrder);
	headerView->setStretchLastSection(false);
	QFontMetrics fm(q->font());
	headerView->resizeSection(0, fm.width(QLatin1String("wwwwwwwwwwwwwwwwwwwwwwwwww")));
	headerView->resizeSection(1, fm.width(QLatin1String("128.88 GB")));
	headerView->resizeSection(2, fm.width(QLatin1String("mp3Folder")));
	headerView->resizeSection(3, fm.width(QLatin1String("10/29/81 02:02PM")));
	headerView->setContextMenuPolicy(Qt::ActionsContextMenu);
/*?
	QActionGroup* showActionGroup = new QActionGroup(q);
	showActionGroup->setExclusive(false);

	QObject::connect(showActionGroup, SIGNAL(triggered(QAction*)),
						q, SLOT(_q_showHeader(QAction*)));

	QAbstractItemModel* abstractModel = model;
#ifndef QT_NO_PROXYMODEL
	if(proxyModel)
		abstractModel = proxyModel;
#endif
	for(int i = 1, n = abstractModel->columnCount(QModelIndex()); i < n; ++i)
	{
		QAction* showHeaderAction = new QAction(showActionGroup);
		showHeaderAction->setCheckable(true);
		showHeaderAction->setChecked(true);
		headerView->addAction(showHeaderAction);
	}
*/
	QItemSelectionModel* selectionModel = treeView->selectionModel();

	QObject::connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
						q, SLOT(_q_selectionChanged(const QItemSelection&, const QItemSelection&)));


#ifndef Q_CC_MSVC
	#warning "TODO: path validator, completer"
#endif
	pathLineEdit = new AddressLineEdit(q);
	pathLineEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	QObject::connect(pathLineEdit, SIGNAL(returnPressed()),
						q, SLOT(_q_addressChanged()));

	createActions();
	QFrame* frame = new QFrame(q);
	frame->setFixedHeight(18);

	QHBoxLayout* frameLayout = new QHBoxLayout;
	frameLayout->setContentsMargins(0, 0, 0, 0);
	frameLayout->addWidget(pathLineEdit);
	foreach(QAction* action, q->actions())
	{
		QSize fixedSize(16, 16);
		QToolButton* button = new QToolButton(frame);
		button->setObjectName(action->objectName().append(QLatin1String("_button")));
		button->setDefaultAction(action);
		button->setFixedSize(fixedSize);
		button->setAutoRaise(true);
		button->setFocusPolicy(Qt::NoFocus);
		frameLayout->addWidget(button);
	}
	frame->setLayout(frameLayout);


	infoLabel = new QLabel(q);


	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(1);
	layout->addWidget(frame);
	layout->addWidget(treeView);
	layout->addWidget(infoLabel);
	q->setLayout(layout);

	q->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void FileWidgetPrivate::createActions()
{
	Q_Q(FileWidget);
#ifndef Q_CC_MSVC
	#warning "TODO: make shortcuts configurable"
#endif
	navigateBackwardAction = new QAction(q);
	navigateBackwardAction->setObjectName(QLatin1String("_navigate_backward_action"));
	navigateBackwardAction->setIcon(q->style()->standardIcon(QStyle::SP_ArrowBack, 0, q));
	navigateBackwardAction->setEnabled(false);
	QObject::connect(navigateBackwardAction, SIGNAL(triggered(bool)), q, SLOT(_q_navigateBackward()));
	q->addAction(navigateBackwardAction);

	navigateForwardAction = new QAction(q);
	navigateForwardAction->setObjectName(QLatin1String("_navigate_forward_action"));
	navigateForwardAction->setIcon(q->style()->standardIcon(QStyle::SP_ArrowForward, 0, q));
	navigateForwardAction->setEnabled(false);
	QObject::connect(navigateForwardAction, SIGNAL(triggered(bool)), q, SLOT(_q_navigateForward()));
	q->addAction(navigateForwardAction);

	navigateToParentAction = new QAction(q);
	navigateToParentAction->setObjectName(QLatin1String("_navigate_to_parent_action"));
	navigateToParentAction->setIcon(q->style()->standardIcon(QStyle::SP_FileDialogToParent, 0, q));
	navigateToParentAction->setText("..");
#ifndef QT_NO_SHORTCUT
//	navigateToParentAction->setShortcut(Qt::CTRL + Qt::UpArrow);
#endif
	navigateToParentAction->setEnabled(false);
	QObject::connect(navigateToParentAction, SIGNAL(triggered(bool)), q, SLOT(_q_navigateToParent()));
	q->addAction(navigateToParentAction);

	navigateToHomeAction = new QAction(q);
	navigateToHomeAction->setObjectName(QLatin1String("_navigate_to_home_action"));
	navigateToHomeAction->setIcon(q->style()->standardIcon(QStyle::SP_DirHomeIcon, 0, q));
	navigateToHomeAction->setText("~");
#ifndef QT_NO_SHORTCUT
//	navigateToHomeAction->setShortcut(Qt::CTRL + Qt::Key_H + Qt::SHIFT);
#endif
	navigateToHomeAction->setEnabled(true);
	QObject::connect(navigateToHomeAction, SIGNAL(triggered(bool)), q, SLOT(_q_navigateToHome()));
	q->addAction(navigateToHomeAction);

	navigateToRootAction = new QAction(q);
	navigateToRootAction->setObjectName(QLatin1String("_navigate_to_root_action"));
//	navigateToRootAction->setIcon(q->style()->standardIcon(QStyle::, 0, q));
	navigateToRootAction->setText("/");
	navigateToRootAction->setEnabled(true);
	QObject::connect(navigateToRootAction, SIGNAL(triggered(bool)), q, SLOT(_q_navigateToRoot()));
	q->addAction(navigateToRootAction);

	navigateHistoryAction = new QAction(q);
	navigateHistoryAction->setObjectName(QLatin1String("_navigate_history_action"));
	navigateHistoryAction->setIcon(q->style()->standardIcon(QStyle::SP_ArrowDown, 0, q));
	navigateHistoryAction->setText("\\/");
	navigateHistoryAction->setEnabled(false);
//	QObject::connect(navigateHistoryAction, SIGNAL(triggered(bool)), q, SLOT());
	q->addAction(navigateHistoryAction);

	// operation actions
#ifndef Q_CC_MSVC
	#warning "TODO: implement more fop actions"
#endif
	newFolderAction = new QAction(q);
	newFolderAction->setObjectName(QLatin1String("_operation_new_folder_action"));
	newFolderAction->setIcon(q->style()->standardIcon(QStyle::SP_FileDialogNewFolder, 0, q));
	newFolderAction->setText(FileWidget::tr("New Folder"));
	newFolderAction->setEnabled(false);
//	QObject::connect(newFolderAction, SIGNAL(triggered(bool)), q, SLOT(_q_createDirectory()));
	q->addAction(newFolderAction);

	renameAction = new QAction(q);
	renameAction->setObjectName(QLatin1String("_operation_rename_action"));
//	renameAction->setIcon(q->style()->standardIcon(QStyle::, 0, q));
	renameAction->setText(FileWidget::tr("Rename"));
	renameAction->setEnabled(false);
	QObject::connect(renameAction, SIGNAL(triggered(bool)), q, SLOT(slotRename()));
//	q->addAction(renameAction);

	deleteAction = new QAction(q);
	deleteAction->setObjectName(QLatin1String("_operation_delete_action"));
//	deleteAction->setIcon(q->style()->standardIcon(QStyle::, 0, q));
	deleteAction->setText(FileWidget::tr("Delete"));
	deleteAction->setEnabled(false);
//	QObject::connect(deleteAction, SIGNAL(triggered(bool)), q, SLOT(_q_createDirectory()));
//	q->addAction(deleteAction);
}

void FileWidgetPrivate::retranslateStrings()
{
	const QList<QAction*>& actions = treeView->header()->actions();
	QAbstractItemModel* abstractModel = model;
#ifndef QT_NO_PROXYMODEL
	if(proxyModel)
		abstractModel = proxyModel;
#endif
	int total = qMin(abstractModel->columnCount(QModelIndex()), actions.count() + 1);
	for(int i = 1; i < total; ++i)
	{
		QString text = abstractModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
		actions[i - 1]->setText(FileWidget::tr("Show %1").arg(text));
	}
}

/*!
	\internal

	Get the initial directory path
*/
QString FileWidgetPrivate::workingDirectory(const QString& path) const
{
	if(!path.isEmpty())
	{
		QString directory = _helper_get_directory(path);
		if(!directory.isEmpty())
			return directory;
	}
/*?
	QString directory = _helper_get_directory(lastVisitedDir);
	if(!directory.isEmpty())
		return directory;
*/
	return QDir::rootPath();
}

/*!
	\internal

	Returns the file system model index that is the root index in the views
*/
QModelIndex FileWidgetPrivate::rootIndex() const
{
	return mapToSource(treeView->rootIndex());
}

/*!
	\internal

	Sets the view root index to be the file system model index
*/
void FileWidgetPrivate::setRootIndex(const QModelIndex& sourceIndex) const
{
	Q_ASSERT(sourceIndex.isValid() ? sourceIndex.model() == model : true);

	QModelIndex index = mapFromSource(sourceIndex);
	treeView->setRootIndex(index);
}

/*!
	\internal

	Set a current file system model index
	returns the index that was selected (or not depending upon sortfilterproxymodel)
*/
QModelIndex FileWidgetPrivate::setCurrentIndex(const QModelIndex& sourceIndex) const
{
	Q_ASSERT(sourceIndex.isValid() ? sourceIndex.model() == model : true);

	const QModelIndex index = mapFromSource(sourceIndex);
	if(index.isValid())
		treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::NoUpdate);

	return index;
}

/*!
	\internal

	Select a file system model index
	returns the index that was selected (or not depending upon sortfilterproxymodel)
*/
QModelIndex FileWidgetPrivate::select(const QModelIndex& sourceIndex) const
{
	Q_ASSERT(sourceIndex.isValid() ? sourceIndex.model() == model : true);

	const QModelIndex index = mapFromSource(sourceIndex);
	if(index.isValid() && !treeView->selectionModel()->isSelected(index))
		treeView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);

	return index;
}

QString FileWidgetPrivate::size(qint64 bytes) const
{
	Q_Q(const FileWidget);
	// According to the Si standard KB is 1000 bytes, KiB is 1024
	// but on windows sizes are calculated by dividing by 1024 so we do what they do.
	const qint64 kb = 1024;
	const qint64 mb = 1024 * kb;
	const qint64 gb = 1024 * mb;
	const qint64 tb = 1024 * gb;
	if (bytes >= tb)
		return FileWidget::tr("%1 TB").arg(q->locale().toString(qreal(bytes) / tb, 'f', 3));
	if (bytes >= gb)
		return FileWidget::tr("%1 GB").arg(q->locale().toString(qreal(bytes) / gb, 'f', 2));
	if (bytes >= mb)
		return FileWidget::tr("%1 MB").arg(q->locale().toString(qreal(bytes) / mb, 'f', 1));
	if (bytes >= kb)
		return FileWidget::tr("%1 KB").arg(q->locale().toString(bytes / kb));
	return FileWidget::tr("%1 bytes").arg(q->locale().toString(bytes));
}

void FileWidgetPrivate::updateDirInfo()
{
	QString information = FileWidget::tr(
		"Selected <b>%1</b> from <b>%2</b> in <b>%3</b>/<b>%4</b> files and <b>%5</b>/<b>%6</b> dirs"
	).arg(size(dirInfo.filesSelectedSize))
	.arg(size(dirInfo.filesSize))
	.arg(dirInfo.filesSelectedCount)
	.arg(dirInfo.filesCount)
	.arg(dirInfo.dirsSelectedCount)
	.arg(dirInfo.dirsCount);
	infoLabel->setText(information);
}

/*!
	\internal

	When parent is root and rows have been inserted when none was there before
	then select the first one.
*/
void FileWidgetPrivate::_q_rowsInserted(const QModelIndex& parent, int start, int end)
{
	for(int row = start; row <= end; ++row)
	{
		const QModelIndex index = model->index(row, 0, parent);

		if(model->isDir(index))
			++dirInfo.dirsCount;
		else
			++dirInfo.filesCount;
		dirInfo.filesSize += model->size(index);
	}
	updateDirInfo();
}

void FileWidgetPrivate::_q_rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
	for(int row = start; row <= end; ++row)
	{
		const QModelIndex index = model->index(row, 0, parent);

		if(model->isDir(index))
			--dirInfo.dirsCount;
		else
			--dirInfo.filesCount;
		dirInfo.filesSize -= model->size(index);
	}
	updateDirInfo();
}

void FileWidgetPrivate::_q_selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	foreach(const QModelIndex& index, selected.indexes())
	{
		const QModelIndex sourceIndex = mapToSource(index);
//
if(sourceIndex.column() != 0)
	continue;
//
		if(model->isDir(sourceIndex))
			++dirInfo.dirsSelectedCount;
		else
			++dirInfo.filesSelectedCount;
		dirInfo.filesSelectedSize += model->size(sourceIndex);
	}

	foreach(const QModelIndex& index, deselected.indexes())
	{
		const QModelIndex sourceIndex = mapToSource(index);
//
if(sourceIndex.column() != 0)
	continue;
//
		if(model->isDir(sourceIndex))
			--dirInfo.dirsSelectedCount;
		else
			--dirInfo.filesSelectedCount;
		dirInfo.filesSelectedSize -= model->size(sourceIndex);
	}

	updateDirInfo();
}

void FileWidgetPrivate::_q_showHeader(QAction* action)
{
	Q_Q(FileWidget);

	QActionGroup* actionGroup = qobject_cast<QActionGroup*>(q->sender());
	if(actionGroup)
		treeView->header()->setSectionHidden(actionGroup->actions().indexOf(action) + 1, !action->isChecked());
}

/*!
	\internal

	Show the context menu for the file/dir under position
*/
void FileWidgetPrivate::_q_showContextMenu(const QPoint& position)
{
#ifdef QT_NO_MENU
	Q_UNUSED(position);
#else
	QAbstractItemView* view = 0;
	view = treeView;
	QModelIndex index = view->indexAt(position);
	index = mapToSource(index);

	QMenu menu(view);
	if(index.isValid())
	{
		// file context menu
		QFile::Permissions p = model->permissions(index);
		renameAction->setEnabled(p & QFile::WriteUser);
		menu.addAction(renameAction);
		deleteAction->setEnabled(p & QFile::WriteUser);
		menu.addAction(deleteAction);
		menu.addSeparator();
	}
	if(newFolderAction->isVisible())
	{
		newFolderAction->setEnabled(newFolderAction->isEnabled());
		menu.addAction(newFolderAction);
	}
	menu.exec(view->viewport()->mapToGlobal(position));
#endif // QT_NO_MENU
}

/*!
	\internal

	This is called when the user changes path in address bar.
*/
void FileWidgetPrivate::_q_addressChanged()
{
	Q_Q(FileWidget);

	q->setDirectory(pathLineEdit->text());
}

/*!
	\internal

	This is called when the user double clicks on a file with the corresponding
	model item \a index.
*/
void FileWidgetPrivate::_q_activate(const QModelIndex& index)
{
	Q_Q(FileWidget);

	// My Computer or a directory
	QModelIndex sourceIndex = mapToSource(index);
	QString path = model->filePath(sourceIndex);
	if(model->isDir(sourceIndex))
	{
		if(model->fileName(sourceIndex) == QLatin1String(".."))
			_q_navigateToParent();
		else
			q->setDirectory(path);
	}
	else
	{
		QFileOperationsThread::execute(path);
	}
}

/*!
	\internal

	Update history with new path
*/
void FileWidgetPrivate::_q_pathChanged(const QString& newPath)
{
	QString path = newPath;
	if(QFileOperationsThread::isLocalFileSystem(newPath))
		path = QDir::toNativeSeparators(newPath);

	pathLineEdit->setText(path);

/*	if(path.endsWith(QLatin1Char('/')))
		d->completer->setCompletionPrefix(path);
	else
		d->completer->setCompletionPrefix(path + QLatin1Char('/'));
*/

	dirInfo.clear();

	QDir dir(model->rootDirectory());
	navigateToParentAction->setEnabled(!dir.isRoot() && dir.exists());

	if(historyLocation < 0 || history.value(historyLocation) != path)
	{
		// remove unneeded forward entries
		while(historyLocation >= 0 && historyLocation + 1 < history.count())
			history.removeLast();
		// remove duplicates of new path
		historyLocation -= history.removeAll(path);
		// add current path
		history.append(path);
		++historyLocation;
	}
	navigateBackwardAction->setEnabled(historyLocation > 0);
	navigateForwardAction->setEnabled(historyLocation + 1 < history.size());

	treeView->setFocus();
}

/*!
	\internal

	Navigates to the last viewed directory.
*/
void FileWidgetPrivate::_q_navigateBackward()
{
	Q_Q(FileWidget);

	if(!history.isEmpty() && historyLocation > 0)
		q->setDirectory(history.at(--historyLocation));
}

/*!
	\internal

	Navigates to the viewed directory.
*/
void FileWidgetPrivate::_q_navigateForward()
{
	Q_Q(FileWidget);

	if(historyLocation + 1 < history.size())
		q->setDirectory(history[++historyLocation]);
}

/*!
	\internal

	Navigates to the parent directory of the currently displayed directory.
*/
void FileWidgetPrivate::_q_navigateToParent()
{
	Q_Q(FileWidget);

	QString newDirectory;
	QDir dir(model->rootDirectory());
	if(!dir.isRoot() && dir.cdUp())
		newDirectory = dir.absolutePath();
	if(newDirectory.isEmpty())
		newDirectory = model->myComputer().toString();

	const QModelIndex idx = rootIndex();
	q->setDirectory(newDirectory);
	(void)setCurrentIndex(idx);
}

/*!
	\internal
*/
void FileWidgetPrivate::_q_navigateToHome()
{
	q_func()->setDirectory(QDir::homePath());
}

/*!
	\internal
*/
void FileWidgetPrivate::_q_navigateToRoot()
{
	q_func()->setDirectory(QFileOperationsThread::rootPath(rootPath()));
}



FileWidget::FileWidget(QWidget* parent) : QWidget(parent),
	d_ptr(new FileWidgetPrivate)
{
	d_func()->q_ptr = this;
	d_func()->init(QString());
}

FileWidget::FileWidget(const QString& path, QWidget* parent) : QWidget(parent),
	d_ptr(new FileWidgetPrivate)
{
	d_func()->q_ptr = this;
	d_func()->init(path);

	//
	// Set the inactive pallete
	QPalette palette = d_func()->treeView->palette();
	palette.setColor(QPalette::Active, QPalette::Highlight,
					qApp->palette().color(QPalette::Inactive, QPalette::Highlight));
	d_func()->treeView->setPalette(palette);
	//

#ifndef Q_CC_MSVC
	#warning "TODO: RenameEditor must have it's own settings"
#endif
/*1*///	selectOnlyFileName = true;
/*1*///	qleRenameEditor = 0;
}

FileWidget::~FileWidget()
{
	delete d_ptr;
}

static const qint32 FileWidgetMagic = 0xbe;

/*!
	Saves the state of the views's layout, history and current directory.

	Typically this is used in conjunction with QSettings to remember the size
	for a future session. A version number is stored as part of the data.
*/
QByteArray FileWidget::saveState() const
{
	Q_D(const FileWidget);

	int version = 1;
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	stream << qint32(FileWidgetMagic);
	stream << qint32(version);

	stream << d->history;
	stream << d->lastVisitedDir;
	stream << d->treeView->header()->saveState();

	return data;
}

/*!
	Restores the views's layout, history and current directory to the \a state specified.

	Typically this is used in conjunction with QSettings to restore the size
	from a past session.

	Returns false if there are errors
*/
bool FileWidget::restoreState(const QByteArray& state)
{
	Q_D(FileWidget);

	int version = 1;
	QByteArray sd = state;
	QDataStream stream(&sd, QIODevice::ReadOnly);
	if(stream.atEnd())
		return false;

	qint32 magic;
	qint32 v;
	QStringList history;
	QString currentDirectory;
	QByteArray headerData;

	stream >> magic;
	stream >> v;
	if(magic != FileWidgetMagic || v != version)
		return false;

	stream >> history;
	stream >> currentDirectory;
	stream >> headerData;

	setHistory(history);
	setDirectory(!currentDirectory.isEmpty() ? currentDirectory : d->lastVisitedDir);
	if(!d->treeView->header()->restoreState(headerData))
		return false;

	return true;
}

/*!
	Returns the directory currently being displayed in the view.
*/
QDir FileWidget::directory() const
{
	Q_D(const FileWidget);

	return d->rootPath();
}

/*!
	\fn void FileWidget::setDirectory(const QDir& directory)

	\overload
*/

/*!
	Sets the file view's current \a directory.
*/
void FileWidget::setDirectory(const QString& directory)
{
	if(directory.isEmpty())
		return;

	Q_D(FileWidget);

	// we remove .. and . from the given path if exist
	QString newDirectory = QDir::cleanPath(directory);
	if(newDirectory.isEmpty() || d->rootPath() == newDirectory)
		return;

	QDir dir(newDirectory);
	bool isReadable = dir.isReadable();
#ifdef Q_WS_WIN
	if(!isReadable)
	{
		if(newDirectory.startsWith(QLatin1String("//")))
			isReadable |= (newDirectory.split(QLatin1Char('/'), QString::SkipEmptyParts).count() == 1);
/*3**/
		if(!isReadable)
		{
			qt_ntfs_permission_lookup++;
			isReadable = dir.isReadable();
			qt_ntfs_permission_lookup--;
		}
/**3*/
	}
#endif
	if(!isReadable)
	{
#ifndef Q_CC_MSVC
	#warning "TODO: must be configurable (message, beep, or something else)"
#endif
#ifndef QT_NO_MESSAGEBOX
		QMessageBox::critical(this, "", tr("You have no enought privilegies"));
#endif
		return;
	}
	d->lastVisitedDir = newDirectory;

	//setUpdatesEnabled(false);

	d->treeView->clearSelection();
	const QModelIndex root = d->model->setRootPath(newDirectory);
	d->newFolderAction->setEnabled(d->model->flags(root) & Qt::ItemIsDropEnabled);
	emit directoryEntered(newDirectory);
	if(root != d->rootIndex())
	{
		d->setRootIndex(root);
	}

	//setUpdatesEnabled(true);
}

void FileWidget::changeEvent(QEvent* event)
{
	Q_D(FileWidget);

	if(event->type() == QEvent::LanguageChange)
		d->retranslateStrings();

	QWidget::changeEvent(event);
}

bool FileWidget::eventFilter(QObject* object, QEvent* event)
{
	Q_D(FileWidget);

	if(object == d->pathLineEdit)
	{
	}
	else if(object == d->treeView)
	{
		if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			switch(keyEvent->key())
			{
				case Qt::Key_Insert:
				{
					QKeyEvent e1(event->type(), Qt::Key_Space, Qt::ControlModifier);
					if(QCoreApplication::sendEvent(d->treeView, &e1))
					{
						QKeyEvent e2(event->type(), Qt::Key_Down, Qt::ControlModifier);
						if(QCoreApplication::sendEvent(d->treeView, &e2))
						{
							event->accept();
							return true;
						}
					}
				}
					break;
				case Qt::Key_Asterisk:
					if(d->treeView->selectionModel())
					{
						QModelIndex root = d->treeView->rootIndex();
						QModelIndex tl = d->treeView->model()->index(0, 0, root);
						QModelIndex br = d->treeView->model()->index(d->treeView->model()->rowCount(root) - 1,
																	d->treeView->model()->columnCount(root) - 1,
																	root);
						QItemSelection selection(tl, br);
						d->treeView->selectionModel()->select(selection, QItemSelectionModel::Toggle);
						event->accept();
						return true;
					}
					break;
				default:
					break;
			}
		}
	}
/*1	else if(object == qleRenameEditor)
	{
		if(event->type() == QEvent::FocusOut)
		{
			qleRenameEditor->close();
		}
		if(event->type() == QEvent::KeyRelease)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if(keyEvent->key() == Qt::Key_Escape)
				qleRenameEditor->close();
		}
	}*/

	return QWidget::eventFilter(object, event);
}

void FileWidget::slotRename()
{
	Q_D(FileWidget);

	QModelIndex index = d->treeView->currentIndex();
	QString filePath = d->model->filePath(d->mapToSource(index));

	if(!(index.flags() & Qt::ItemIsEditable))
		return;

	index = d->treeView->model()->index(index.row(), 0, index.parent());
	d->treeView->edit(index);
	return;


/*1	if(!qleRenameEditor)
	{
		qleRenameEditor = new QLineEdit(fi.fileName(), d->treeView);
		qleRenameEditor->setAttribute(Qt::WA_DeleteOnClose);
		connect(qleRenameEditor, SIGNAL(destroyed()),
				this, SLOT(slotCloseEditor()));
		connect(qleRenameEditor, SIGNAL(returnPressed()),
				this, SLOT(slotRenameFile()));
		qleRenameEditor->installEventFilter(this);
		qleRenameEditor->move(d->treeView->visualRect(index).left() + d->treeView->iconSize().width(),
								d->treeView->visualRect(index).top() + d->treeView->header()->height());
		qleRenameEditor->resize(d->treeView->columnWidth(0) + d->treeView->columnWidth(1) -
								d->treeView->iconSize().width(),
								d->treeView->visualRect(index).height());
		QPalette palette = qleRenameEditor->palette();
		palette.setColor(QPalette::Active,
						QPalette::Highlight,
						qApp->palette().color(QPalette::Active, QPalette::Highlight));
		qleRenameEditor->setPalette(palette);
		qleRenameEditor->show();
		qleRenameEditor->setFocus();
	}*/

#ifndef Q_CC_MSVC
	#warning "TODO: shedule this work to custom delegate"
	#warning "TODO: since neigher model nor view can not use editorForIndex"
#endif
/*1	QString qsFileName = qleRenameEditor->text();
	int pos = qsFileName.lastIndexOf(".");
	if(!selectOnlyFileName || qleRenameEditor->selectedText() == qsFileName.left(pos) || qleRenameEditor->selectedText().isEmpty())
		qleRenameEditor->selectAll();
	else
		qleRenameEditor->setSelection(0, pos);*/
}

/*1void FileWidget::slotCloseEditor()
{
	qleRenameEditor = 0;
	d_func()->treeView->setFocus();
}*/

/*!
	Returns the filter that is used when displaying files.

	\sa setFilter()
*/
QDir::Filters FileWidget::filter() const
{
	Q_D(const FileWidget);

	return d->model->filter();
}

/*!
	Sets the filter used by the model to \a filters. The filter is used
	to specify the kind of files that should be shown.

	\sa filter()
*/
void FileWidget::setFilter(QDir::Filters filters)
{
	Q_D(FileWidget);

	d->model->setFilter(filters);
}

#ifndef Q_CC_MSVC
	#warning "TODO: check history behavoir"
	#warning "TODO: implement `smart' history"
#endif
/*!
	Returns the browsing history as a list of paths.
*/
QStringList FileWidget::history() const
{
	Q_D(const FileWidget);

	return d->history;
}

/*!
	Sets the browsing history to contain the given \a paths.
*/
void FileWidget::setHistory(const QStringList& paths)
{
	Q_D(FileWidget);

	if(paths.isEmpty() || d->history == paths)
		return;

	d->history = paths;
	while(d->history.size() > 20)
		d->history.removeFirst();
	d->historyLocation = d->history.size() - 1;

	d->navigateBackwardAction->setEnabled(d->historyLocation > 0);
	d->navigateForwardAction->setEnabled(d->historyLocation + 1 < d->history.size());
}

/*!
	Returns the icon provider used by the view.
*/
QFileIconProvider* FileWidget::iconProvider() const
{
	Q_D(const FileWidget);

	return d->model->iconProvider();
}

/*!
	Sets the icon provider used by the view to the specified \a provider.
*/
void FileWidget::setIconProvider(QFileIconProvider* provider)
{
	Q_D(FileWidget);

	d->model->setIconProvider(provider);
}

/*!
	Returns the item delegate used to render the items in the view.
*/
QAbstractItemDelegate* FileWidget::itemDelegate() const
{
	Q_D(const FileWidget);

	return d->treeView->itemDelegate();
}

/*!
	Sets the item delegate used to render items in the view to the given \a delegate.

	\warning You should not share the same instance of a delegate between views.
	Doing so can cause incorrect or unintuitive editing behavior since each
	view connected to a given delegate may receive the \l{QAbstractItemDelegate::}{closeEditor()}
	signal, and attempt to access, modify or close an editor that has already been closed.

	Note that the model used is AbstractFileSystemModel. It has custom item data roles,
	which is described by the \l{AbstractFileSystemModel::}{Roles} enum.
	You can use a QFileIconProvider if you only want custom icons.

	\sa itemDelegate(), setIconProvider(), AbstractFileSystemModel
*/
void FileWidget::setItemDelegate(QAbstractItemDelegate* delegate)
{
	Q_D(FileWidget);

	d->treeView->setItemDelegate(delegate);
}

/*!
	Returns the absolute path of the current file in the view.

	\sa setCurrentFile()
*/
QString FileWidget::currentFile() const
{
	Q_D(const FileWidget);

	const QModelIndex index = d->treeView->currentIndex();
	return d->model->filePath(d->mapToSource(index));
}

/*!
	Selects the given \a filePath as current in the view.

	\sa currentFile()
*/
void FileWidget::setCurrentFile(const QString& fileName)
{
	if(fileName.isEmpty())
		return;

	Q_D(FileWidget);

	if(!QDir::isRelativePath(fileName))
	{
		QFileInfo info(fileName);
		QString filePath = info.absoluteDir().path();

		if(d->model->rootPath() != filePath)
			setDirectory(filePath);
	}

	const QModelIndex index = d->model->index(fileName);
	if(index.isValid())
		d->setCurrentIndex(index);
}

/*!
	Returns a list of strings containing the absolute paths of the
	selected files in the view. If no files are selected
	selectedFiles() contains the current file path.

	\sa selectFile()
*/
QStringList FileWidget::selectedFiles() const
{
	Q_D(const FileWidget);

	QStringList files;

	foreach(const QModelIndex& index, d->treeView->selectionModel()->selectedRows())
		files.append(d->model->filePath(d->mapToSource(index)));

#ifndef Q_CC_MSVC
	#warning "TODO: check behavior"
#endif
	if(files.isEmpty())
		files.append(currentFile());

	return files;
}

/*!
	Selects the given \a filename in the view.

	\sa selectedFiles()
*/
void FileWidget::selectFile(const QString& fileName, bool clearSelection)
{
	if(fileName.isEmpty())
		return;

	Q_D(FileWidget);

	if(!QDir::isRelativePath(fileName))
	{
		QFileInfo info(fileName);
		QString filePath = info.absoluteDir().path();

		if(d->model->rootPath() != filePath)
			setDirectory(filePath);
	}

	const QModelIndex index = d->model->index(fileName);
	if(index.isValid())
	{
		if(clearSelection)
			d->treeView->clearSelection();
		d->select(index);
	}
}

/*!
	Clears the current selection in the view.

	\sa selectedFiles()
*/
void FileWidget::clearSelection()
{
	Q_D(FileWidget);

	//QString curFile = currentFile();
	d->treeView->clearSelection();
	//setCurrentFile(curFile);
}


//
void FileWidget::gotoHome()
{
	d_func()->_q_navigateToHome();
}
//
void FileWidget::gotoRoot()
{
	d_func()->_q_navigateToRoot();
}
//
void FileWidget::cdUP()
{
	d_func()->_q_navigateToParent();
}

#include "moc_filewidget.cpp"
