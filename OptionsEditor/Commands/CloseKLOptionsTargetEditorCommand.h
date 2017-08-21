//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_CLOSE_KL_OPTIONS_TARGET_EDITOR_COMMAND__
#define __UI_CLOSE_KL_OPTIONS_TARGET_EDITOR_COMMAND__

#include <FabricUI/Commands/BaseRTValScriptableCommand.h>
 
namespace FabricUI {
namespace OptionsEditor {

class CloseKLOptionsTargetEditorCommand : public Commands::BaseRTValScriptableCommand
{
  /**
    Command to close an option editor widget. 

    Arguments:
      - editorID (String): ID of the KL OptionsEditorTarget in the registry
      - failSilently (Boolean): If false, throws an error if the widget has not been closed
  */
  Q_OBJECT
  
  public:
    CloseKLOptionsTargetEditorCommand();

    virtual ~CloseKLOptionsTargetEditorCommand();

    /// Implementation of Commands.
    virtual bool doIt();

    /// Implementation of Commands.
    virtual bool canUndo();

    /// Implementation of Commands.
    virtual bool canLog();

    /// Implementation of Commands.
    virtual QString getHelp();

    /// Implementation of Command.
    virtual QString getHistoryDesc();

  private:
    bool m_canLog;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_CLOSE_KL_OPTIONS_TARGET_EDITOR_COMMAND__
