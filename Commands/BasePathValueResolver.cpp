//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BasePathValueResolver.h"
#include <FabricUI/Util/RTValUtil.h>
#include "PathValueResolverRegistry.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

BasePathValueResolver::BasePathValueResolver()
 : QObject()
{
}

BasePathValueResolver::~BasePathValueResolver()
{
	QString name = PathValueResolverRegistry::getRegistry()->getResolverName(
		this);

	PathValueResolverRegistry::getRegistry()->unregisterFactory(
		name);
}

void BasePathValueResolver::registrationCallback(
  QString const&name,
  void *userData)
{
}

bool BasePathValueResolver::knownPath(
  RTVal pathValue)
{
  FabricException::Throw(
    "BasePathValueResolver::knownPath",
    "The method 'knownPath' must be overriden"
    );
  return false;
}

QString BasePathValueResolver::getType(
  RTVal pathValue)
{
  FabricException::Throw(
    "BasePathValueResolver::getType",
    "The method 'getType' must be overriden"
    );
  return "";
}

void BasePathValueResolver::getValue(
  RTVal pathValue)
{
  FabricException::Throw(
    "BasePathValueResolver::getValue",
    "The method 'getValue' must be overriden"
    );
}

void BasePathValueResolver::setValue(
  RTVal pathValue)
{
  FabricException::Throw(
    "BasePathValueResolver::setValue",
    "The method 'setValue' must be overriden"
    );
}
