//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QMap>
#include "DFGCreatePVToolAction.h"
#include <FabricUI/Commands/CommandManager.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace DFG;
using namespace Commands;
using namespace Application;

DFGCreatePVToolAction::DFGCreatePVToolAction(
  QObject *parent,
	QString const& itemPath)
  : Actions::BaseAction(parent)
  , m_itemPath(itemPath)
{
	setText("Create tool");
	
 	connect(
    this,
    SIGNAL(triggered(bool)),
    this,
    SLOT(onTriggered(bool))
    );
}

DFGCreatePVToolAction::~DFGCreatePVToolAction()
{
}

void DFGCreatePVToolAction::onTriggered(
	bool checked)
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
	QString const& itemPath)
  : Actions::BaseAction(parent)
  , m_itemPath(itemPath)
{
	setText("Delete tool");
	
 	connect(
    this,
    SIGNAL(triggered(bool)),
    this,
    SLOT(onTriggered(bool))
    );
}

DFGDeletePVToolAction::~DFGDeletePVToolAction()
{
}

void DFGDeletePVToolAction::onTriggered(
	bool checked)
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


