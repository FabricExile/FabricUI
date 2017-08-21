//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND__
#define __UI_KL_COMMAND__

#include <FabricCore.h>
#include "BaseCommand.h"

namespace FabricUI {
namespace Commands {

class KLCommand : public BaseCommand
{
  /**
    KLCommand wraps the KL commands so they are accessible from C++/Python.
    They are created on-demand by the C++/Python registry when a KL command 
    is created from KL (after manager synchronisation).
    
    Usage:
    - KL:
      object MyCommand : BaseCommand {
        String name;
      };

      String MyCommand.getName() {
        return this.name;
      }

      MyCommand.setName!( String name ) {
        this.name = name;
      }

      Boolean MyCommand.canUndo() {
        return true;
      }

      Boolean MyCommand.doIt!( io String error ) {
        ... Do your logic
        return true;
      }

      Boolean MyCommand.undoIt!() {
        ... Undo it
        return true;
      }

      Boolean MyCommand.redoIt!() {
        ... Redo it
        return true;
      }
      
      // Register the command
      getCommandRegistry().registerCommand(
        "myCommand", 
        MyCommand);

    - C++:
      // Create an execute the command
      CommandManager *manager = CommandManager.getCommandManager();
      BaseCommand *cmd = manager->createCommand("myCommand") 

    - Python:
      // Create an execute the command
      manager = getCommandManager()
      cmd = manager.createCommand("myCommand") 
  */
  
  Q_OBJECT
  
  public:
    KLCommand(
      FabricCore::RTVal klCmd
      );

    virtual ~KLCommand();

    /// Implementation of BaseCommand.
    virtual QString getName();
 
    /// Implementation of BaseCommand.
    virtual bool canUndo();

    /// Implementation of BaseCommand.
    virtual bool canLog();

    /// Implementation of BaseCommand.
    virtual bool doIt();

    /// Implementation of BaseCommand.
    virtual bool undoIt();

    /// Implementation of BaseCommand.
    virtual bool redoIt();

    /// Implementation of BaseCommand.
    virtual QString getHelp();

    /// Implementation of BaseCommand.
    virtual QString getHistoryDesc();

    /// Implementation of BaseCommand.
    virtual void setCanMergeID(
      int canMergeID
      );

    /// Implementation of BaseCommand.
    virtual int getCanMergeID();

    /// Implementation of BaseCommand.
    virtual bool canMerge(
      BaseCommand *prevCmd,
      bool &undoPrevAndMergeFirst
      );

    /// Implementation of BaseCommand.
    virtual void merge(
      BaseCommand *prevCmd
      );
    
  private:
    /// KL BaseCommand
    FabricCore::RTVal m_klCmd;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND__
