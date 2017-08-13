//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_PV_TOOLS_NOTIFIER__
#define __UI_DFG_PV_TOOLS_NOTIFIER__

#include <QList>
#include <QString>
#include <QObject>
#include <FabricUI/DFG/DFGNotifier.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/Commands/DFGPathValueResolver.h>
 
namespace FabricUI {
namespace DFG {

class DFGPVToolsNotifierRegistry;

class DFGPVToolsNotifierRegistry_NotifProxy : public QObject
{
  public:
    DFGPVToolsNotifierRegistry_NotifProxy(
      DFGPVToolsNotifierRegistry *dst,
      QObject *parent
      );

    virtual ~DFGPVToolsNotifierRegistry_NotifProxy();

  protected:
    DFGPVToolsNotifierRegistry *m_dst;
};

class DFGPVToolsNotifierRegistry_BindingNotifProxy :
  public DFGPVToolsNotifierRegistry_NotifProxy
{
  Q_OBJECT

  public:
    DFGPVToolsNotifierRegistry_BindingNotifProxy(
      DFGPVToolsNotifierRegistry *dst,
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

class DFGPVToolsNotifier;

struct DFGPVToolsNotifierPortPaths : public DFG::DFGPathValueResolver::DFGPortPaths
{
  public:
    QString oldPortName; 
    QString oldBlockName;
    QString oldNodeName;

    DFGPVToolsNotifierPortPaths::DFGPVToolsNotifierPortPaths()
      : DFG::DFGPathValueResolver::DFGPortPaths()
    {
    }
 
    QString getOldRelativePortPath() {
      if(isExecBlockPort())
        return oldNodeName + "." + oldBlockName + "." + oldPortName;
      else if(isExecArg())
        return oldPortName;
      else if(!oldNodeName.isEmpty())
        return oldNodeName + "." + oldPortName;
      else
        return "";
    }

    QString getOldAbsolutePortPath() {
     return execPath.isEmpty()
      ? getOldRelativePortPath()
      : execPath + "." + getOldRelativePortPath();
    }

    QString getOldAbsoluteNodePath() {
     if(!oldNodeName.isEmpty())
        return execPath.isEmpty()
          ? oldNodeName
          : execPath + "." + oldNodeName;
      return "";;
    }
};
 
class DFGPVToolsNotifierRegistry : public QObject
{
  Q_OBJECT

  public:
    DFGPVToolsNotifierRegistry( 
      DFG::DFGController *dfgContoller 
      );

    ~DFGPVToolsNotifierRegistry();

    void initConnections();

    void registerPathValueTool(
      FabricCore::RTVal pathValue
      );
 
    void deletePathValueTool(
      DFGPVToolsNotifierPortPaths dfgPortPath,
      bool fromNode = false
      );

    void changedNotifiedToolPath(
      DFGPVToolsNotifierPortPaths dfgPortPath,
      bool fromNode = false
      );

    /// Update the value of the tool 
    /// associated to the `pathValue`.
    void toolValueChanged(
      DFGPVToolsNotifierPortPaths dfgPortPath
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

  signals:
    void toolUpdated();

  private slots:
    void onControllerBindingChanged(
      FabricCore::DFGBinding const&binding
      );

  private:
    /// Gets the KL tools manager.
    FabricCore::RTVal getKLToolManager();

    FabricCore::RTVal pathToPathValue(
      QString const&toolPath
      );

    void setupConnections(
      FabricUI::DFG::DFGController *dfgController
      );

    DFG::DFGController *m_dfgContoller;
    QSharedPointer<DFG::DFGNotifier> m_notifier;
    QList<DFGPVToolsNotifier *> m_registeredNotifiers;
    DFGPVToolsNotifierRegistry_NotifProxy *m_notifProxy;
};

class DFGPVToolsNotifier : public QObject
{
  Q_OBJECT

  public:
    DFGPVToolsNotifier( 
      DFGPVToolsNotifierRegistry *registry,
      FabricCore::RTVal pathValue
      );

    ~DFGPVToolsNotifier();
   
    DFGPVToolsNotifierPortPaths getDFGPVToolsNotifierPortPaths() const;
  
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

    void onInstBlockPortRemoved(
      FTL::CStrRef nodeName,
      FTL::CStrRef blockName,
      unsigned portIndex,
      FTL::CStrRef portName
      );

    void onExecNodeRemoved(
      FTL::CStrRef nodeName
      );

    void onInstBlockRemoved(
      FTL::CStrRef nodeName,
      FTL::CStrRef blockName
      );

    void onExecNodeRenamed(
      FTL::CStrRef oldNodeName,
      FTL::CStrRef newNodeName
      );

    void onInstBlockRenamed(
      FTL::CStrRef nodeName,
      FTL::CStrRef oldBlockName,
      FTL::CStrRef newBlockName
      );

    void onInstBlockPortRenamed(
      FTL::CStrRef nodeName,
      FTL::CStrRef blockName,
      unsigned portIndex,
      FTL::CStrRef oldPortName,
      FTL::CStrRef newPortName
      );

    void onExecNodePortDefaultValuesChanged(
      FTL::CStrRef nodeName,
      FTL::CStrRef portName
      );

    void onInstBlockPortDefaultValuesChanged(
      FTL::CStrRef nodeName,
      FTL::CStrRef blockName,
      FTL::CStrRef portName
      );

    void onExecNodePortResolvedTypeChanged(
      FTL::CStrRef nodeName,
      FTL::CStrRef portName,
      FTL::CStrRef newResolvedTypeName
      );
    
    void onInstBlockPortResolvedTypeChanged(
      FTL::CStrRef nodeName,
      FTL::CStrRef blockName,
      FTL::CStrRef portName,
      FTL::CStrRef newResolvedTypeName
      );
 
  private:
    void setupConnections(
      FabricCore::DFGExec exec
      );

    DFGPVToolsNotifierRegistry *m_registry;
    DFGPVToolsNotifierPortPaths m_dfgPortPaths;
    QSharedPointer<DFG::DFGNotifier> m_notifier;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_PV_TOOLS_NOTIFIER__
