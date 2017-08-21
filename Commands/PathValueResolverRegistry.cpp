//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "PathValueResolverRegistry.h"
#include <vector>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;

bool PathValueResolverRegistry::s_instanceFlag = false;
PathValueResolverRegistry* PathValueResolverRegistry::s_registry = 0;

PathValueResolverRegistry::PathValueResolverRegistry()
 : BaseFactoryRegistry()
{
  if(s_instanceFlag == false)
    s_instanceFlag = true;
}

void PathValueResolverRegistry::clear() {
  // Copy resolvers first; their destructor might unregister so iterating is not safe
  std::vector<BasePathValueResolver*> resolvers;
  foreach( BasePathValueResolver* resolver, m_registeredResolvers )
    resolvers.push_back( resolver );

  // clearing first, else the resolvers will delete themselve twice
  m_registeredResolvers.clear();

  foreach( BasePathValueResolver* resolver, resolvers )
    delete resolver;
}


PathValueResolverRegistry::~PathValueResolverRegistry()
{
  s_instanceFlag = false;
  clear();
  if( s_registry == this )
    s_registry = 0;
}

PathValueResolverRegistry* PathValueResolverRegistry::getRegistry()
{
  if(s_instanceFlag == false)
    s_registry = new PathValueResolverRegistry();
  return s_registry;
}

bool PathValueResolverRegistry::hasResolver(
  QString const&name)
{
  return m_registeredResolvers.count(name);
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

BasePathValueResolver* PathValueResolverRegistry::getResolver(
  QString const&name)
{
  return hasResolver(name) ? m_registeredResolvers[name] : 0;
}

BasePathValueResolver* PathValueResolverRegistry::getResolver(
  RTVal pathValue)
{
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver->knownPath(pathValue))
      return resolver;
  }
  return 0;
}
 
bool PathValueResolverRegistry::knownPath(
  RTVal pathValue)
{
  return getResolver(pathValue) != 0;
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

void PathValueResolverRegistry::unregisterFactory(
  QString const&name)
{
  // unregister the factory
  BaseFactoryRegistry::unregisterFactory(name);

  // unregister the resolver
  if(hasResolver(name))
  {
    BasePathValueResolver* resolver = m_registeredResolvers[name];
    // Remove first, else it will unregister again in its destructor
    m_registeredResolvers.remove( name );
    if(resolver != 0)
    {
      delete resolver;
      resolver = 0;
    }
  }
}

void PathValueResolverRegistry::registerResolver(
  BasePathValueResolver *resolver,
  QString const&name)
{
  m_registeredResolvers[name] = resolver;
}
