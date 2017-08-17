//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QMap>
#include "DFGPVToolActions.h"
#include <FabricUI/Tools/ToolManager.h>
#include <FabricUI/Commands/CommandManager.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace DFG;
using namespace Tools;
using namespace Commands;
using namespace Application;
using namespace FabricCore;

DFGCreatePVToolAction::DFGCreatePVToolAction(
  QObject *parent,
  QString const& name,
	QString const& text,
	QString const& itemPath)
  : Actions::BaseAction(parent, name, text, QKeySequence())
  , m_itemPath(itemPath)
{
}

DFGCreatePVToolAction::~DFGCreatePVToolAction()
{
}

void DFGCreatePVToolAction::onTriggered()
{
	try
	{
   	CommandManager* manager = CommandManager::getCommandManager();
    QMap<QString, QString> args;
    args["target"] = "<" + m_itemPath + ">";
    manager->createCommand( "createDFGPVTool", args );
	}

	catch(FabricException &e)
	{
		FabricException::Throw(
			"DFGCreatePVToolAction::onTriggered",
			e.what(),
			"",
			FabricException::LOG
			);
	}
}

DFGDeletePVToolAction::DFGDeletePVToolAction(
  QObject *parent,
  QString const& name,
	QString const& text,
	QString const& itemPath)
  : Actions::BaseAction(parent, name, text, QKeySequence())
  , m_itemPath(itemPath)
{
}

DFGDeletePVToolAction::~DFGDeletePVToolAction()
{
}

void DFGDeletePVToolAction::onTriggered()
{
	try
	{
   	CommandManager* manager = CommandManager::getCommandManager();
    QMap<QString, QString> args;
    args["target"] = "<" + m_itemPath + ">";
    manager->createCommand( "deleteDFGPVTool", args );
	}

	catch(FabricException &e)
	{
		FabricException::Throw(
			"DFGDeletePVToolAction::onTriggered",
			e.what(),
			"",
			FabricException::LOG
			);
	}
}

DFGDeleteAllPVToolsAction::DFGDeleteAllPVToolsAction(
  QObject *parent,
  QString const& name,
  QString const& text)
  : Actions::BaseAction(parent, name, text, QKeySequence())
{
}

DFGDeleteAllPVToolsAction::~DFGDeleteAllPVToolsAction()
{
}

void DFGDeleteAllPVToolsAction::onTriggered()
{
	try
	{
   	CommandManager* manager = CommandManager::getCommandManager();
    manager->createCommand( "deleteAllDFGPVTools" );
	}

	catch(FabricException &e)
	{
		FabricException::Throw(
			"DFGDeleteAllPVToolsAction::onTriggered",
			e.what(),
			"",
			FabricException::LOG
			);
	}
}

DFGDeleteAllAndCreatePVToolAction::DFGDeleteAllAndCreatePVToolAction(
  QObject *parent,
  QString const& name,
	QString const& text,
	QString const& itemPath)
  : Actions::BaseAction(parent, name, text, QKeySequence())
  , m_itemPath(itemPath)
{
}

DFGDeleteAllAndCreatePVToolAction::~DFGDeleteAllAndCreatePVToolAction()
{
}

void DFGDeleteAllAndCreatePVToolAction::onTriggered()
{
	try
	{
   	CommandManager* manager = CommandManager::getCommandManager();
    QMap<QString, QString> args;
    args["target"] = "<" + m_itemPath + ">";
    manager->createCommand( "deleteAllAndCreateDFGPVTool", args );
	}

	catch(FabricException &e)
	{
		FabricException::Throw(
			"DFGDeleteAllAndCreatePVToolAction::onTriggered",
			e.what(),
			"",
			FabricException::LOG
			);
	}
}

DFGPVToolMenu::DFGPVToolMenu(
  QWidget *parent,
  QString const& itemPath)
  : QMenu(parent),
  m_itemPath(itemPath)
{
  QObject::connect(
    this, 
    SIGNAL(aboutToShow()), 
    this, 
    SLOT(onConstructMenu())
    );
}

DFGPVToolMenu::~DFGPVToolMenu()
{
}

bool DFGPVToolMenu::canCreate(
  QString const& itemPath)
{
  FABRIC_CATCH_BEGIN();
 
  if(!itemPath.isEmpty())
    return ToolManager::canCreatePathValueTool(itemPath); 

  FABRIC_CATCH_END("DFGPVToolMenu::canCreate");

  return 0;
}

QMenu* DFGPVToolMenu::createMenu(
  QWidget *parent,
  QString const& itemPath)
{
  return new DFGPVToolMenu( 
    parent,
    itemPath
    );
}

void DFGPVToolMenu::onConstructMenu()
{
  QAction* action;
  foreach(action, createActions(this, m_itemPath))
    addAction(action);
}

QList<QAction*> DFGPVToolMenu::createActions(
  QWidget *parent,
  QString const& itemPath)
{
  QList<QAction*> actions;

  RTVal pathValueTool = ToolManager::getPathValueTool(itemPath);
  bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

  if(!pathValueToolIsValid && ToolManager::canCreatePathValueTool(itemPath))
  {
    QAction* action = new DFGDeleteAllAndCreatePVToolAction(
      parent,
      "DFGDeleteAllAndCreatePVToolAction",
      "Edit With Tool",
      itemPath
      );

    if(action)
      actions.append(action);
  }

  else if(pathValueToolIsValid)
  {
    QAction* action = new DFGDeletePVToolAction(
      parent,
      "DFGDeletePVToolAction",
      "Close Tool",
      itemPath
      );

    if(action)
      actions.append(action);
  }

  return actions;
}
