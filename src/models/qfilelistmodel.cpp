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
* Author:		PanteR
* Contact:	panter.dsd@gmail.com
*******************************************************************/

#include "qfilelistmodel.h"
#include "qfilelistmodel_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDirIterator>
#include <QtCore/QList>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtCore/qtconcurrentrun.h>

#ifdef Q_WS_WIN
#  include <qt_windows.h>
#endif

#include "dirsorter.h"
#include "qfileoperationsthread.h"

static void getInfoList(const QDir& dir, QFileInfoList* infos)
{
	Q_ASSERT(infos);

	QDirIterator it(dir);
	while(it.hasNext())
	{
		it.next();

		QFileInfo info(it.fileInfo());
		if(info.fileName() != QLatin1String(".") && !(info.isRoot() && info.fileName() == QLatin1String("..")))
			infos->append(info);
	}
}

static void getIcons(QList<QPCFileInfo*>* infos, QFileIconProvider* iconProvider)
{
	Q_ASSERT(infos);
	Q_ASSERT(iconProvider);

	for(int i = 0, n = infos->size(); i < n; ++i)
	{
		QPCFileInfo* info = (*infos)[i];
		if(info->icon().isNull())
		{
			if(info->fileName() == QLatin1String(".."))
				info->setIcon(iconProvider->icon(QFileIconProvider::Desktop));
			else
				info->setIcon(iconProvider->icon(info->fileInfo()));
		}
	}
}


QFileListModelPrivate::QFileListModelPrivate() : q_ptr(0),
	sortColumn(0), sortOrder(Qt::AscendingOrder), sort(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name),
	inUpdate(false), sheduledUpdate(false)
{
	iconProvider = new QFileIconProvider();

#ifndef QT_NO_FILESYSTEMWATCHER
	fileSystemWatcher = new QFileSystemWatcher;
#endif
}

QFileListModelPrivate::~QFileListModelPrivate()
{
	abort();

	qDeleteAll(nodes);
	nodes.clear();

	delete iconProvider;
	iconProvider = 0;

#ifndef QT_NO_FILESYSTEMWATCHER
	delete fileSystemWatcher;
	fileSystemWatcher = 0;
#endif

	q_ptr = 0;
}

QPCFileInfo* QFileListModelPrivate::node(const QModelIndex& index) const
{
	if(!index.isValid())
		return const_cast<QPCFileInfo*>(&root);
	QPCFileInfo* node = static_cast<QPCFileInfo*>(index.internalPointer());
	Q_ASSERT(node);
	return node;
}

QModelIndex QFileListModelPrivate::index(const QPCFileInfo* node) const
{
	QPCFileInfo* parentNode = 0;//(node ? node->parent : 0);
	if(!parentNode || node == &root)
		return QModelIndex();

	// get the parent's row
	Q_ASSERT(node);
	int visualRow = nodes.indexOf(const_cast<QPCFileInfo*>(node));
	return q_func()->createIndex(visualRow, 0, const_cast<QPCFileInfo*>(node));
}

QModelIndex QFileListModelPrivate::index(const QString& fileName, int column) const
{
	if(fileName == root.fileName())
		return QModelIndex();

	// get the parent's row
	for(int row = 0, n = nodes.size(); row < n; ++row)
	{
		QPCFileInfo* node = nodes.at(row);
		if(fileName == node->fileName())
			return q_func()->createIndex(row, column, node);
	}

	return QModelIndex();
}

void QFileListModelPrivate::abort()
{
	future.cancel();
	future.waitForFinished();
	sheduledUpdate = false;
}

void QFileListModelPrivate::fetchFileList()
{
	Q_Q(QFileListModel);

	if(inUpdate)
	{
		sheduledUpdate = true;
		return;
	}

	inUpdate = true;

	abort();

	rootDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	rootDir.setSorting(QDir::Unsorted);


	int rowCount = nodes.size();
	if(rowCount > 0)
	{
		q->beginRemoveRows(QModelIndex(), 0, rowCount - 1);

		qDeleteAll(nodes);
		nodes.clear();

		q->endRemoveRows();
	}


	//Get file list
	QFileInfoList infos;
	future = QtConcurrent::run(getInfoList, rootDir, &infos);
	while(future.isRunning())
		QCoreApplication::processEvents();

	rowCount = infos.size();
	if(rowCount > 0)
	{
		//Sort file list
		QFuture<QFileInfoList> sortFuture = QtConcurrent::run(Dir::sortFileList, infos, sort);
		while(sortFuture.isRunning())
			QCoreApplication::processEvents();
		infos = sortFuture.result();


		q->beginInsertRows(QModelIndex(), 0, rowCount - 1);

		for(int row = 0; row < rowCount; ++row)
			nodes.append(new QPCFileInfo(infos.at(row)));

		q->endInsertRows();


		future = QtConcurrent::run(getIcons, &nodes, iconProvider);
		futureWatcher.setFuture(future);
	}

	inUpdate = false;
}

void QFileListModelPrivate::updateFileList()
{
	Q_Q(QFileListModel);

	if(inUpdate)
	{
		sheduledUpdate = true;
		return;
	}

	inUpdate = true;

	abort();

	rootDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	rootDir.setSorting(QDir::Unsorted);


	//Get file list
	QFileInfoList infos;
	future = QtConcurrent::run(getInfoList, rootDir, &infos);
	while(future.isRunning())
		QCoreApplication::processEvents();


	int d = nodes.size() - infos.size();
	if(d < 0)
	{
		// add some rows
		d = -d;
		int size = nodes.size();
		q->beginInsertRows(QModelIndex(), size, size + d - 1);

		for(int row = size, n = size + d; row < n; ++row)
			nodes.append(new QPCFileInfo(infos.at(row)));

		q->endInsertRows();
	}
	else if(d > 0)
	{
		// remove extra rows
		int size = infos.size();
		q->beginRemoveRows(QModelIndex(), size, size + d - 1);

		for(int row = size, n = size + d; row < n; ++row)
			delete nodes.takeLast();

		q->endRemoveRows();
	}


	int rowCount = infos.size();
	if(rowCount > 0)
	{
		emit q->layoutAboutToBeChanged();
		QModelIndexList oldList = q->persistentIndexList();
		QList<QPair<QString, int> > oldNodes;
		for(int i = 0, n = oldList.size(); i < n; ++i)
		{
			QPair<QString, int> pair(node(oldList.at(i))->fileName(), oldList.at(i).column());
			oldNodes.append(pair);
		}

		//Sort file list
		QFuture<QFileInfoList> sortFuture = QtConcurrent::run(Dir::sortFileList, infos, sort);
		while(sortFuture.isRunning())
			QCoreApplication::processEvents();
		infos = sortFuture.result();


		qDeleteAll(nodes);
		// update changed rows
		for(int row = 0; row < rowCount; ++row)
			nodes[row] = new QPCFileInfo(infos.at(row));


		QModelIndexList newList;
		for(int i = 0, n = oldNodes.size(); i < n; ++i)
		{
			QModelIndex idx = index(oldNodes.at(i).first, oldNodes.at(i).second);
			newList.append(idx);
		}
		q->changePersistentIndexList(oldList, newList);
		emit q->layoutChanged();


		future = QtConcurrent::run(getIcons, &nodes, iconProvider);
		futureWatcher.setFuture(future);
	}

	inUpdate = false;
}

void QFileListModelPrivate::_q_directoryChanged()
{
	Q_Q(QFileListModel);
	sheduledUpdate = true;
	if(!updateTimer.isActive())
		updateTimer.start(300, q);
}

void QFileListModelPrivate::_q_finishedLoadIcons()
{
	Q_Q(QFileListModel);
	emit q->dataChanged(q->index(0, QFileListModel::NameColumn),
						q->index(nodes.size() - 1, QFileListModel::NameColumn));
}


QFileListModel::QFileListModel(QObject* parent) : QAbstractItemModel(parent),
	d_ptr(new QFileListModelPrivate)
{
	d_ptr->q_ptr = this;

	connect(&d_ptr->futureWatcher, SIGNAL(finished()), this, SLOT(_q_finishedLoadIcons()));

#ifndef QT_NO_FILESYSTEMWATCHER
	connect(d_ptr->fileSystemWatcher, SIGNAL(directoryChanged(const QString&)),
			this, SLOT(_q_directoryChanged()));
#endif
}

QFileListModel::~QFileListModel()
{
	delete d_ptr;
}

QModelIndex QFileListModel::index(int row, int column, const QModelIndex& parent) const
{
	Q_D(const QFileListModel);

	if(row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))
		return QModelIndex();
	return createIndex(row, column, d->nodes.at(row));
}

QModelIndex QFileListModel::buddy(const QModelIndex& index) const
{
	return index.sibling(index.row(), NameColumn);
}

QModelIndex QFileListModel::parent(const QModelIndex& child) const
{
//	if(!child.isValid() || (child.row() == 0 && child.column() == 0))
//		return QModelIndex();
	return QModelIndex();
}

bool QFileListModel::hasChildren(const QModelIndex& parent) const
{
	Q_D(const QFileListModel);

	if(parent.column() > 0)
		return false;

	if(!parent.isValid())
		return !d->nodes.isEmpty();

	return d->node(parent)->isDir();
}

int QFileListModel::rowCount(const QModelIndex& parent) const
{
	Q_D(const QFileListModel);

	if(parent.column() > 0)
		return 0;

	if(!parent.isValid())
		return d->nodes.size();

	return 0;
}

int QFileListModel::columnCount(const QModelIndex& parent) const
{
	return (parent.column() > 0) ? 0 : ColumnCount;
}

Qt::ItemFlags QFileListModel::flags(const QModelIndex& index) const
{
	Q_D(const QFileListModel);
	QPCFileInfo* node = d->node(index);
	if(!node)
		return Qt::NoItemFlags;

	Qt::ItemFlags flags = Qt::ItemIsEnabled;

	bool dotDot = node->fileName() == QLatin1String("..");
	if(!dotDot)
		flags |= Qt::ItemIsSelectable;
	QFile::Permissions perms = node->permissions();
	if(!dotDot && perms & QFile::ReadUser)
		flags |= Qt::ItemIsDragEnabled;
	if(perms & QFile::WriteUser)
	{
		if(node->isDir())
			flags |= Qt::ItemIsDropEnabled;
		if(!dotDot && index.column() == 0)
			flags |= Qt::ItemIsEditable;
	}

	return flags;
}

QString QFileListModelPrivate::size(qint64 bytes)
{
	// According to the Si standard KB is 1000 bytes, KiB is 1024
	// but on windows sizes are calculated by dividing by 1024 so we do what they do.
	const qint64 kb = 1024;
	const qint64 mb = 1024 * kb;
	const qint64 gb = 1024 * mb;
	const qint64 tb = 1024 * gb;
	if (bytes >= tb)
		return QFileListModel::tr("%1 TB").arg(QLocale().toString(qreal(bytes) / tb, 'f', 3));
	if (bytes >= gb)
		return QFileListModel::tr("%1 GB").arg(QLocale().toString(qreal(bytes) / gb, 'f', 2));
	if (bytes >= mb)
		return QFileListModel::tr("%1 MB").arg(QLocale().toString(qreal(bytes) / mb, 'f', 1));
	if (bytes >= kb)
		return QFileListModel::tr("%1 KB").arg(QLocale().toString(bytes / kb));
	return QFileListModel::tr("%1 bytes").arg(QLocale().toString(bytes));
}

QVariant QFileListModel::data(const QModelIndex& index, int role) const
{
	Q_D(const QFileListModel);
	if(!index.isValid() || index.model() != this)
		return QVariant();

	QPCFileInfo* node = d->node(index);
	switch(role)
	{
		case Qt::EditRole:
			switch(index.column())
			{
				case NameColumn: return node->fileName();
				default: break;
			}
			break;
		case Qt::DisplayRole:
			switch(index.column())
			{
				case NameColumn: return node->name();
				case TypeColumn: return node->ext();
				case SizeColumn: return !node->isDir() || node->isSymLink() ? d->size(node->size()) : tr("<folder>");
				case ModifiedTimeColumn: return node->lastModified().toString("dd.MM.yy hh:mm:ss");
				case OwnerColumn: return node->fileInfo().owner();
				case GroupColumn: return node->fileInfo().group();
				case PermissionsColumn:
				{
					QFile::Permissions perms = node->permissions();

					QString ret;
					ret.append(node->isDir() ? "d" : node->isSymLink() ? "l" : "-");

					ret.append((perms & QFile::ReadUser) ? "r" : "-");
					ret.append((perms & QFile::WriteUser) ? "w" : "-");
					ret.append((perms & QFile::ExeUser) ? "x" : "-");

					ret.append((perms & QFile::ReadGroup) ? "r" : "-");
					ret.append((perms & QFile::WriteGroup) ? "w" : "-");
					ret.append((perms & QFile::ExeGroup) ? "x" : "-");

					ret.append((perms & QFile::ReadOther) ? "r" : "-");
					ret.append((perms & QFile::WriteOther) ? "w" : "-");
					ret.append((perms & QFile::ExeOther) ? "x" : "-");
					return ret;
				}
					break;
				case AttributesColumn:
#ifdef Q_WS_WIN
				{
					qint64 attributes = QFileOperationsThread::winFileAttributes(node->absoluteFilePath());
					if (attributes != INVALID_FILE_ATTRIBUTES)
					{
						QString attr;
						attr += (attributes & FILE_ATTRIBUTE_READONLY) ? "r" : "-";
						attr += (attributes & FILE_ATTRIBUTE_ARCHIVE) ? "a" : "-";
						attr += (attributes & FILE_ATTRIBUTE_HIDDEN) ? "h" : "-";
						attr += (attributes & FILE_ATTRIBUTE_SYSTEM) ? "s" : "-";
						attr += (attributes & FILE_ATTRIBUTE_COMPRESSED) ? "c" : "-";
						attr += (attributes & FILE_ATTRIBUTE_ENCRYPTED) ? "e" : "-";
						return attr;
					}
				}
#endif // Q_WS_WIN
					break;
				default: break;
			}
			break;
		case FilePathRole:
			return node->filePath();
		case FileNameRole:
			return node->fileName();
		case Qt::DecorationRole:
			if(index.column() == NameColumn)
			{
				QIcon icon = node->icon();
				if(icon.isNull())
					icon = d->iconProvider->icon(node->isDir() ? QFileIconProvider::Folder : QFileIconProvider::File);
				return icon;
			}
			break;
		case Qt::TextAlignmentRole:
			if(index.column() == SizeColumn)
				return !isDir(index) ? Qt::AlignRight : Qt::AlignHCenter;
			break;
		case Qt::ToolTipRole:
			switch(index.column())
			{
				case NameColumn: return node->fileName();
				case TypeColumn: return node->fileName();
				case SizeColumn: return node->size();
				case ModifiedTimeColumn: return node->lastModified().toString("dd.MM.yyyy hh:mm:ss");
				default: break;
			}
			break;
		case Qt::ForegroundRole:
			if(node->lastModified().date().daysTo(QDate::currentDate()) <= 3)
				return Qt::blue;
			break;
		case FilePermissions:
			return (int)node->permissions();
	}

	return QVariant();
}

bool QFileListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
/* if (role != Qt::EditRole)
		 return false;

	TreeItem *item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result)
		 emit dataChanged(index, index);

	return result;*/
	return false;
}

QVariant QFileListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole)
	{
		if(orientation == Qt::Horizontal)
		{
			switch(section)
			{
				case NameColumn: return tr("Name");
				case TypeColumn: return tr("Type");
				case SizeColumn: return tr("Size");
				case ModifiedTimeColumn: return tr("Date");
				case OwnerColumn: return tr("Owner");
				case GroupColumn: return tr("Group");
				case PermissionsColumn: return tr("Permissions");
				case AttributesColumn: return tr("Attr");
				default: break;
			}
		}

		return section + 1;
	}

	return QVariant();
}

bool QFileListModel::isDir(const QModelIndex& index) const
{
	Q_D(const QFileListModel);
	QPCFileInfo* node = d->node(index);
	return node ? node->isDir() : false;
}

qint64 QFileListModel::size(const QModelIndex& index) const
{
	Q_D(const QFileListModel);
	QPCFileInfo* node = d->node(index);
	return node ? node->size() : 0;
}

QFile::Permissions QFileListModel::permissions(const QModelIndex& index) const
{
	Q_D(const QFileListModel);
	QPCFileInfo* node = d->node(index);
	return node ? node->permissions() : QFile::Permissions(0);
}

QString QFileListModel::filePath(const QModelIndex& index) const
{
	Q_D(const QFileListModel);
	QPCFileInfo* node = d->node(index);
	return node ? node->filePath() : QString();
}

QString QFileListModel::fileName(const QModelIndex& index) const
{
	Q_D(const QFileListModel);
	QPCFileInfo* node = d->node(index);
	return node ? node->fileName() : QString();
}

QVariant QFileListModel::myComputer() const
{
	return QDir::rootPath();
}

QDir QFileListModel::rootDirectory() const
{
	Q_D(const QFileListModel);
	return d->rootDir;
}

QString QFileListModel::rootPath() const
{
	Q_D(const QFileListModel);
	return d->rootDir.absolutePath();
}

QModelIndex QFileListModel::setRootPath(const QString& path)
{
	Q_D(QFileListModel);

	d->rootDir.setPath(path);
	d->root.setFileInfo(QFileInfo(path));
	d->fetchFileList();

#ifndef QT_NO_FILESYSTEMWATCHER
	if(!d->fileSystemWatcher->directories().isEmpty())
		d->fileSystemWatcher->removePaths(d->fileSystemWatcher->directories());
#ifdef Q_WS_WIN
	if(!d->rootDir.absolutePath().startsWith(QLatin1String("//")))
#endif
		d->fileSystemWatcher->addPath(d->rootDir.absolutePath());
#endif

	emit rootPathChanged(d->rootDir.absolutePath());

	return QModelIndex();
}

QModelIndex QFileListModel::index(const QString& fileName) const
{
	Q_D(const QFileListModel);

	for(int row = 0, n = d->nodes.size(); row < n; ++row)
	{
		if(d->nodes.at(row)->fileName() == fileName)
			return index(row, 0, QModelIndex());
	}
	return QModelIndex();
}

QDir::Filters QFileListModel::filter() const
{
	Q_D(const QFileListModel);
	return d->rootDir.filter();
}

void QFileListModel::setFilter(QDir::Filters filters)
{
	Q_D(QFileListModel);
	d->rootDir.setFilter(filters);
	d->fetchFileList();
}

QFileIconProvider* QFileListModel::iconProvider() const
{
	Q_D(const QFileListModel);
	return d->iconProvider;
}

void QFileListModel::setIconProvider(QFileIconProvider* iconProvider)
{
	Q_D(QFileListModel);

	if(d->iconProvider == iconProvider)
		return;

	delete d->iconProvider;
	d->iconProvider = iconProvider;
}

void QFileListModel::sort(int column, Qt::SortOrder order)
{
	Q_D(QFileListModel);
	if(d->sortColumn == column && d->sortOrder == order)
		return;

	emit layoutAboutToBeChanged();
	QModelIndexList oldList = persistentIndexList();
	QList<QPair<QString, int> > oldNodes;
	for(int i = 0, n = oldList.size(); i < n; ++i)
	{
		QPair<QString, int> pair(d->node(oldList.at(i))->fileName(), oldList.at(i).column());
		oldNodes.append(pair);
	}

	d->sortColumn = column;
	d->sortOrder = order;

	d->sort = QDir::DirsFirst | QDir::IgnoreCase;
	d->sort |= QDir::SortFlags(0x100 | 0x200 | 0x300);
	if(d->sortColumn == NameColumn)
		d->sort |= QDir::Name;
	else if(d->sortColumn == TypeColumn)
		d->sort |= QDir::Type;
	else if(d->sortColumn == SizeColumn)
		d->sort |= QDir::Size;
	else if(d->sortColumn == ModifiedTimeColumn)
		d->sort |= QDir::Time;
	if(d->sortOrder != Qt::AscendingOrder)
		d->sort |= QDir::Reversed;

	//Sort file list
	QFileInfoList infos;
	for(int i = 0, n = d->nodes.size(); i < n; ++i)
		infos.append(d->nodes.at(i)->fileInfo());
	QFuture<QFileInfoList> sortFuture = QtConcurrent::run(Dir::sortFileList, infos, d->sort);
	while(sortFuture.isRunning())
		QCoreApplication::processEvents();
	infos = sortFuture.result();
	for(int i = 0, n = infos.size(); i < n; ++i)
	{
		int j = d->index(infos.at(i).fileName(), 0).row();
		d->nodes.swap(i, j);
	}


	QModelIndexList newList;
	for(int i = 0, n = oldNodes.size(); i < n; ++i)
	{
		QModelIndex idx = d->index(oldNodes.at(i).first, oldNodes.at(i).second);
		newList.append(idx);
	}
	changePersistentIndexList(oldList, newList);
	emit layoutChanged();
}

void QFileListModel::abort()
{
	Q_D(QFileListModel);
	d->updateTimer.stop();
	d->abort();
}

void QFileListModel::refresh()
{
	Q_D(QFileListModel);
	d->updateTimer.stop();
	d->_q_directoryChanged();
}

/*!
	Returns a list of MIME types that can be used to describe a list of items
	in the model.
*/
QStringList QFileListModel::mimeTypes() const
{
	return QStringList("text/uri-list");
}

/*!
	Returns an object that contains a serialized description of the specified
	\a indexes. The format used to describe the items corresponding to the
	indexes is obtained from the mimeTypes() function.

	If the list of indexes is empty, 0 is returned rather than a serialized
	empty list.
*/
QMimeData* QFileListModel::mimeData(const QModelIndexList& indexes) const
{
	Q_D(const QFileListModel);

	QList<QUrl> urls;
	QList<QModelIndex>::const_iterator it = indexes.constBegin();
	for(; it != indexes.constEnd(); ++it)
	{
		if((*it).column() == 0)
		{
			QPCFileInfo* node = d->node(*it);
			if(node->isLocalFile())
				urls.append(QUrl::fromLocalFile(node->absoluteFilePath()));
			else
				urls.append(QUrl(node->absoluteFilePath()));
		}
	}
	if(!urls.isEmpty())
	{
		QMimeData* data = new QMimeData();
		data->setUrls(urls);
		return data;
	}
	return 0;
}

/*!
	Handles the \a data supplied by a drag and drop operation that ended with
	the given \a action over the row in the model specified by the \a row and
	\a column and by the \a parent index.

	\sa supportedDropActions()
*/
bool QFileListModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	Q_UNUSED(row);
	Q_UNUSED(column);

	if(!data->hasFormat("text/uri-list") || !parent.isValid() || isReadOnly())
		return false;

	bool success = true;
	QString to = filePath(parent) + QDir::separator();

	QList<QUrl> urls = data->urls();
	QList<QUrl>::const_iterator it = urls.constBegin();

	switch(action)
	{
		case Qt::CopyAction:
			for(; it != urls.constEnd(); ++it)
			{
				QString path = (*it).scheme() == QLatin1String("file") ? (*it).toLocalFile() : (*it).toString();
				success = QFile::copy(path, to + QFileInfo(path).fileName()) && success;
			}
			break;
		case Qt::LinkAction:
			for(; it != urls.constEnd(); ++it)
			{
				QString path = (*it).scheme() == QLatin1String("file") ? (*it).toLocalFile() : (*it).toString();
				success = QFile::link(path, to + QFileInfo(path).fileName()) && success;
			}
			break;
		case Qt::MoveAction:
			for(; it != urls.constEnd(); ++it)
			{
				QString path = (*it).scheme() == QLatin1String("file") ? (*it).toLocalFile() : (*it).toString();
				success = QFile::copy(path, to + QFileInfo(path).fileName()) && QFile::remove(path) && success;
			}
			break;
		default:
			return false;
	}

	return success;
}

/*!
	\reimp
*/
Qt::DropActions QFileListModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

bool QFileListModel::event(QEvent* event)
{
	//Q_D(QFileListModel);
	if(event->type() == QEvent::LanguageChange)
	{
	}
	return QAbstractItemModel::event(event);
}

void QFileListModel::timerEvent(QTimerEvent* event)
{
	Q_D(QFileListModel);
	if(event->timerId() == d->updateTimer.timerId())
	{
		d->updateTimer.stop();
		if(d->sheduledUpdate)
		{
			d->updateTimer.start(1500, this);
			d->updateFileList();
		}
	}
	QAbstractItemModel::timerEvent(event);
}

#include "moc_qfilelistmodel.cpp"
