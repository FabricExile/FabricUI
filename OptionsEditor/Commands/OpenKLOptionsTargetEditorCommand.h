//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPEN_KL_OPTIONS_TARGET_EDITOR_COMMAND__
#define __UI_OPEN_KL_OPTIONS_TARGET_EDITOR_COMMAND__

#include <QDockWidget>
#include "../BaseRTValOptionsEditor.h"
#include <FabricUI/Commands/BaseRTValScriptableCommand.h>
 
namespace FabricUI {
namespace OptionsEditor {

class OpenKLOptionsTargetEditorCommand : public Commands::BaseRTValScriptableCommand
{
  /**
    Command to open an option editor widget. 

    Arguments:
      - editorID (String): ID of the KL OptionsEditorTarget in the registry
      - editorTitle (String): Title of the widget
  */
  Q_OBJECT
  
  public:
    OpenKLOptionsTargetEditorCommand();

    virtual ~OpenKLOptionsTargetEditorCommand();

    /// Implementation of Command.
    virtual bool doIt();

    /// Implementation of Commands.
    virtual bool canUndo();

    /// Implementation of Command.
    virtual bool canLog();
    
    /// Implementation of Command.
    virtual QString getHelp();

    /// Implementation of Command.
    virtual QString getHistoryDesc();

  private:
    bool m_canLog;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPEN_KL_OPTIONS_TARGET_EDITOR_COMMAND__
