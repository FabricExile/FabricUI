//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_PV_TOOLS_NOTIFIER__
#define __UI_DFG_PV_TOOLS_NOTIFIER__

#include <QList>
#include <QString>
#include <QObject>
#include <QSharedPointer>
#include <FabricUI/DFG/DFGNotifier.h>
#include <FabricUI/DFG/Commands/DFGPathValueResolver.h>
 
namespace FabricUI {
namespace DFG {

class BaseDFGPVToolsNotifier;

class DFGPVToolsNotifierRegistry : public QObject
{
  Q_OBJECT

  public:
    DFGPVToolsNotifierRegistry();

    ~DFGPVToolsNotifierRegistry();

    void registerPathValueTool(
      FabricCore::RTVal pathValue
      );

    void unregisterPathValueTool(
      QString const& toolPath
      );
    
    struct DFGPVToolsNotifierPortPaths : public DFGPathValueResolver::DFGPortPaths
    {
      public:
        QString oldPortName; 
        QString oldBlockName;
        QString oldNodeName;

        DFGPVToolsNotifierPortPaths();
     
        QString getOldRelativePortPath();

        QString getOldAbsolutePortPath(
          bool addBindingID = true
          );

        QString getOldFullItemPath(
          bool addBindingID = true
          );
        
        QString getOldAbsoluteNodePath(
          bool addBindingID = true
          );
    };

    void unregisterPathValueTool(
      DFGPVToolsNotifierPortPaths dfgPortPath,
      bool fromNode = false
      );

    void unregisterAllPathValueTools();

    void renamePathValueToolPath(
      DFGPVToolsNotifierPortPaths dfgPortPath,
      bool fromNode = false
      );

    /// Update the value of the tool 
    /// associated to the `pathValue`.
    void setPathValueToolValue(
      DFGPVToolsNotifierPortPaths dfgPortPath
      );

  signals:
    void toolUpdated(
      QString const& toolPath
      );

    void toolRegistered(
      QString const& toolPath
      );

  private:
    QList<BaseDFGPVToolsNotifier *> m_registeredNotifiers;
};

class BaseDFGPVToolsNotifier : public QObject
{
  Q_OBJECT

  public:
    BaseDFGPVToolsNotifier( 
      DFGPVToolsNotifierRegistry *registry,
      DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPaths
      );

    ~BaseDFGPVToolsNotifier();

   DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths 
      getDFGPVToolsNotifierPortPaths() const;

  protected:
    DFGPVToolsNotifierRegistry *m_registry;
    QSharedPointer<DFG::DFGNotifier> m_notifier;
    DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths m_dfgPortPaths;
};

class DFGBindingPVToolsNotifier;

class DFGBindingPVToolsNotifier_NotifProxy : public QObject
{
  Q_OBJECT

  public:
    DFGBindingPVToolsNotifier_NotifProxy(
      DFGBindingPVToolsNotifier *dst,
      QObject *parent
      );

    virtual ~DFGBindingPVToolsNotifier_NotifProxy();

  protected:
    DFGBindingPVToolsNotifier *m_dst;
};

class DFGBindingPVToolsNotifier_BindingNotifProxy :
  public DFGBindingPVToolsNotifier_NotifProxy
{
  Q_OBJECT

  public:
    DFGBindingPVToolsNotifier_BindingNotifProxy(
      DFGBindingPVToolsNotifier *dst,
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

class DFGBindingPVToolsNotifier : public BaseDFGPVToolsNotifier
{
  Q_OBJECT

  public:
    DFGBindingPVToolsNotifier( 
      DFGPVToolsNotifierRegistry *registry,
      DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPaths,
      FabricCore::DFGBinding binding
      );

    ~DFGBindingPVToolsNotifier();
 
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

  private:
    DFGBindingPVToolsNotifier_NotifProxy *m_notifProxy;
};

class DFGExecPVToolsNotifier : public BaseDFGPVToolsNotifier
{
  Q_OBJECT

  public:
    DFGExecPVToolsNotifier( 
      DFGPVToolsNotifierRegistry *registry,
      DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPaths,
      FabricCore::DFGExec exec
      );

    ~DFGExecPVToolsNotifier();
   
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
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_PV_TOOLS_NOTIFIER__
