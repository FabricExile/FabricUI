//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_CREATE_PV_TOOL_COMMAND__
#define __UI_DFG_CREATE_PV_TOOL_COMMAND__

#include "DFGPVToolsNotifier.h"
#include <FabricUI/Tools/CreatePVToolCommand.h>
#include <FabricUI/Tools/DeletePVToolCommand.h>

namespace FabricUI {
namespace DFG {

class DFGCreatePVToolCommand : public Tools::CreatePVToolCommand
{
  Q_OBJECT
  
  public:
    DFGCreatePVToolCommand();

    /// Implementation of BaseCommand.
    virtual void registrationCallback(
      QString const&name, 
      void *userData
      );

    virtual ~DFGCreatePVToolCommand();
 
    /// Implementation of BaseCommand.
    virtual bool doIt();
 
  private:
    DFGPVToolsNotifierRegistry* m_registry;
};

class DFGDeletePVToolCommand : public Tools::DeletePVToolCommand
{
  Q_OBJECT
  
  public:
    DFGDeletePVToolCommand();

    /// Implementation of BaseCommand.
    virtual void registrationCallback(
      QString const&name, 
      void *userData
      );

    virtual ~DFGDeletePVToolCommand();
 
    /// Implementation of BaseCommand.
    virtual bool doIt();
 
  private:
    DFGPVToolsNotifierRegistry* m_registry;
};


} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_CREATE_PV_TOOL_COMMAND__
