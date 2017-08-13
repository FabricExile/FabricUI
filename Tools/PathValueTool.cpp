//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <iostream>
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

void PathValueTool::deleteTool(
	QString const& toolPath)
{
  FABRIC_CATCH_BEGIN();

	FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();

  RTVal toolPathVal = RTVal::ConstructString(
  	appStates->getClient(), 
  	toolPath.toUtf8().constData()
  	);

  getKLToolManager().callMethod(
  	"", 
  	"deletePathValueTool", 
  	1, 
  	&toolPathVal);
  
  FABRIC_CATCH_END("PathValueTool::deleteTool");
}

void PathValueTool::renameTool(
	QString const& oldToolPath,
	QString const& newToolPath)
{
  FABRIC_CATCH_BEGIN();

	FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();
	Client client = appStates->getClient();

  RTVal args[2] = { 
    RTVal::ConstructString(client, oldToolPath.toUtf8().constData()),
    RTVal::ConstructString(client, newToolPath.toUtf8().constData())
  };

  getKLToolManager().callMethod(
  	"", 
  	"renamePathValueTool", 
  	2, 
  	args);   

  FABRIC_CATCH_END("PathValueTool::renameTool");
}

void PathValueTool::toolValueChanged(
	QString const& toolPath)
{
  FABRIC_CATCH_BEGIN();

	RTVal pathValue = pathToPathValue(toolPath);

	getKLToolManager().callMethod(
    "",
    "pathValueToolValueChanged",
    1,
    &pathValue);

  FABRIC_CATCH_END("PathValueTool::toolValueChanged");
}

RTVal PathValueTool::pathToPathValue(
  QString const&toolPath,
  bool resolveValue)
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

  if(resolveValue)
    PathValueResolverRegistry::getRegistry()->getValue(pathValue);
   
  FABRIC_CATCH_END("PathValueTool::pathToPathValue");

  return pathValue;
}

RTVal PathValueTool::getKLToolManager()
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
