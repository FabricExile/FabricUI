//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "ToolManager.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace Tools;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

inline RTVal pathToPathValue(
  QString const&toolPath,
  bool resolve)
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

  if(resolve)
    PathValueResolverRegistry::getRegistry()->getValue(pathValue);
   
  FABRIC_CATCH_END("ToolManager::pathToPathValue");

  return pathValue;
}

inline RTVal pathToPathValue(
  QString const&toolPath)
{
  return pathToPathValue(
    toolPath, 
    true
    );
}

inline RTVal getKLToolManager()
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

  FABRIC_CATCH_END("ToolManager::getKLToolManager");

  return toolRegistry;
}

bool ToolManager::canCreateTool(
  QString const&toolPath)
{
 	return canCreateTool(
 		pathToPathValue(toolPath)
 		);
}

bool ToolManager::canCreateTool(
  RTVal value)
{
	bool res = false;

  FABRIC_CATCH_BEGIN();
  
  res = getKLToolManager().callMethod(
  	"Boolean", 
  	"canCreatePathValueTool", 
  	1, 
  	&value).getBoolean();
 
  FABRIC_CATCH_END("ToolManager::canCreateTool");

  return res;
}

RTVal ToolManager::createTool(
  QString const& toolPath)
{
 	return createTool(
 		pathToPathValue(toolPath)
 		);
}

RTVal ToolManager::createTool(
  RTVal pathValue)
{
	RTVal pathValueTool;

  FABRIC_CATCH_BEGIN();
  
  if(!canCreateTool(pathValue))
    FabricException::Throw(
      "ToolManager::createTool",
      "Cannot createTool a tool for un-register type '" + 
      RTValUtil::getType(RTValUtil::toRTVal(pathValue).maybeGetMember("value")) + "'"
      );
   
  pathValueTool = getKLToolManager().callMethod(
    "Tool::BaseTool",
    "createPathValueTool",
    1,
    &pathValue);
 	
  FABRIC_CATCH_END("ToolManager::createTool");

  return pathValueTool;
}

RTVal ToolManager::getTool(
  QString const& toolPath)
{
  return getTool(
    pathToPathValue(toolPath)
    );
}

RTVal ToolManager::getTool(
  RTVal pathValue)
{
  RTVal pathValueTool;

  FABRIC_CATCH_BEGIN();
 
  pathValueTool = getKLToolManager().callMethod(
    "Tool::BaseTool",
    "getPathValueTool",
    1,
    &pathValue);
  
  FABRIC_CATCH_END("ToolManager::getTool");

  return pathValueTool;
}

void ToolManager::deleteTool(
  QString const& toolPath)
{
  deleteTool(
    pathToPathValue(toolPath, false)
    );
}

void ToolManager::deleteTool(
	FabricCore::RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
 
  getKLToolManager().callMethod(
  	"", 
  	"deletePathValueTool", 
  	1, 
  	&pathValue);
  
  FABRIC_CATCH_END("ToolManager::deleteTool");
}

void ToolManager::setToolPath(
	QString const& oldToolPath,
	QString const& newToolPath)
{
  setToolPath(
    pathToPathValue(oldToolPath, false),
    pathToPathValue(newToolPath, false)   
    );
}

void ToolManager::setToolPath(
  FabricCore::RTVal oldPathValue,
  FabricCore::RTVal newPathValue)
{
  FABRIC_CATCH_BEGIN();
 
  RTVal args[2] = { 
    oldPathValue,
    newPathValue
  };

  getKLToolManager().callMethod(
    "", 
    "setPathValueToolPath", 
    2, 
    args);   

  FABRIC_CATCH_END("ToolManager::setToolPath");
}

void ToolManager::setToolValue(
	QString const& toolPath)
{
  setToolValue(
    pathToPathValue(toolPath)
    );
}

void ToolManager::setToolValue(
  FabricCore::RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  getKLToolManager().callMethod(
    "",
    "setPathValueToolValue",
    1,
    &pathValue);

  FABRIC_CATCH_END("ToolManager::setToolValue");
}
