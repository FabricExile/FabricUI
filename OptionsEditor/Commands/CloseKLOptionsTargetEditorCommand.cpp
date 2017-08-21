//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <FabricUI/Util/QtUtil.h>
#include "../OptionsEditorHelpers.h"
#include "CloseKLOptionsTargetEditorCommand.h"
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;
using namespace OptionsEditor;

CloseKLOptionsTargetEditorCommand::CloseKLOptionsTargetEditorCommand() 
  : BaseRTValScriptableCommand()
  , m_canLog(true)
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg("editorID", "String");

  declareRTValArg(
    "failSilently",
    "Boolean",
    CommandArgFlags::OPTIONAL_ARG,
    RTVal::ConstructBoolean(
      FabricApplicationStates::GetAppStates()->getContext(), 
      false)
    );

  FABRIC_CATCH_END("CloseKLOptionsTargetEditorCommand::CloseKLOptionsTargetEditorCommand");
};

CloseKLOptionsTargetEditorCommand::~CloseKLOptionsTargetEditorCommand()
{
}

bool CloseKLOptionsTargetEditorCommand::canUndo() {
  return false;
}

bool CloseKLOptionsTargetEditorCommand::canLog() {
  return m_canLog;
}

bool CloseKLOptionsTargetEditorCommand::doIt() 
{ 
  bool res = false;

  FABRIC_CATCH_BEGIN();

  bool failSilently = getRTValArgValue("failSilently").getBoolean();
  QString editorID = getRTValArgValue("editorID").getStringCString();

  QWidget *dock = QtUtil::getDockWidget(editorID);

  if(dock == 0)
  {
    m_canLog = false;
    res = failSilently;
  }

  else
  {
    dock->close();
    res = true;
  }

  FABRIC_CATCH_END("CloseKLOptionsTargetEditorCommand::doIt");

  return res;
}

QString CloseKLOptionsTargetEditorCommand::getHelp()
{
  QMap<QString, QString> argsHelp;

  argsHelp["editorID"] = "Qt objectName of the option editor / ID of the KL option in the OptionsTargetRegistry";
  argsHelp["failSilently"] = "If false, throws an error if the widget has not been closed";

  return CommandHelpers::createHelpFromRTValArgs(
    this, 
    "Close a Qt editor that edits a KL OptionsTarget",
    argsHelp);
}

QString CloseKLOptionsTargetEditorCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;

  argsDesc["editorID"] = getRTValArgValue(
    "editorID").getStringCString();

  argsDesc["failSilently"] = QString::number(
    getRTValArgValue("failSilently").getBoolean()
    );
 
  return CommandHelpers::createHistoryDescFromArgs(
    this,
    argsDesc);
}
