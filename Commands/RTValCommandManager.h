//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_COMMAND_MANAGER__
#define __UI_RTVAL_COMMAND_MANAGER__
 
#include <FabricCore.h>
#include "CommandManager.h"
 
namespace FabricUI {
namespace Commands {

class RTValCommandManager : public CommandManager
{
  /**
    RTValCommandManager specializes CommandManager to support 
    RTValScriptable commands.
  */
  Q_OBJECT

  public:
    RTValCommandManager();

    virtual ~RTValCommandManager();
    
    /// Implementation of CommandManager.
    virtual BaseCommand* createCommand(
      QString const&cmdName, 
      QMap<QString, QString> const&args, 
      bool doCmd = true,
      int canMergeID = NoCanMergeID
      );

    /// Creates and executes a command (if doCmd == true).
    /// If executed, the command is added to the manager stack.
    /// Throws an exception if an error occurs.
    virtual BaseCommand* createCommand(
      QString const&cmdName, 
      QMap<QString, FabricCore::RTVal> const&args, 
      bool doCmd = true,
      int canMergeID = NoCanMergeID
      );

  protected:
    /// Checks the command arguments before doing it.
    /// Throws an exception if an error occurs.
    void checkRTValCommandArgs(
      BaseCommand *cmd,
      QMap<QString, FabricCore::RTVal> const&args
      );

  private:
    /// Implementation of CommandManager.
    /// Used to resolve the command PathValue args.
    virtual void preDoCommand(
      BaseCommand* cmd
      );

    /// Implementation of CommandManager.
    /// Used to resolve the command PathValue args.
    virtual void postDoCommand(
      BaseCommand* cmd
      );
};
 
} // namespace Commands
} // namespace FabricUI

#endif // __UI_RTVAL_COMMAND_MANAGER__
