//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include <iostream>
#include "ToolsNotifier.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/DFG/DFGController.h>
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

ToolsNotifierRegistry::ToolsNotifierRegistry( 
  DFGWidget *dfgWidget)
  : m_dfgWidget(dfgWidget)
  , m_notifProxy( NULL )
{
}

ToolsNotifierRegistry::~ToolsNotifierRegistry()
{
}

void ToolsNotifierRegistry::initConnections()
{
  std::cout << "\nToolsNotifierRegistry::initConnections" << std::endl;
  connect(
    m_dfgWidget->getDFGController(),
    SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
    this,
    SLOT(onControllerBindingChanged(FabricCore::DFGBinding const &))
    );
 
  setupConnections(m_dfgWidget->getDFGController());
}
 
void ToolsNotifierRegistry::onControllerBindingChanged(
  FabricCore::DFGBinding const &newBinding)
{
  setupConnections(m_dfgWidget->getDFGController());
}
 
void ToolsNotifierRegistry::setupConnections(
  DFGController *dfgController)
{
  if ( m_notifProxy )
  {
    m_notifProxy->setParent( NULL );
    delete m_notifProxy;
    m_notifProxy = NULL;
  }

  m_notifier.clear();
  m_notifier = m_dfgWidget->getDFGController()->getBindingNotifier();

  ToolsNotifierRegistry_BindingNotifProxy *notifProxy =
    new ToolsNotifierRegistry_BindingNotifProxy( this, this );
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

void ToolsNotifierRegistry::onBindingArgTypeChanged( 
  unsigned index, 
  FTL::CStrRef name, 
  FTL::CStrRef newType)
{
  FABRIC_CATCH_BEGIN();
  std::cout << "\nToolsNotifierRegistry::onBindingArgTypeChanged" << std::endl;

  DFGToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = name.data();
  deletePathValueTool(dfgPortPath);

  FABRIC_CATCH_END("ToolsNotifierRegistry::onBindingArgTypeChanged");
}

void ToolsNotifierRegistry::onBindingArgRemoved( 
  unsigned index, 
  FTL::CStrRef name)
{
  FABRIC_CATCH_BEGIN();
  std::cout << "\nToolsNotifierRegistry::onBindingArgRemoved" << std::endl;

  DFGToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = name.data();
  deletePathValueTool(dfgPortPath);

  FABRIC_CATCH_END("ToolsNotifierRegistry::onBindingArgRemoved");
}

void ToolsNotifierRegistry::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName
  )
{
  FABRIC_CATCH_BEGIN();
  std::cout << "\nToolsNotifierRegistry::onBindingArgRenamed" << std::endl;

  DFGToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = newArgName.data();
  dfgPortPath.oldPortName = oldArgName.data();
  changedNotifiedToolPath(dfgPortPath);

  FABRIC_CATCH_END("ToolsNotifierRegistry::onBindingArgRenamed");
}

void ToolsNotifierRegistry::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  FABRIC_CATCH_BEGIN();
  std::cout << "\nToolsNotifierRegistry::onBindingArgValueChanged" << std::endl;
  DFGToolsNotifierPortPaths dfgPortPath;
  dfgPortPath.portName = name.data();
  toolValueChanged(dfgPortPath);
  FABRIC_CATCH_END("ToolsNotifierRegistry::onBindingArgValueChanged");
}

RTVal ToolsNotifierRegistry::getKLToolManager()
{
  RTVal toolRegistry;

  FABRIC_CATCH_BEGIN();

  toolRegistry = RTVal::Create(
    FabricApplicationStates::GetAppStates()->getContext(),
    "Tool::AppToolsManager",
    0,
    0);

  toolRegistry = toolRegistry.callMethod(
    "Tool::ToolsManager",
    "getToolsManager",
    0,
    0);

  FABRIC_CATCH_END("ToolsNotifierRegistry::getKLToolManager");

  return toolRegistry;
}

void ToolsNotifierRegistry::createPathValueTool(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
  
  bool isRegistered = getKLToolManager().callMethod(
    "Boolean",
    "isTypeRegistered",
    1,
    &pathValue).getBoolean();

  if(isRegistered)
  {
    getKLToolManager().callMethod(
      "Tool::BaseTool",
      "createPathValueTool",
      1,
      &pathValue);

    ToolsNotifier *notifier = new ToolsNotifier(
      this,
      pathValue);

    m_registeredNotifiers.append(notifier);

    // Update the tool'value from its pathValue.
    toolValueChanged(notifier->getDFGToolsNotifierPortPaths());
  }

  FABRIC_CATCH_END("ToolsNotifierRegistry::createPathValueTool");
}

RTVal ToolsNotifierRegistry::pathToPathValue(
  QString const&toolPath)
{
  RTVal pathValue;

  FABRIC_CATCH_BEGIN();

  FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();
 
  RTVal toolPathVal = RTVal::ConstructString(
    appStates->getClient(), 
    toolPath.toUtf8().constData()
    );

  pathValue = RTVal::Construct(
    appStates->getClient(), 
    "PathValue", 
    1, 
    &toolPathVal);

  DFGPathValueResolver *resolver = qobject_cast<DFGPathValueResolver *>(
    PathValueResolverRegistry::getRegistry()->getResolver(pathValue)
    );

  // Gets the port value via the resolver.
  if(resolver)
    resolver->getValue(pathValue);
  
  FABRIC_CATCH_END("ToolsNotifierRegistry::pathToPathValue");

  return pathValue;
}

void ToolsNotifierRegistry::deletePathValueTool(
  DFGToolsNotifierPortPaths dfgPortPath,
  bool fromNode)
{
  FABRIC_CATCH_BEGIN();
 
  std::cout 
    << "\nToolsNotifierRegistry::deletePathValueTool " 
    << " getAbsolutePortPath "
    << dfgPortPath.getAbsolutePortPath().toUtf8().constData() 
    << " fromNode "
    << fromNode
    << std::endl;

  if(dfgPortPath.isExecArg())
  {
    FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();
    RTVal pathVal = RTVal::ConstructString(appStates->getClient(), dfgPortPath.getAbsolutePortPath().toUtf8().constData());
    getKLToolManager().callMethod("Boolean", "deleteTool", 1, &pathVal);
  }

  else
  {
    foreach(ToolsNotifier *notifier, m_registeredNotifiers)
    {
      DFGToolsNotifierPortPaths notDFGPortPath = notifier->getDFGToolsNotifierPortPaths();
      bool deleteTool = false;
 
      if(!fromNode)
        deleteTool = notDFGPortPath.getAbsolutePortPath() == dfgPortPath.getAbsolutePortPath();
      else
        deleteTool = notDFGPortPath.getAbsoluteNodePath() == dfgPortPath.getAbsoluteNodePath();

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

        FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();
        RTVal pathVal = RTVal::ConstructString(appStates->getClient(), notDFGPortPath.getAbsolutePortPath().toUtf8().constData());
        getKLToolManager().callMethod("Boolean", "deleteTool", 1, &pathVal);
      }
    }
  }
 
  FABRIC_CATCH_END("ToolsNotifierRegistry::deletePathValueTool");
}

void ToolsNotifierRegistry::changedNotifiedToolPath(
  DFGToolsNotifierPortPaths dfgPortPath,
  bool fromNode)
{
  FABRIC_CATCH_BEGIN();
 
  std::cout 
    << "ToolsNotifierRegistry::changedNotifiedToolPath " 
    << " getAbsolutePortPath "
    << dfgPortPath.getAbsolutePortPath().toUtf8().constData() 
    << " getOldAbsolutePortPath "
    << dfgPortPath.getOldAbsolutePortPath().toUtf8().constData() 
    << std::endl;

  if(dfgPortPath.isExecArg())
  {
    FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();
    RTVal args[2] = { 
      RTVal::ConstructString(appStates->getClient(), dfgPortPath.getOldAbsolutePortPath().toUtf8().constData()),
      RTVal::ConstructString(appStates->getClient(), dfgPortPath.getAbsolutePortPath().toUtf8().constData())
    };
    getKLToolManager().callMethod("Boolean", "renameTool", 2, args);
  }

  else
  {
    foreach(ToolsNotifier *notifier, m_registeredNotifiers)
    {
      DFGToolsNotifierPortPaths notDFGPortPath = notifier->getDFGToolsNotifierPortPaths();
      bool renameTool = false;
      if(!fromNode)
        renameTool = notDFGPortPath.getOldAbsolutePortPath() == dfgPortPath.getOldAbsolutePortPath();
      else
        renameTool = notDFGPortPath.getOldAbsoluteNodePath() == dfgPortPath.getOldAbsoluteNodePath();

       std::cout 
        << "notDFGPortPath.getAbsolutePortPath "
        << notDFGPortPath.getAbsolutePortPath().toUtf8().constData() 
        << " notDFGPortPath.getAbsoluteNodePath "
        << notDFGPortPath.getAbsoluteNodePath().toUtf8().constData() 
        << " renameTool "
        << renameTool
        << std::endl;

      if(renameTool)
      {
        FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();
        RTVal args[2] = { 
          RTVal::ConstructString(appStates->getClient(), notDFGPortPath.getOldAbsolutePortPath().toUtf8().constData()),
          RTVal::ConstructString(appStates->getClient(), notDFGPortPath.getAbsolutePortPath().toUtf8().constData())
        };
        getKLToolManager().callMethod("Boolean", "renameTool", 2, args);      
      }
    }
  }

  FABRIC_CATCH_END("ToolsNotifierRegistry::changedNotifiedToolPath");
}

void ToolsNotifierRegistry::toolValueChanged(
  DFGToolsNotifierPortPaths dfgPortPath)
{
  FABRIC_CATCH_BEGIN();
 
  RTVal pathValue = pathToPathValue(dfgPortPath.getAbsolutePortPath());

  getKLToolManager().callMethod(
    "",
    "toolValueChanged",
    1,
    &pathValue);

  FABRIC_CATCH_END("ToolsNotifierRegistry::toolValueChanged");
}

ToolsNotifierRegistry_NotifProxy::ToolsNotifierRegistry_NotifProxy(
  ToolsNotifierRegistry *dst,
  QObject *parent)
  : QObject( parent )
  , m_dst( dst )
{
}

ToolsNotifierRegistry_NotifProxy::~ToolsNotifierRegistry_NotifProxy() 
{
}

ToolsNotifierRegistry_BindingNotifProxy::ToolsNotifierRegistry_BindingNotifProxy(
  ToolsNotifierRegistry *dst,
  QObject *parent)
  : ToolsNotifierRegistry_NotifProxy( dst, parent )
{
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgValueChanged( index, name );
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName)
{
  m_dst->onBindingArgRenamed( argIndex, oldArgName, newArgName );
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgRemoved(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgRemoved( index, name );
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgTypeChanged(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef newType)
{
  m_dst->onBindingArgTypeChanged( index, name, newType );
}

ToolsNotifier::ToolsNotifier( 
  ToolsNotifierRegistry *registry,
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

  FABRIC_CATCH_END("ToolsNotifier::ToolsNotifier");
}

ToolsNotifier::~ToolsNotifier()
{
  m_notifier.clear();
}

DFGToolsNotifierPortPaths ToolsNotifier::getDFGToolsNotifierPortPaths() const 
{ 
  return m_dfgPortPaths; 
};

void ToolsNotifier::setupConnections(
  FabricCore::DFGExec exec)
{
  std::cout << "\nToolsNotifier::setupConnections 2" << std::endl;

  m_notifier.clear();

  m_notifier = DFG::DFGExecNotifier::Create( exec );
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
}

void ToolsNotifier::onExecNodePortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->toolValueChanged(m_dfgPortPaths);
  FABRIC_CATCH_END("ToolsNotifier::onExecNodePortDefaultValuesChanged");
}

void ToolsNotifier::onInstBlockPortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == portName.data())
    m_registry->toolValueChanged(m_dfgPortPaths);
  FABRIC_CATCH_END("ToolsNotifier::onExecNodePortDefaultValuesChanged");
}
 
void ToolsNotifier::onExecNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  FABRIC_CATCH_BEGIN();
  onExecNodePortRemoved(nodeName, 0, portName);
  FABRIC_CATCH_END("ToolsNotifier::onExecNodePortResolvedTypeChanged");
}
 
void ToolsNotifier::onExecNodePortRenamed(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == oldPortName.data())
  {
    std::cout << "\nToolsNotifier::onExecNodePortRenamed 1" << std::endl;
    m_dfgPortPaths.oldNodeName = nodeName.data();
    m_dfgPortPaths.oldPortName = oldPortName.data();
    m_dfgPortPaths.portName = newPortName.data();
    m_registry->changedNotifiedToolPath(m_dfgPortPaths);
    std::cout << "ToolsNotifier::onExecNodePortRenamed 2" << std::endl;
  }
  FABRIC_CATCH_END("ToolsNotifier::onExecNodePortRenamed");
}

void ToolsNotifier::onInstBlockPortRenamed(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == oldPortName.data())
  {
    std::cout << "\nToolsNotifier::onInstBlockPortRenamed 1" << std::endl;
    m_dfgPortPaths.oldNodeName = nodeName.data();
    m_dfgPortPaths.oldBlockName = blockName.data();
    m_dfgPortPaths.oldPortName = oldPortName.data();
    m_dfgPortPaths.portName = newPortName.data();
    m_registry->changedNotifiedToolPath(m_dfgPortPaths);
    std::cout << "ToolsNotifier::onInstBlockPortRenamed 2" << std::endl;
  }
  FABRIC_CATCH_END("ToolsNotifier::onInstBlockPortRenamed");
}
 
void ToolsNotifier::onExecNodePortRemoved(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.portName == portName.data())
  {
    std::cout << "\nToolsNotifier::onExecNodePortRemoved 1" << std::endl;
    m_registry->deletePathValueTool(m_dfgPortPaths);
    std::cout << "ToolsNotifier::onExecNodePortRemoved 2" << std::endl;
  }
  FABRIC_CATCH_END("ToolsNotifier::onExecNodePortRemoved");
}

void ToolsNotifier::onInstBlockPortRemoved(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data() && m_dfgPortPaths.portName == portName.data())
  {
    std::cout << "\nToolsNotifier::onInstBlockPortRemoved 1" << std::endl;
    m_registry->deletePathValueTool(m_dfgPortPaths);
    std::cout << "ToolsNotifier::onInstBlockPortRemoved 2" << std::endl;
  }
  FABRIC_CATCH_END("ToolsNotifier::onInstBlockPortRemoved");
}

void ToolsNotifier::onExecNodeRemoved(
  FTL::CStrRef nodeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data())
  {
    std::cout << "\nToolsNotifier::onExecNodeRemoved 1 " << std::endl;
    m_registry->deletePathValueTool(m_dfgPortPaths, true);
    std::cout << "ToolsNotifier::onExecNodeRemoved 2 " << std::endl;
  }
  FABRIC_CATCH_END("ToolsNotifier::onExecNodeRemoved");
}

void ToolsNotifier::onInstBlockRemoved(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == blockName.data())
  {
    std::cout << "\nToolsNotifier::onInstBlockRemoved 1 " << std::endl;
    m_registry->deletePathValueTool(m_dfgPortPaths, true);
    std::cout << "ToolsNotifier::onInstBlockRemoved 2 " << std::endl;
  }
  FABRIC_CATCH_END("ToolsNotifier::onInstBlockRemoved");
}

void ToolsNotifier::onExecNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == oldNodeName.data())
  {
    std::cout << "\nToolsNotifier::onExecNodeRenamed 1 " << std::endl;
    m_dfgPortPaths.oldPortName = m_dfgPortPaths.portName;
    m_dfgPortPaths.oldNodeName = oldNodeName.data();
    m_dfgPortPaths.nodeName = newNodeName.data();
    m_registry->changedNotifiedToolPath(m_dfgPortPaths, true);
    std::cout << "ToolsNotifier::onExecNodeRenamed 2 " << std::endl;
  }
  FABRIC_CATCH_END("ToolsNotifier::onExecNodeRenamed");
}

void ToolsNotifier::onInstBlockRenamed(
  FTL::CStrRef nodeName,
  FTL::CStrRef oldBlockName,
  FTL::CStrRef newBlockName)
{
  FABRIC_CATCH_BEGIN();
  if(m_dfgPortPaths.nodeName == nodeName.data() && m_dfgPortPaths.blockName == oldBlockName.data())
  {
    std::cout << "\nToolsNotifier::onInstBlockRenamed 1 " << std::endl;
    m_dfgPortPaths.oldNodeName = nodeName.data();
    m_dfgPortPaths.oldBlockName = m_dfgPortPaths.blockName;
    m_dfgPortPaths.blockName = newBlockName.data();
    m_dfgPortPaths.oldPortName = m_dfgPortPaths.portName;
    m_registry->changedNotifiedToolPath(m_dfgPortPaths, true);
    std::cout << "ToolsNotifier::onInstBlockRenamed 2 " << std::endl;
  }
  FABRIC_CATCH_END("ToolsNotifier::onInstBlockRenamed");
}
