//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_TOOLS_COMMAND_REGISTRATION__
#define __UI_DFG_TOOLS_COMMAND_REGISTRATION__

#include "DFGPVToolsNotifier.h"
#include "DFGCreatePVToolCommand.h"
#include <FabricUI/Commands/CommandRegistry.h>
 
namespace FabricUI {
namespace DFG {

class DFGToolsCommandRegistration
{
  /**
    DFGToolsCommandRegistration registered all the DFG Tools commands. 
  */

  public:
   	static void RegisterCommands(
   		DFG::DFGPVToolsNotifierRegistry *registry)
    {
      Commands::CommandFactory<DFGCreatePVToolCommand>::Register(
        "createDFGPVTool",
        (void*)registry
        );

      Commands::CommandFactory<DFGDeletePVToolCommand>::Register(
        "deleteDFGPVTool",
        (void*)registry
        );
    }
};

} // namespace DFG 
} // namespace FabricUI

#endif // __UI_DFG_TOOLS_COMMAND_REGISTRATION__
