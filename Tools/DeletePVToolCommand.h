//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DELETE_PV_TOOL_COMMAND__
#define __UI_DELETE_PV_TOOL_COMMAND__

#include <FabricUI/Commands/BaseRTValScriptableCommand.h>

namespace FabricUI {
namespace Tools {

class DeletePVToolCommand : public Commands::BaseRTValScriptableCommand
{
  Q_OBJECT
  
  public:
    DeletePVToolCommand();

    virtual ~DeletePVToolCommand();

    /// Implementation of BaseCommand.
    virtual bool canUndo();

    /// Implementation of BaseCommand.
    virtual bool canLog();

    /// Implementation of BaseCommand.
    virtual bool doIt();

    /// Implementation of BaseCommand.
    virtual QString getHelp();
};

} // namespace Tools
} // namespace FabricUI

#endif // __UI_DELETE_PV_TOOL_COMMAND__
