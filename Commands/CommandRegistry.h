//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_REGISTRY__
#define __UI_COMMAND_REGISTRY__

#include <QPair>
#include "BaseCommand.h"
#include <FabricUI/Util/Factory.h> 

namespace FabricUI {
namespace Commands {

class CommandRegistry : public Util::BaseFactoryRegistry
{
  /**
    CommandRegistry registers C++ command-factories and creates commands from them. 
    When a command is registered, the signal `registrationDone` is emitted. When 
    specialized in Python , the same registry is shared between C++ and Python,
    so commands implemented in Python can be created from C++ and vice versa.
    
    The registry sets it-self as a singleton when it's constructed:
    - Create the singleton: new CommandRegistry();
    - Get the singleton: CommandRegistry *cmdRegistry = CommandRegistry::getCommandRegistry();
  */  
  Q_OBJECT

  public:
    // Command type
    QString COMMAND_CPP; 
 
    CommandRegistry();

    virtual ~CommandRegistry();

    /// Gets the registry singleton.
    /// Thows an error if the registry has not been created.
    static CommandRegistry* getCommandRegistry();
    
    /// Checks if the registry has been created.
    static bool isInitalized();

    /// Creates a command named 'cmdName'.
    /// Throws an error if the command isn't registered.
    virtual BaseCommand* createCommand(
      QString const&cmdName
      );

    /// Checks if a command (C++/Python) 
    /// has been registered under 'cmdName'.
    bool isCommandRegistered(
      QString const&cmdName
      );

    /// Gets the command and implementation type (C++/Python).
    /// Returns an empty list if the command is not registred.
    QPair<QString, QString> getCommandSpecs(
      QString const&cmdName
      );

    /// Gets the name of all the registered commands.
    QList<QString> getCommandNames();

    /// Gets all the registred commands (C++/Python) 
    /// and the specs as a string, used for debugging.
    QString getContent();

    /// \internal
    /// Clears the registered content
    virtual void clear();
  
  signals:
    /// Emitted when a command has been registered.
    /// \param cmdName The name of the command
    /// \param cmdType Object type
    /// \param implType Implementation : C++ or Python
    void registrationDone(
      QString const&cmdName,
      QString const&cmdType,
      QString const&implType
      );

  public slots:
    /// Implementation of Util::FactoryRegistry.
    virtual void registerFactory(
      QString const&name, 
      Util::Factory *factory
      );

    /// Implementation of Util::FactoryRegistry.
    /// Does nothing.
    virtual void unregisterFactory(
      QString const&name 
      );
    
  protected:
    /// Informs that a command has been registered. 
    /// \param cmdName The name of the command
    /// \param cmdType Object type
    /// \param implType Implementation : C++ or Python
    virtual void commandRegistered(
      QString const&cmdName,
      QString const&cmdType,
      QString const&implType
      );

  private:
    /// Dictionaries of registered commands (Python/C++): 
    /// {cmdName, {type, implementation type}}
    QMap< QString, QPair<QString, QString> > m_cmdSpecs;
    /// CommandRegistry singleton, set from Constructor.
    static CommandRegistry *s_cmdRegistry;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
};

template<typename T> 
class CommandFactory : public Util::TemplateFactory<T>
{
  /**
    CommandFactory is used to register commands in the CommandRegistry.
    - Register a command: CommandFactory<cmdType>::Register(cmdName, userData);
  */
  public:
    CommandFactory(void *userData) 
      : Util::TemplateFactory<T>(userData) 
    {
    }

    /// Registers the command <T> under "cmdName".
    static void Register(
      QString const&cmdName,
      void *userData=0) 
    {
      Util::TemplateFactory<T>::Register(
        CommandRegistry::getCommandRegistry(),
        cmdName,
        userData);
    }
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_REGISTRY__
