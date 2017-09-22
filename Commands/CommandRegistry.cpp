//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandRegistry.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace Application;

bool CommandRegistry::s_instanceFlag = false;
CommandRegistry* CommandRegistry::s_cmdRegistry = 0;

CommandRegistry::CommandRegistry() 
  : Util::BaseFactoryRegistry()
{
  COMMAND_CPP = "CPP";
}

CommandRegistry::~CommandRegistry() 
{
  s_instanceFlag = false;
  if( s_cmdRegistry == this )
    s_cmdRegistry = 0;
}

void CommandRegistry::setCommandRegistrySingleton(
  CommandRegistry* registry)
{
  if(registry != 0)
  {
    s_instanceFlag = true;
    s_cmdRegistry = registry;
  }
}

CommandRegistry* CommandRegistry::getCommandRegistry()
{
  if(!s_instanceFlag)
    FabricException::Throw(
      "CommandRegistry::getCommandRegistry",
      "The registry is null");

  return s_cmdRegistry;
}

bool CommandRegistry::isInitalized()
{
  return s_instanceFlag;
}

void CommandRegistry::registerFactory(
  QString const&name, 
  Factory *factory) 
{
  if(!isCommandRegistered(name))
  {
    Util::BaseFactoryRegistry::registerFactory(
      name, 
      factory);
    
    commandRegistered(
      name,
      // Get the name of the cmd class.
      factory->getType(),
      COMMAND_CPP);
  }
}

void CommandRegistry::unregisterFactory(
  QString const&name)
{
  // Does nothing.
}

bool CommandRegistry::isCommandRegistered(
  QString const&cmdName) 
{
  return m_cmdSpecs.count(cmdName) > 0;
}

void CommandRegistry::clear() {
  m_cmdSpecs.clear();
}


QPair<QString, QString> CommandRegistry::getCommandSpecs(
  QString const&cmdName) 
{
  if(!isCommandRegistered(cmdName))
    FabricException::Throw( 
      "CommandRegistry::getCommandSpecs",
      "cannot create command '" + cmdName + "', it's not registered");

  return m_cmdSpecs[cmdName];
}

QList<QString> CommandRegistry::getCommandNames() 
{
  return m_cmdSpecs.keys();
}

BaseCommand* CommandRegistry::createCommand(
  QString const&cmdName) 
{  
  if(!isCommandRegistered(cmdName))
    FabricException::Throw( 
      "CommandRegistry::createCommand",
      "cannot create command '" + cmdName + "', it's not registered");

  FABRIC_CATCH_BEGIN();

  QPair<QString, QString> spec = getCommandSpecs(cmdName);
  
  if(spec.second == COMMAND_CPP) 
  {
    Factory *factory = Util::BaseFactoryRegistry::getFactory(
      cmdName);

    BaseCommand* cmd = (BaseCommand*)factory->create(); 
    if(cmd == 0)
      FabricException::Throw(
        "CommandRegistry::createCommand",
        "resulting command is null");
  
    cmd->registrationCallback(
      cmdName,
      factory->getUserData());

    return cmd;
  }

  FABRIC_CATCH_END("CommandRegistry::createCommand");

  return 0;
}

QString CommandRegistry::getContent()
{
  QString res = "--> CommandRegistry:\n";
  QMapIterator< QString, QPair<QString, QString> > specsIt(m_cmdSpecs);

  while(specsIt.hasNext()) 
  {
    specsIt.next();
    QString name = specsIt.key();
    QPair<QString, QString> spec = specsIt.value();
    res +=  "["+ name + "] type:" + spec.first + ", implType:" + spec.second + "\n";
  }

  return res;
}

void CommandRegistry::commandRegistered(
  QString const&cmdName,
  QString const&cmdType,
  QString const&implType) 
{  
  // sets the command specs
  QPair<QString, QString> spec;
  spec.first = cmdType;
  spec.second = implType;
  m_cmdSpecs[cmdName] = spec;
    
  // inform a command has been registered.
  emit registrationDone(
    cmdName,
    cmdType,
    implType);
}
