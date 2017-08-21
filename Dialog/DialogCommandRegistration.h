//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DIALOG_COMMAND_REGISTRATION__
#define __UI_DIALOG_COMMAND_REGISTRATION__

#include "OpenSimpleDialogCommand.h"
#include <FabricUI/Commands/CommandRegistry.h>
 
namespace FabricUI {
namespace Dialog {

class DialogCommandRegistration
{
  /**
    DialogCommandRegistration registered all the Dialog commands. 
  */

  public:
   	static void RegisterCommands()
    {
      Commands::CommandFactory<OpenSimpleDialogCommand>::Register(
        "openSimpleDialog");
    }
};

} // namespace Dialog 
} // namespace FabricUI

#endif // __UI_DIALOG_COMMAND_REGISTRATION__
