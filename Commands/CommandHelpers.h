//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef _UI_COMMAND_HELPERS__
#define _UI_COMMAND_HELPERS__

#include <QMap>
#include <QObject>
#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Commands {

class CommandArgFlags
{
  /**
    CommandArgFlags lists the command arguments flags (see BaseScriptableCommand::declareArg)
  */
  public:
    static int NO_FLAG_ARG;
    static int OPTIONAL_ARG;
    static int DONT_LOG_ARG;
    static int IN_ARG;
    static int OUT_ARG;
    static int IO_ARG;
    //static int CREATE_IF_NULL;// Reserved, not implemented yet in C++
    //static int CLONE_FOR_UNDO;// Reserved, not implemented yet in C++
};

class BaseCommand;

class CommandHelpers 
{  
  /**
    CommandHelpers implements statics methods that are mainly used in Python
    to check and cast the commands arguments. We have to go back to C++ because
    if a C++ command hasn't been wrapped, then Python is not able to resolve 
    the object inheritance. Thus, a C++ BaseScriptableCommand will be seen
    as a ScriptableCommand by python.  
   */
  public:
    /// Creates the command's description from a subsets of arguments.
    /// \param cmd A BaseScriptableCommand
    /// \param argsDesc Map of [arg, arg desc]
    static QString createHistoryDescFromArgs(
      BaseCommand *cmd,
      QMap<QString, QString> const&argsDesc
      );

    /// Creates the `key` argument description.
    /// \param cmd A BaseScriptableCommand
    /// \param key The arg key (name)
    static QString getArgsTypeSpecs(
      BaseCommand *cmd,
      QString const&key
      );

    /// Create the command's help from a subsets of arguments.
    /// \param cmd A BaseScriptableCommand
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString createHelpFromArgs(
      BaseCommand *cmd,
      QString const&commandHelp,
      QMap<QString, QString> const&argsHelp
      );

    /// Creates the command's helps from a subsets of arguments.
    /// \param cmd A BaseRTValScriptableCommand
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString createHelpFromRTValArgs(
      BaseCommand *cmd,
      QString const&commandHelp,
      QMap<QString, QString> const&argsHelp
      );
    
    /// Checks that the command is a BaseScriptableCommand
    /// \param cmd A BaseCommand
    static bool isScriptableCommand(
      BaseCommand *cmd
      );

    /// Checks that the command is a BaseRTValScriptableCommand
    /// \param cmd A BaseCommand
    static bool isRTValScriptableCommand(
      BaseCommand *cmd
      );

    /// Checks that the command arg 'key' has the flags 'flags'
    /// \param cmd A BaseScriptableCommand
    /// \param key Argument name
    /// \param flags Flags to test.
    static bool hasCommandFlag(
      BaseCommand *cmd,
      QString const&key,
      int flags
      );

    /// Checks that the command arg 'key' has been set
    /// \param cmd A BaseScriptableCommand
    /// \param key Argument name
    static bool isCommandArgSet(
      BaseCommand *cmd,
      QString const&key
      );

    /// Checks that the command has an arg named 'key'
    /// \param cmd A BaseScriptableCommand
    /// \param key Argument name
    static bool hasCommandArg(
      BaseCommand *cmd,
      QString const&key
      );

    /// Checks that the command arg 'key' has the flags 'flags'
    /// \param cmd A BaseScriptableCommand
    static QList<QString> getCommandArgKeys(
      BaseCommand *cmd
      );

    /// Gets the command argument keys list. 
    /// \param cmd A BaseScriptableCommand
    /// \param key Argument name
    static QString getCommandArg(
      BaseCommand *cmd,
      QString const&key
      );

    /// Gets the command argument as an RTVal (PathValue). 
    /// \param cmd A BaseRTValScriptableCommand
    /// \param key Argument name
    static FabricCore::RTVal getRTValCommandArg(
      BaseCommand *cmd,
      QString const&key
      );

    /// Gets the command argument type (PathValue.value.type). 
    /// \param cmd A BaseRTValScriptableCommand
    /// \param key Argument name
    static QString getRTValCommandArgType(
      BaseCommand *cmd,
      QString const&key
      );

    /// Casts JSON command arguments so it is printed
    /// nicely in the script-editor.
    static QString encodeJSON(
      QString const&str
      );

    /// Casts JSON command arguments array so it is 
    /// printed nicely in the script-editor.
    static QString encodeJSONs(
      QString const&str
      );

    /// Adds '<, >' characters at the begining and end
    /// of the path to specify it's a PathValue path
    /// in the scripting.
    static QString castFromPathValuePath(
      QString const&path
      );

    /// Remove '<, >' characters at the begining and end
    /// of the path when it's been set from scripting.
    static QString castToPathValuePath(
      QString const&path
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // _UI_COMMAND_HELPERS__
