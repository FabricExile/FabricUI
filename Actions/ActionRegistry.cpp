// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ActionRegistry.h"

using namespace FabricUI;
using namespace Actions;

bool ActionRegistry::s_instanceFlag = false;
ActionRegistry* ActionRegistry::s_actionRegistry = 0;

ActionRegistry::ActionRegistry()
{
}

ActionRegistry::~ActionRegistry()
{
  s_instanceFlag = false;
}

ActionRegistry* ActionRegistry::GetActionRegistry()
{
  // Construct the singleton if needed.
  if(!s_instanceFlag)
  {
    s_actionRegistry = new ActionRegistry();
    s_instanceFlag = true;
  }
  return s_actionRegistry;
}

void ActionRegistry::registerAction(
  BaseAction *action)
{
  registerAction(
    action->getName(), 
    action);
}

void ActionRegistry::registerAction(
  QString const&actionName,
  QAction *action)
{
  QObject::connect(
    action,
    SIGNAL(destroyed(QObject *)),
    this,
    SLOT(onUnregisterAction(QObject *))
  );

  m_registeredActions[actionName].insert(action);

  emit actionRegistered(actionName, action);
}

void ActionRegistry::onUnregisterAction(
  QObject *obj)
{
  QAction* action = (QAction*)obj;
  if(!action)
      return;

  QString actionName = getActionName(action);
  if(actionName.isEmpty())
    return;

  m_registeredActions[actionName].remove(action);

  if(m_registeredActions[actionName].empty())
    m_registeredActions.remove(actionName);
 
  emit actionUnregistered(actionName);
}

bool ActionRegistry::isActionRegistered(
  QString const&actionName) const
{
  return m_registeredActions.count(actionName) > 0;
}

int ActionRegistry::getRegistrationCount(
  QString const&actionName) const
{
  if(!isActionRegistered(actionName))
    return 0;

  return m_registeredActions[actionName].count();
}

inline QAction *GetFirstAction(
  QMapIterator<QString, QSet< QAction * > > &ite) 
{
  QSetIterator<  QAction * > i(ite.value());
  return i.next();
}

QList<QAction*> ActionRegistry::isShortcutUsed(
  QKeySequence const&shortcut) const
{
  QList<QAction*> res;

  QMapIterator<QString, QSet< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QSetIterator<  QAction * > i(ite.value());

    while (i.hasNext()) 
    {
      QAction *action = i.next();
      if(action->shortcut() == shortcut)
        res.append(action);
    }
  }

  return res;
}

QList<QAction*> ActionRegistry::isShortcutUsed(
  QList<QKeySequence> const&shortcuts) const
{
  QList<QAction*> res;

  QMapIterator<QString, QSet< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QSetIterator<  QAction * > i(ite.value());

    while (i.hasNext()) 
    {
      QAction *action = i.next();
      if(action->shortcuts() == shortcuts)
        res.append(action);
    }
  }
  
  return res;
}

void ActionRegistry::setShortcut(
  QString const&actionName,
  QKeySequence const&shortcut) const
{
  if(!isActionRegistered(actionName))
    return;

  foreach (QAction * action, m_registeredActions[actionName])
    action->setShortcut(shortcut);
}

void ActionRegistry::setShortcuts(
  QString const&actionName,
  QList<QKeySequence> const&shortcuts) const
{
  if(!isActionRegistered(actionName))
    return;

  foreach (QAction * action, m_registeredActions[actionName])
    action->setShortcuts(shortcuts);
}
 
QKeySequence ActionRegistry::getShortcut(
  QString const&actionName) const
{
  return isActionRegistered(actionName)
    ? getAction(actionName)->shortcut()
    : QKeySequence();
}

QList<QKeySequence> ActionRegistry::getShortcuts(
  QString const&actionName) const
{
  QList<QKeySequence> dum;
  return isActionRegistered(actionName)
    ? getAction(actionName)->shortcuts()
    : dum;
}

QAction* ActionRegistry::getAction(
  QString const&actionName) const
{
  if(!isActionRegistered(actionName))
    return 0;
    
  QSetIterator<  QAction * > i(m_registeredActions[actionName]);
  QAction *action = i.next();
  return action;
}

QString ActionRegistry::getActionName(
  QAction *action) const
{
  QMapIterator<QString, QSet< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    if(ite.value().contains(action))
      return ite.key();
  }
  return "";
}

QList<QString> ActionRegistry::getActionNameList() const
{
  QList<QString> actionNameList;
  QMapIterator<QString, QSet< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    actionNameList.append(ite.key());
  }
  return actionNameList;
}

QString ActionRegistry::getContent() const
{
  QString res = "--> ActionRegistry:\n";
  QMapIterator<QString, QSet< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QAction *action = GetFirstAction(ite);
    QList<QKeySequence> shortcutsList = action->shortcuts();

    res += ite.key();
    res +=+ "[";
    for(int i=0; i<shortcutsList.size(); ++i)
    {
      res += shortcutsList[i].toString();
      res += (i < shortcutsList.size()-1) ? ", " : "";
    }
    res +=+ "]";
    res += '\n';
  }
  return res;
}

bool ActionRegistry::isActionContextGlobal(
  QString const&actionName) const
{
  if(isActionRegistered(actionName))
  {
    QAction *action = getAction(actionName);
    return action->shortcutContext() == Qt::WindowShortcut ||  
      action->shortcutContext() == Qt::ApplicationShortcut;
  }
  return false;
}

