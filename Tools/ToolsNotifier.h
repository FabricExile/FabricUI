//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_TOOLS_NOTIFIER__
#define __UI_TOOLS_NOTIFIER__

#include <QList>
#include <QString>
#include <QObject>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGNotifier.h>
#include <FabricUI/ValueEditor/VEEditorOwner.h>
 
namespace FabricUI {
namespace Tools {

class ToolsNotifierRegistry;

class ToolsNotifierRegistry_NotifProxy : public QObject
{
  public:
    ToolsNotifierRegistry_NotifProxy(
      ToolsNotifierRegistry *dst,
      QObject *parent
      );

    virtual ~ToolsNotifierRegistry_NotifProxy();

  protected:
    ToolsNotifierRegistry *m_dst;
};

class ToolsNotifierRegistry_BindingNotifProxy :
  public ToolsNotifierRegistry_NotifProxy
{
  Q_OBJECT

  public:
    ToolsNotifierRegistry_BindingNotifProxy(
      ToolsNotifierRegistry *dst,
      QObject *parent
      );

  public slots:
    void onBindingArgValueChanged(
      unsigned index,
      FTL::CStrRef name
      );
 
    void onBindingArgRenamed(
      unsigned argIndex,
      FTL::CStrRef oldArgName,
      FTL::CStrRef newArgName
      );

    void onBindingArgRemoved(
      unsigned index,
      FTL::CStrRef name
      );

    void onBindingArgTypeChanged(
      unsigned index,
      FTL::CStrRef name,
      FTL::CStrRef newType
      );
};

class ToolsNotifier;

class ToolsNotifierRegistry : public QObject
{
  Q_OBJECT

  public:
    ToolsNotifierRegistry( 
      DFG::DFGWidget *dfgWidget 
      );

    ~ToolsNotifierRegistry();

    DFG::DFGWidget *getDFGWidget();

  public slots:
    void onBindingArgValueChanged( 
      unsigned index, 
      FTL::CStrRef name 
      );

    void onBindingArgRenamed(
      unsigned argIndex,
      FTL::CStrRef oldArgName,
      FTL::CStrRef newArgName
      );

    void onBindingArgRemoved(
      unsigned index,
      FTL::CStrRef name
      );

    void onBindingArgTypeChanged(
      unsigned index,
      FTL::CStrRef name,
      FTL::CStrRef newType
      );

    void initConnections();

    void createPathValueTool(
      QString const&targetPath
      );

    void createPathValueTool(
      FabricCore::RTVal pathValue
      );

    void deletePathValueTool(
      QString const&targetPath
      );

    void toolValueChanged(
      QString const&targetPath
      );

  private slots:
    void onControllerBindingChanged(
      FabricCore::DFGBinding const&binding
      );

  private:
    /// Update the value of the tool 
    /// associated to the `pathValue`.
    void toolValueChanged(
      FabricCore::RTVal pathValue
      );

    void deletePathValueTool(
      FabricCore::RTVal pathValue
      );

    /// Gets the KL tools manager.
    FabricCore::RTVal getKLToolManager();

    FabricCore::RTVal pathToPathValue(
      QString const&targetPath
      );

    void setupConnections(
      FabricUI::DFG::DFGController *dfgController
      );

    DFG::DFGWidget *m_dfgWidget;
    QSharedPointer<DFG::DFGNotifier> m_notifier;
    QList<ToolsNotifier *> m_registeredNotifiers;
    ToolsNotifierRegistry_NotifProxy *m_notifProxy;
};

class ToolsNotifier : public QObject
{
  Q_OBJECT

  public:
    ToolsNotifier( 
      ToolsNotifierRegistry *registry,
      FabricCore::RTVal pathValue
      );

    ~ToolsNotifier();

    QString getToolTargetPath();

  protected slots:
    void onExecNodePortRenamed(
      FTL::CStrRef nodeName,
      unsigned portIndex,
      FTL::CStrRef oldNodePortName,
      FTL::CStrRef newNodePortName
      );

    void onExecNodePortRemoved(
      FTL::CStrRef nodeName,
      unsigned portIndex,
      FTL::CStrRef portName
      );

    void onExecNodeRemoved(
      FTL::CStrRef nodeName
      );

    void onExecNodeRenamed(
      FTL::CStrRef oldNodeName,
      FTL::CStrRef newNodeName
      );

    void onExecNodePortDefaultValuesChanged(
      FTL::CStrRef nodeName,
      FTL::CStrRef portName
      );

    void onExecNodePortResolvedTypeChanged(
      FTL::CStrRef nodeName,
      FTL::CStrRef portName,
      FTL::CStrRef newResolvedTypeName
      );
 
  private:
    void setupConnections(
      FabricCore::DFGExec exec
      );

    QString m_execPath;
    QString m_toolTargetPath;
    ToolsNotifierRegistry *m_registry;
    QSharedPointer<DFG::DFGNotifier> m_notifier;
};

} // namespace Tools
} // namespace FabricUI

#endif // __UI_TOOLS_NOTIFIER__