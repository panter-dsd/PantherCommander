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

#include <QtGui>
#include <QDebug>

#include <QtCore/QCoreApplication>
#include <QtCore/QFileSystemWatcher>

#include <QtGui/QFileIconProvider>

#ifdef Q_WS_WIN
#	include <windows.h>
#endif

#include "dirsorter.h"
#include "qfileoperationsthread.h"

static void getInfoList(const QDir& dir, QFileInfoList* infos)
{
	Q_ASSERT(infos);

	bool isRoot = QFileOperationsThread::isRoot(dir.absolutePath());

	QDirIterator it(dir);
	while(it.hasNext())
	{
		it.next();

		QFileInfo info(it.fileInfo());
		if(info.fileName() != QLatin1String(".") && !(isRoot && info.fileName() == QLatin1String("..")))
			infos->append(info);
	}
}

static void getIcons(QList<QPCFileInfo*>* infos, QFileIconProvider* prov)
{
	Q_ASSERT(infos);
	Q_ASSERT(prov);

	QList<QPCFileInfo*>::iterator it = infos->begin();
	for(; it != infos->end(); ++it)
	{
		if((*it)->icon.isNull())
			(*it)->icon = prov->icon((*it)->fileInfo());
	}
}


QFileListModelPrivate::QFileListModelPrivate() : q_ptr(0),
	sortColumn(0), sortOrder(Qt::AscendingOrder), sort(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name)
{
	provider = new QFileIconProvider();
	qiFolderIcon = provider->icon(QFileIconProvider::Folder);
	qiFileIcon = provider->icon(QFileIconProvider::File);

	fileSystemWatcher = new QFileSystemWatcher;
}

QFileListModelPrivate::~QFileListModelPrivate()
{
	future.cancel();
	future.waitForFinished();

	qDeleteAll(infoList);
	infoList.clear();

	delete provider;
	provider = 0;

	delete fileSystemWatcher;
	fileSystemWatcher = 0;
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
	int visualRow = infoList.indexOf(const_cast<QPCFileInfo*>(node));
	return q_func()->createIndex(visualRow, 0, const_cast<QPCFileInfo*>(node));
}

void QFileListModelPrivate::getFileList()
{
	Q_Q(QFileListModel);

	bool blockWatcherSignals = fileSystemWatcher->blockSignals(true);

	future.cancel();
	future.waitForFinished();

	qdCurrentDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	qdCurrentDir.setSorting(QDir::Unsorted);


	int rowCount = infoList.size();
	if(rowCount > 0)
	{
		q->beginRemoveRows(QModelIndex(), 0, rowCount - 1);

		qDeleteAll(infoList);
		infoList.clear();

		q->endRemoveRows();
	}


	//Get file list
	QFileInfoList infos;
	future = QtConcurrent::run(getInfoList, qdCurrentDir, &infos);
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
			infoList.append(new QPCFileInfo(infos.at(row)));

		q->endInsertRows();


		future = QtConcurrent::run(getIcons, &infoList, provider);
		futureWatcher.setFuture(future);
	}

	fileSystemWatcher->blockSignals(blockWatcherSignals);
}

void QFileListModelPrivate::_q_refresh()
{
	Q_Q(QFileListModel);

	bool blockWatcherSignals = fileSystemWatcher->blockSignals(true);

	future.cancel();
	future.waitForFinished();

	qdCurrentDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	qdCurrentDir.setSorting(QDir::Unsorted);


	//Get file list
	QFileInfoList infos;
	future = QtConcurrent::run(getInfoList, qdCurrentDir, &infos);
	while(future.isRunning())
		QCoreApplication::processEvents();


	int d = infoList.size() - infos.size();
	if(d < 0)
	{
		// add some rows
		d = -d;
		int size = infoList.size();
		q->beginInsertRows(QModelIndex(), size, size + d - 1);

		for(int row = size, n = size + d; row < n; ++row)
			infoList.append(new QPCFileInfo(infos.at(row)));

		q->endInsertRows();
	}
	else if(d > 0)
	{
		// remove extra rows
		int size = infos.size();
		q->beginRemoveRows(QModelIndex(), size, size + d - 1);

		for(int row = size, n = size + d; row < n; ++row)
			delete infoList.takeLast();

		q->endRemoveRows();
	}


	int rowCount = infos.size();
	if(rowCount > 0)
	{
		emit q->layoutAboutToBeChanged();
		QModelIndexList oldList = q->persistentIndexList();
		QList<QPair<QPCFileInfo*, int> > oldNodes;
		for(int i = 0, n = oldList.size(); i < n; ++i)
		{
			QPair<QPCFileInfo*, int> pair(node(oldList.at(i)), oldList.at(i).column());
			oldNodes.append(pair);
		}

		//Sort file list
		QFuture<QFileInfoList> sortFuture = QtConcurrent::run(Dir::sortFileList, infos, sort);
		while(sortFuture.isRunning())
			QCoreApplication::processEvents();
		infos = sortFuture.result();


		qDeleteAll(infoList);
		// update changed rows
		for(int row = 0; row < rowCount; ++row)
			infoList[row] = new QPCFileInfo(infos.at(row));


		QModelIndexList newList;
		for(int i = 0, n = oldNodes.size(); i < n; ++i)
		{
			QModelIndex idx = index(oldNodes.at(i).first);
			idx = idx.sibling(idx.row(), oldNodes.at(i).second);
			newList.append(idx);
		}
		q->changePersistentIndexList(oldList, newList);
		emit q->layoutChanged();


		future = QtConcurrent::run(getIcons, &infoList, provider);
		futureWatcher.setFuture(future);
	}

	fileSystemWatcher->blockSignals(blockWatcherSignals);
}

void QFileListModelPrivate::_q_finishedLoadIcons()
{
	Q_Q(QFileListModel);
	emit q->dataChanged(q->index(0, QFileListModel::NAME),
						q->index(infoList.size() - 1, QFileListModel::NAME));
}


QFileListModel::QFileListModel(QObject* parent) : QAbstractItemModel(parent),
	d_ptr(new QFileListModelPrivate)
{
	d_ptr->q_ptr = this;

	Q_D(QFileListModel);
	connect(d->fileSystemWatcher, SIGNAL(directoryChanged(const QString&)),
			this, SLOT(_q_refresh()));

	connect(&d->futureWatcher, SIGNAL(finished()), this, SLOT(_q_finishedLoadIcons()));
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
	return createIndex(row, column, d->infoList.at(row));
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
		return !d->infoList.isEmpty();

	return d->node(parent)->isDir();
}

int QFileListModel::rowCount(const QModelIndex& parent) const
{
	Q_D(const QFileListModel);

	if(parent.column() > 0)
		return 0;

	if(!parent.isValid())
		return d->infoList.size();

	return 0;
}

int QFileListModel::columnCount(const QModelIndex& parent) const
{
	return (parent.column() > 0) ? 0 : COLUMNS;
}

Qt::ItemFlags QFileListModel::flags(const QModelIndex& index) const
{
	Q_D(const QFileListModel);

	if(!d->indexValid(index))
		return 0;

	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	QPCFileInfo* node = d->node(index);
	if(node->fileName() != QLatin1String(".."))
	{
		QFile::Permissions perms = node->permissions();
		if(perms & QFile::ReadUser)
			flags |= Qt::ItemIsDragEnabled;
		if(perms & QFile::WriteUser)
		{
			if(node->isDir())
				flags |= Qt::ItemIsDropEnabled;
			if(index.column() == 0)
				flags |= Qt::ItemIsEditable;
		}
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
	if(!index.isValid() || index.row() >= rowCount(index.parent())
						|| index.column() >= columnCount(index.parent()))
	{
		return QVariant();
	}

	Q_D(const QFileListModel);

	QPCFileInfo* node = d->node(index);
	if (role==Qt::DisplayRole)
	{
		int iTmp;
		switch (index.column())
		{
			case NAME:
				iTmp=node->fileName().lastIndexOf(".");
				if (iTmp==0 || node->isDir())
					return node->fileName();
				if (iTmp!=-1 && iTmp!=node->fileName().length()-1)
					return node->fileName().left(iTmp);
				else
					return node->fileName();
				break;
			case EXT:
				iTmp=node->fileName().lastIndexOf(".");
				if (iTmp==0 || node->isDir())
					return QString();
				if (iTmp!=-1 && iTmp!=node->fileName().length()-1)
					return node->fileName().mid(iTmp+1);
				else
					return QString();
				break;
			case SIZE:
				if (node->isDir())
					return tr("<folder>");
				else
					return d->size(node->size());
				break;
			case TIME_LAST_UPDATE:
				return node->lastModified().toString("dd.MM.yy hh:mm:ss");
			case ATTR:
#ifdef Q_WS_WIN
					QString attr;
/*					if (node->attr & FILE_ATTRIBUTE_READONLY)
						attr+="r";
					else
						attr+="-";
					if (node->attr & FILE_ATTRIBUTE_ARCHIVE)
						attr+="a";
					else
						attr+="-";
					if (node->attr & FILE_ATTRIBUTE_HIDDEN)
						attr+="h";
					else
						attr+="-";
					if (node->attr & FILE_ATTRIBUTE_SYSTEM)
						attr+="s";
					else
						attr+="-";
					if (node->attr & FILE_ATTRIBUTE_COMPRESSED)
						attr+="c";
					else
						attr+="-";*/
					return attr;
#endif // Q_WS_WIN
				break;
		}
	}
	if (role==Qt::DecorationRole && index.column() == NAME)
	{
		if (!node->icon.isNull())
			return node->icon;
		else
			return (node->isDir() ? d->qiFolderIcon : d->qiFileIcon);
	}
	if (role == Qt::UserRole )
	{
		switch (index.column())
		{
			case SIZE:
				return node->size();
				break;
		}
	}
	if (role == Qt::EditRole && index.column() == NAME)
	{
		return node->fileName();
	}
	if (role == Qt::TextAlignmentRole && index.column() == SIZE)
	{
		if (!isDir(index))
			return Qt::AlignRight;
		else
			return Qt::AlignHCenter;
	}
	if (role == Qt::ToolTipRole)
	{
		switch(index.column())
		{
			case NAME:return node->fileName(); break;
			case EXT:return node->fileName(); break;
			case SIZE: return node->size(); break;
			case TIME_LAST_UPDATE: return node->lastModified().toString("dd.MM.yyyy hh:mm:ss"); break;
			default: return QVariant();
		};
	}
	if (role==Qt::ForegroundRole)
	{
		if (node->lastModified().date().daysTo(QDate::currentDate())<=3)
			return Qt::blue;
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
			switch (section)
			{
				case NAME: return tr("Name"); break;
				case EXT: return tr("Type"); break;
				case SIZE: return tr("Size"); break;
				case TIME_LAST_UPDATE: return tr("Date"); break;
				case ATTR: return tr("Attr"); break;
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
	return d->qdCurrentDir;
}

QString QFileListModel::rootPath() const
{
	Q_D(const QFileListModel);
	return d->qdCurrentDir.absolutePath();
}

QModelIndex QFileListModel::setRootPath(const QString& path)
{
	Q_D(QFileListModel);

	d->qdCurrentDir.setPath(path);
	d->getFileList();

	if(!d->fileSystemWatcher->directories().isEmpty())
		d->fileSystemWatcher->removePaths(d->fileSystemWatcher->directories());
	d->fileSystemWatcher->addPath(d->qdCurrentDir.absolutePath());

	emit rootPathChanged(d->qdCurrentDir.absolutePath());

	return QModelIndex();
}

QModelIndex QFileListModel::index(const QString& fileName) const
{
	Q_D(const QFileListModel);

	for(int row = 0, n = d->infoList.size(); row < n; ++row)
	{
		if(d->infoList.at(row)->fileName() == fileName)
			return index(row, 0, QModelIndex());
	}
	return QModelIndex();
}

QDir::Filters QFileListModel::filter() const
{
	Q_D(const QFileListModel);
	return d->qdCurrentDir.filter();
}

void QFileListModel::setFilter(QDir::Filters filters)
{
	Q_D(QFileListModel);
	d->qdCurrentDir.setFilter(filters);
	d->getFileList();
}

QFileIconProvider* QFileListModel::iconProvider() const
{
	Q_D(const QFileListModel);
	return d->provider;
}

void QFileListModel::setIconProvider(QFileIconProvider* iconProvider)
{
	Q_D(QFileListModel);

	if(d->provider == iconProvider)
		return;

	delete d->provider;
	d->provider = iconProvider;
}

void QFileListModel::sort(int column, Qt::SortOrder order)
{
	Q_D(QFileListModel);
	if(d->sortColumn == column && d->sortOrder == order)
		return;

	emit layoutAboutToBeChanged();
	QModelIndexList oldList = persistentIndexList();
	QList<QPair<QPCFileInfo*, int> > oldNodes;
	for(int i = 0, n = oldList.size(); i < n; ++i)
	{
		QPair<QPCFileInfo*, int> pair(d->node(oldList.at(i)), oldList.at(i).column());
		oldNodes.append(pair);
	}

	d->sortColumn = column;
	d->sortOrder = order;

	d->sort = QDir::DirsFirst | QDir::IgnoreCase;
	d->sort |= QDir::SortFlags(0x100 | 0x200 | 0x300);
	if(d->sortColumn == NAME)
		d->sort |= QDir::Name;
	else if(d->sortColumn == EXT)
		d->sort |= QDir::Type;
	else if(d->sortColumn == SIZE)
		d->sort |= QDir::Size;
	else if(d->sortColumn == TIME_LAST_UPDATE)
		d->sort |= QDir::Time;
	if(d->sortOrder != Qt::AscendingOrder)
		d->sort |= QDir::Reversed;

	//Sort file list
	QFileInfoList infos;
	for(int i = 0, n = d->infoList.size(); i < n; ++i)
		infos.append(d->infoList.at(i)->fileInfo());
	QFuture<QFileInfoList> sortFuture = QtConcurrent::run(Dir::sortFileList, infos, d->sort);
	while(sortFuture.isRunning())
		QCoreApplication::processEvents();
	infos = sortFuture.result();
	qDeleteAll(d->infoList);
	for(int i = 0, n = infos.size(); i < n; ++i)
		d->infoList[i] = new QPCFileInfo(infos.at(i));


	QModelIndexList newList;
	for(int i = 0, n = oldNodes.size(); i < n; ++i)
	{
		QModelIndex idx = d->index(oldNodes.at(i).first);
		idx = idx.sibling(idx.row(), oldNodes.at(i).second);
		newList.append(idx);
	}
	changePersistentIndexList(oldList, newList);
	emit layoutChanged();
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
			urls.append(QUrl(d->node(*it)->absoluteFilePath()));
	}
	QMimeData* data = new QMimeData;
	data->setUrls(urls);
	return data;
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
				QString path = (*it).toLocalFile();
				success = QFile::copy(path, to + QFileInfo(path).fileName()) && success;
			}
			break;
		case Qt::LinkAction:
			for(; it != urls.constEnd(); ++it)
			{
				QString path = (*it).toLocalFile();
				success = QFile::link(path, to + QFileInfo(path).fileName()) && success;
			}
			break;
		case Qt::MoveAction:
			for(; it != urls.constEnd(); ++it)
			{
				QString path = (*it).toLocalFile();
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

#include "moc_qfilelistmodel.cpp"
