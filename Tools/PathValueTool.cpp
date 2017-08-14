//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "PathValueTool.h"
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
   
  FABRIC_CATCH_END("PathValueTool::pathToPathValue");

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

  FABRIC_CATCH_END("PathValueTool::getKLToolManager");

  return toolRegistry;
}

bool PathValueTool::canCreateTool(
  QString const&toolPath)
{
 	return canCreateTool(
 		pathToPathValue(toolPath)
 		);
}

bool PathValueTool::canCreateTool(
  RTVal pathValue)
{
	bool res = false;

  FABRIC_CATCH_BEGIN();
  
  res = getKLToolManager().callMethod(
  	"Boolean", 
  	"canCreatePathValueTool", 
  	1, 
  	&pathValue).getBoolean();
 
  FABRIC_CATCH_END("PathValueTool::canCreateTool");

  return res;
}

RTVal PathValueTool::createTool(
  QString const& toolPath)
{
 	return createTool(
 		pathToPathValue(toolPath)
 		);
}

RTVal PathValueTool::createTool(
  RTVal pathValue)
{
	RTVal pathValueTool;

  FABRIC_CATCH_BEGIN();
  
  if(!canCreateTool(pathValue))
    FabricException::Throw(
      "PathValueTool::createTool",
      "Cannot createTool a tool for un-register type '" + 
      RTValUtil::getType(RTValUtil::toRTVal(pathValue).maybeGetMember("value")) + "'"
      );
   
  pathValueTool = getKLToolManager().callMethod(
    "Tool::BaseTool",
    "createPathValueTool",
    1,
    &pathValue);
 	
  FABRIC_CATCH_END("PathValueTool::createTool");

  return pathValueTool;
}

RTVal PathValueTool::getTool(
  QString const& toolPath)
{
  return getTool(
    pathToPathValue(toolPath)
    );
}

RTVal PathValueTool::getTool(
  RTVal pathValue)
{
  RTVal pathValueTool;

  FABRIC_CATCH_BEGIN();
 
  pathValueTool = getKLToolManager().callMethod(
    "Tool::BaseTool",
    "getPathValueTool",
    1,
    &pathValue);
  
  FABRIC_CATCH_END("PathValueTool::getTool");

  return pathValueTool;
}

void PathValueTool::deleteTool(
  QString const& toolPath)
{
  deleteTool(
    pathToPathValue(toolPath, false)
    );
}

void PathValueTool::deleteTool(
	FabricCore::RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
 
  getKLToolManager().callMethod(
  	"", 
  	"deletePathValueTool", 
  	1, 
  	&pathValue);
  
  FABRIC_CATCH_END("PathValueTool::deleteTool");
}

void PathValueTool::setToolPath(
	QString const& oldToolPath,
	QString const& newToolPath)
{
  setToolPath(
    pathToPathValue(oldToolPath, false),
    pathToPathValue(newToolPath, false)   
    );
}

void PathValueTool::setToolPath(
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

  FABRIC_CATCH_END("PathValueTool::setToolPath");
}

void PathValueTool::setToolValue(
	QString const& toolPath)
{
  setToolValue(
    pathToPathValue(toolPath)
    );
}

void PathValueTool::setToolValue(
  FabricCore::RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  getKLToolManager().callMethod(
    "",
    "setPathValueToolValue",
    1,
    &pathValue);

  FABRIC_CATCH_END("PathValueTool::setToolValue");
}