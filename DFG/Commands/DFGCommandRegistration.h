//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_COMMAND_REGISTRATION__
#define __UI_DFG_COMMAND_REGISTRATION__

#include "DFGPathValueResolver.h"
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/Commands/CommandRegistry.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>

namespace FabricUI {
namespace DFG {

class DFGCommandRegistration
{
  /**
    DFGCommandRegistration registered the commands for the DFG.
  */

  public:
    static void RegisterCommands(
      DFG::DFGController *controller)
    {
      Commands::PathValueResolverFactory<DFGPathValueResolver>::Register(
        "DFGPathValueResolver",
        (void*)(controller));
    }
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_COMMAND_REGISTRATION__
