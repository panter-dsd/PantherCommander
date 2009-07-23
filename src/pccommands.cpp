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
