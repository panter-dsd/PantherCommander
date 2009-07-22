#ifndef FILECONTEXTEMNU_H
#define FILECONTEXTEMNU_H

#include <QtCore/QStringList>

#include <QtGui/QMenu>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>
#include <QtGui/QWidgetAction>

class FileContextMenuPrivate;
class FileContextMenu : public QMenu
{
	Q_OBJECT

public:
	explicit FileContextMenu(QWidget* parent = 0);
	explicit FileContextMenu(const QString& title, QWidget* parent = 0);
	virtual ~FileContextMenu();

	void setPath(const QString& path);
	void setPaths(const QStringList& paths);

	QAction* executeNativeMenu(const QPoint& pos);

protected:
	void moveEvent(QMoveEvent* event);
	void paintEvent(QPaintEvent* event);

private:
	friend class FileContextMenuPrivate;
	FileContextMenuPrivate* const d;
	Q_PRIVATE_SLOT(d, void _q_aboutToShow())
	Q_PRIVATE_SLOT(d, void _q_aboutToHide())
	Q_PRIVATE_SLOT(d, void _q_nativeActionTriggered())

	friend class EvilWidget;
};


class FileContextMenuActionPrivate;
class FileContextMenuAction : public QWidgetAction
{
	Q_OBJECT

public:
	explicit FileContextMenuAction(QObject* parent = 0);
	virtual ~FileContextMenuAction();

private:
	FileContextMenuActionPrivate* const d;

	friend class FileContextMenu;
	friend class FileContextMenuPrivate;
};


class EvilWidget : public QWidget
{
	Q_OBJECT

public:
	explicit EvilWidget(QWidget* parent = 0);

protected:
	void paintEvent(QPaintEvent* event);

private:
	friend class FileContextMenuPrivate;

	FileContextMenu* qmenu;
	FileContextMenuAction* action;
	bool drawn;
};

#endif // FILECONTEXTEMNU_H
