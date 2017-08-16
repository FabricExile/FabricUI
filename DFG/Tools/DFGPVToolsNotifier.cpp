//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGPVToolsNotifier.h"
#include <FabricUI/Tools/ToolManager.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Tools;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

DFGPVToolsNotifierRegistry::DFGPVToolsNotifierRegistry( 
  DFGController *dfgContoller)
  : m_dfgContoller(dfgContoller)
  , m_notifProxy( NULL )
{
}

DFGPVToolsNotifierRegistry::~DFGPVToolsNotifierRegistry()
{
  foreach(DFGPVToolsNotifier *notifier, m_registeredNotifiers)
  {
    m_registeredNotifiers.removeAll(notifier);
    delete notifier;
    notifier = 0;
  }
}

void DFGPVToolsNotifierRegistry::initConnections()
{
  connect(
    m_dfgContoller,
    SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
    this,
    SLOT(onControllerBindingChanged(FabricCore::DFGBinding const &))
    );
 
  setupConnections(m_dfgContoller);
}
 
void DFGPVToolsNotifierRegistry::onControllerBindingChanged(
  FabricCore::DFGBinding const &newBinding)
{
  setupConnections(m_dfgContoller);
}
 
void DFGPVToolsNotifierRegistry::setupConnections(
  DFGController *dfgController)
{
  if ( m_notifProxy )
  {
    m_notifProxy->setParent( NULL );
    delete m_notifProxy;
    m_notifProxy = NULL;
  }

  m_notifier.clear();
  m_notifier = m_dfgContoller->getBindingNotifier();

  DFGPVToolsNotifierRegistry_BindingNotifProxy *notifProxy =
    new DFGPVToolsNotifierRegistry_BindingNotifProxy( this, this );
  m_notifProxy = notifProxy;

  connect(
    m_notifier.data(),
    SIGNAL( argRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
    notifProxy,
    SLOT( onBindingArgRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) )
    );
  connect(
    m_notifier.data(),
    SIGNAL( argRemoved( unsigned, FTL::CStrRef ) ),
    notifProxy,
    SLOT( onBindingArgRemoved( unsigned, FTL::CStrRef ) )
    );
  connect(
    m_notifier.data(),
    SIGNAL( argTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
    notifProxy,
    SLOT( onBindingArgTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) )
    );
  connect(
    m_notifier.data(),
    SIGNAL( argValueChanged( unsigned, FTL::CStrRef ) ),
    notifProxy,
    SLOT( onBindingArgValueChanged( unsigned, FTL::CStrRef ) )
    );
}

void DFGPVToolsNotifierRegistry::onBindingArgTypeChanged( 
  unsigned index, 
  FTL::CStrRef name, 
  FTL::CStrRef newType)
{
  FABRIC_CATCH_BEGIN();

  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = name.data();
  unregisterPathValueTool(dfgPortPath);

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::onBindingArgTypeChanged");
}

void DFGPVToolsNotifierRegistry::onBindingArgRemoved( 
  unsigned index, 
  FTL::CStrRef name)
{
  FABRIC_CATCH_BEGIN();

  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = name.data();
  unregisterPathValueTool(dfgPortPath);

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::onBindingArgRemoved");
}

void DFGPVToolsNotifierRegistry::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName
  )
{
  FABRIC_CATCH_BEGIN();

  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = newArgName.data();
  dfgPortPath.oldPortName = oldArgName.data();
  renamePathValueToolPath(dfgPortPath);

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::onBindingArgRenamed");
}

void DFGPVToolsNotifierRegistry::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  FABRIC_CATCH_BEGIN();
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = name.data();
  setPathValueToolValue(dfgPortPath);
  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::onBindingArgValueChanged");
}

void DFGPVToolsNotifierRegistry::registerPathValueTool(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
 
  DFGPVToolsNotifier *notifier = new DFGPVToolsNotifier(
    this,
    pathValue);

  m_registeredNotifiers.append(notifier);

  // Update the tool'value from its pathValue.
  setPathValueToolValue(notifier->getDFGPVToolsNotifierPortPaths());
  
  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::registerPathValueTool");
}

void DFGPVToolsNotifierRegistry::unregisterAllPathValueTools()
{
  foreach(DFGPVToolsNotifier *notifier, m_registeredNotifiers)
  {
    DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths notDFGPortPath = notifier->getDFGPVToolsNotifierPortPaths();

    ToolManager::deletePathValueTool(
      notDFGPortPath.getAbsolutePortPath()
      );  

    m_registeredNotifiers.removeAll(notifier);
    delete notifier;
    notifier = 0;
  }

  emit toolUpdated();
}

void DFGPVToolsNotifierRegistry::unregisterPathValueTool(
  QString const& itemPath)
{
  FABRIC_CATCH_BEGIN();
 
  foreach(DFGPVToolsNotifier *notifier, m_registeredNotifiers)
  {
    DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths notDFGPortPath = notifier->getDFGPVToolsNotifierPortPaths();
    
    bool deletePathValueTool = notDFGPortPath.getAbsolutePortPath() == itemPath;     

    if(deletePathValueTool)
    {
      ToolManager::deletePathValueTool(
        itemPath
        );  

      m_registeredNotifiers.removeAll(notifier);
      delete notifier;
      notifier = 0;
    }
  }

  emit toolUpdated();
 
  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::unregisterPathValueTool");
}

void DFGPVToolsNotifierRegistry::unregisterPathValueTool(
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPath,
  bool fromNode)
{
  FABRIC_CATCH_BEGIN();
 
  if(dfgPortPath.isExecArg())
    ToolManager::deletePathValueTool(
      dfgPortPath.getAbsolutePortPath()
      );   
 
  else
  {
    foreach(DFGPVToolsNotifier *notifier, m_registeredNotifiers)
    {
      DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths notDFGPortPath = notifier->getDFGPVToolsNotifierPortPaths();
      
      bool deletePathValueTool = fromNode
        ? notDFGPortPath.getAbsoluteNodePath() == dfgPortPath.getAbsoluteNodePath()
        : notDFGPortPath.getAbsolutePortPath() == dfgPortPath.getAbsolutePortPath();

      if(deletePathValueTool)
      {
        ToolManager::deletePathValueTool(
          notDFGPortPath.getAbsolutePortPath()
          );   

        m_registeredNotifiers.removeAll(notifier);
        delete notifier;
        notifier = 0;
      }
    }
  }
 
  emit toolUpdated();

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::unregisterPathValueTool");
}

void DFGPVToolsNotifierRegistry::renamePathValueToolPath(
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPath,
  bool fromNode)
{
  FABRIC_CATCH_BEGIN();
 
  if(dfgPortPath.isExecArg())
    ToolManager::renamePathValueToolPath(
      dfgPortPath.getOldAbsolutePortPath(), 
      dfgPortPath.getAbsolutePortPath()
      );   
 
  else
  {
    foreach(DFGPVToolsNotifier *notifier, m_registeredNotifiers)
    {
      DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths notDFGPortPath = notifier->getDFGPVToolsNotifierPortPaths();
      
      bool renameTool = fromNode
        ? notDFGPortPath.getOldAbsoluteNodePath() == dfgPortPath.getOldAbsoluteNodePath()
        : notDFGPortPath.getOldAbsolutePortPath() == dfgPortPath.getOldAbsolutePortPath();
    
      if(renameTool)
        ToolManager::renamePathValueToolPath(
          notDFGPortPath.getOldAbsolutePortPath(), 
          notDFGPortPath.getAbsolutePortPath()
          );      
    }
  }

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::renamePathValueToolPath");
}

void DFGPVToolsNotifierRegistry::setPathValueToolValue(
  DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths dfgPortPath)
{
  FABRIC_CATCH_BEGIN();
 
  ToolManager::setPathValueToolValue(
    dfgPortPath.getAbsolutePortPath()
    );
  
  emit toolUpdated();

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

QString DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths::getOldAbsolutePortPath() 
{
 return execPath.isEmpty()
  ? getOldRelativePortPath()
  : execPath + "." + getOldRelativePortPath();
}

QString DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths::getOldAbsoluteNodePath() 
{
 if(!oldNodeName.isEmpty())
    return execPath.isEmpty()
      ? oldNodeName
      : execPath + "." + oldNodeName;
  return "";;
}

DFGPVToolsNotifierRegistry_NotifProxy::DFGPVToolsNotifierRegistry_NotifProxy(
  DFGPVToolsNotifierRegistry *dst,
  QObject *parent)
  : QObject( parent )
  , m_dst( dst )
{
}

DFGPVToolsNotifierRegistry_NotifProxy::~DFGPVToolsNotifierRegistry_NotifProxy() 
{
}

DFGPVToolsNotifierRegistry_BindingNotifProxy::DFGPVToolsNotifierRegistry_BindingNotifProxy(
  DFGPVToolsNotifierRegistry *dst,
  QObject *parent)
  : DFGPVToolsNotifierRegistry_NotifProxy( dst, parent )
{
}

void DFGPVToolsNotifierRegistry_BindingNotifProxy::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgValueChanged( index, name );
}

void DFGPVToolsNotifierRegistry_BindingNotifProxy::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName)
{
  m_dst->onBindingArgRenamed( argIndex, oldArgName, newArgName );
}

void DFGPVToolsNotifierRegistry_BindingNotifProxy::onBindingArgRemoved(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgRemoved( index, name );
}

void DFGPVToolsNotifierRegistry_BindingNotifProxy::onBindingArgTypeChanged(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef newType)
{
  m_dst->onBindingArgTypeChanged( index, name, newType );
}

DFGPVToolsNotifier::DFGPVToolsNotifier( 
  DFGPVToolsNotifierRegistry *registry,
  RTVal pathValue)
  : m_registry(registry)
{
  FABRIC_CATCH_BEGIN();

  DFGPathValueResolver *resolver = qobject_cast<DFGPathValueResolver *>(
    PathValueResolverRegistry::getRegistry()->getResolver(pathValue)
    );

  if(resolver)
  {
    DFGExec exec = resolver->getDFGPortPaths(
      pathValue, 
      m_dfgPortPaths
      );

    m_dfgPortPaths.oldPortName = m_dfgPortPaths.portName;
    m_dfgPortPaths.oldBlockName = m_dfgPortPaths.blockName;
    m_dfgPortPaths.oldNodeName = m_dfgPortPaths.nodeName;

    setupConnections(exec);
  }

  FABRIC_CATCH_END("DFGPVToolsNotifier::DFGPVToolsNotifier");
}

DFGPVToolsNotifier::~DFGPVToolsNotifier()
{
  // ToolManager::deletePathValueTool(
  //   m_dfgPortPaths.getAbsolutePortPath()
  //   ); 

  m_notifier.clear();
}

DFGPVToolsNotifierRegistry::DFGPVToolsNotifierPortPaths DFGPVToolsNotifier::getDFGPVToolsNotifierPortPaths() const 
{ 
  return m_dfgPortPaths; 
};

void DFGPVToolsNotifier::setupConnections(
  FabricCore::DFGExec exec)
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

void DFGPVToolsNotifier::onExecNodePortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->setPathValueToolValue(m_dfgPortPaths);
  FABRIC_CATCH_END("DFGPVToolsNotifier::onExecNodePortDefaultValuesChanged");
}

void DFGPVToolsNotifier::onInstBlockPortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->setPathValueToolValue(m_dfgPortPaths);
  FABRIC_CATCH_END("DFGPVToolsNotifier::onExecNodePortDefaultValuesChanged");
}
 
void DFGPVToolsNotifier::onExecNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == portName.data())
    onExecNodePortRemoved(nodeName, 0, portName);
  FABRIC_CATCH_END("DFGPVToolsNotifier::onExecNodePortResolvedTypeChanged");
}

void DFGPVToolsNotifier::onInstBlockPortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName, 
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == portName.data())
    onInstBlockPortRemoved(nodeName, blockName, 0, portName);
  FABRIC_CATCH_END("DFGPVToolsNotifier::onInstBlockPortResolvedTypeChanged");
}
 
void DFGPVToolsNotifier::onExecNodePortRenamed(
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
  FABRIC_CATCH_END("DFGPVToolsNotifier::onExecNodePortRenamed");
}

void DFGPVToolsNotifier::onInstBlockPortRenamed(
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
  FABRIC_CATCH_END("DFGPVToolsNotifier::onInstBlockPortRenamed");
}
 
void DFGPVToolsNotifier::onExecNodePortRemoved(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->unregisterPathValueTool(m_dfgPortPaths);
  FABRIC_CATCH_END("DFGPVToolsNotifier::onExecNodePortRemoved");
}

void DFGPVToolsNotifier::onInstBlockPortRemoved(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->unregisterPathValueTool(m_dfgPortPaths);
  FABRIC_CATCH_END("DFGPVToolsNotifier::onInstBlockPortRemoved");
}

void DFGPVToolsNotifier::onExecNodeRemoved(
  FTL::CStrRef nodeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data())
    m_registry->unregisterPathValueTool(m_dfgPortPaths, true);
  FABRIC_CATCH_END("DFGPVToolsNotifier::onExecNodeRemoved");
}

void DFGPVToolsNotifier::onInstBlockRemoved(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data())
    m_registry->unregisterPathValueTool(m_dfgPortPaths, true);
  FABRIC_CATCH_END("DFGPVToolsNotifier::onInstBlockRemoved");
}

void DFGPVToolsNotifier::onExecNodeRenamed(
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
  FABRIC_CATCH_END("DFGPVToolsNotifier::onExecNodeRenamed");
}

void DFGPVToolsNotifier::onInstBlockRenamed(
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
  FABRIC_CATCH_END("DFGPVToolsNotifier::onInstBlockRenamed");
}
