// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_ACTION_REGISTRY__
#define __UI_ACTION_REGISTRY__

#include <QMap>
#include <QSet>
#include <QList>
#include <QObject>
#include "BaseAction.h"

namespace FabricUI {
namespace Actions {

class ActionRegistry : public QObject
{
  /**
    ActionRegistry registers QAction and BaseAction as a key-value pair in a map
    so we know which actions and key-shortcuts are used in the application. These
    informations are not provided by Qt and are needed by the command system. 

    When an action is registered-unregistered, the signals `actionRegistered` and 
    `actionUnregistered` are emitted. The same action may be created several times 
    from differents places. At each time, the action is registered and pushed in a 
    flat list. When an action is deleted, it's removed from the list. When the list 
    size is null, the is action unregistered. ActionRegistry provides functionalities 
    to synchronizes the shortcut sequence of all the actions sharing the same name.

    The registry sets it-self as a singleton:
    - C++:
      #include <FabricUI/Actions/ActionRegistry.h>
      ActionRegistry *actionRegistry = ActionRegistry::GetActionRegistry();
    
    - Python:
      from FabricEngine.FabricUI import Actions
      actionRegistry = Actions.ActionRegistry.GetActionRegistry()
  
    Usage:
    - Register a QAction: actionRegistry->:registerAction(actionName, new QAction(...));
    
    - Register a BaseAction: actionRegistry->:registerAction(new BaseAction(...));

    - Check if an action is registered: actionRegistry->isActionRegistered(actionName);
    
    - Check if a shortcut is used: actionRegistry->isShortcutUsed(actionName, QAction *action);

    - Get an action: QAction *action = actionRegistry->getAction(actionName);
  */  
  Q_OBJECT

  public: 
    ActionRegistry();

    ~ActionRegistry();

    /// Gets the registry singleton.
    /// Creates it if needed.
    static ActionRegistry* GetActionRegistry();

    /// Registers a QAction 
    /// under 'actionName'.
    void registerAction(
      QString const&actionName,
      QAction *action
      );

    /// Registers a BaseAction 
    /// under 'actionName'.
    void registerAction(
      BaseAction *action
      );

    /// Checks if an action is
    /// registered under 'actionName'.
    bool isActionRegistered(
      QString const&actionName
      ) const;

    /// Gets the number of time the
    /// action has been registered.
    int getRegistrationCount(
      QString const&actionName
      ) const;

    /// Checks if the shortcut is  
    /// already used by an action.
    QList<QAction*> isShortcutUsed(
      QKeySequence const&shortcut
      ) const;

    /// Checks if the shortcuts is  
    /// already used by an action.
    QList<QAction*> isShortcutUsed(
      QList<QKeySequence> const&shortcuts
      ) const;

    /// Sets the shortcut of the actions
    /// registered under `actionName`.
    void setShortcut(
      QString const&actionName,
      QKeySequence const&shortcut
      ) const;

    /// Sets the shortcuts of the actions
    /// registered under `actionName`.
    void setShortcuts(
      QString const&actionName,
      QList<QKeySequence> const&shortcuts
      ) const;

    /// Gets the shortcut. Returns an
    /// empty sequence if the action 
    /// has not been registered
    QKeySequence getShortcut(
      QString const&actionName
      ) const;

    /// Gets the shortcuts. Returns 
    /// an empty list if the action 
    /// has not been registered
    QList<QKeySequence> getShortcuts(
      QString const&actionName
      ) const;

    /// Gets the action registered under
    /// 'actionName'. Returns null if the
    /// action has not been registered.
    QAction* getAction(
      QString const&actionName
      ) const;

    /// Gets the action name. Returns 
    /// an empty string if the action 
    /// has not been registered.
    QString getActionName(
      QAction* action
      ) const;

    /// Gets all the registered action.
    QList<QString> getActionNameList() const;

    /// Gets all the registred actions and their
    /// shortcuts as a string, used for debugging.
    QString getContent() const;

    /// Checks if the action context is global,
    /// Qt::WindowShortcut or Qt::ApplicationShortcut.
    bool isActionContextGlobal(
      QString const&actionName
      ) const;
    
  signals:
    /// Emitted when an action 
    /// has been registered.
    void actionRegistered(
      QString const&actionName,
      QAction *action
      );

    /// Emitted when an action 
    /// is unregistered.
    void actionUnregistered(
      QString const&actionName
      );

  private slots:
    /// Called when an action 
    /// is destroyed.
    void onUnregisterAction(
      QObject *obj
      );

  private:
    /// Dictionaries of registered actions.
    QMap< QString, QSet<QAction*> > m_registeredActions;
    /// Registry singleton.
    static ActionRegistry * s_actionRegistry;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
};

} // namespace Actions
} // namespace FabricUI

#endif // __UI_ACTION_REGISTRY__
