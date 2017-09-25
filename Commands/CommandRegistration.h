//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_REGISTRATION__
#define __UI_COMMAND_REGISTRATION__

#include "SetPathValueCommand.h"
#include "CommandRegistry.h"

namespace FabricUI {
namespace Commands {

class CommandRegistration
{
  public:
    static void RegisterCommands()
    {
      Commands::CommandFactory<SetPathValueCommand>::Register(
        "setPathValue");
    }
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_REGISTRATION__
