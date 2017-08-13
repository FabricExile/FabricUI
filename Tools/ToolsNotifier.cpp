//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <iostream>
#include "ToolsNotifier.h"
#include "PathValueTool.h"
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
  std::cout 
    << "\nDFGPVToolsNotifierRegistry::~DFGPVToolsNotifierRegistry " 
    << std::endl;

  foreach(DFGPVToolsNotifier *notifier, m_registeredNotifiers)
  {
    m_registeredNotifiers.removeAll(notifier);
    delete notifier;
    notifier = 0;
  }
}

void DFGPVToolsNotifierRegistry::initConnections()
{
  std::cout << "\nDFGPVToolsNotifierRegistry::initConnections" << std::endl;
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
  std::cout << "\nDFGPVToolsNotifierRegistry::onBindingArgTypeChanged" << std::endl;

  DFGPVToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = name.data();
  deletePathValueTool(dfgPortPath);

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::onBindingArgTypeChanged");
}

void DFGPVToolsNotifierRegistry::onBindingArgRemoved( 
  unsigned index, 
  FTL::CStrRef name)
{
  FABRIC_CATCH_BEGIN();
  std::cout << "\nDFGPVToolsNotifierRegistry::onBindingArgRemoved" << std::endl;

  DFGPVToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = name.data();
  deletePathValueTool(dfgPortPath);

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::onBindingArgRemoved");
}

void DFGPVToolsNotifierRegistry::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName
  )
{
  FABRIC_CATCH_BEGIN();
  std::cout << "\nDFGPVToolsNotifierRegistry::onBindingArgRenamed" << std::endl;

  DFGPVToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = newArgName.data();
  dfgPortPath.oldPortName = oldArgName.data();
  changedNotifiedToolPath(dfgPortPath);

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::onBindingArgRenamed");
}

void DFGPVToolsNotifierRegistry::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  FABRIC_CATCH_BEGIN();
  std::cout << "\nDFGPVToolsNotifierRegistry::onBindingArgValueChanged" << std::endl;
  DFGPVToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = name.data();
  toolValueChanged(dfgPortPath);
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
  toolValueChanged(notifier->getDFGPVToolsNotifierPortPaths());
  
  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::registerPathValueTool");
}

void DFGPVToolsNotifierRegistry::deletePathValueTool(
  DFGPVToolsNotifierPortPaths dfgPortPath,
  bool fromNode)
{
  FABRIC_CATCH_BEGIN();
 
  std::cout 
    << "\nDFGPVToolsNotifierRegistry::deletePathValueTool " 
    << " getAbsolutePortPath "
    << dfgPortPath.getAbsolutePortPath().toUtf8().constData() 
    << " fromNode "
    << fromNode
    << std::endl;

  if(dfgPortPath.isExecArg())
    PathValueTool::deleteTool(
      dfgPortPath.getAbsolutePortPath()
      );   
 
  else
  {
    foreach(DFGPVToolsNotifier *notifier, m_registeredNotifiers)
    {
      DFGPVToolsNotifierPortPaths notDFGPortPath = notifier->getDFGPVToolsNotifierPortPaths();
      
      bool deleteTool = fromNode
        ? notDFGPortPath.getAbsoluteNodePath() == dfgPortPath.getAbsoluteNodePath()
        : notDFGPortPath.getAbsolutePortPath() == dfgPortPath.getAbsolutePortPath();
  
       std::cout 
        << "notDFGPortPath.getAbsolutePortPath "
        << notDFGPortPath.getAbsolutePortPath().toUtf8().constData()
        << "notDFGPortPath.getAbsoluteNodePath "
        << notDFGPortPath.getAbsoluteNodePath().toUtf8().constData()
        << " deleteTool "
        << deleteTool
        << std::endl;

      if(deleteTool)
      {
        m_registeredNotifiers.removeAll(notifier);
        delete notifier;
        notifier = 0;
      }
    }
  }
 
  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::deletePathValueTool");
}

void DFGPVToolsNotifierRegistry::changedNotifiedToolPath(
  DFGPVToolsNotifierPortPaths dfgPortPath,
  bool fromNode)
{
  FABRIC_CATCH_BEGIN();
 
  std::cout 
    << "DFGPVToolsNotifierRegistry::changedNotifiedToolPath " 
    << " getAbsolutePortPath "
    << dfgPortPath.getAbsolutePortPath().toUtf8().constData() 
    << " getOldAbsolutePortPath "
    << dfgPortPath.getOldAbsolutePortPath().toUtf8().constData() 
    << std::endl;

  if(dfgPortPath.isExecArg())
    PathValueTool::renameTool(
      dfgPortPath.getOldAbsolutePortPath(), 
      dfgPortPath.getAbsolutePortPath()
      );   
 
  else
  {
    foreach(DFGPVToolsNotifier *notifier, m_registeredNotifiers)
    {
      DFGPVToolsNotifierPortPaths notDFGPortPath = notifier->getDFGPVToolsNotifierPortPaths();
      
      bool renameTool = fromNode
        ? notDFGPortPath.getOldAbsoluteNodePath() == dfgPortPath.getOldAbsoluteNodePath()
        : notDFGPortPath.getOldAbsolutePortPath() == dfgPortPath.getOldAbsolutePortPath();
    
       std::cout 
        << "notDFGPortPath.getAbsolutePortPath "
        << notDFGPortPath.getAbsolutePortPath().toUtf8().constData() 
        << " notDFGPortPath.getAbsoluteNodePath "
        << notDFGPortPath.getAbsoluteNodePath().toUtf8().constData() 
        << " renameTool "
        << renameTool
        << std::endl;

      if(renameTool)
        PathValueTool::renameTool(
          notDFGPortPath.getOldAbsolutePortPath(), 
          notDFGPortPath.getAbsolutePortPath()
          );      
    }
  }

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::changedNotifiedToolPath");
}

void DFGPVToolsNotifierRegistry::toolValueChanged(
  DFGPVToolsNotifierPortPaths dfgPortPath)
{
  FABRIC_CATCH_BEGIN();
 
  PathValueTool::toolValueChanged(
    dfgPortPath.getAbsolutePortPath()
    );
  
  emit toolUpdated();

  FABRIC_CATCH_END("DFGPVToolsNotifierRegistry::toolValueChanged");
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
  std::cout 
    << "\nDFGPVToolsNotifierRegistry::~DFGPVToolsNotifier " 
    << " getAbsolutePortPath "
    << m_dfgPortPaths.getAbsolutePortPath().toUtf8().constData() 
    << std::endl;

  PathValueTool::deleteTool(
    m_dfgPortPaths.getAbsolutePortPath()
    ); 

  m_notifier.clear();
}

DFGPVToolsNotifierPortPaths DFGPVToolsNotifier::getDFGPVToolsNotifierPortPaths() const 
{ 
  return m_dfgPortPaths; 
};

void DFGPVToolsNotifier::setupConnections(
  FabricCore::DFGExec exec)
{
  std::cout << "\nDFGPVToolsNotifier::setupConnections 2" << std::endl;

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
    m_registry->toolValueChanged(m_dfgPortPaths);
  FABRIC_CATCH_END("DFGPVToolsNotifier::onExecNodePortDefaultValuesChanged");
}

void DFGPVToolsNotifier::onInstBlockPortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->toolValueChanged(m_dfgPortPaths);
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
    std::cout << "\nDFGPVToolsNotifier::onExecNodePortRenamed 1" << std::endl;
    m_dfgPortPaths.oldNodeName = nodeName.data();
    m_dfgPortPaths.oldPortName = oldPortName.data();
    m_dfgPortPaths.portName = newPortName.data();
    m_registry->changedNotifiedToolPath(m_dfgPortPaths);
    std::cout << "DFGPVToolsNotifier::onExecNodePortRenamed 2" << std::endl;
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
    std::cout << "\nDFGPVToolsNotifier::onInstBlockPortRenamed 1" << std::endl;
    m_dfgPortPaths.oldNodeName = nodeName.data();
    m_dfgPortPaths.oldBlockName = blockName.data();
    m_dfgPortPaths.oldPortName = oldPortName.data();
    m_dfgPortPaths.portName = newPortName.data();
    m_registry->changedNotifiedToolPath(m_dfgPortPaths);
    std::cout << "DFGPVToolsNotifier::onInstBlockPortRenamed 2" << std::endl;
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
  {
    std::cout << "\nDFGPVToolsNotifier::onExecNodePortRemoved 1" << std::endl;
    m_registry->deletePathValueTool(m_dfgPortPaths);
    std::cout << "DFGPVToolsNotifier::onExecNodePortRemoved 2" << std::endl;
  }
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
  {
    std::cout << "\nDFGPVToolsNotifier::onInstBlockPortRemoved 1" << std::endl;
    m_registry->deletePathValueTool(m_dfgPortPaths);
    std::cout << "DFGPVToolsNotifier::onInstBlockPortRemoved 2" << std::endl;
  }
  FABRIC_CATCH_END("DFGPVToolsNotifier::onInstBlockPortRemoved");
}

void DFGPVToolsNotifier::onExecNodeRemoved(
  FTL::CStrRef nodeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data())
  {
    std::cout << "\nDFGPVToolsNotifier::onExecNodeRemoved 1 " << std::endl;
    m_registry->deletePathValueTool(m_dfgPortPaths, true);
    std::cout << "DFGPVToolsNotifier::onExecNodeRemoved 2 " << std::endl;
  }
  FABRIC_CATCH_END("DFGPVToolsNotifier::onExecNodeRemoved");
}

void DFGPVToolsNotifier::onInstBlockRemoved(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data())
  {
    std::cout << "\nDFGPVToolsNotifier::onInstBlockRemoved 1 " << std::endl;
    m_registry->deletePathValueTool(m_dfgPortPaths, true);
    std::cout << "DFGPVToolsNotifier::onInstBlockRemoved 2 " << std::endl;
  }
  FABRIC_CATCH_END("DFGPVToolsNotifier::onInstBlockRemoved");
}

void DFGPVToolsNotifier::onExecNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == oldNodeName.data())
  {
    std::cout << "\nDFGPVToolsNotifier::onExecNodeRenamed 1 " << std::endl;
    m_dfgPortPaths.oldPortName = m_dfgPortPaths.portName;
    m_dfgPortPaths.oldNodeName = oldNodeName.data();
    m_dfgPortPaths.nodeName = newNodeName.data();
    m_registry->changedNotifiedToolPath(m_dfgPortPaths, true);
    std::cout << "DFGPVToolsNotifier::onExecNodeRenamed 2 " << std::endl;
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
    std::cout << "\nDFGPVToolsNotifier::onInstBlockRenamed 1 " << std::endl;
    m_dfgPortPaths.oldNodeName = nodeName.data();
    m_dfgPortPaths.oldBlockName = m_dfgPortPaths.blockName;
    m_dfgPortPaths.blockName = newBlockName.data();
    m_dfgPortPaths.oldPortName = m_dfgPortPaths.portName;
    m_registry->changedNotifiedToolPath(m_dfgPortPaths, true);
    std::cout << "DFGPVToolsNotifier::onInstBlockRenamed 2 " << std::endl;
  }
  FABRIC_CATCH_END("DFGPVToolsNotifier::onInstBlockRenamed");
}
