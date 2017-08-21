//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_EDITOR_COMMAND_REGISTRATION__
#define __UI_OPTIONS_EDITOR_COMMAND_REGISTRATION__

#include "OptionsPathValueResolver.h"
#include "OpenKLOptionsTargetEditorCommand.h"
#include "CloseKLOptionsTargetEditorCommand.h"
#include <FabricUI/Commands/CommandRegistry.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>

namespace FabricUI {
namespace OptionsEditor {

class OptionEditorCommandRegistration
{
  /**
    OptionEditorCommandRegistration registered all the OptionsEditor commands. 
  */

  public:
   	static void RegisterCommands()
    {
      Commands::PathValueResolverFactory<OptionsPathValueResolver>::Register(
        "OptionsPathValueResolver");

      Commands::CommandFactory<OpenKLOptionsTargetEditorCommand>::Register(
        "openKLOptionsTargetEditor");

      Commands::CommandFactory<CloseKLOptionsTargetEditorCommand>::Register(
        "closeKLOptionsTargetEditor");
    }
};


} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPTIONS_EDITOR_COMMAND_REGISTRATION__
