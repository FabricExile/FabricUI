//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "PathValueTool.h"
#include "CreatePVToolCommand.h"
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Tools;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

CreatePVToolCommand::CreatePVToolCommand() 
  : BaseRTValScriptableCommand()
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg("target", "RTVal",
    CommandArgFlags::IO_ARG);

  RTVal createIfExistVal = RTVal::ConstructBoolean(
    FabricApplicationStates::GetAppStates()->getContext(), 
    false
    );

  declareRTValArg(
    "createIfExist", 
    "Boolean",
    CommandArgFlags::IN_ARG | CommandArgFlags::OPTIONAL_ARG,
    createIfExistVal
    );

  FABRIC_CATCH_END("CreatePVToolCommand::CreatePVToolCommand");
}

CreatePVToolCommand::~CreatePVToolCommand() 
{
}

bool CreatePVToolCommand::canUndo()
{
  return false;
}

bool CreatePVToolCommand::canLog()
{
  return true;
}

bool CreatePVToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  // Update the tool'value from its target.
  RTVal pathValue = getRTValArg("target");
  bool createIfExist = getRTValArgValue("createIfExist").getBoolean();

  RTVal pathValueTool = PathValueTool::getTool(pathValue);
  bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

  if(!createIfExist && pathValueToolIsValid)
    FabricException::Throw(
      "CreatePVToolCommand::doIt",
      "A PathValueTool of type '" + getRTValArgType("target") + "' already targets the path '" + getRTValArgPath("target") + "'"
      );

  pathValueTool = PathValueTool::createTool(pathValue);
  pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

  if(!pathValueToolIsValid)
    FabricException::Throw(
      "CreatePVToolCommand::doIt",
      "The PathValue tool of type '" + getRTValArgType("target") + "' targeting the path '" + getRTValArgPath("target") + "' is invalid"
);

  return true;
  
  FABRIC_CATCH_END("CreatePVToolCommand::doIt");

  return false;
}

QString CreatePVToolCommand::getHelp()
{
  FABRIC_CATCH_BEGIN();

  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Path of the DFG port that the tool targets";
  argsHelp["createIfExist"] = "Force the creation a new tool targeting 'target'";

  return CommandHelpers::createHelpFromRTValArgs(
    this,
    "Create a new PathValue tool",
    argsHelp);

  FABRIC_CATCH_END("CreatePVToolCommand::getHelp");

  return "";
}
