//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "KLCommandHelpers.h"
#include "KLCommandRegistry.h"
#include "KLScriptableCommand.h"
#include "SetPathValueCommand.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

KLCommandRegistry::KLCommandRegistry() 
  : CommandRegistry()
{
  COMMAND_KL = "KL";
  
  // Construct the KL registry.
  KLCommandHelpers::getKLCommandRegistry();
  
  CommandFactory<SetPathValueCommand>::Register(
    "setPathValue");
}

KLCommandRegistry::~KLCommandRegistry() 
{
}

BaseCommand* KLCommandRegistry::createCommand(
  QString const&cmdName) 
{  
  if(!isCommandRegistered(cmdName))
    FabricException::Throw( 
      "KLCommandRegistry::createCommand",
      "Cannot create command '" + cmdName + "', it's not registered"
      );

  QPair<QString, QString> spec = getCommandSpecs(
    cmdName);

  return spec.second == COMMAND_KL
    ? createKLCommand(cmdName)
    : CommandRegistry::createCommand(cmdName);
}

void KLCommandRegistry::synchronizeKL() 
{
  FABRIC_CATCH_BEGIN();

  // KL -> C++ synchronization
  RTVal klCmdNameList = KLCommandHelpers::getKLCommandRegistry().callMethod(
    "String[]", 
    "getRegisteredCommandList", 
    0, 0);
  
  for(unsigned int i=0; i<klCmdNameList.getArraySize(); ++i)
  {
    QString cmdName(klCmdNameList.getArrayElement(i).getStringCString());
    if(!isCommandRegistered(cmdName))
      registerKLCommand(cmdName);
  } 

  // C++ -> KL synchronization
  QString cmdName;
  foreach(cmdName, getCommandNames())
  {
    RTVal args[2] = {
      RTVal::ConstructString(
        KLCommandHelpers::getKLCommandRegistry().getContext(), 
        cmdName.toUtf8().constData()),

      RTVal::Construct(
        KLCommandHelpers::getKLCommandRegistry().getContext(), 
        "Type", 
        0, 0),
    };

    bool isCmdRegistered = KLCommandHelpers::getKLCommandRegistry().callMethod(
      "Boolean", 
      "isCommandRegistered", 
      2, 
      args).getBoolean();

  
    if(!isCmdRegistered)
    {
      QPair<QString, QString> spec = getCommandSpecs(cmdName);
      if(spec.second != COMMAND_KL) 
        KLCommandHelpers::getKLCommandRegistry().callMethod(
          "",
          "registerAppCommand",
          1,
          &args[0]);

      else
      {
        args[1] = RTVal::Create(
          KLCommandHelpers::getKLCommandRegistry().getContext(), 
            spec.first.toUtf8().constData(), 
            0, 
            0).callMethod("Type", "type", 0, 0);
 
        KLCommandHelpers::getKLCommandRegistry().callMethod(
          "",
          "registerCommand",
          2,
          args);
      }
    }
  }
  
  FABRIC_CATCH_END("KLCommandRegistry::synchronizeKL");
}
 
void KLCommandRegistry::registerKLCommand(
  QString const&cmdName) 
{
  FABRIC_CATCH_BEGIN();

  // Not sure the command is registered in KL. 
  RTVal args[2] = {
    RTVal::ConstructString(
      KLCommandHelpers::getKLCommandRegistry().getContext(), 
      cmdName.toUtf8().constData()),

    RTVal::Construct(
      KLCommandHelpers::getKLCommandRegistry().getContext(), 
      "Type", 
      0, 0),
  };

  bool isCmdRegistered = KLCommandHelpers::getKLCommandRegistry().callMethod(
    "Boolean", 
    "isCommandRegistered", 
    2, 
    args).getBoolean();

  // The command is already registered in KL.
  // Don't call KLCommandRegistry::commandRegistered.
  if(isCmdRegistered)
    CommandRegistry::commandRegistered(
      cmdName,
      RTVal::Construct(
        KLCommandHelpers::getKLCommandRegistry().getContext(), 
        "String", 
        1, 
        &args[1]).getStringCString(),
      COMMAND_KL);

  FABRIC_CATCH_END("KLCommandRegistry::registerKLCommand");
}
 
BaseCommand* KLCommandRegistry::createKLCommand(
  QString const&cmdName)
{  
  FABRIC_CATCH_BEGIN();

  RTVal args[2] = {
    RTVal::ConstructString(
      KLCommandHelpers::getKLCommandRegistry().getContext(), 
      cmdName.toUtf8().constData()),

    RTVal::ConstructString(
      KLCommandHelpers::getKLCommandRegistry().getContext(), 
      "")
  };

  // Creates the KL command from the KL registery. 
  // Check if it's a scriptable command
  RTVal klCmd = KLCommandHelpers::getKLCommandRegistry().callMethod(
    "BaseCommand", 
    "createCommand", 
    2, 
    args);

  RTVal sriptKLCmd = RTVal::Construct(
    klCmd.getContext(),
    "BaseScriptableCommand", 
    1, 
    &klCmd);
 
  if(sriptKLCmd.isValid() && !sriptKLCmd.isNullObject())
     return new KLScriptableCommand(sriptKLCmd);
  else
    return new KLCommand(klCmd);
 
  FABRIC_CATCH_END("KLCommandRegistry::createKLCommand");

  return 0;
}

void KLCommandRegistry::commandRegistered(
  QString const&cmdName,
  QString const&cmdType,
  QString const&implType) 
{
  FABRIC_CATCH_BEGIN();
 
  RTVal nameVal = RTVal::ConstructString(
    KLCommandHelpers::getKLCommandRegistry().getContext(),
    cmdName.toUtf8().constData());

  KLCommandHelpers::getKLCommandRegistry().callMethod(
    "",
    "registerAppCommand",
    1,
    &nameVal);
 
  CommandRegistry::commandRegistered(
    cmdName,
    cmdType,
    implType);

  FABRIC_CATCH_END("KLCommandRegistry::commandRegistered");
}
