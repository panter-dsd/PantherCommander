#include "FileListModel.h"
#include "FileListModel_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDirIterator>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QtGui>

#ifdef Q_WS_WIN
#  include <qt_windows.h>
#endif

#include "src/tools/DirSorter.h"

static void getInfoList (const QDir &dir, QFileInfoList *infos)
{
    Q_ASSERT (infos);

    QDirIterator it (dir);

    while (it.hasNext ()) {
        it.next ();

        const QFileInfo info (it.fileInfo ());
        const bool isDot = info.fileName () == QLatin1String (".");
        const bool isDotDot = info.fileName () == QLatin1String ("..");
        const bool canAddDotDot = isDotDot && !dir.isRoot ();

        if (!isDot && (!isDotDot || canAddDotDot)) {
            infos->append (info);
        }
    }
}

static void getIcons (QList<FileInfo *> *infos, QFileIconProvider *iconProvider)
{
    Q_ASSERT (infos);
    Q_ASSERT (iconProvider);

    for (FileInfo *info : *infos) {
        if (info->icon ().isNull ()) {
            const bool isDotDot = info->fileName () == QLatin1String ("..") ;
            info->setIcon (iconProvider->icon (isDotDot ? QFileInfo(info->filePath ()) : info->fileInfo ()));
        }
    }
}

FileListModelPrivate::FileListModelPrivate ()
    : q_ptr (0)
    , sortColumn (0)
    , sortOrder (Qt::AscendingOrder)
    , sort (QDir::DirsFirst | QDir::IgnoreCase | QDir::Name)
    , inUpdate (false)
    , sheduledUpdate (false)
{
    iconProvider = new QFileIconProvider ();

#ifndef QT_NO_FILESYSTEMWATCHER
    fileSystemWatcher = new QFileSystemWatcher;
#endif
}

FileListModelPrivate::~FileListModelPrivate ()
{
    abort ();

    qDeleteAll (nodes);
    nodes.clear ();

    delete iconProvider;
    iconProvider = 0;

#ifndef QT_NO_FILESYSTEMWATCHER
    delete fileSystemWatcher;
    fileSystemWatcher = 0;
#endif

    q_ptr = 0;
}

FileInfo *FileListModelPrivate::node (const QModelIndex &index) const
{
    if (!index.isValid ()) {
        return const_cast<FileInfo *> (&root);
    }

    FileInfo *node = static_cast<FileInfo *> (index.internalPointer ());
    Q_ASSERT (node);
    return node;
}

QModelIndex FileListModelPrivate::index (const FileInfo *node) const
{
    FileInfo *parentNode = 0;//(node ? node->parent : 0);

    if (!parentNode || node == &root) {
        return QModelIndex ();
    }

    // get the parent's row
    Q_ASSERT (node);
    int visualRow = nodes.indexOf (const_cast<FileInfo *> (node));
    return q_func ()->createIndex (visualRow, 0, const_cast<FileInfo *> (node));
}

QModelIndex FileListModelPrivate::index (const QString &fileName, int column) const
{
    if (fileName == root.fileName ()) {
        return QModelIndex ();
    }

    // get the parent's row
    for (int row = 0, n = nodes.size (); row < n; ++row) {
        FileInfo *node = nodes.at (row);

        if (fileName == node->fileName ()) {
            return q_func ()->createIndex (row, column, node);
        }
    }

    return QModelIndex ();
}

void FileListModelPrivate::abort ()
{
    future.cancel ();
    future.waitForFinished ();
    sheduledUpdate = false;
}

void FileListModelPrivate::fetchFileList ()
{
    Q_Q (FileListModel);

    if (inUpdate) {
        sheduledUpdate = true;
        return;
    }

    inUpdate = true;

    abort ();

    rootDir.setFilter (QDir::AllEntries | QDir::Hidden | QDir::System);
    rootDir.setSorting (QDir::Unsorted);

    int rowCount = nodes.size ();

    if (rowCount > 0) {
        q->beginRemoveRows (QModelIndex (), 0, rowCount - 1);

        qDeleteAll (nodes);
        nodes.clear ();

        q->endRemoveRows ();
    }


    //Get file list
    QFileInfoList infos;
    future = QtConcurrent::run (getInfoList, rootDir, &infos);

    while (future.isRunning ()) {
        QCoreApplication::processEvents ();
    }

    rowCount = infos.size ();

    if (rowCount > 0) {
        //Sort file list
        QFuture<QFileInfoList> sortFuture = QtConcurrent::run (Dir::sortFileList, infos, sort);

        while (sortFuture.isRunning ()) {
            QCoreApplication::processEvents ();
        }

        infos = sortFuture.result ();

        q->beginInsertRows (QModelIndex (), 0, rowCount - 1);

        for (int row = 0; row < rowCount; ++row) {
            nodes.append (new FileInfo (infos.at (row)));
        }

        q->endInsertRows ();

        future = QtConcurrent::run (getIcons, &nodes, iconProvider);
        futureWatcher.setFuture (future);
    }

    inUpdate = false;
}

void FileListModelPrivate::updateFileList ()
{
    Q_Q (FileListModel);

    if (inUpdate) {
        sheduledUpdate = true;
        return;
    }

    inUpdate = true;

    abort ();

    rootDir.setFilter (QDir::AllEntries | QDir::Hidden | QDir::System);
    rootDir.setSorting (QDir::Unsorted);


    //Get file list
    QFileInfoList infos;
    future = QtConcurrent::run (getInfoList, rootDir, &infos);

    while (future.isRunning ()) {
        QCoreApplication::processEvents ();
    }

    int d = nodes.size () - infos.size ();

    if (d < 0) {
        // add some rows
        d = -d;
        int size = nodes.size ();
        q->beginInsertRows (QModelIndex (), size, size + d - 1);

        for (int row = size, n = size + d; row < n; ++row) {
            nodes.append (new FileInfo (infos.at (row)));
        }

        q->endInsertRows ();
    } else if (d > 0) {
        // remove extra rows
        int size = infos.size ();
        q->beginRemoveRows (QModelIndex (), size, size + d - 1);

        for (int row = size, n = size + d; row < n; ++row) {
            delete nodes.takeLast ();
        }

        q->endRemoveRows ();
    }

    int rowCount = infos.size ();

    if (rowCount > 0) {
        emit q->layoutAboutToBeChanged ();
        QModelIndexList oldList = q->persistentIndexList ();
        QList<QPair<QString, int> > oldNodes;

        for (int i = 0, n = oldList.size (); i < n; ++i) {
            QPair<QString, int> pair (node (oldList.at (i))->fileName (), oldList.at (i).column ());
            oldNodes.append (pair);
        }

        //Sort file list
        QFuture<QFileInfoList> sortFuture = QtConcurrent::run (Dir::sortFileList, infos, sort);

        while (sortFuture.isRunning ()) {
            QCoreApplication::processEvents ();
        }

        infos = sortFuture.result ();

        qDeleteAll (nodes);

        // update changed rows
        for (int row = 0; row < rowCount; ++row) {
            nodes[row] = new FileInfo (infos.at (row));
        }

        QModelIndexList newList;

        for (int i = 0, n = oldNodes.size (); i < n; ++i) {
            QModelIndex idx = index (oldNodes.at (i).first, oldNodes.at (i).second);
            newList.append (idx);
        }

        q->changePersistentIndexList (oldList, newList);
        emit q->layoutChanged ();

        future = QtConcurrent::run (getIcons, &nodes, iconProvider);
        futureWatcher.setFuture (future);
    }

    inUpdate = false;
}

void FileListModelPrivate::_q_directoryChanged ()
{
    Q_Q (FileListModel);
    sheduledUpdate = true;

    if (!updateTimer.isActive ()) {
        updateTimer.start (300, q);
    }
}

void FileListModelPrivate::_q_finishedLoadIcons ()
{
    Q_Q (FileListModel);
    emit q->dataChanged (q->index (0, FileListModel::NameColumn),
                         q->index (nodes.size () - 1, FileListModel::NameColumn));
}

FileListModel::FileListModel (QObject *parent)
    : QAbstractItemModel (parent)
    , d_ptr (new FileListModelPrivate)
{
    d_ptr->q_ptr = this;

    connect (&d_ptr->futureWatcher, SIGNAL (finished ()), this, SLOT (_q_finishedLoadIcons ()));
#ifndef QT_NO_FILESYSTEMWATCHER
    connect (d_ptr->fileSystemWatcher, SIGNAL (directoryChanged (const QString &)),
             this, SLOT (_q_directoryChanged ()));
#endif
}

FileListModel::~FileListModel ()
{
    delete d_ptr;
}

QModelIndex FileListModel::index (int row, int column, const QModelIndex &parent) const
{
    Q_D (const FileListModel);

    if (row < 0 || column < 0 || row >= rowCount (parent) || column >= columnCount (parent)) {
        return QModelIndex ();
    }

    return createIndex (row, column, d->nodes.at (row));
}

QModelIndex FileListModel::buddy (const QModelIndex &index) const
{
    return index.sibling (index.row (), NameColumn);
}

QModelIndex FileListModel::parent (const QModelIndex &child) const
{
//	if(!child.isValid() || (child.row() == 0 && child.column() == 0))
//		return QModelIndex();
    return QModelIndex ();
}

bool FileListModel::hasChildren (const QModelIndex &parent) const
{
    Q_D (const FileListModel);

    if (parent.column () > 0) {
        return false;
    }

    if (!parent.isValid ()) {
        return !d->nodes.isEmpty ();
    }

    return d->node (parent)->isDir ();
}

int FileListModel::rowCount (const QModelIndex &parent) const
{
    Q_D (const FileListModel);

    if (parent.column () > 0) {
        return 0;
    }

    if (!parent.isValid ()) {
        return d->nodes.size ();
    }

    return 0;
}

int FileListModel::columnCount (const QModelIndex &parent) const
{
    return (parent.column () > 0) ? 0 : ColumnCount;
}

Qt::ItemFlags FileListModel::flags (const QModelIndex &index) const
{
    Q_D (const FileListModel);
    FileInfo *node = d->node (index);

    if (!node) {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled;

    bool dotDot = node->fileName () == QLatin1String ("..");

    if (!dotDot) {
        flags |= Qt::ItemIsSelectable;
    }

    QFile::Permissions perms = node->permissions ();

    if (!dotDot && (perms & QFile::ReadUser)) {
        flags |= Qt::ItemIsDragEnabled;
    }

    if (perms & QFile::WriteUser) {
        if (node->isDir ()) {
            flags |= Qt::ItemIsDropEnabled;
        }

        if (!dotDot && index.column () == 0) {
            flags |= Qt::ItemIsEditable;
        }
    }

    return flags;
}

QString FileListModelPrivate::size (qint64 bytes)
{
    // According to the Si standard KB is 1000 bytes, KiB is 1024
    // but on windows sizes are calculated by dividing by 1024 so we do what they do.
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;
    const qint64 tb = 1024 * gb;

    if (bytes >= tb) {
        return FileListModel::tr ("%1 TB").arg (QLocale ().toString (qreal (bytes) / tb, 'f', 3));
    }

    if (bytes >= gb) {
        return FileListModel::tr ("%1 GB").arg (QLocale ().toString (qreal (bytes) / gb, 'f', 2));
    }

    if (bytes >= mb) {
        return FileListModel::tr ("%1 MB").arg (QLocale ().toString (qreal (bytes) / mb, 'f', 1));
    }

    if (bytes >= kb) {
        return FileListModel::tr ("%1 KB").arg (QLocale ().toString (bytes / kb));
    }

    return FileListModel::tr ("%1 bytes").arg (QLocale ().toString (bytes));
}

QVariant FileListModel::data (const QModelIndex &index, int role) const
{
    Q_D (const FileListModel);

    if (!index.isValid () || index.model () != this) {
        return QVariant ();
    }

    FileInfo *node = d->node (index);

    switch (role) {
        case Qt::EditRole:
            switch (index.column ()) {
                case NameColumn:
                    return node->fileName ();

                default:
                    break;
            }

            break;

        case Qt::DisplayRole:
            switch (index.column ()) {
                case NameColumn:
                    return node->name ();

                case TypeColumn:
                    return node->ext ();

                case SizeColumn:
                    return !node->isDir () || node->isSymLink () ? d->size (node->size ()) : tr ("<folder>");

                case ModifiedTimeColumn:
                    return node->lastModified ().toString ("dd.MM.yy hh:mm:ss");

                case OwnerColumn:
                    return node->fileInfo ().owner ();

                case GroupColumn:
                    return node->fileInfo ().group ();

                case PermissionsColumn: {
                    QFile::Permissions perms = node->permissions ();

                    QString ret;
                    ret.append (node->isDir () ? "d" : node->isSymLink () ? "l" : "-");

                    ret.append ((perms & QFile::ReadUser) ? "r" : "-");
                    ret.append ((perms & QFile::WriteUser) ? "w" : "-");
                    ret.append ((perms & QFile::ExeUser) ? "x" : "-");

                    ret.append ((perms & QFile::ReadGroup) ? "r" : "-");
                    ret.append ((perms & QFile::WriteGroup) ? "w" : "-");
                    ret.append ((perms & QFile::ExeGroup) ? "x" : "-");

                    ret.append ((perms & QFile::ReadOther) ? "r" : "-");
                    ret.append ((perms & QFile::WriteOther) ? "w" : "-");
                    ret.append ((perms & QFile::ExeOther) ? "x" : "-");
                    return ret;
                }
                    break;

                default:
                    break;
            }

            break;

        case FilePathRole:
            return node->filePath ();

        case FileNameRole:
            return node->fileName ();

        case Qt::DecorationRole:
            if (index.column () == NameColumn) {
                QIcon icon = node->icon ();

                if (icon.isNull ()) {
                    icon = d->iconProvider->icon (node->isDir () ? QFileIconProvider::Folder : QFileIconProvider::File);
                }

                return icon;
            }

            break;

        case Qt::TextAlignmentRole:
            if (index.column () == SizeColumn) {
                return !isDir (index) ? Qt::AlignRight : Qt::AlignHCenter;
            }

            break;

        case Qt::ToolTipRole:
            switch (index.column ()) {
                case NameColumn:
                    return node->fileName ();

                case TypeColumn:
                    return node->fileName ();

                case SizeColumn:
                    return node->size ();

                case ModifiedTimeColumn:
                    return node->lastModified ().toString ("dd.MM.yyyy hh:mm:ss");

                default:
                    break;
            }

            break;

        case Qt::ForegroundRole:
            if (node->lastModified ().date ().daysTo (QDate::currentDate ()) <= 3) {
                return QColor (Qt::blue);
            }

            break;

        case FilePermissions:
            return (int) node->permissions ();
    }

    return QVariant ();
}

bool FileListModel::setData (const QModelIndex &index, const QVariant &value, int role)
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

QVariant FileListModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        static const QMap<int, QString> header {
            {NameColumn,         tr ("Name")},
            {TypeColumn,         tr ("Type")},
            {SizeColumn,         tr ("Size")},
            {ModifiedTimeColumn, tr ("Date")},
            {OwnerColumn,        tr ("Owner")},
            {GroupColumn,        tr ("Group")},
            {PermissionsColumn,  tr ("Permissions")},
        };
        return header.value (section, QString {});
    }

    return QVariant ();
}

bool FileListModel::isDir (const QModelIndex &index) const
{
    Q_D (const FileListModel);
    FileInfo *node = d->node (index);
    return node ? node->isDir () : false;
}

qint64 FileListModel::size (const QModelIndex &index) const
{
    Q_D (const FileListModel);
    FileInfo *node = d->node (index);
    return node ? node->size () : 0;
}

QFile::Permissions FileListModel::permissions (const QModelIndex &index) const
{
    Q_D (const FileListModel);
    FileInfo *node = d->node (index);
    return node ? node->permissions () : QFile::Permissions (0);
}

QString FileListModel::filePath (const QModelIndex &index) const
{
    Q_D (const FileListModel);
    FileInfo *node = d->node (index);
    return node ? node->filePath () : QString ();
}

QString FileListModel::fileName (const QModelIndex &index) const
{
    Q_D (const FileListModel);
    FileInfo *node = d->node (index);
    return node ? node->fileName () : QString ();
}

QVariant FileListModel::myComputer () const
{
    return QDir::rootPath ();
}

QDir FileListModel::rootDirectory () const
{
    Q_D (const FileListModel);
    return d->rootDir;
}

QString FileListModel::rootPath () const
{
    Q_D (const FileListModel);
    return d->rootDir.absolutePath ();
}

QModelIndex FileListModel::setRootPath (const QString &path)
{
    Q_D (FileListModel);

    d->rootDir.setPath (path);
    d->root.setFileInfo (QFileInfo (path));

    d->fetchFileList ();

    emit rootPathChanged (d->rootDir.absolutePath ());

#ifndef QT_NO_FILESYSTEMWATCHER

    if (!d->fileSystemWatcher->directories ().isEmpty ()) {
        d->fileSystemWatcher->removePaths (d->fileSystemWatcher->directories ());
    }

#ifdef Q_WS_WIN

    if (!d->rootDir.absolutePath().startsWith (QLatin1String ("//")))
#endif
    d->fileSystemWatcher->addPath (d->rootDir.absolutePath ());

#endif

    return QModelIndex ();
}

QModelIndex FileListModel::index (const QString &fileName) const
{
    Q_D (const FileListModel);

    for (int row = 0, n = d->nodes.size (); row < n; ++row) {
        if (d->nodes.at (row)->fileName () == fileName) {
            return index (row, 0, QModelIndex ());
        }
    }

    return QModelIndex ();
}

QDir::Filters FileListModel::filter () const
{
    Q_D (const FileListModel);
    return d->rootDir.filter ();
}

void FileListModel::setFilter (QDir::Filters filters)
{
    Q_D (FileListModel);
    d->rootDir.setFilter (filters);
    d->fetchFileList ();
}

QFileIconProvider *FileListModel::iconProvider () const
{
    Q_D (const FileListModel);
    return d->iconProvider;
}

void FileListModel::setIconProvider (QFileIconProvider *iconProvider)
{
    Q_D (FileListModel);

    if (d->iconProvider == iconProvider) {
        return;
    }

    delete d->iconProvider;
    d->iconProvider = iconProvider;
}

void FileListModel::sort (int column, Qt::SortOrder order)
{
    Q_D (FileListModel);

    if (d->sortColumn == column && d->sortOrder == order) {
        return;
    }

    emit layoutAboutToBeChanged ();
    QModelIndexList oldList = persistentIndexList ();
    QList<QPair<QString, int> > oldNodes;

    for (int i = 0, n = oldList.size (); i < n; ++i) {
        QPair<QString, int> pair (d->node (oldList.at (i))->fileName (), oldList.at (i).column ());
        oldNodes.append (pair);
    }

    d->sortColumn = column;
    d->sortOrder = order;

    d->sort = QDir::DirsFirst | QDir::IgnoreCase;
    d->sort |= QDir::SortFlags (0x100 | 0x200 | 0x300);

    if (d->sortColumn == NameColumn) {
        d->sort |= QDir::Name;
    } else if (d->sortColumn == TypeColumn) {
        d->sort |= QDir::Type;
    } else if (d->sortColumn == SizeColumn) {
        d->sort |= QDir::Size;
    } else if (d->sortColumn == ModifiedTimeColumn) {
        d->sort |= QDir::Time;
    }

    if (d->sortOrder != Qt::AscendingOrder) {
        d->sort |= QDir::Reversed;
    }

    //Sort file list
    QFileInfoList infos;

    for (int i = 0, n = d->nodes.size (); i < n; ++i) {
        infos.append (d->nodes.at (i)->fileInfo ());
    }

    QFuture<QFileInfoList> sortFuture = QtConcurrent::run (Dir::sortFileList, infos, d->sort);

    while (sortFuture.isRunning ()) {
        QCoreApplication::processEvents ();
    }

    infos = sortFuture.result ();

    for (int i = 0, n = infos.size (); i < n; ++i) {
        int j = d->index (infos.at (i).fileName (), 0).row ();
        d->nodes.swap (i, j);
    }

    QModelIndexList newList;

    for (int i = 0, n = oldNodes.size (); i < n; ++i) {
        QModelIndex idx = d->index (oldNodes.at (i).first, oldNodes.at (i).second);
        newList.append (idx);
    }

    changePersistentIndexList (oldList, newList);
    emit layoutChanged ();
}

void FileListModel::abort ()
{
    Q_D (FileListModel);
    d->updateTimer.stop ();
    d->abort ();
}

void FileListModel::refresh ()
{
    Q_D (FileListModel);
    d->updateTimer.stop ();
    d->_q_directoryChanged ();
}

/*!
	Returns a list of MIME types that can be used to describe a list of items
	in the model.
*/
QStringList FileListModel::mimeTypes () const
{
    return QStringList ("text/uri-list");
}

/*!
	Returns an object that contains a serialized description of the specified
	\a indexes. The format used to describe the items corresponding to the
	indexes is obtained from the mimeTypes() function.

	If the list of indexes is empty, 0 is returned rather than a serialized
	empty list.
*/
QMimeData *FileListModel::mimeData (const QModelIndexList &indexes) const
{
    Q_D (const FileListModel);

    QList<QUrl> urls;
    QList<QModelIndex>::const_iterator it = indexes.constBegin ();

    for (; it != indexes.constEnd (); ++it) {
        if ((*it).column () == 0) {
            FileInfo *node = d->node (*it);

            if (node->isLocalFile ()) {
                urls.append (QUrl::fromLocalFile (node->absoluteFilePath ()));
            } else {
                urls.append (QUrl (node->absoluteFilePath ()));
            }
        }
    }

    if (!urls.isEmpty ()) {
        QMimeData *data = new QMimeData ();
        data->setUrls (urls);
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
bool FileListModel::dropMimeData (const QMimeData *data, Qt::DropAction action, int row, int column,
                                  const QModelIndex &parent)
{
    Q_UNUSED (row);
    Q_UNUSED (column);

    if (!data->hasFormat ("text/uri-list") || data->urls ().isEmpty () || isReadOnly ()) {
        return false;
    }

    if (!(flags (parent) & (Qt::ItemIsEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable))) {
        return false;
    }

    bool success = true;

    QList<QUrl> urls = data->urls ();
    QList<QUrl>::const_iterator it = urls.constBegin ();

    QString destination = filePath (parent) + QDir::separator ();

    switch (action) {
        case Qt::CopyAction:
            for (; it != urls.constEnd (); ++it) {
                QString path = (*it).scheme () == QLatin1String ("file") ? (*it).toLocalFile () : (*it).toString ();
                success = QFile::copy (path, destination + QFileInfo (path).fileName ()) && success;
            }

            break;

        case Qt::LinkAction:
            for (; it != urls.constEnd (); ++it) {
                QString path = (*it).scheme () == QLatin1String ("file") ? (*it).toLocalFile () : (*it).toString ();
                success = QFile::link (path, destination + QFileInfo (path).fileName ()) && success;
            }

            break;

        case Qt::MoveAction:
            for (; it != urls.constEnd (); ++it) {
                QString path = (*it).scheme () == QLatin1String ("file") ? (*it).toLocalFile () : (*it).toString ();
                success = QFile::copy (path, destination + QFileInfo (path).fileName ()) && QFile::remove (path) &&
                          success;
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
Qt::DropActions FileListModel::supportedDropActions () const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

void FileListModel::timerEvent (QTimerEvent *event)
{
    Q_D (FileListModel);

    if (event->timerId () == d->updateTimer.timerId ()) {
        d->updateTimer.stop ();

        if (d->sheduledUpdate) {
            d->updateTimer.start (1500, this);
            d->updateFileList ();
        }
    }

    QAbstractItemModel::timerEvent (event);
}

bool FileListModel::isReadOnly () const
{
    return false;
}

void FileListModel::setReadOnly (bool)
{
}

#include <moc_FileListModel.cpp>
