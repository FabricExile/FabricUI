//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "PathValueResolverRegistry.h"
 
using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;

bool PathValueResolverRegistry::s_instanceFlag = false;
PathValueResolverRegistry* PathValueResolverRegistry::s_registry = 0;

PathValueResolverRegistry::PathValueResolverRegistry()
{
}

PathValueResolverRegistry::~PathValueResolverRegistry()
{
  s_instanceFlag = false;
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    delete resolver;
    resolver = 0;
  }
  m_registeredResolvers.clear();
}

PathValueResolverRegistry* PathValueResolverRegistry::GetRegistry()
{
  if(s_instanceFlag == false)
  {
    s_registry = new PathValueResolverRegistry();
    s_instanceFlag = true;
  }
  return s_registry;
}

bool PathValueResolverRegistry::hasResolver(
  QString const&name)
{
  return m_registeredResolvers.count(
    name);
}

QString PathValueResolverRegistry::getResolverName(
  BasePathValueResolver* resolver)
{
  QMapIterator<QString, BasePathValueResolver*> it(m_registeredResolvers);
  while(it.hasNext()) 
  {
    it.next();
    if(resolver == it.value())
      return it.key();
  }
  return "";
}

BasePathValueResolver* PathValueResolverRegistry::getOrCreateResolver(
  QString const&name)
{
  if(hasResolver(name))
    return m_registeredResolvers[name];
  
  else
  {
    Factory *factory = Util::BaseFactoryRegistry::getFactory(
      name);
    
    BasePathValueResolver* resolver = (BasePathValueResolver*)factory->create(); 
    m_registeredResolvers[name] = resolver;
    
    void *userData = factory->getUserData();
    resolver->registrationCallback(
      name, 
      userData
      );

    return resolver;
  }
}
 
bool PathValueResolverRegistry::knownPath(
  RTVal pathValue)
{
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver->knownPath(pathValue))
      return true;
  }
  return false;
}

QString PathValueResolverRegistry::getType(
  RTVal pathValue)
{
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver->knownPath(pathValue))
      return resolver->getType(pathValue);
  }
  return "";
}

void PathValueResolverRegistry::getValue(
  RTVal pathValue)
{
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver->knownPath(pathValue))
      resolver->getValue(pathValue);
  }
}

void PathValueResolverRegistry::setValue(
  RTVal pathValue)
{
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver->knownPath(pathValue))
      resolver->setValue(pathValue);
  }
}