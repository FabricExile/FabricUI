//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandHelpers.h"
#include <FabricUI/Util/RTValUtil.h>
#include "BaseRTValScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

BaseRTValScriptableCommand::BaseRTValScriptableCommand() 
  : BaseScriptableCommand()
{
}

BaseRTValScriptableCommand::~BaseRTValScriptableCommand() 
{
}
 
bool BaseRTValScriptableCommand::isArgTypeKnown(
  QString const&key) 
{
  return m_rtvalArgSpecs[key].type != "RTVal";
}

void BaseRTValScriptableCommand::declareArg(
  QString const&key, 
  int flags, 
  QString const&defaultValue)
{ 
  declareRTValArg(key, "RTVal", flags); 
}

bool BaseRTValScriptableCommand::hasArg(
  QString const&key)
{
  return m_rtvalArgSpecs.count(key) > 0;
}

void BaseRTValScriptableCommand::checkHasArg(
  QString const&methodName,
  QString const&key)
{
  if(!hasArg(key)) 
    FabricException::Throw(
      methodName,
      "No arg named '" + key + "' in command '" + getName() + "'"
      );
}

void BaseRTValScriptableCommand::checkEmptyKey(
  QString const&methodName,
  QString const&key)
{
  if(key.isEmpty()) 
    FabricException::Throw(
      methodName,
      "Command '" + getName() + "', key not specified"
      );
}

void BaseRTValScriptableCommand::checkRTValType(
  QString const&methodName,
  QString const&key,
  QString const&type)
{
  if(!FabricApplicationStates::GetAppStates()->getClient().isValidType(type.toUtf8().constData()))
    FabricException::Throw(
      methodName,
      "Argument '" + key + "' in command '" + getName() + "' has not a valid kl type '" + type + "'"
      );
}

bool BaseRTValScriptableCommand::hasArgFlag(
  QString const&key,
  int flag)
{
  checkEmptyKey("BaseRTValScriptableCommand::hasArgFlag", key); 
  checkHasArg("BaseRTValScriptableCommand::hasArgFlag", key); 
  return (m_rtvalArgSpecs[key].flags & flag);
}

QList<QString> BaseRTValScriptableCommand::getArgKeys()
{
  return m_rtvalArgSpecs.keys(); 
}

bool BaseRTValScriptableCommand::isArgSet(
  QString const&key)
{
  // The arg can be set as RTVal or as JSON.
  return m_rtvalArgs.count(key) && 
    ( RTValUtil::getType(getRTValArgValue(key)) == m_rtvalArgSpecs[key].type || //TODO: should check "isA"
      !getRTValArgPath(key).isEmpty() || !m_rtvalArgs[key].second.isEmpty() 
    );
}

QString BaseRTValScriptableCommand::getArg(
  QString const&key)
{
  checkHasArg("BaseRTValScriptableCommand::getArg", key); 

  FABRIC_CATCH_BEGIN();

  // Known RTVal of known type, get the json from it.
  return (m_rtvalArgs[key].second.isEmpty() && isArgTypeKnown(key))
    ? (isArgSet(key) ? RTValUtil::toJSON(getRTValArgValue(key)) : "")
    // Otherwise, return the Json if it's been set.
    // It happens if the arg's been declared with an unknown type
    : m_rtvalArgs[key].second;
  
  FABRIC_CATCH_END("BaseRTValScriptableCommand::getArg");

  return "";
}

void BaseRTValScriptableCommand::setArg(
  QString const&key, 
  QString const&json) 
{
  checkHasArg("BaseRTValScriptableCommand::setArg", key); 

  FABRIC_CATCH_BEGIN();

  QString pathValuePath = CommandHelpers::castFromPathValuePath(json);

  if( !pathValuePath.isEmpty() )
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
 
  // Known type, cast the JSON to a RTVal.
  else if(isArgTypeKnown(key))
  {      
    RTVal rtVal = RTValUtil::fromJSON(
      FabricApplicationStates::GetAppStates()->getContext(),
      json,
      getRTValArgType(key));

    setRTValArgValue(key, rtVal);
  }

  // Store the JSON directly.
  else
    m_rtvalArgs[key].second = json;

  FABRIC_CATCH_END("BaseRTValScriptableCommand::setArg");
}

void BaseRTValScriptableCommand::validateSetArgs()
{
  QMapIterator<QString, ScriptableCommandRTValArgSpec> it(m_rtvalArgSpecs);
  
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    ScriptableCommandRTValArgSpec spec = it.value();

    // We support unknown type.
    if(!hasArgFlag(key, CommandArgFlags::OPTIONAL_ARG) && !isArgSet(key))
      FabricException::Throw(
        "BaseRTValScriptableCommand::validateSetArgs",
        "Argument '" + key + "' in command '" + getName() + "' has not been set"
        );
  }
}

QString BaseRTValScriptableCommand::getArgsDescription() 
{
  QString res;

  FABRIC_CATCH_BEGIN();

  int count = 0;
  QMapIterator<QString, ScriptableCommandRTValArgSpec> it(m_rtvalArgSpecs);
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    ScriptableCommandRTValArgSpec spec = it.value();

    res += "    ["  + key + "]";
    res += ", opt: " + CommandHelpers::getArgsTypeSpecs(this, key);
    res += ", path: <" + getRTValArgPath(key) + ">";
    res += ", val: " + getArg(key);

    if(spec.defaultValue.isValid())
      res += ", defVal: " + RTValUtil::toJSON(spec.defaultValue);
 
    res += (count < m_rtvalArgSpecs.size() - 1) ? "\n" : "";
    count++;
  }
  
  FABRIC_CATCH_END("BaseRTValScriptableCommand::getArgsDescription");

  return res;
}

void BaseRTValScriptableCommand::declareRTValArg(
  QString const&key, 
  QString const&type,
  int flags, 
  RTVal defaultValue) 
{
  checkEmptyKey("BaseRTValScriptableCommand::declareRTValArg", key); 
  checkRTValType("BaseRTValScriptableCommand::declareRTValArg", key, type); 
 
  FABRIC_CATCH_BEGIN();

  ScriptableCommandRTValArgSpec spec;
  spec.type = type;
  spec.flags = flags;
  spec.defaultValue = defaultValue;
  m_rtvalArgSpecs.insert(key, spec);

  RTVal pathValue;
  Context context = FabricApplicationStates::GetAppStates()->getContext();
  
  if(!type.isEmpty() && defaultValue.isValid())
  {
    RTVal args[2] = {
      RTVal::ConstructString(context, ""),
      RTValUtil::toKLRTVal(defaultValue)
    };

    pathValue = RTVal::Construct(
      context, 
      "PathValue", 
      2, 
      args);
  }
  else
    pathValue = RTVal::Construct(
      context, 
      "PathValue", 
      0, 
      0);
 
  QPair<FabricCore::RTVal, QString> pair;
  pair.first = pathValue;
  m_rtvalArgs.insert(key, pair);
  
  FABRIC_CATCH_END("BaseRTValScriptableCommand::declareRTValArg");
}

QString BaseRTValScriptableCommand::getRTValArgType(
  QString const&key)
{
  checkHasArg("BaseRTValScriptableCommand::getRTValArgType", key); 
  return m_rtvalArgSpecs[key].type;
}

QString BaseRTValScriptableCommand::getRTValArgPath(
  QString const&key)
{
  checkHasArg("BaseRTValScriptableCommand::getRTValArgPath", key); 
     
  FABRIC_CATCH_BEGIN();

  return RTValUtil::toRTVal(
    RTValUtil::toRTVal(m_rtvalArgs[key].first).maybeGetMember("path")
    ).getStringCString();
 
  FABRIC_CATCH_END("BaseRTValScriptableCommand::getRTValArgPath");

  return "";
}

RTVal BaseRTValScriptableCommand::getRTValArgValue(
  QString const&key)
{
  checkHasArg("BaseRTValScriptableCommand::getRTValArgValue", key); 
  
  if(!m_rtvalArgs[key].first.isValid() && !m_rtvalArgs[key].second.isEmpty())
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArgValue",
        "RTVal argument '" + key + "' of command '" + getName() + 
        "' has been set in JSON only, \n !!! use getRTValArgValue(key, type) instead !!!"
        );

  FABRIC_CATCH_BEGIN();
 
  return RTValUtil::toRTVal(
    RTValUtil::toRTVal(m_rtvalArgs[key].first).maybeGetMember("value")
    );
 
  FABRIC_CATCH_END("BaseRTValScriptableCommand::getRTValArgValue");

  return RTVal();
}

RTVal BaseRTValScriptableCommand::getRTValArgValue(
  QString const&key,
  QString const&type)
{
  checkHasArg("BaseRTValScriptableCommand::getRTValArgValue", key); 
  checkRTValType("BaseRTValScriptableCommand::getRTValArgValue", key, type); 

  FABRIC_CATCH_BEGIN();

  // If the type is unknown, set it
  if(!isArgTypeKnown(key))
    m_rtvalArgSpecs[key].type = type;

  // If the arg has been set in JSON,  
  // construct it since we know its type
  if(!m_rtvalArgs[key].second.isEmpty())
  {
    RTVal val = RTValUtil::fromJSON(
      FabricApplicationStates::GetAppStates()->getContext(),
      m_rtvalArgs[key].second,
      m_rtvalArgSpecs[key].type);

    setRTValArgValue(key, val);
  }
 
  FABRIC_CATCH_END("BaseRTValScriptableCommand::getRTValArgValue");

  return getRTValArgValue(key);
}

void BaseRTValScriptableCommand::setRTValArg(
  QString const&key, 
  RTVal pathValue) 
{ 
  checkHasArg("BaseRTValScriptableCommand::setRTValArg", key); 

  if(RTValUtil::getType(pathValue) != "PathValue")
     FabricException::Throw(
      "BaseRTValScriptableCommand::setRTValArg",
      "Arg '" + key + "' is not a PathValue"
      );

  FABRIC_CATCH_BEGIN();

  m_rtvalArgSpecs[key].type = RTValUtil::getType(
    RTValUtil::toRTVal(pathValue).maybeGetMember("value")
    );

  QPair<FabricCore::RTVal, QString> pair;
  pair.first = pathValue;
  m_rtvalArgs.insert(key, pair);
 
  FABRIC_CATCH_END("BaseRTValScriptableCommand::setRTValArg")
}

RTVal BaseRTValScriptableCommand::getRTValArg(
  QString const&key)
{ 
  checkHasArg("BaseRTValScriptableCommand::getRTValArg", key); 
 
  FABRIC_CATCH_BEGIN();

  return RTValUtil::toRTVal(m_rtvalArgs[key].first);
 
  FABRIC_CATCH_END("BaseRTValScriptableCommand::getRTValArg")

  return RTVal();
}

void BaseRTValScriptableCommand::setRTValArgValue(
  QString const&key, 
  RTVal value) 
{ 
  checkHasArg("BaseRTValScriptableCommand::setRTValArgValue", key); 

  FABRIC_CATCH_BEGIN();

  RTVal pathValue = getRTValArg(key);
  pathValue.setMember("value", RTValUtil::toKLRTVal(value));
  setRTValArg(key, pathValue);
  
  FABRIC_CATCH_END("BaseRTValScriptableCommand::setRTValArgValue")
}
