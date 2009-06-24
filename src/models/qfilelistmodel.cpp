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

#include <QtGui>
#include <QDebug>

#include "dirsorter.h"
#include "qfileoperationsthread.h"
//
QFileListModel::QFileListModel(QObject* parent) : QAbstractItemModel(parent)
{
	provider = new QFileIconProvider();
	qiFolderIcon = provider->icon(QFileIconProvider::Folder);
	qiFileIcon = provider->icon(QFileIconProvider::File);

	fileSystemWatcher = new QFileSystemWatcher(this);
	connect(fileSystemWatcher, SIGNAL(directoryChanged(const QString&)),
			this, SLOT(slotRefresh()));

	connect(&futureWatcher, SIGNAL(finished()), this, SLOT(finishedLoadIcons()));
}
//
QFileListModel::~QFileListModel()
{
	future.cancel();
	future.waitForFinished();

	delete provider;
}
//
void QFileListModel::getFileList()
{
	bool blockWatcherSignals = fileSystemWatcher->blockSignals(true);

	future.cancel();
	future.waitForFinished();

	qdCurrentDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	qdCurrentDir.setSorting(QDir::Unsorted);


	int rowCount = infoList.size();
	if(rowCount > 0)
	{
		beginRemoveRows(QModelIndex(), 0, rowCount - 1);

		infoList.clear();

		endRemoveRows();
	}


	//Get file list
	QFileInfoList infos;
	future = QtConcurrent::run(getInfoList, qdCurrentDir, &infos);
	while(future.isRunning())
		qApp->processEvents();

	rowCount = infos.size();
	if(rowCount > 0)
	{
		//Sort file list
		QFuture<QFileInfoList> sortFuture = QtConcurrent::run(Dir::sortFileList, infos, QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
		while(sortFuture.isRunning())
			qApp->processEvents();
		infos = sortFuture.result();


		beginInsertRows(QModelIndex(), 0, rowCount - 1);

		for(int row = 0; row < rowCount; ++row)
			infoList.append(QPCFileInfo(infos.at(row)));

		endInsertRows();


		future = QtConcurrent::run(getIcons, &infoList, provider);
		futureWatcher.setFuture(future);
	}

	fetchMore();

	reset();
	fileSystemWatcher->blockSignals(blockWatcherSignals);
}
//
QModelIndex QFileListModel::index(int row, int column, const QModelIndex& parent) const
{
	if(row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))
		return QModelIndex();
	return createIndex(row, column);//, row * column + 1);
}
//
QModelIndex QFileListModel::parent(const QModelIndex& child) const
{
//	if(!child.isValid() || (child.row() == 0 && child.column() == 0))
//		return QModelIndex();
	return QModelIndex();
}
//
bool QFileListModel::hasChildren(const QModelIndex& parent) const
{
	if(parent.column() > 0)
		return false;

	if(!parent.isValid())
		return !infoList.isEmpty();

	return infoList.at(parent.row()).isDir();
}

int QFileListModel::rowCount(const QModelIndex& parent) const
{
	if(parent.column() > 0)
		return 0;

	if(!parent.isValid())
		return infoList.size();

	return 0;
}

int QFileListModel::columnCount(const QModelIndex& parent) const
{
	return (parent.column() > 0) ? 0 : COLUMNS;
}
//
Qt::ItemFlags QFileListModel::flags(const QModelIndex& index) const
{
	if(!indexValid(index))
		return 0;

	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	if(infoList.at(index.row()).fileName() != QLatin1String(".."))
	{
		QFile::Permissions perms = infoList.at(index.row()).permissions();
		if(perms & QFile::ReadUser)
			flags |= Qt::ItemIsDragEnabled;
		if(perms & QFile::WriteUser)
		{
			if(infoList.at(index.row()).isDir())
				flags |= Qt::ItemIsDropEnabled;
			if(index.column() == 0)
				flags |= Qt::ItemIsEditable;
		}
	}
	return flags;
}
//
QString QFileListModel::size(qint64 bytes)
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

	if (role==Qt::DisplayRole)
	{
		int iTmp;
		switch (index.column())
		{
			case NAME:
				iTmp=infoList.at(index.row()).fileName().lastIndexOf(".");
				if (iTmp==0 || (infoList.at(index.row()).isDir()))
					return infoList.at(index.row()).fileName();
				if (iTmp!=-1 && iTmp!=infoList.at(index.row()).fileName().length()-1)
					return infoList.at(index.row()).fileName().left(iTmp);
				else
					return infoList.at(index.row()).fileName();
				break;
			case EXT:
				iTmp=infoList.at(index.row()).fileName().lastIndexOf(".");
				if (iTmp==0 || (infoList.at(index.row()).isDir()))
					return QString();
				if (iTmp!=-1 && iTmp!=infoList.at(index.row()).fileName().length()-1)
					return infoList.at(index.row()).fileName().mid(iTmp+1);
				else
					return QString();
				break;
			case SIZE:
				if (infoList.at(index.row()).isDir())
					return tr("<folder>");
				else
					return size(infoList.at(index.row()).size());
				break;
			case TIME_LAST_UPDATE:
				return infoList.at(index.row()).lastModified().toString("dd.MM.yy hh:mm:ss");
			case ATTR:
#ifdef Q_WS_WIN
					QString attr;
/*					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_READONLY)
						attr+="r";
					else
						attr+="-";
					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_ARCHIVE)
						attr+="a";
					else
						attr+="-";
					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_HIDDEN)
						attr+="h";
					else
						attr+="-";
					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_SYSTEM)
						attr+="s";
					else
						attr+="-";
					if (infoList.at(index.row()).attr & FILE_ATTRIBUTE_COMPRESSED)
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
		if (!infoList.at(index.row()).icon.isNull())
			return infoList.at(index.row()).icon;
		else
			return (infoList.at(index.row()).isDir()) ? qiFolderIcon : qiFileIcon;
	}
	if (role == Qt::UserRole )
	{
		switch (index.column())
		{
			case SIZE:
				return infoList.at(index.row()).size();
				break;
		}
	}
	if (role == Qt::EditRole && index.column() == NAME)
	{
		return infoList.at(index.row()).fileName();
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
			case NAME:return infoList.at(index.row()).fileName(); break;
			case EXT:return infoList.at(index.row()).fileName(); break;
			case SIZE: return infoList.at(index.row()).size(); break;
			case TIME_LAST_UPDATE: return infoList.at(index.row()).lastModified().toString("dd.MM.yyyy hh:mm:ss"); break;
			default: return QVariant();
		};
	}
	if (role==Qt::ForegroundRole)
	{
		if (infoList.at(index.row()).lastModified().date().daysTo(QDate::currentDate())<=3)
			return Qt::blue;
	}
	return QVariant();
}
//
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
//
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

void QFileListModel::slotRefresh()
{
	bool blockWatcherSignals = fileSystemWatcher->blockSignals(true);

	future.cancel();
	future.waitForFinished();

	qdCurrentDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
	qdCurrentDir.setSorting(QDir::Unsorted);


	//Get file list
	QFileInfoList infos;
	future = QtConcurrent::run(getInfoList, qdCurrentDir, &infos);
	while(future.isRunning())
		qApp->processEvents();


	int d = infoList.size() - infos.size();
	if(d < 0)
	{
		// add some rows
		d = -d;
		int size = infoList.size();
		beginInsertRows(QModelIndex(), size, size + d - 1);

		for(int row = size, n = size + d; row < n; ++row)
			infoList.append(QPCFileInfo(infos.at(row)));

		endInsertRows();
	}
	else if(d > 0)
	{
		// remove extra rows
		int size = infos.size();
		beginRemoveRows(QModelIndex(), size, size + d - 1);

		for(int row = size, n = size + d; row < n; ++row)
			infoList.removeLast();

		endRemoveRows();
	}


	int rowCount = infos.size();
	if(rowCount > 0)
	{
		//Sort file list
		QFuture<QFileInfoList> sortFuture = QtConcurrent::run(Dir::sortFileList, infos, QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
		while(sortFuture.isRunning())
			qApp->processEvents();
		infos = sortFuture.result();


		// update changed rows
		//TODO: check QFileInfo == QFileInfo
		for(int row = 0; row < rowCount; ++row)
		{
			if(infoList.at(row).fileInfo() != infos.at(row))
			{
				infoList[row] = QPCFileInfo(infos.at(row));
				emit dataChanged(index(row, 0), index(row, COLUMNS));
				/*for(int col = 0; col < COLUMNS; ++col)
				{
					const QModelIndex idx = index(row, col);
					emit dataChanged(idx, idx);
				}*/
			}
		}


		future = QtConcurrent::run(getIcons, &infoList, provider);
		futureWatcher.setFuture(future);
	}

	fileSystemWatcher->blockSignals(blockWatcherSignals);
}

void QFileListModel::getInfoList(const QDir& dir, QFileInfoList* infos)
{
	Q_ASSERT(infos);

	QDirIterator it(dir);
	while(it.hasNext())
	{
		it.next();

		QFileInfo info(it.fileInfo());
		if((info.fileName() != QLatin1String(".")) && !((info.fileName() == QLatin1String("..")) && QFileOperationsThread::isRoot(dir.absolutePath())))
			infos->append(info);
	}
}

void QFileListModel::getIcons(QList<QPCFileInfo>* infos, QFileIconProvider* prov)
{
	Q_ASSERT(infos);
	Q_ASSERT(prov);

	QList<QPCFileInfo>::iterator it = infos->begin();
	for(; it != infos->end(); ++it)
	{
		if((*it).icon.isNull())
			(*it).icon = prov->icon((*it).fileInfo());
	}
}

bool QFileListModel::isDir(const QModelIndex& index) const
{
	return index.isValid() ? infoList.at(index.row()).isDir() : false;
}
//
qint64 QFileListModel::size(const QModelIndex& index) const
{
	return index.isValid() ? infoList.at(index.row()).size() : 0;
}
//
QFile::Permissions QFileListModel::permissions(const QModelIndex& index) const
{
	return index.isValid() ? infoList.at(index.row()).permissions() : QFile::Permissions(0);
}
//
QString QFileListModel::filePath(const QModelIndex& index) const
{
	return index.isValid() ? infoList.at(index.row()).filePath() : QString();
}
//
QString QFileListModel::fileName(const QModelIndex& index) const
{
	return index.isValid() ? infoList.at(index.row()).fileName() : QString();
}
//
QDir QFileListModel::rootDirectory() const
{
	return qdCurrentDir;
}
//
QVariant QFileListModel::myComputer() const
{
	return QDir::rootPath();
}
//
QModelIndex QFileListModel::setRootPath(const QString& path)
{
	qdCurrentDir.setPath(path);
	getFileList();

	if(!fileSystemWatcher->directories().isEmpty())
		fileSystemWatcher->removePaths(fileSystemWatcher->directories());
	fileSystemWatcher->addPath(qdCurrentDir.absolutePath());

	emit rootPathChanged(qdCurrentDir.absolutePath());

	return QModelIndex();
}

QModelIndex QFileListModel::index(const QString& fileName) const
{
	for(int row = 0, n = infoList.size(); row < n; ++row)
	{
		if(infoList.at(row).fileName() == fileName)
			return index(row, 0, QModelIndex());
	}
	return QModelIndex();
}

void QFileListModel::setFilter(QDir::Filters filters)
{
	qdCurrentDir.setFilter(filters);
	getFileList();
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
	QList<QUrl> urls;
	QList<QModelIndex>::const_iterator it = indexes.constBegin();
	for(; it != indexes.constEnd(); ++it)
	{
		if((*it).column() == 0)
			urls.append(QUrl(filePath(*it)));
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
//
void QFileListModel::finishedLoadIcons()
{
	emit dataChanged(index(0, NAME), index(infoList.size() - 1, NAME));
}
//
