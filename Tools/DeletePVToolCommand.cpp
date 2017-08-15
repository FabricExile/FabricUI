//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "ToolManager.h"
#include "DeletePVToolCommand.h"
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Tools;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

DeletePVToolCommand::DeletePVToolCommand() 
  : BaseRTValScriptableCommand()
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg("target", "RTVal",
    CommandArgFlags::IO_ARG);
 
  FABRIC_CATCH_END("DeletePVToolCommand::DeletePVToolCommand");
}

DeletePVToolCommand::~DeletePVToolCommand() 
{
}

bool DeletePVToolCommand::canUndo()
{
  return false;
}

bool DeletePVToolCommand::canLog()
{
  return true;
}

bool DeletePVToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  // Update the tool'value from its target.
  RTVal pathValue = getRTValArg("target");
 
  RTVal pathValueTool = ToolManager::getPathValueTool(pathValue);
  bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

  if(!pathValueToolIsValid)
    FabricException::Throw(
      "DeletePVToolCommand::doIt",
      "The PathValue tool of type '" + getRTValArgType("target") + "' targeting the path '" + getRTValArgPath("target") + "' is invalid"
      );

  ToolManager::deletePathValueTool(pathValue);

  return true;
  
  FABRIC_CATCH_END("DeletePVToolCommand::doIt");

  return false;
}

QString DeletePVToolCommand::getHelp()
{
  FABRIC_CATCH_BEGIN();

  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Path of the DFG port that the tool targets";

  return CommandHelpers::createHelpFromRTValArgs(
    this,
    "Delete a PathValue tool",
    argsHelp);

  FABRIC_CATCH_END("DeletePVToolCommand::getHelp");

  return "";
}
