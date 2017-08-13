//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QMap>
#include "DFGCreatePVToolAction.h"
#include <FabricUI/Commands/CommandManager.h>
#include <FabricUI/Application/FabricException.h>
#include <iostream>

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
 	createTool();
}
 
void DFGCreatePVToolAction::createTool()
{ 
  std::cout << "DFGCreatePVToolAction::createTool 1 " << std::endl;

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
			"DFGCreatePVToolAction::createTool",
			e.what(),
			"",
			FabricException::LOG
			);
	}

  std::cout << "DFGCreatePVToolAction::createTool 2 " << std::endl;
}
