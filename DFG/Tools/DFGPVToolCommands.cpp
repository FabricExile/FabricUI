//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGPVToolCommands.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace DFG;
using namespace Tools;
using namespace FabricCore;
using namespace Application;

DFGCreatePVToolCommand::DFGCreatePVToolCommand() 
  : CreatePVToolCommand()
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
    m_registry = static_cast<DFGPVToolsNotifierRegistry*>(userData);
}
 
bool DFGCreatePVToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  if(CreatePVToolCommand::doIt())
  {
    RTVal pathValue = getRTValArg("target");
    m_registry->registerPathValueTool(pathValue);
    return true;
  }
 
  FABRIC_CATCH_END("DFGCreatePVToolCommand::doIt");

  return false;
}

DFGDeletePVToolCommand::DFGDeletePVToolCommand() 
  : DeletePVToolCommand()
{
}

DFGDeletePVToolCommand::~DFGDeletePVToolCommand() 
{
}

void DFGDeletePVToolCommand::registrationCallback(
  QString const&name, 
  void *userData)
{
  if(userData != 0)
    m_registry = static_cast<DFGPVToolsNotifierRegistry*>(userData);
}
 
bool DFGDeletePVToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  if(DeletePVToolCommand::doIt())
  {
    RTVal pathValue = getRTValArg("target");
    m_registry->registerPathValueTool(pathValue);
    return true;
  }
 
  FABRIC_CATCH_END("DFGDeletePVToolCommand::doIt");

  return false;
}
