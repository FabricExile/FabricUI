//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "CommandRegistry.h"
#include "KLCommandManager.h"
#include "KLCommandHelpers.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;
 
KLCommandManager::KLCommandManager() 
  : RTValCommandManager()
{
  // Construct the KL manager.
  KLCommandHelpers::getKLCommandManager();
}

KLCommandManager::~KLCommandManager() 
{
}
 
void KLCommandManager::clear() 
{
  FABRIC_CATCH_BEGIN();

  KLCommandHelpers::getKLCommandManager().callMethod(
    "", 
    "clear", 
    0, 0);

  CommandManager::clear();

  FABRIC_CATCH_END("KLCommandManager::clear");
}

QString KLCommandManager::getContent(
  bool withArgs)
{
  FABRIC_CATCH_BEGIN();

  QString res = CommandManager::getContent(
    withArgs);

  res += QString("\n") + KLCommandHelpers::getKLCommandManager().callMethod(
    "String", 
    "getContent", 
    0, 0).getStringCString();  
  
  return res;

  FABRIC_CATCH_END("KLCommandManager::getContent");

  return "";
}
 
int KLCommandManager::getNewCanMergeID()
{
  m_canMergeIDCounter++;
  
  FABRIC_CATCH_BEGIN();

  int canMergeIDCounter = KLCommandHelpers::getKLCommandManager().callMethod(
    "UInt32", 
    "getCanMergeIDCounter", 
    0, 0).getUInt32();

  if(canMergeIDCounter < m_canMergeIDCounter)
  {
    RTVal canMergeIDCounterVal = RTVal::ConstructUInt32(
      KLCommandHelpers::getKLCommandManager().getContext(),
      m_canMergeIDCounter);

    KLCommandHelpers::getKLCommandManager().callMethod(
      "", 
      "setCanMergeIDCounter", 
      1, &canMergeIDCounterVal);
  }

  else if(m_canMergeIDCounter > canMergeIDCounter)
    m_canMergeIDCounter = canMergeIDCounter;

  FABRIC_CATCH_END("KLCommandManager::getNewCanMergeID");

  return m_canMergeIDCounter;
}

void KLCommandManager::clearRedoStack() 
{
  FABRIC_CATCH_BEGIN();

  KLCommandHelpers::getKLCommandManager().callMethod(
    "", 
    "clearRedoStack", 
    0, 0);
    
  CommandManager::clearRedoStack();

  FABRIC_CATCH_END("KLCommandManager::clearRedoStack");
}

void KLCommandManager::synchronizeKL() 
{
  FABRIC_CATCH_BEGIN();

  // Gets the KL command from the KL manager. 
  RTVal klAppCmdStack = KLCommandHelpers::getKLCommandManager().callMethod(
    "Command[]", 
    "getAppStack", 
    0, 0);
 
  for(unsigned i=0; i<klAppCmdStack.getArraySize(); ++i)
  {
    // Gets the KL command from the KL manager. 
    RTVal klCmd = klAppCmdStack.getArrayElementRef(i);

    // Check if it's an AppCommand.
    // Construct C++ commands from KL
    RTVal appCmd = RTVal::Construct(
      klCmd.getContext(),
      "AppCommand", 
      1, 
      &klCmd);

    if(appCmd.isValid() && !appCmd.isNullObject())
      createAppCommand(appCmd);

    // KL commands have actually been 
    // created, create the C++ wrappers.
    else
      doKLCommand(klCmd);
  } 

  KLCommandHelpers::getKLCommandManager().callMethod(
    "", 
    "clearAppStack", 
    0, 0);

  FABRIC_CATCH_END("KLCommandManager::synchronizeKL");
}

void KLCommandManager::createAppCommand(
  RTVal appCmd)
{
  FABRIC_CATCH_BEGIN();

  QString cmdName = appCmd.callMethod("String", "getName", 0, 0).getStringCString();
  int canMergeID = appCmd.callMethod("SInt32", "getCanMergeID", 0, 0).getSInt32();

  // Creates the command without executing it since it can  
  // be BaseScriptableCommand or a BaseRTValScriptableCommand
  BaseCommand *cmd = CommandRegistry::getCommandRegistry()->createCommand(
    cmdName);
    
  bool logBlocked = appCmd.callMethod( "Boolean", "isLogBlocked", 0, 0 ).getBoolean();
  if( logBlocked )
    cmd->blockLog();

  RTVal keys = appCmd.callMethod(
    "String[]", 
    "getArgKeys", 
    0, 0);

  // BaseRTValScriptableCommand
  BaseRTValScriptableCommand *rtValScriptCmd = qobject_cast<BaseRTValScriptableCommand *>(cmd);
  if(rtValScriptCmd)
  {
    QMap<QString, RTVal> rtValArgs;

    for(unsigned i=0; i<keys.getArraySize(); ++i)
    {
      RTVal argName = keys.getArrayElement(i);
      rtValArgs[argName.getStringCString()] = appCmd.callMethod(
        "RTVal", 
        "getArg", 
        1, 
        &argName);
    }

    if(rtValArgs.size() > 0) 
      checkRTValCommandArgs(cmd, rtValArgs);
  }

  // BaseScriptableCommand
  else
  {
    QMap<QString, QString> args;

    for(unsigned i=0; i<keys.getArraySize(); ++i)
    {
      RTVal argName = keys.getArrayElement(i);
      RTVal pathValueArg = RTValUtil::toRTVal(appCmd.callMethod(
        "RTVal", 
        "getArg", 
        1, 
        &argName));

      args[argName.getStringCString()] = 
        RTValUtil::toRTVal(pathValueArg.maybeGetMember("value")).getStringCString();
    }

    if(args.size() > 0) 
      checkCommandArgs(cmd, args);
  }

  doCommand(cmd, canMergeID);

  FABRIC_CATCH_END("KLCommandManager::createAppCommand");
}

void KLCommandManager::doKLCommand(
  RTVal klCmd)
{
  FABRIC_CATCH_BEGIN();

  RTVal baseCmd = RTVal::Construct(
    klCmd.getContext(),
    "BaseCommand", 
    1, 
    &klCmd);

  RTVal klScriptCmd = RTVal::Construct(
    klCmd.getContext(),
    "BaseScriptableCommand", 
    1, 
    &klCmd);

  BaseCommand *cmd = 0;
  if(klScriptCmd.isValid() && !klScriptCmd.isNullObject())
    cmd = new KLScriptableCommand(klScriptCmd);
  else
    cmd = new KLCommand(baseCmd);

  doCommand(
    cmd,
    baseCmd.callMethod("SInt32", "getCanMergeID", 0, 0).getSInt32()
    );

  FABRIC_CATCH_END("KLCommandManager::doKLCommand");
}
