//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "Factory.h"

using namespace FabricUI;
using namespace Util;

BaseFactoryRegistry::BaseFactoryRegistry() 
  : QObject()
{
}

BaseFactoryRegistry::~BaseFactoryRegistry() 
{
  QMapIterator<QString, Factory*> it(m_factories);
  while(it.hasNext()) 
  {
    it.next();
    Factory* factory = (Factory*)it.value();
    if(factory != 0)
    {
      delete factory;
      factory = 0;
    }
    m_factories.remove(it.key());
  }
}

Factory* BaseFactoryRegistry::getFactory(
  QString const&name) 
{
  return hasFactory(name) 
    ? m_factories[name] 
    : 0;
}

bool BaseFactoryRegistry::hasFactory(
  QString const&name) 
{
  return m_factories.count(name) > 0;
}

void BaseFactoryRegistry::registerFactory(
  QString const&name, 
  Factory *factory) 
{
  if(!hasFactory(name))
  {
    m_factories[name] = factory;
    emit factoryRegistered(
      name,
      factory);
  }
}

void BaseFactoryRegistry::unregisterFactory(
  QString const&name) 
{
  if(hasFactory(name))
  {
    delete m_factories[name];
    m_factories[name]= 0;
    m_factories.remove(name);
    emit factoryUnregistered(name);
  }
}

void BaseFactoryRegistry::unregisterFactory(
  Factory *factory) 
{
  QMapIterator<QString, Factory*> it(m_factories);
  while(it.hasNext()) 
  {
    it.next();
    if(it.value() == factory)
      unregisterFactory(it.key());
  }
}
