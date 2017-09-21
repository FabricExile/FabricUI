//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseCommand.h"
#include "CommandManager.h"

using namespace FabricUI;
using namespace Commands;

BaseCommand::BaseCommand() 
  : QObject()
  , m_canMergeID(CommandManager::NoCanMergeID)
  , m_blockLogEnabled(false)
{
}

BaseCommand::~BaseCommand() 
{
}

void BaseCommand::registrationCallback(
  QString const&name,
  void *userData) 
{
  m_name = name;
}

QString BaseCommand::getName() 
{
  return m_name;
}

bool BaseCommand::canUndo() 
{
  return false;
}

bool BaseCommand::canLog() 
{
  return canUndo() && !m_blockLogEnabled;
}

bool BaseCommand::doIt() 
{
  return false;
}

bool BaseCommand::undoIt() 
{
  return false;
}

bool BaseCommand::redoIt() 
{
  return false;
}

QString BaseCommand::getHelp() 
{
  return "";
}

QString BaseCommand::getHistoryDesc() 
{
  return getName();
}

void BaseCommand::setCanMergeID(
  int canMergeID)
{
  m_canMergeID = canMergeID;
}

int BaseCommand::getCanMergeID()
{
  return m_canMergeID;
}

void BaseCommand::blockLog() {
  m_blockLogEnabled = true;
}


bool BaseCommand::canMerge(
  BaseCommand *prevCmd, 
  bool& undoPrevAndMergeFirst)
{
  if(prevCmd) 
  {
    if(getCanMergeID() > CommandManager::NoCanMergeID && 
      getName() == prevCmd->getName() &&
      getCanMergeID() == prevCmd->getCanMergeID())
      return true;
  }
  return false;
}

void BaseCommand::merge(
  BaseCommand *prevCmd) 
{
}
