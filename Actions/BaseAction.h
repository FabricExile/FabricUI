// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_BASE_ACTION__
#define __UI_BASE_ACTION__

#include <QString>
#include <QAction>
#include <QShortcut>
#include <QKeySequence>

namespace FabricUI {
namespace Actions {

class BaseAction : public QAction
{
  /**
    BaseAction inherates QActions and are registered in the ActionRegistry
    when created. The action unregisteres it-self when it's destroyed. Any
    actions using shortcuts shall specialize this class. An action can be 
    edited. If so, it shortcut can be set from the hotkey editor. 

    Usage:
    -C++:
      #include <FabricUI/Actions/BaseAction.h>

      class MyAction : pulic BaseAction
      {
        public:
          MyAction(QObject *parent)
          : BaseAction(
              parent
            , "MyAction" 
            , "Do my action" 
            , Qt::Key_R)
          {
          }
          
          virtual ~MyAction()
          {
          }

        private:
          virtual void onTriggered()
          {
            // Do you action
          }
      };
      
      or

      class MyAction : pulic BaseAction
      {
        public:
          MyAction(QObject *parent)
          : BaseAction(parent)
          {
            init(
              "MyAction" 
              , "Do my action" 
              , Qt::Key_R);
          }
          
          virtual ~MyAction()
          {
          }

        private:
          virtual void onTriggered()
          {
            // Do you action
          }
      };

    -Python:
      from PySide import QtCore
      from FabricEngine.FabricUI import Actions

      class MyAction(Actions.BaseAction):
      
        def __init__(self, parent)
          super(MyAction, self),__init__(
            parent
            , "MyAction" 
            , "Do my action" 
            , QtCore.Qt.Key_R)
          
          def onTriggered(self):
            # Do you action
            pass
      
      or 

      class MyAction(Actions.BaseAction):
      
        def __init__(self, parent)
          super(MyAction, self),__init__(parent)
            
          super(MyAction, self).init("MyAction" 
            , "Do my action" 
            , QtCore.Qt.Key_R)
          
          def onTriggered(self):
            # Do you action
            pass
  */
  Q_OBJECT

  public:
    BaseAction(
      QObject *parent
      );

    BaseAction(
      QObject *parent,
      QString name, 
      QString text = "", 
      QKeySequence shortcut = QKeySequence(),
      Qt::ShortcutContext context = Qt::ApplicationShortcut,
      bool enable = true,
      const QIcon &icon = QIcon()
      );

    BaseAction(
      QObject *parent,
      QString name, 
      QString text = "", 
      const QList<QKeySequence> &shortcuts = QList<QKeySequence>(),
      Qt::ShortcutContext context = Qt::ApplicationShortcut,
      bool enable = true,
      const QIcon &icon = QIcon()
      );

    virtual ~BaseAction();

    /// Gets the action name.
    QString getName() const;

    /// Check if the action is editable.
    /// Used by the hotkey editor
    bool isEditable() const;
 
  protected slots:
    /// To override.
    virtual void onTriggered();

    /// To override.
    virtual void onToggled(bool checked);

  protected:
    /// Intializes the BaseAction.
    void init(
      QString name, 
      QString text,
      bool enable = true,
      bool isEditable = true
      );

    /// Intializes the BaseAction.
    void init(
      QString name, 
      QString text,
      QKeySequence shortuct,
      Qt::ShortcutContext context,
      bool enable = true,
      bool isEditable = true
      );

    /// Intializes the BaseAction.
    void init(
      QString name, 
      QString text,
      const QList<QKeySequence> &shortcuts,
      Qt::ShortcutContext context,
      bool enable = true,
      bool isEditable = true
      );

    /// Intializes the BaseAction.
    void init(
      QString name, 
      QString text,
      QKeySequence shortuct,
      Qt::ShortcutContext context,
      const QIcon &icon,
      bool enable = true,
      bool isEditable = true
      );

    /// Intializes the BaseAction.
    void init(
      QString name, 
      QString text,
      const QList<QKeySequence> &shortcuts,
      Qt::ShortcutContext context,
      const QIcon &icon,
      bool enable = true,
      bool isEditable = true
      );

  private:
    /// Action name.
    QString m_name;
    /// Can be edited by the hotkey editor.
    bool m_isEditable;
};

} // namespace Actions
} // namespace FabricUI

#endif // __UI_BASE_ACTION__
