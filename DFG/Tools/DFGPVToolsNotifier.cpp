//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGPVToolsNotifier.h"
#include <FabricUI/Tools/ToolManager.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
#include <FabricUI/DFG/DFGBindingNotifier.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>
#include <FabricUI/Application/FabricApplicationStates.h>
#include <iostream>

using namespace FabricUI;
using namespace DFG;
using namespace Tools;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

// DFGPVToolsNotifierRegistry
DFGPVToolsNotifierRegistry::DFGPVToolsNotifierRegistry() 
{
}

DFGPVToolsNotifierRegistry::~DFGPVToolsNotifierRegistry() 
{
}

void DFGPVToolsNotifierRegistry::registerPathValueTool(
  RTVal pathValue)
{ 
  FABRIC_CATCH_BEGIN();

  DFGPathValueResolver *resolver = qobject_cast<DFGPathValueResolver *>(
    PathValueResolverRegistry::getRegistry()->getResolver(pathValue)
    );

  if(resolver)
  {
    DFGPathValueResolver::DFGType dfgType;
    DFGPVToolsNotifierPortPaths dfgPortPaths;
    DFGExec exec = resolver->getDFGPortPathsAndType(
      pathValue, 
      dfgPortPaths,
      dfgType
      );

    dfgPortPaths.oldPortName = dfgPortPaths.portName;
    dfgPortPaths.oldBlockName = dfgPortPaths.blockName;
    dfgPortPaths.oldNodeName = dfgPortPaths.nodeName;
 
    BaseDFGPVToolsNotifier *notifier = 0;
    if(dfgPortPaths.isExecArg())
      notifier = new DFGBindingPVToolsNotifier(this, dfgPortPaths, resolver->getDFGBinding());
    else
      notifier = new DFGExecPVToolsNotifier(this, dfgPortPaths, exec);
 
    m_registeredNotifiers.append(notifier);

    emit toolRegistered(dfgPortPaths.getFullItemPath());

    // Update the tool'value from its pathValue.
    setPathValueToolValue(notifier->getDFGPVToolsNotifierPortPaths());
  }
 
  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::registerPathValueTool");
}

void DFGPVToolsNotifierRegistry::unregisterAllPathValueTools()
{
  foreach(BaseDFGPVToolsNotifier *notifier, m_registeredNotifiers)
  {
    delete notifier;
    notifier = 0;
  }

  m_registeredNotifiers.clear();

  ToolManager::deleteAllPathValueTools(
    );  

  emit toolUpdated("");
}

void DFGPVToolsNotifierRegistry::unregisterPathValueTool(
  QString const& itemPath)
{
  FABRIC_CATCH_BEGIN();

  foreach(BaseDFGPVToolsNotifier *notifier, m_registeredNotifiers)
  {
    DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths notDFGPortPaths = notifier->getDFGPVToolsNotifierPortPaths();
    
    bool deletePathValueTool = notDFGPortPaths.getFullItemPath() == itemPath;     

    if(deletePathValueTool)
    {
      ToolManager::deletePathValueTool(
        itemPath
        );  

      m_registeredNotifiers.removeAll(notifier);
      delete notifier;
      notifier = 0;

      emit toolUpdated(itemPath);

      break;
    }
  }
 
  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::unregisterPathValueTool");
}

void DFGPVToolsNotifierRegistry::unregisterPathValueTool(
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPaths,
  bool fromNode)
{
  FABRIC_CATCH_BEGIN();
 
  if(dfgPortPaths.isExecArg())
    ToolManager::deletePathValueTool(
      dfgPortPaths.getFullItemPath()
      );   
 
  else
  {
    foreach(BaseDFGPVToolsNotifier *notifier, m_registeredNotifiers)
    {
      DFGPVToolsNotifierPortPaths notDFGPortPaths = notifier->getDFGPVToolsNotifierPortPaths();
      
      bool deletePathValueTool = fromNode
        ? notDFGPortPaths.getAbsoluteNodePath() == dfgPortPaths.getAbsoluteNodePath()
        : notDFGPortPaths.getFullItemPath() == dfgPortPaths.getFullItemPath();

      if(deletePathValueTool)
      {
        ToolManager::deletePathValueTool(
          notDFGPortPaths.getFullItemPath()
          );   

        m_registeredNotifiers.removeAll(notifier);
        delete notifier;
        notifier = 0;

        emit toolUpdated(notDFGPortPaths.getFullItemPath());

        if(!fromNode)
          break;
      }
    }
  }
 
  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::unregisterPathValueTool");
}

void DFGPVToolsNotifierRegistry::renamePathValueToolPath(
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPaths,
  bool fromNode)
{
  FABRIC_CATCH_BEGIN();
 
  if(dfgPortPaths.isExecArg())
    ToolManager::renamePathValueToolPath(
      dfgPortPaths.getOldFullItemPath(), 
      dfgPortPaths.getFullItemPath()
      );   
 
  else
  {
    foreach(BaseDFGPVToolsNotifier *notifier, m_registeredNotifiers)
    {
      DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths notDFGPortPaths = notifier->getDFGPVToolsNotifierPortPaths();
      
      bool renameTool = fromNode
        ? notDFGPortPaths.getOldAbsoluteNodePath() == dfgPortPaths.getOldAbsoluteNodePath()
        : notDFGPortPaths.getOldFullItemPath() == dfgPortPaths.getOldFullItemPath();
    
      if(renameTool)
        ToolManager::renamePathValueToolPath(
          notDFGPortPaths.getOldFullItemPath(), 
          notDFGPortPaths.getFullItemPath()
          );      
    }
  }

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::renamePathValueToolPath");
}

void DFGPVToolsNotifierRegistry::setPathValueToolValue(
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPaths)
{
  FABRIC_CATCH_BEGIN();
 
  ToolManager::setPathValueToolValue(
    dfgPortPaths.getFullItemPath()
    );
  
  emit toolUpdated(dfgPortPaths.getFullItemPath());

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::setPathValueToolValue");
}

DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths::DFGPVToolsNotifierPortPaths()
  : DFGPathValueResolver::DFGPortPaths()
{
}

QString DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths::getOldRelativePortPath() 
{
  if(isExecBlockPort())
    return oldNodeName + "." + oldBlockName + "." + oldPortName;
  else if(isExecArg())
    return oldPortName;
  else if(!oldNodeName.isEmpty())
    return oldNodeName + "." + oldPortName;
  else
    return "";
}

QString DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths::getOldAbsolutePortPath(
  bool addBindingID) 
{
  QString absPath = execPath.isEmpty()
    ? getOldRelativePortPath()
    : execPath + "." + getOldRelativePortPath();

  return addBindingID && !id.isEmpty()
    ? id + "." + absPath
    : absPath;
}

QString DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths::getOldFullItemPath(
  bool addBindingID)
{
  QString absPath = getOldAbsolutePortPath(addBindingID);
  return isArrayElement()
    ? absPath + "[" + QString::number(arrayIndex) + "]"
    : absPath;
}

QString DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths::getOldAbsoluteNodePath(
  bool addBindingID)
{
  if(!oldNodeName.isEmpty())
  {
    QString absPath = execPath.isEmpty()
      ? oldNodeName
      : execPath + "." + oldNodeName;

    return addBindingID && !id.isEmpty()
      ? id + "." + absPath
      : absPath;
  }
   
  return "";
}

// BaseDFGPVToolsNotifier
BaseDFGPVToolsNotifier::BaseDFGPVToolsNotifier( 
  DFGPVToolsNotifierRegistry *registry,
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPaths)
  : m_registry(registry)
  , m_dfgPortPaths(dfgPortPaths)
{
}

BaseDFGPVToolsNotifier::~BaseDFGPVToolsNotifier()
{
  m_notifier.clear();
}

DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths BaseDFGPVToolsNotifier::getDFGPVToolsNotifierPortPaths() const 
{ 
  return m_dfgPortPaths; 
};

// DFGBindingPVToolsNotifier
DFGBindingPVToolsNotifier_NotifProxy::DFGBindingPVToolsNotifier_NotifProxy(
  DFGBindingPVToolsNotifier *dst,
  QObject *parent)
  : QObject( parent )
  , m_dst( dst )
{
}

DFGBindingPVToolsNotifier_NotifProxy::~DFGBindingPVToolsNotifier_NotifProxy() 
{
}

DFGBindingPVToolsNotifier_BindingNotifProxy::DFGBindingPVToolsNotifier_BindingNotifProxy(
  DFGBindingPVToolsNotifier *dst,
  QObject *parent)
  : DFGBindingPVToolsNotifier_NotifProxy( dst, parent )
{
}

void DFGBindingPVToolsNotifier_BindingNotifProxy::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgValueChanged( index, name );
}

void DFGBindingPVToolsNotifier_BindingNotifProxy::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName)
{
  m_dst->onBindingArgRenamed( argIndex, oldArgName, newArgName );
}

void DFGBindingPVToolsNotifier_BindingNotifProxy::onBindingArgRemoved(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgRemoved( index, name );
}

void DFGBindingPVToolsNotifier_BindingNotifProxy::onBindingArgTypeChanged(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef newType)
{
  m_dst->onBindingArgTypeChanged( index, name, newType );
}

DFGBindingPVToolsNotifier::DFGBindingPVToolsNotifier(
  DFGPVToolsNotifierRegistry *registry,
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPaths,
  FabricCore::DFGBinding binding)
  : BaseDFGPVToolsNotifier(registry, dfgPortPaths)
{
  m_notifier.clear();
  m_notifier = DFGBindingNotifier::Create( binding );
  m_notifProxy = new DFGBindingPVToolsNotifier_BindingNotifProxy( this, this );
 
  connect(
    m_notifier.data(),
    SIGNAL( argRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
    m_notifProxy,
    SLOT( onBindingArgRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) )
    );

  connect(
    m_notifier.data(),
    SIGNAL( argRemoved( unsigned, FTL::CStrRef ) ),
    m_notifProxy,
    SLOT( onBindingArgRemoved( unsigned, FTL::CStrRef ) )
    );

  connect(
    m_notifier.data(),
    SIGNAL( argTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
    m_notifProxy,
    SLOT( onBindingArgTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) )
    );

  connect(
    m_notifier.data(),
    SIGNAL( argValueChanged( unsigned, FTL::CStrRef ) ),
    m_notifProxy,
    SLOT( onBindingArgValueChanged( unsigned, FTL::CStrRef ) )
    );
}

DFGBindingPVToolsNotifier::~DFGBindingPVToolsNotifier()
{
  m_notifProxy->setParent( NULL );
  delete m_notifProxy;
  m_notifProxy = NULL;
}
 
void DFGBindingPVToolsNotifier::onBindingArgTypeChanged( 
  unsigned index, 
  FTL::CStrRef name, 
  FTL::CStrRef newType)
{
  FABRIC_CATCH_BEGIN();

  m_dfgPortPaths.portName = name.data();
  m_registry->unregisterPathValueTool(m_dfgPortPaths);

  FABRIC_CATCH_END("DFGBindingPVToolsNotifier::onBindingArgTypeChanged");
}

void DFGBindingPVToolsNotifier::onBindingArgRemoved( 
  unsigned index, 
  FTL::CStrRef name)
{
  FABRIC_CATCH_BEGIN();

  m_dfgPortPaths.portName = name.data();
  m_registry->unregisterPathValueTool(m_dfgPortPaths);

  FABRIC_CATCH_END("DFGBindingPVToolsNotifier::onBindingArgRemoved");
}

void DFGBindingPVToolsNotifier::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName
  )
{
  FABRIC_CATCH_BEGIN();

  m_dfgPortPaths.portName = newArgName.data();
  m_dfgPortPaths.oldPortName = oldArgName.data();
  m_registry->renamePathValueToolPath(m_dfgPortPaths);

  FABRIC_CATCH_END("DFGBindingPVToolsNotifier::onBindingArgRenamed");
}

void DFGBindingPVToolsNotifier::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  FABRIC_CATCH_BEGIN();
 
  m_dfgPortPaths.portName = name.data();
  m_registry->setPathValueToolValue(m_dfgPortPaths);

  FABRIC_CATCH_END("DFGBindingPVToolsNotifier::onBindingArgValueChanged");
}

// DFGExecPVToolsNotifier
DFGExecPVToolsNotifier::DFGExecPVToolsNotifier(
  DFGPVToolsNotifierRegistry *registry,
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPaths,
  FabricCore::DFGExec exec)
  : BaseDFGPVToolsNotifier(registry, dfgPortPaths)
{
  m_notifier.clear();
  m_notifier = DFGExecNotifier::Create( exec );

  connect(
    m_notifier.data(),
    SIGNAL(nodeRenamed(FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodeRenamed(FTL::CStrRef, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(nodeRemoved(FTL::CStrRef)),
    this,
    SLOT(onExecNodeRemoved(FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(instBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(nodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortRenamed(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortRenamed(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortRemoved(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortRemoved(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(nodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(instBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(nodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(nodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );

  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );
}

DFGExecPVToolsNotifier::~DFGExecPVToolsNotifier()
{
}

void DFGExecPVToolsNotifier::onExecNodePortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->setPathValueToolValue(m_dfgPortPaths);
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onExecNodePortDefaultValuesChanged");
}

void DFGExecPVToolsNotifier::onInstBlockPortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->setPathValueToolValue(m_dfgPortPaths);
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onInstBlockPortDefaultValuesChanged");
}
 
void DFGExecPVToolsNotifier::onExecNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == portName.data())
    onExecNodePortRemoved(nodeName, 0, portName);
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onExecNodePortResolvedTypeChanged");
}

void DFGExecPVToolsNotifier::onInstBlockPortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName, 
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == portName.data())
    onInstBlockPortRemoved(nodeName, blockName, 0, portName);
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onInstBlockPortResolvedTypeChanged");
}
 
void DFGExecPVToolsNotifier::onExecNodePortRenamed(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == oldPortName.data())
  {
    m_dfgPortPaths.oldNodeName = nodeName.data();
    m_dfgPortPaths.oldPortName = oldPortName.data();
    m_dfgPortPaths.portName = newPortName.data();
    m_registry->renamePathValueToolPath(m_dfgPortPaths);
  }
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onExecNodePortRenamed");
}

void DFGExecPVToolsNotifier::onInstBlockPortRenamed(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == oldPortName.data())
  {
    m_dfgPortPaths.oldNodeName = nodeName.data();
    m_dfgPortPaths.oldBlockName = blockName.data();
    m_dfgPortPaths.oldPortName = oldPortName.data();
    m_dfgPortPaths.portName = newPortName.data();
    m_registry->renamePathValueToolPath(m_dfgPortPaths);
  }
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onInstBlockPortRenamed");
}
 
void DFGExecPVToolsNotifier::onExecNodePortRemoved(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->unregisterPathValueTool(m_dfgPortPaths);
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onExecNodePortRemoved");
}

void DFGExecPVToolsNotifier::onInstBlockPortRemoved(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->unregisterPathValueTool(m_dfgPortPaths);
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onInstBlockPortRemoved");
}

void DFGExecPVToolsNotifier::onExecNodeRemoved(
  FTL::CStrRef nodeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data())
    m_registry->unregisterPathValueTool(m_dfgPortPaths, true);
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onExecNodeRemoved");
}

void DFGExecPVToolsNotifier::onInstBlockRemoved(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data())
    m_registry->unregisterPathValueTool(m_dfgPortPaths, true);
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onInstBlockRemoved");
}

void DFGExecPVToolsNotifier::onExecNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == oldNodeName.data())
  {
    m_dfgPortPaths.oldPortName = m_dfgPortPaths.portName;
    m_dfgPortPaths.oldNodeName = oldNodeName.data();
    m_dfgPortPaths.nodeName = newNodeName.data();
    m_registry->renamePathValueToolPath(m_dfgPortPaths, true);
  }
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onExecNodeRenamed");
}

void DFGExecPVToolsNotifier::onInstBlockRenamed(
  FTL::CStrRef nodeName,
  FTL::CStrRef oldBlockName,
  FTL::CStrRef newBlockName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == oldBlockName.data())
  {
    m_dfgPortPaths.oldNodeName = nodeName.data();
    m_dfgPortPaths.oldBlockName = m_dfgPortPaths.blockName;
    m_dfgPortPaths.blockName = newBlockName.data();
    m_dfgPortPaths.oldPortName = m_dfgPortPaths.portName;
    m_registry->renamePathValueToolPath(m_dfgPortPaths, true);
  }
  FABRIC_CATCH_END("DFGExecPVToolsNotifier::onInstBlockRenamed");
}
