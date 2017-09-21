//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandManager.h"
#include "KLCommandHelpers.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

KLScriptableCommand::KLScriptableCommand(
  RTVal klCmd)
  : BaseRTValScriptableCommand()
  , m_klCmd(klCmd)
{
}

KLScriptableCommand::~KLScriptableCommand() 
{
}

// BaseCommand
QString KLScriptableCommand::getName() 
{
  return KLCommandHelpers::getKLCommandName(
    m_klCmd);
}

bool KLScriptableCommand::canUndo() 
{
  return KLCommandHelpers::canKLCommandUndo(
    m_klCmd);
}

bool KLScriptableCommand::canLog() 
{
  return KLCommandHelpers::canKLCommandLog(
    m_klCmd);
}

bool KLScriptableCommand::doIt() 
{ 
  FABRIC_CATCH_BEGIN();

  RTVal cmd = RTVal::Construct(
    m_klCmd.getContext(),
    "Command",
    1,
    &m_klCmd);
  
  return KLCommandHelpers::doKLCommand(cmd);

  FABRIC_CATCH_END("KLScriptableCommand::doIt");

  return false;
}

bool KLScriptableCommand::undoIt() 
{ 
  return KLCommandHelpers::undoKLCommand();
}

bool KLScriptableCommand::redoIt() 
{  
  return KLCommandHelpers::redoKLCommand();
}

QString KLScriptableCommand::getHelp() 
{
  return KLCommandHelpers::getKLCommandHelp(
    m_klCmd);
}

QString KLScriptableCommand::getHistoryDesc() 
{
  return KLCommandHelpers::getKLCommandHistoryDesc(
    m_klCmd);
}

void KLScriptableCommand::setCanMergeID(
  int canMergeID)
{
  KLCommandHelpers::setKLCommandCanMergeID(
    m_klCmd, 
    canMergeID);
}

int KLScriptableCommand::getCanMergeID()
{
  return KLCommandHelpers::getKLCommandCanMergeID(
    m_klCmd);
}

bool KLScriptableCommand::canMerge(
  BaseCommand *prevCmd,
  bool &undoPrevAndMergeFirst)
{
  KLScriptableCommand* scriptCmd = qobject_cast<KLScriptableCommand*>(prevCmd);
  if(scriptCmd == 0)
    return false;
  
  if(!BaseCommand::canMerge(prevCmd, undoPrevAndMergeFirst))
    return false;

  return KLCommandHelpers::canMergeKLCommand(
    m_klCmd, 
    scriptCmd->m_klCmd,
    undoPrevAndMergeFirst);
}

void KLScriptableCommand::merge(
  BaseCommand *prevCmd) 
{
  FABRIC_CATCH_BEGIN();

  KLScriptableCommand* scriptCmd = qobject_cast<KLScriptableCommand*>(prevCmd);

  RTVal cmd = RTVal::Construct(
    m_klCmd.getContext(),
    "Command",
    1,
    &scriptCmd->m_klCmd);

  KLCommandHelpers::mergeKLCommand(m_klCmd, cmd);

  FABRIC_CATCH_END("KLScriptableCommand::merge");
}

// BaseScriptableCommand
void KLScriptableCommand::declareArg( 
  QString const&key, 
  int flag, 
  QString const&defaultValue)
{
  // Do nothing.
}

bool KLScriptableCommand::hasArg(
  QString const&key) 
{
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return m_klCmd.callMethod(
    "Boolean",
    "hasArg",
    1,
    &keyVal
    ).getBoolean();

  FABRIC_CATCH_END("KLScriptableCommand::hasArg");

  return false;
}

bool KLScriptableCommand::hasArgFlag(
  QString const&key,
  int flag) 
{
  FABRIC_CATCH_BEGIN();

  RTVal args[2] = 
  {
    RTVal::ConstructString(
      m_klCmd.getContext(), 
      key.toUtf8().constData()),

    RTVal::ConstructUInt32(
      m_klCmd.getContext(), 
      flag)
  };

  return m_klCmd.callMethod(
    "Boolean",
    "hasArgFlag",
    2,
    args
    ).getBoolean();
  
  FABRIC_CATCH_END("KLScriptableCommand::hasArgFlag");

  return false;
}

QList<QString> KLScriptableCommand::getArgKeys()
{
  QList<QString> keys;

  FABRIC_CATCH_BEGIN();

  RTVal rtvalKeys = m_klCmd.callMethod(
    "String[]", 
    "getArgKeys", 
    0, 0);

  for (unsigned i = 0; i < rtvalKeys.getArraySize(); i++) 
    keys.append(rtvalKeys.getArrayElementRef(
      i).getStringCString()); 

  FABRIC_CATCH_END("KLScriptableCommand::getArgKeys");
  
  return keys;
}

bool KLScriptableCommand::isArgSet(
  QString const&key)
{
  return true;
}

QString KLScriptableCommand::getArg(
  QString const&key) 
{
  FABRIC_CATCH_BEGIN();

  return RTValUtil::toJSON(getRTValArg(key));

  FABRIC_CATCH_END("KLScriptableCommand::getArg");

  return "";
}

void KLScriptableCommand::setArg(
  QString const&key, 
  QString const&json) 
{
  checkHasArg("KLScriptableCommand::setArg", key);

  FABRIC_CATCH_BEGIN();

  QString pathValuePath = CommandHelpers::castFromPathValuePath(json);

  if(  !pathValuePath.isEmpty() )
  {
    RTVal pathVal = RTVal::ConstructString(
      FabricApplicationStates::GetAppStates()->getContext(),
      pathValuePath.toUtf8().constData()
      );

    RTVal pathValue = RTVal::Construct(
      FabricApplicationStates::GetAppStates()->getContext(),
      "PathValue",
      1,
      &pathVal);

    setRTValArg(key, pathValue);
  }
 
  else
    setRTValArgValue(
      key, 
      RTValUtil::fromJSON(m_klCmd.getContext(), json, getRTValArgType(key))
      );

  FABRIC_CATCH_END("KLScriptableCommand::setArg");
}

void KLScriptableCommand::validateSetArgs()
{
  FABRIC_CATCH_BEGIN();

  RTVal errorVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    "");

  m_klCmd.callMethod(
    "", 
    "validateSetArgs", 
    1, 
    &errorVal);

  // Get possible KL error.
  QString strError = errorVal.getStringCString();
  if(!strError.isEmpty())
    FabricException::Throw(
      "KLScriptableCommand::validateSetArgs",
      "",
      strError);

  FABRIC_CATCH_END("KLScriptableCommand::validateSetArgs");
}

QString KLScriptableCommand::getArgsDescription() 
{ 
  FABRIC_CATCH_BEGIN();

  return m_klCmd.callMethod(
    "String",
    "getArgsDescription", 
    0, 
    0).getStringCString();

  FABRIC_CATCH_END("KLScriptableCommand::getArgsDescription");

  return "";
}

// BaseRTValScriptableCommand
void KLScriptableCommand::declareRTValArg( 
  QString const&key, 
  QString const&type,
  int flag, 
  FabricCore::RTVal defaultValue)
{
  // Do nothing.
}

QString KLScriptableCommand::getRTValArgType(
  QString const&key)
{
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return m_klCmd.callMethod(
    "String", 
    "getArgType", 
    1, 
    &keyVal
    ).getStringCString();

  FABRIC_CATCH_END("KLScriptableCommand::getRTValArgType");

  return "None";
}

QString KLScriptableCommand::getRTValArgPath(
  QString const&key)
{
  checkHasArg("KLScriptableCommand::getRTValArgPath", key);
 
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return m_klCmd.callMethod(
    "String", 
    "getArgPath", 
    1, 
    &keyVal
    ).getStringCString();

  FABRIC_CATCH_END("KLScriptableCommand::getRTValArgPath");

  return "";
}

RTVal KLScriptableCommand::getRTValArgValue(
  QString const&key)
{
  checkHasArg("KLScriptableCommand::getRTValArgValue", key);
 
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return RTValUtil::toRTVal(m_klCmd.callMethod(
    "RTVal", 
    "getArgValue", 
    1, 
    &keyVal));

  FABRIC_CATCH_END("KLScriptableCommand::getRTValArgPath");

  return RTVal();
}

RTVal KLScriptableCommand::getRTValArgValue(
  QString const&key,
  QString const&type)
{
  return getRTValArgValue(key);
}

void KLScriptableCommand::setRTValArgValue( 
  QString const&key, 
  FabricCore::RTVal value)
{
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());
  
  RTVal args[3] = { 
    keyVal, 
    RTValUtil::toKLRTVal(value), 
    // error
    RTVal::ConstructString(m_klCmd.getContext(), "") 
  };

  m_klCmd.callMethod(
    "", 
    "setArgValue", 
    3, 
    args);

  // Get possible KL error.
  QString strError = args[2].getStringCString();
  if(!strError.isEmpty())
    FabricException::Throw(
      "KLScriptableCommand::setRTValArgValue",
      "",
      strError);

  FABRIC_CATCH_END("KLScriptableCommand::setRTValArgValue");
}

RTVal KLScriptableCommand::getRTValArg(
  QString const&key)
{
  checkHasArg("KLScriptableCommand::getRTValArg", key);
 
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return RTValUtil::toRTVal(m_klCmd.callMethod(
    "RTVal", 
    "getArg", 
    1, 
    &keyVal));

  FABRIC_CATCH_END("KLScriptableCommand::getRTValArg");

  return RTVal();
}

void KLScriptableCommand::setRTValArg( 
  QString const&key, 
  FabricCore::RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
 
  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());
  
  RTVal args[3] = { 
    keyVal, 
    RTValUtil::toRTVal(pathValue), 
    // error
    RTVal::ConstructString(m_klCmd.getContext(), "") 
  };

  m_klCmd.callMethod(
    "", 
    "setArg", 
    3, 
    args);

  // Get possible KL error.
  QString strError = args[2].getStringCString();
  if(!strError.isEmpty())
    FabricException::Throw(
      "KLScriptableCommand::setRTValArg",
      "",
      strError);

  FABRIC_CATCH_END("KLScriptableCommand::setRTValArg");
}
