//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPEN_SIMPLE_DIALOG_COMMAND__
#define __UI_OPEN_SIMPLE_DIALOG_COMMAND__

#include <FabricUI/Commands/BaseScriptableCommand.h>

namespace FabricUI {
namespace Dialog {

class OpenSimpleDialogCommand : public Commands::BaseScriptableCommand
{
  Q_OBJECT
  
  public:
    OpenSimpleDialogCommand();

    virtual ~OpenSimpleDialogCommand();

    /// Implementation of BaseCommand.
    virtual bool canUndo();

    /// Implementation of BaseCommand.
    virtual bool canLog();

    /// Implementation of BaseCommand.
    virtual bool doIt();

    /// Implementation of BaseCommand.
    virtual QString getHelp();
};

} // namespace Dialog
} // namespace FabricUI

#endif // __UI_OPEN_SIMPLE_DIALOG_COMMAND__
