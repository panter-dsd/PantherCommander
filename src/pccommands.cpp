#include <QtCore/QCoreApplication>
#include <QtGui/QAction>

#include "pccommands.h"

PCCommands* PCCommands::pInstance = 0;

PCCommands* PCCommands::instance()
{
	if (!PCCommands::pInstance)
		PCCommands::pInstance = new PCCommands();
	return PCCommands::pInstance;
}

PCCommands::PCCommands(QObject* parent) : QObject(parent)
{
}

PCCommands::~PCCommands()
{
	if(pInstance == this)
		pInstance = 0;
}

void PCCommands::addAction(const QString &category, QAction *action)
{
	actionHash.insert(category, action);
}

QStringList PCCommands::categories()
{
	return actionHash.uniqueKeys();
}

QList<QAction*> PCCommands::actions(const QString &categoty)
{
	return categoty.isEmpty() ? actionHash.values() : actionHash.values(categoty);
}

QAction* PCCommands::action(const QString& actionName)
{
	foreach(QAction *action, actionHash.values())
		if (action->objectName() == actionName)
			return action;
	return 0;
}
