//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CreateToolCommand.h"
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Tools;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

CreateToolCommand::CreateToolCommand() 
  : BaseRTValScriptableCommand()
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg("target", "RTVal",
    CommandArgFlags::IO_ARG);

  FABRIC_CATCH_END("CreateToolCommand::CreateToolCommand");
}

CreateToolCommand::~CreateToolCommand() 
{
}

void CreateToolCommand::registrationCallback(
  QString const&name, 
  void *userData)
{
  if(userData != 0)
    m_registry = static_cast<ToolsNotifierRegistry*>(userData);
}

bool CreateToolCommand::canUndo()
{
  return false;
}

bool CreateToolCommand::canLog()
{
  return true;
}

bool CreateToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  // Update the tool'value from its target.
  m_registry->createPathValueTool(
    getRTValArg("target")
    );
 
  return true;

  FABRIC_CATCH_END("CreateToolCommand::doIt");

  return false;
}

QString CreateToolCommand::getHelp()
{
  FABRIC_CATCH_BEGIN();

  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Path of the DFG port that the tool targets";

  return CommandHelpers::createHelpFromRTValArgs(
    this,
    "Create a new PathValue tool",
    argsHelp);

  FABRIC_CATCH_END("CreateToolCommand::getHelp");

  return "";
}
