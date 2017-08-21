//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_MANAGER__
#define __UI_COMMAND_MANAGER__

#include <QMap>
#include <QList>
#include "BaseCommand.h"
#include <QSharedPointer>

// Need to use a typedef because gcc doesn't support templated default arguments:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=39426 
typedef QMap<QString, QString> QMapString;

namespace FabricUI {
namespace Commands {

class CommandManager : public QObject
{
  /**
    CommandManager contains and manages the command undo-redo stack. It's in charge of creating  
    the commands, executing and undo-redoing them. CommandManager is specialized depending on   
    the command registration system, C++ vs Phyton, cf, CommandManager for C++ implementation, 
    CommandManager_Python and Python/Canvas/CommandManager.py for Python. 

    The manager is shared between C++ and Python, so commands defined in Python can be created
    from C++, and vice versa. The manager can only create commands /C++/Python) registered in 
    the registry. When a command is added to the stack, the signal `commandDone` is emitted. 
    
    The manager sets it-self as a singleton when it's constructed:
    - Create the singleton: new CommandManager();
    - Get the singleton: CommandManager *cmdManager = CommandManager::getCommandManager();
  */
  Q_OBJECT

  public:
    /// Command merging
    static int NoCanMergeID; 

    /// Debug modes
    static int NoDebug;
    static int Debug;
    static int VerboseDebug;

    CommandManager();

    virtual ~CommandManager();
  
    /// Gets the manager singleton.
    /// Thows an error if the manager has not been created.
    static CommandManager* getCommandManager();
    
    /// Checks if the manager has been created.
    static bool isInitalized();

    /// Creates and executes a command (if doCmd == true).
    /// If executed, the command is added to the manager stack.
    /// Throws an exception if an error occurs.
    virtual BaseCommand* createCommand(
      QString const&cmdName, 
      QMap<QString, QString> const&args = QMapString(), 
      bool doCmd = true,
      int canMergeID = NoCanMergeID
      );

    /// Executes a command and adds it to the undo stack.
    /// Throws an exception if an error occurs.
    void doCommand(
      BaseCommand* cmd,
      int canMergeID = NoCanMergeID
      );

    /// Undoes the current command.
    /// Throws an exception if an error occurs.
    void undoCommand();

    /// Redoes the next command.
    /// Throws an exception if an error occurs.
    void redoCommand();

    /// Clears all the commands.
    virtual void clear();

    /// Returns the number of top commands.
    int count();

    /// Returns the number of commands
    /// (top+low) in the undo stack.
    int totalUndoCount();

    /// Gets the current index (next command to undo).
    /// If -1 is returned, there is no command to undo.
    int getStackIndex();

    /// Gets the stack content as a string.
    /// Used for debugging.
    virtual QString getContent(
      bool withArgs = true
      );
 
    /// Gets a new interaction ID.
    virtual int getNewCanMergeID();

    /// Sets the debug mode (NoDebug, Debug or VerboseDebug)
    void setDebugMode(
      int debugMode
      );

  signals:
    /// Emitted when a top command has been succefully executed.
    /// \param cmd The command that has been done (executed).
    /// \param canUndo If true, the command can undo and is on the top of the command stack.
    void commandDone(
      FabricUI::Commands::BaseCommand *cmd,
      bool canUndo
      );

    /// Emitted when the manager is cleared.
    void cleared();

  protected:
    /// Checks the command arguments before doing it.
    /// Throws an exception if an error occurs.
    void checkCommandArgs(
      BaseCommand *cmd,
      QMap<QString, QString> const&args
      );

    /// Informs a command has been pushed. 
    /// Needed by the python implementation.
    virtual void commandPushed(
      BaseCommand *cmd,
      bool isLowCmd = false
      );

    /// Clears the redo stack.
    virtual void clearRedoStack();

    /// Pre-processes the args.
    /// Called before creating a command.
    virtual void preDoCommand(
      BaseCommand* cmd
      );

    /// Post-processes the args. Called after 
    /// creating, undoing or redoing a command.
    virtual void postDoCommand(
      BaseCommand* cmd
      );

    /// Cleans the stacks if errors occur when
    /// doing a command and throws an exception.
    virtual void cleanupUnfinishedCommandsAndThrow(
      BaseCommand *cmd,
      QString const&error
      );

    /// Contains a command (top) and its 
    /// sub-commands (low) in a flat array.
    struct StackedCommand 
    {
      bool succeeded;
      // Use shared pointer so th
      QSharedPointer< BaseCommand > topLevelCmd;
      QList< QSharedPointer<BaseCommand> > lowLevelCmds;
      StackedCommand() { succeeded = false; }
    };

    /// Undo-redo stacks
    QList<StackedCommand> m_undoStack, m_redoStack;
    /// Command merging counter.
    int m_canMergeIDCounter;

  private:
    /// Clears a specific stack.
    void clearCommandStack(
      QList<StackedCommand> &stackedCmd
      );

    /// Pushes a sub-command.
    void pushLowCommand(
      BaseCommand *cmd
      );

    /// Pushes a top-command.
    void pushTopCommand(
      BaseCommand *cmd,
      bool succeeded = false
      );

    /// Gets a specific stack content 
    /// as a string, used for debugging.
    QString getStackContent(
      QString const&stackName, 
      QList<StackedCommand> const& stack,
      bool withArgs = true
      );

    /// Cleans the stacks if errors occur when
    /// undoing a command and throws an exception.
    void cleanupUnfinishedUndoLowCommandsAndThrow(
      int lowCmdIndex, 
      StackedCommand &stackedCmd,
      QString const&topCmdName,
      QString const&LowCmdName,
      QString const&error
      );

    /// Cleans the stacks if errors occur when
    /// redoing a command and throws an exception.
    void cleanupUnfinishedRedoLowCommandsAndThrow(
      int lowCmdIndex, 
      StackedCommand &stackedCmd,
      QString const&topCmdName,
      QString const&LowCmdName,
      QString const&error
      );
    
    /// CommandManager singleton, set from Constructor.
    static CommandManager *s_cmdManager;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
    /// Debug mode (NoDebug, Debug or VerboseDebug)
    int m_debugMode;
};
 
} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_MANAGER__
