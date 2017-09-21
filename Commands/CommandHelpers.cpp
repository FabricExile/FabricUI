//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseCommand.h"
#include "CommandHelpers.h"
#include "KLCommandManager.h"
#include "BaseScriptableCommand.h"
#include "BaseRTValScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>
 
using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

// Command arg flags (see BaseScriptableCommand::declareArg)
int CommandArgFlags::NO_FLAG_ARG = 0;
int CommandArgFlags::OPTIONAL_ARG = 1<<0;
int CommandArgFlags::DONT_LOG_ARG = 1<<1;
int CommandArgFlags::IN_ARG = 1<<2;
int CommandArgFlags::OUT_ARG = 1<<3;
int CommandArgFlags::IO_ARG = 1<<4;
// int CommandArgFlags::CREATE_IF_NULL = 1<<5;// Reserved, not implemented yet in C++
// int CommandArgFlags::CLONE_FOR_UNDO = 1<<6;// Reserved, not implemented yet in C++

inline BaseScriptableCommand* CastToBaseScriptableCommand(
  BaseCommand *cmd) 
{
  BaseScriptableCommand* scriptCmd = qobject_cast<BaseScriptableCommand*>(cmd);

  if(scriptCmd == 0)
    FabricException::Throw(
      "CommandHelpers::CastToBaseScriptableCommand",
      "Bad cast, cmd '" + cmd->getName() + "' is not a BaseScriptableCommand"
      );

  return scriptCmd;
}

inline BaseRTValScriptableCommand* CastToBaseRTValScriptableCommand(
  BaseCommand *cmd) 
{
  BaseRTValScriptableCommand* scriptCmd = qobject_cast<BaseRTValScriptableCommand*>(cmd);

  if(scriptCmd == 0)
    FabricException::Throw(
      "CommandHelpers::CastToBaseRTValScriptableCommand",
      "Bad cast");

  return scriptCmd;
}

QString CommandHelpers::createHistoryDescFromArgs(
  BaseCommand *cmd,
  QMap<QString, QString> const&argsDesc)
{
  CastToBaseScriptableCommand(cmd);

  FABRIC_CATCH_BEGIN();

  BaseRTValScriptableCommand *rtValScriptCmd = qobject_cast<BaseRTValScriptableCommand*>(
    cmd);

  QString desc = cmd->getName();

  if(argsDesc.size() > 0)
  {
    int count = 0;
    desc +=  "(";

    QMapIterator<QString, QString> it(argsDesc);
    while(it.hasNext()) 
    {
      it.next();

      QString key = it.key();
      QString argDesc = ( (rtValScriptCmd != 0) && !rtValScriptCmd->getRTValArgPath(key).isEmpty())
        ? "<" + it.value() + ">"
        : it.value();

      desc += key + "=\"" + argDesc + "\"";
      if(count < argsDesc.size()-1)
        desc += ", ";
      count++;
    }

    desc +=  ")";
  }

  return desc;

  FABRIC_CATCH_END("CommandHelpers::createHistoryDescFromArgs");

  return "";
}

QString CommandHelpers::getArgsTypeSpecs(
  BaseCommand *cmd,
  QString const&key)
{
  QString specs; 

  FABRIC_CATCH_BEGIN();

  BaseScriptableCommand *scriptCmd = CastToBaseScriptableCommand(cmd);

  if(scriptCmd &&scriptCmd->hasArg(key)) 
  {
    specs += "["; 

    BaseRTValScriptableCommand *rtValScriptCmd = qobject_cast<BaseRTValScriptableCommand*>(
      cmd);

    if(rtValScriptCmd)
      specs += "PathValue(" + rtValScriptCmd->getRTValArgType(key) + ")"; 
    else
      specs += "String";

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::OPTIONAL_ARG))
      specs += ", optional"; 

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::DONT_LOG_ARG ))
      specs += ", dont-log"; 

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::IN_ARG))
      specs += ", IN"; 

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::OUT_ARG))
      specs += ", OUT"; 

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::IO_ARG))
      specs += ", IO"; 
    
    specs += "]"; 
  }

  FABRIC_CATCH_END("CommandHelpers::getArgsTypeSpecs");

  return specs;
}

QString CommandHelpers::createHelpFromArgs(
  BaseCommand *cmd,
  QString const&commandHelp,
  QMap<QString, QString> const&argsHelp)
{
  QString help;
  
  FABRIC_CATCH_BEGIN();

  // Be sure it's a scriptable command.
  CastToBaseScriptableCommand(cmd);

  help = commandHelp + "\n";

  if(argsHelp.size() > 0)
    help +=  "Arguments:\n";

  QMapIterator<QString, QString> it(argsHelp);
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    help +=  "- " + key + getArgsTypeSpecs(cmd, key) + ": " + it.value() + "\n";
  }

  FABRIC_CATCH_END("CommandHelpers::createHelpFromArgs");

  return help;
}

QString CommandHelpers::createHelpFromRTValArgs(
  BaseCommand *cmd,
  QString const&commandHelp,
  QMap<QString, QString> const&argsHelp)
{
  QString help;
  
  FABRIC_CATCH_BEGIN();

  BaseRTValScriptableCommand *rtValScriptCmd = qobject_cast<BaseRTValScriptableCommand*>(
    cmd);

  if(rtValScriptCmd == 0)
    FabricException::Throw(
      "CommandHelpers::createHelpFromRTValArgs",
      "cmd '" + cmd->getName() + "' is not a BaseRTValScriptableCommand");

  help = commandHelp + "\n";

  if(argsHelp.size() > 0)
    help +=  "Arguments:\n";

  QMapIterator<QString, QString> it(argsHelp);
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    if(rtValScriptCmd->hasArg(key)) 
      help += "- " + key + getArgsTypeSpecs(cmd, key) + ": " + it.value() + "\n";
  }
 
  FABRIC_CATCH_END("CommandHelpers::createHelpFromRTValArgs");
  
  return help;
}

bool CommandHelpers::isRTValScriptableCommand(
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseRTValScriptableCommand* scriptCmd = qobject_cast<BaseRTValScriptableCommand*>(cmd);
  return scriptCmd != 0;
  FABRIC_CATCH_END("CommandHelpers::isRTValScriptableCommand");
  return false;
}

bool CommandHelpers::isScriptableCommand(
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = qobject_cast<BaseScriptableCommand*>(cmd);
  return scriptCmd != 0;
  FABRIC_CATCH_END("CommandHelpers::isScriptableCommand");
  return false;
}

bool CommandHelpers::hasCommandFlag(
  BaseCommand *cmd,
  QString const&key,
  int flags)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = CastToBaseScriptableCommand(cmd);
  return scriptCmd->hasArgFlag(key, flags);
  FABRIC_CATCH_END("CommandHelpers::hasCommandFlag");
  return false;
}

bool CommandHelpers::isCommandArgSet(
  BaseCommand *cmd,
  QString const&key)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = qobject_cast<BaseScriptableCommand*>(cmd);
  return scriptCmd->isArgSet(key);
  FABRIC_CATCH_END("CommandHelpers::isCommandArgSet");
  return false;
}

bool CommandHelpers::hasCommandArg(
  BaseCommand *cmd,
  QString const&key)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = CastToBaseScriptableCommand(cmd);
  return scriptCmd->hasArg(key);
  FABRIC_CATCH_END("CommandHelpers::hasCommandArg");
  return false;
}

QList<QString> CommandHelpers::getCommandArgKeys(
  BaseCommand *cmd)
{
  QList<QString> keys;
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = CastToBaseScriptableCommand(cmd);
  keys = scriptCmd->getArgKeys();
  FABRIC_CATCH_END("CommandHelpers::getCommandArgKeys");
  return keys;
}

QString CommandHelpers::getCommandArg(
  BaseCommand *cmd,
  QString const&key)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = CastToBaseScriptableCommand(cmd);
  return scriptCmd->getArg(key);
  FABRIC_CATCH_END("CommandHelpers::getCommandArg");
  return "";
}

RTVal CommandHelpers::getRTValCommandArg(
  BaseCommand *cmd,
  QString const&key)
{
  FABRIC_CATCH_BEGIN();
  BaseRTValScriptableCommand* scriptCmd = CastToBaseRTValScriptableCommand(cmd);
  return scriptCmd->getRTValArg(key);
  FABRIC_CATCH_END("CommandHelpers::getRTValCommandArg");
  return RTVal();
}

QString CommandHelpers::getRTValCommandArgType(
  BaseCommand *cmd,
  QString const&key)
{
  FABRIC_CATCH_BEGIN();
  BaseRTValScriptableCommand* scriptCmd = CastToBaseRTValScriptableCommand(cmd);
  return scriptCmd->getRTValArgType(key);
  FABRIC_CATCH_END("CommandHelpers::getRTValCommandArgType");
  return "";
}

inline QString encodeJSONChars(
  QString const&str)
{
  QString res = str;
  res = res.replace("\"", "'").replace("\\", "\\\\").replace(" ", "");
  return res.replace("\r", "").replace("\n", "").replace("\t", "");
}

QString CommandHelpers::encodeJSON(
  QString const&str)
{
  return "\"" + encodeJSONChars(str) + "\"";
}

QString CommandHelpers::encodeJSONs(
  QString const&string)
{
  /*
  result = "\""
    for i in range(0, len(strings)):
        if i > 0:
            result += "|"
        result += CommandHelpers.__EncodeJSONChars(strings[i])
    result += "\""
    return result
  */
  return "";
}

QString CommandHelpers::castFromPathValuePath(
  QString const&path)
{
  if( path.startsWith("<") && path.endsWith(">") )
  {
    QString castedPath = path;
    castedPath.remove(0, 1);
    return castedPath.remove(castedPath.size()-1, 1);
  }
  return "";
}

QString CommandHelpers::castToPathValuePath(
  QString const&path)
{
  return "<" + path + ">";
}
