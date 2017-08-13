//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_CREATE_TOOLS_COMMAND__
#define __UI_DFG_CREATE_TOOLS_COMMAND__

#include <FabricUI/Tools/CreatePVToolCommand.h>
#include <FabricUI/Tools/DFGPVToolsNotifierRegistry.h>

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
    Tools::DFGPVToolsNotifierRegistry* m_registry;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_CREATE_TOOLS_COMMAND__
