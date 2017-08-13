//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "PathValueTool.h"
#include "CreatePVToolCommand.h"
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>

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

  PathValueTool::createTool(pathValue);
  
  return true;
  
  FABRIC_CATCH_END("CreatePVToolCommand::doIt");

  return false;
}

QString CreatePVToolCommand::getHelp()
{
  FABRIC_CATCH_BEGIN();

  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Path of the DFG port that the tool targets";

  return CommandHelpers::createHelpFromRTValArgs(
    this,
    "Create a new PathValue tool",
    argsHelp);

  FABRIC_CATCH_END("CreatePVToolCommand::getHelp");

  return "";
}
