//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SCRIPTABLE_COMMAND__
#define __UI_BASE_SCRIPTABLE_COMMAND__

#include <QMap>
#include "BaseCommand.h"
#include "CommandHelpers.h"

namespace FabricUI {
namespace Commands {

class BaseScriptableCommand : public BaseCommand
{
  /**
    BaseScriptableCommand defines methods for commands with arguments. 
    Any scriptable command must implements this interface class.

    Because the arguments must be scriptable (to work with any app/DCC),
    they must rely on string (QString here). QString provides helpers to   
    cast string to others types, see http://doc.qt.io/qt-4.8/qstring.html.
    Another possibility is to use JSON format, see BaseRTValScriptableCommand.
    Arguments are set from within the CommandManager, method 'createCommand'.

    Arguments can be declared with various options:
      - as optional: The arg not need to be set
      - as loggable: If true, the argument will be logged in the script
      - as in/out/io: The value is an input, and output, or both/modified (io)
      - with a default argument

    C++ interfaces cannot be wrapped in python by shiboken. New commands
    must specialize this class to be accessible from python.
  */
    
  Q_OBJECT

  public:
    BaseScriptableCommand();
    
    virtual ~BaseScriptableCommand();

    /**
      Declares an argument, at construction time.
      Flags can be a combination of these: 

      - CommandArgFlag_IN, CommandArgFlag_OUT or CommandArgFlag_IO : one of these should be set to
        specify if an argument is an input, and output, or an input + output, respectively.

      - CommandArgFlag_OPTIONAL : if set, then that argument doesn't have to be defined, in which
        case getArgValue() will return a null RTVal( value.isNull() == true )

      - CommandArgFlag_DONT_LOG: if set, then the command will not be logged nor part
        of the undo / redo stack.This can be useful when only the last operation should be logged
        ( in which all but the last command have this flag )
    */

    virtual void declareArg(
      QString const&key, 
      int flag, 
      QString const&defaultValue
      );

    /// Checks if the command has the arg `key`.
    virtual bool hasArg(
      QString const&key 
      );

    /// Checks if the arg `key` has the input flag(s) (see BaseScriptableCommand::declareArg)
    virtual bool hasArgFlag(
      QString const&key,
      int flag
      );

    /// Gets the arguments keys list.
    virtual QList<QString> getArgKeys();

    /// Checks if an arg has been set.
    /// To get safely optional argument.
    virtual bool isArgSet(
      QString const&key
      );

    /// Gets the `key` argument value.
    virtual QString getArg( 
      QString const&key 
      );
 
    /// Sets the `key` argument value,
    /// called from the manager.
    virtual void setArg(
      QString const&key, 
      QString const&value
      );

    /// Checks the args are correctly set before  
    /// doing the command, called from the manager.
    virtual void validateSetArgs();

    /// Gets a decription of the args, for debugging.
    virtual QString getArgsDescription();

  private:    
    /// Defines the arguments specs:
    /// default value, flags (optional, loggable)
    struct ScriptableCommandArgSpec 
    {
      int flags;
      QString defaultValue;
    };

    /// List of arguments {argName, argValue}
    QMap<QString, QString> m_args;
    /// List of arguments specs {argName, spec}
    QMap<QString, ScriptableCommandArgSpec> m_argSpecs;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_SCRIPTABLE_COMMAND__
