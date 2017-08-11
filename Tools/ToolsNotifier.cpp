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

DFG::DFGWidget *ToolsNotifierRegistry::getDFGWidget()
{
  return m_dfgWidget;
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
  std::cout << "\nToolsNotifierRegistry::onBindingArgTypeChanged" << std::endl;
  deletePathValueTool(name.data());
}

void ToolsNotifierRegistry::onBindingArgRemoved( 
  unsigned index, 
  FTL::CStrRef name)
{
  std::cout << "\nToolsNotifierRegistry::onBindingArgRemoved" << std::endl;
  deletePathValueTool(name.data());
}

void ToolsNotifierRegistry::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName
  )
{
  FABRIC_CATCH_BEGIN();
  std::cout << "\nToolsNotifierRegistry::onBindingArgRenamed" << std::endl;

  changedNotifiedToolPath(
    oldArgName.data(), 
    newArgName.data()
    );

  FABRIC_CATCH_END("ToolsNotifierRegistry::onBindingArgRenamed");
}

void ToolsNotifierRegistry::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  std::cout << "\nToolsNotifierRegistry::onBindingArgValueChanged" << std::endl;
  toolValueChanged(name.data());
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
  QString const&toolPath)
{
  FABRIC_CATCH_BEGIN();
  
  RTVal pathValue = pathToPathValue(toolPath);

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
    toolValueChanged(toolPath);
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
  QString const&toolPath,
  bool isNode)
{
  FABRIC_CATCH_BEGIN();
 
  std::cout 
    << "\nToolsNotifierRegistry::deletePathValueTool " 
    << toolPath.toUtf8().constData() 
    << " isNode "
    << isNode
    << std::endl;

  FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();
  RTVal args[2] = { 
    RTVal::ConstructString(appStates->getClient(), toolPath.toUtf8().constData()),
    RTVal::ConstructBoolean(appStates->getClient(), isNode)
  };

  if(getKLToolManager().callMethod("Boolean", "deleteTool", 2, args))
  {
    foreach(ToolsNotifier *notifier, m_registeredNotifiers)
    {
      bool notifyTool = notifier->notifyToolAtPath(toolPath, isNode);
      std::cout << "notifyTool " << notifyTool << std::endl;

      //std::cout << "notifier->getToolTargetPath() " << notifier->getToolTargetPath().toUtf8().constData() << std::endl;
      if(notifyTool)
      {
        m_registeredNotifiers.removeAll(notifier);
        delete notifier;
        notifier = 0;
        break;
      }
    }
  }

  FABRIC_CATCH_END("ToolsNotifierRegistry::deletePathValueTool");
}

void ToolsNotifierRegistry::changedNotifiedToolPath(
  QString const&oldToolPath,
  QString const&newToolPath,
  bool isNode)
{
  FABRIC_CATCH_BEGIN();
 
  std::cout 
    << "\nToolsNotifierRegistry::changedNotifiedToolPath "
    << " oldToolPath " 
    << oldToolPath.toUtf8().constData() 
    << " newToolPath " 
    << newToolPath.toUtf8().constData() 
    << " isNode "
    << isNode
    << std::endl;

  FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();
  RTVal args[3] = { 
    RTVal::ConstructString(appStates->getClient(), oldToolPath.toUtf8().constData()),
    RTVal::ConstructString(appStates->getClient(), newToolPath.toUtf8().constData()),
    RTVal::ConstructBoolean(appStates->getClient(), isNode)
  };

  if(getKLToolManager().callMethod("Boolean", "renameTool", 3, args))
  {
    foreach(ToolsNotifier *notifier, m_registeredNotifiers)
    {
      bool notifyTool = notifier->notifyToolAtPath(oldToolPath, isNode);
      std::cout << "notifyTool " << notifyTool << std::endl;
      if(notifyTool)
        notifier->changedNotifiedToolPath(oldToolPath, newToolPath, isNode);
    }
  }

  FABRIC_CATCH_END("ToolsNotifierRegistry::changedNotifiedToolPath");
}

void ToolsNotifierRegistry::toolValueChanged(
  QString const&toolPath)
{
  FABRIC_CATCH_BEGIN();
 
  RTVal pathValue = pathToPathValue(toolPath);

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
    m_toolPath = RTValUtil::toRTVal(pathValue).maybeGetMember(
      "path").getStringCString();

    DFGExec exec = resolver->getDFGPortPaths(
      pathValue, 
      m_dfgPortPaths
      );

    FabricCore::String path = exec.getExecPath();
  
    m_execPath = QString(std::string(path.getCStr(), path.getSize()).c_str());
    m_execPath = !m_execPath.isEmpty() ? m_execPath + "." : m_execPath;

    setupConnections(exec);
  }

  FABRIC_CATCH_END("ToolsNotifier::ToolsNotifier");
}

ToolsNotifier::~ToolsNotifier()
{
  m_notifier.clear();
}

bool ToolsNotifier::notifyToolAtPath(
  QString const &toolPath,
  bool isNode)
{
  std::cout 
    << "ToolsNotifier::notifyToolAtPath 1 "
    << " toolPath "
    << toolPath.toUtf8().constData() 
    << " isNode "
    << isNode
    << std::endl;

  bool res = false;

  if(isNode)
  {
    int index = m_toolPath.lastIndexOf(".");
    QString nodePath = m_toolPath.mid(0, index);
    res =  nodePath == toolPath;
  }
  else
    res = m_toolPath == toolPath;

  std::cout 
    << "ToolsNotifier::notifyToolAtPath 2 "
    << " res "
    << res
    << std::endl;

  return res;
}

void ToolsNotifier::changedNotifiedToolPath(
  QString const &oldToolPath,
  QString const &newToolPath,
  bool isNode)
{
  std::cout 
    << "ToolsNotifier::changedNotifiedToolPath 1 "
    << " oldToolPath "
    << oldToolPath.toUtf8().constData() 
    << " newToolPath "
    << newToolPath.toUtf8().constData() 
    << " isNode "
    << isNode
    << std::endl;

  if(isNode)
  {
    int index = m_toolPath.lastIndexOf(".");
    QString nodePath = m_toolPath.mid(0, index);
    QString toolName = m_toolPath.mid(index+1);

    if(nodePath == oldToolPath)
      m_toolPath = newToolPath + "." + toolName;
  }
  
  if(m_toolPath == oldToolPath)
    m_toolPath = newToolPath;

  std::cout 
    << "ToolsNotifier::changedNotifiedToolPath 2 "
    << m_toolPath.toUtf8().constData() 
    << std::endl;
}

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
    SIGNAL(nodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(nodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef))
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
 
  QString toolPath = nodeName != ""
    ? QString(nodeName.c_str()) + "." + QString(portName.c_str())
    : portName.c_str();

  m_registry->toolValueChanged(m_execPath + toolPath);
 
  FABRIC_CATCH_END("ToolsNotifier::onExecNodePortDefaultValuesChanged");
}

void ToolsNotifier::onInstBlockPortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();
 
  QString toolPath = nodeName != ""
    ? nodeName.c_str() + QString(".") + blockName.c_str() + QString(".") + portName.c_str()
    : blockName.c_str() + QString(".") + portName.c_str();

  m_registry->toolValueChanged(m_execPath + toolPath);
 
  FABRIC_CATCH_END("ToolsNotifier::onExecNodePortDefaultValuesChanged");
}
 
void ToolsNotifier::onExecNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  // To-do
  //std::cout << "ToolsNotifier::onExecNodePortResolvedTypeChanged" << std::endl;
}
 
void ToolsNotifier::onExecNodePortRenamed(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  std::cout << "ToolsNotifier::onExecNodePortRenamed 1" << std::endl;
 
  QString oldToolPath = nodeName != ""
    ? m_execPath + QString(nodeName.c_str()) + "." + QString(oldPortName.c_str())
    : m_execPath + oldPortName.c_str();

  QString newToolPath = nodeName != ""
    ? m_execPath + QString(nodeName.c_str()) + "." + QString(newPortName.c_str())
    : m_execPath + newPortName.c_str();

  m_registry->changedNotifiedToolPath(oldToolPath, newToolPath);

  std::cout << "ToolsNotifier::onExecNodePortRenamed 2" << std::endl;
}
 
void ToolsNotifier::onExecNodePortRemoved(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsNotifier::onExecNodePortRemoved 1" << std::endl;
 
  QString toolPath = nodeName != ""
    ? QString(nodeName.c_str()) + "." + QString(portName.c_str())
    : portName.c_str();

  m_registry->deletePathValueTool(m_execPath + toolPath);
  std::cout << "ToolsNotifier::onExecNodePortRemoved 2" << std::endl;
}

void ToolsNotifier::onExecNodeRemoved(
  FTL::CStrRef nodeName)
{
  // To-do
  // Remove all the tools that are attached to any port of the node.
  std::cout << "ToolsNotifier::onExecNodeRemoved 1 " << std::endl;
  
  m_registry->deletePathValueTool(m_execPath + nodeName.c_str(), true);
 
  std::cout << "ToolsNotifier::onExecNodeRemoved 2 " << std::endl;
}

void ToolsNotifier::onExecNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName)
{
  std::cout << "ToolsNotifier::onExecNodeRenamed 1 " << std::endl;

  m_registry->changedNotifiedToolPath(
    m_execPath + oldNodeName.c_str(), 
    m_execPath + newNodeName.c_str(), 
    true);

  std::cout << "ToolsNotifier::onExecNodeRenamed 2 " << std::endl;
}
