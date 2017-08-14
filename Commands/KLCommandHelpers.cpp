//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandManager.h"
#include "KLCommandHelpers.h"
#include "KLCommandManager.h"
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

RTVal KLCommandHelpers::getKLCommandManager() 
{
  FABRIC_CATCH_BEGIN();

  RTVal klCmdManager = RTVal::Create(
    FabricApplicationStates::GetAppStates()->getContext(), 
    "AppCommandManager", 
    0, 0);

  return klCmdManager.callMethod(
    "AppCommandManager", 
    "getCommandManager", 
    0, 0);

  FABRIC_CATCH_END("KLCommandHelpers::getKLCommandManager");

  return RTVal();
}

RTVal KLCommandHelpers::getKLCommandRegistry() 
{
  FABRIC_CATCH_BEGIN();

  RTVal klCmdRegistry = RTVal::Construct(
    FabricApplicationStates::GetAppStates()->getContext(), 
    "AppCommandRegistry", 
    0, 0);

  return klCmdRegistry.callMethod(
    "AppCommandRegistry", 
    "getCommandRegistry", 
    0, 0);

  FABRIC_CATCH_END("KLCommandHelpers::getKLCommandRegistry");

  return RTVal();
}

QString KLCommandHelpers::getKLCommandName(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "String", 
    "getName", 
    0, 
    0).getStringCString();
  
  FABRIC_CATCH_END("KLCommandHelpers::getKLCommandName");

  return "";
}

bool KLCommandHelpers::canKLCommandUndo(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "Boolean", 
    "canUndo", 
    0, 
    0).getBoolean();

  FABRIC_CATCH_END("KLCommandHelpers::canKLCommandUndo");

  return false;
}

bool KLCommandHelpers::canKLCommandLog(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();
  return klCmd.callMethod(
    "Boolean", 
    "canLog", 
    0, 
    0).getBoolean();

  FABRIC_CATCH_END("KLCommandHelpers::canKLCommandLog");
  
  return false;
}

bool KLCommandHelpers::doKLCommand( 
  RTVal klCmd) 
{ 
  FABRIC_CATCH_BEGIN();

  RTVal args[2] = { 
    klCmd, 
    // error
    RTVal::ConstructString(klCmd.getContext(), "") 
  };

  getKLCommandManager().callMethod(
    "", 
    "_doCommand", 
    2, 
    args);
  
  QString strError = args[1].getStringCString();
  if(!strError.isEmpty())
    FabricException::Throw(
      "KLCommandHelpers::doKLCommand",
      strError);

  return true;
  
  FABRIC_CATCH_END("KLCommandHelpers::doKLCommand");

  return false;
}

bool KLCommandHelpers::undoKLCommand() 
{ 
  FABRIC_CATCH_BEGIN();
 
  RTVal valError = RTVal::ConstructString(
    FabricApplicationStates::GetAppStates()->getContext(), 
    "");

  bool res = getKLCommandManager().callMethod(
    "Boolean", 
    "undoCommand", 
    1, 
    &valError).getBoolean();
  
  QString strError = valError.getStringCString();
  if(!res || !strError.isEmpty())
    FabricException::Throw(
      "KLCommandHelpers::undoKLCommand",
      strError);

  return true;
  
  FABRIC_CATCH_END("KLCommandHelpers::undoKLCommand");

  return false;
}

bool KLCommandHelpers::redoKLCommand() 
{  
  FABRIC_CATCH_BEGIN();

  RTVal valError = RTVal::ConstructString(
    FabricApplicationStates::GetAppStates()->getContext(), 
    "");

  bool res = getKLCommandManager().callMethod(
    "Boolean", 
    "redoCommand", 
    1, 
    &valError).getBoolean();

  QString strError = valError.getStringCString();
  if(!res || !strError.isEmpty())
    FabricException::Throw(
      "KLCommandHelpers::redoKLCommand",
      strError);

  return true;

  FABRIC_CATCH_END("KLCommandHelpers::getKLCommandHelp");

  return false;
}

QString KLCommandHelpers::getKLCommandHelp(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "String", 
    "getHelp", 
    0, 
    0).getStringCString();
  
  FABRIC_CATCH_END("KLCommandHelpers::getKLCommandHelp");

  return "";
}

QString KLCommandHelpers::getKLCommandHistoryDesc(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "String", 
    "getHistoryDesc", 
    0, 
    0).getStringCString();

  FABRIC_CATCH_END("KLCommandHelpers::getKLCommandHistoryDesc");

  return "";
}
 
void KLCommandHelpers::setKLCommandCanMergeID(
  FabricCore::RTVal klCmd,
  int canMergeID)
{
  FABRIC_CATCH_BEGIN();

  RTVal canMergeIDVal = RTVal::ConstructSInt32(
    klCmd.getContext(),
    canMergeID);

  klCmd.callMethod(
    "", 
    "setCanMergeID", 
    1, 
    &canMergeIDVal);

  FABRIC_CATCH_END("KLCommandHelpers::setKLCommandCanMergeID");
}

int KLCommandHelpers::getKLCommandCanMergeID(
  FabricCore::RTVal klCmd)
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "SInt32", 
    "getCanMergeID", 
    0, 
    0).getSInt32();
  
  FABRIC_CATCH_END("KLCommandHelpers::getKLCommandCanMergeID");

  return -1;
}

bool KLCommandHelpers::canMergeKLCommand(
  FabricCore::RTVal klCmd,
  FabricCore::RTVal prevKlCmd,
  bool &undoPrevAndMergeFirst)
{
  FABRIC_CATCH_BEGIN();

  RTVal args[2];
  args[0] = prevKlCmd;
  args[1] = RTVal::ConstructBoolean(
    klCmd.getContext(),
    undoPrevAndMergeFirst );

  bool result = klCmd.callMethod(
    "Boolean", 
    "canMerge", 
    2, 
    args
    ).getBoolean();

  undoPrevAndMergeFirst = args[1].getBoolean();

  return result;

  FABRIC_CATCH_END("KLCommandHelpers::canMergeKLCommand");

  return false;
}

void KLCommandHelpers::mergeKLCommand(
  FabricCore::RTVal klCmd,
  FabricCore::RTVal prevKlCmd)
{
  FABRIC_CATCH_BEGIN();

  klCmd.callMethod(
    "", 
    "merge", 
    1, 
    &prevKlCmd);

  FABRIC_CATCH_END("KLCommandHelpers::mergeKLCommand");
}
