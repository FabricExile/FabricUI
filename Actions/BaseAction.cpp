// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "BaseAction.h"
#include "ActionRegistry.h"

using namespace FabricUI;
using namespace Actions;

void BaseAction::init(
  QString name, 
  QString text, 
  bool enable,
  bool isEditable)
{
  m_name = name;
  m_isEditable = isEditable;
  
  setText(text);
  setEnabled(enable);

  connect(
    this, 
    SIGNAL(triggered()), 
    this, 
    SLOT(onTriggered())
    );

  connect(
    this, 
    SIGNAL(toggled(bool)), 
    this, 
    SLOT(onToggled(bool))
    );

  // Register the action.
  ActionRegistry *registry = ActionRegistry::GetActionRegistry();
  registry->registerAction(this);
}

void BaseAction::init(
  QString name, 
  QString text, 
  QKeySequence shortcut,
  Qt::ShortcutContext context,
  bool enable,
  bool isEditable)
{
  // Checks if an action under this name has been 
  // registered already. If so, use its shortcuts.
  // Synchronize the shortcuts of all the actions.
  ActionRegistry *registry = ActionRegistry::GetActionRegistry();
  setShortcut( 
    (registry->getRegistrationCount(name) > 0)
    ? registry->getShortcut(name) 
    : shortcut
  );
  setShortcutContext(context);

  init(name, 
    text, 
    enable, 
    isEditable);
}

void BaseAction::init(
  QString name, 
  QString text, 
  const QList<QKeySequence> & shortcuts,
  Qt::ShortcutContext context,
  bool enable,
  bool isEditable)
{
  ActionRegistry *registry = ActionRegistry::GetActionRegistry();
  setShortcuts( 
    (registry->getRegistrationCount(name) > 0)
    ? registry->getShortcuts(name) 
    : shortcuts
  );
  setShortcutContext(context);

  init(name, 
    text, 
    enable, 
    isEditable);
}

void BaseAction::init(
  QString name, 
  QString text, 
  QKeySequence shortcut,
  Qt::ShortcutContext context,
  const QIcon &icon,
  bool enable,
  bool isEditable)
{
  setIcon(icon);
  
  init(name, 
    text, 
    shortcut,
    context, 
    enable, 
    isEditable);
}

void BaseAction::init(
  QString name, 
  QString text, 
  const QList<QKeySequence> & shortcuts,
  Qt::ShortcutContext context,
  const QIcon &icon,
  bool enable,
  bool isEditable)
{
  setIcon(icon);
    
  init(name, 
    text, 
    shortcuts,
    context, 
    enable, 
    isEditable);
}

BaseAction::BaseAction(
  QObject *parent)
  : QAction(parent)
{
}

BaseAction::BaseAction(
  QObject *parent,
  QString name, 
  QString text, 
  QKeySequence shortcut,
  Qt::ShortcutContext context,
  bool enable,
  const QIcon &icon)
  : QAction(parent)
{
  init(name, 
    text, 
    shortcut,
    context, 
    icon,
    enable);
}

BaseAction::BaseAction(
  QObject *parent,
  QString name, 
  QString text, 
  const QList<QKeySequence> & shortcuts,
  Qt::ShortcutContext context,
  bool enable,
  const QIcon &icon)
  : QAction(parent)
{
  init(name, 
    text, 
    shortcuts,
    context, 
    icon,
    enable);
}

BaseAction::~BaseAction()
{
}

QString BaseAction::getName() const
{
  return m_name;
}

bool BaseAction::isEditable() const
{
  return m_isEditable;
}

void BaseAction::onTriggered()
{
}

void BaseAction::onToggled(bool checked)
{
}
