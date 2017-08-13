//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGCreatePVToolCommand.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace DFG;
using namespace Tools;
using namespace FabricCore;
using namespace Application;

DFGCreatePVToolCommand::DFGCreatePVToolCommand() 
  : CreateToolCommand()
{
}

DFGCreatePVToolCommand::~DFGCreatePVToolCommand() 
{
}

void DFGCreatePVToolCommand::registrationCallback(
  QString const&name, 
  void *userData)
{
  if(userData != 0)
    m_registry = static_cast<ToolsNotifierRegistry*>(userData);
}
 
bool DFGCreatePVToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  if(CreateToolCommand::doIt())
  {
    RTVal pathValue = getRTValArg("target");
    m_registry->registerPathValueTool(pathValue);
    return true;
  }
 
  FABRIC_CATCH_END("DFGCreatePVToolCommand::doIt");

  return false;
}
