#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QtCore/QStringList>

class FileOperation
{
public:
	enum OperationType {
		None = 0,
		Copy,
		Move,
		Remove,
		User = 1000,
		MaxUser = 65535};

	enum OperationFlag {
		Interactive = 0x01,
		Force = 0x02,
		MakeLinks = 0x04,
		FollowLinks = 0x08
	};

	Q_DECLARE_FLAGS(OperationFlags, OperationFlag);

	FileOperation(OperationType type, OperationFlags flags)
			: operationType(type), operationFlags(flags), canceled(false), complited(false)
	{}

	virtual ~FileOperation()
	{}

	inline OperationType type() const
	{return operationType;}

	const QStringList& getSources() const
	{return qslSources;}

	void setSources(const QStringList& sources)
	{qslSources = sources;}

	const QString& getDest() const
	{return qsDest;}

	void setDest(const QString& dest)
	{qsDest = dest;}

protected:
	OperationType operationType;

	QStringList qslSources;
	QString qsDest;

	ushort operationFlags;

	bool canceled;
	bool complited;
};

class CopyFileOperation: public FileOperation
{
public:
	CopyFileOperation(const QStringList& from, const QString& to, OperationFlags flags)
			:FileOperation(Copy, flags)
	{
		qslSources = from;
		qsDest = to;
		operationType = Copy;
		operationFlags = flags;
	}

	virtual ~CopyFileOperation()
	{}
};

class MoveFileOperation: public FileOperation
{
public:
	MoveFileOperation(const QStringList& from, const QString& to, OperationFlags flags)
			:FileOperation(Move, flags)
	{
		qslSources = from;
		qsDest = to;
		operationType = Move;
		operationFlags = flags;
	}

	virtual ~MoveFileOperation()
	{}
};

class RemoveFileOperation: public FileOperation
{
public:
	RemoveFileOperation(const QStringList& files, OperationFlags flags)
			:FileOperation(Remove, flags)
	{
		qslSources = files;
		operationType = Remove;
		operationFlags = flags;
	}

	virtual ~RemoveFileOperation()
	{}
};

#endif // FILEOPERATIONS_H
