//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "../OptionsEditorHelpers.h"
#include "../KLOptionsTargetEditor.h"
#include "OpenKLOptionsTargetEditorCommand.h"
#include <FabricUI/Util/QtUtil.h>
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace OptionsEditor;

OpenKLOptionsTargetEditorCommand::OpenKLOptionsTargetEditorCommand() 
  : BaseRTValScriptableCommand()
  , m_canLog(true)
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg("editorID", "String");
  
  declareRTValArg("editorTitle", "String");

  declareRTValArg(
    "groupName",
    "String",
    CommandArgFlags::OPTIONAL_ARG);
 
  FABRIC_CATCH_END("OpenKLOptionsTargetEditorCommand::OpenKLOptionsTargetEditorCommand");
};

OpenKLOptionsTargetEditorCommand::~OpenKLOptionsTargetEditorCommand()
{
}

bool OpenKLOptionsTargetEditorCommand::canUndo() 
{
  return false;
}

bool OpenKLOptionsTargetEditorCommand::canLog() 
{
  return m_canLog;
}

bool OpenKLOptionsTargetEditorCommand::doIt() 
{ 
  bool res = false;

  FABRIC_CATCH_BEGIN();

  QString editorID = getRTValArgValue(
    "editorID"
    ).getStringCString();

  QDockWidget* dock = QtUtil::getDockWidget(editorID);
  if(dock != 0)
  {
    if(dock->isHidden())
      dock->show();
    else
      m_canLog = false;
  }

  else
  {
    QString editorTitle = getRTValArgValue(
      "editorTitle"
      ).getStringCString();

    QString groupName = isArgSet("groupName")
      ? getRTValArgValue("groupName").getStringCString()
      : QString();

    dock = KLOptionsTargetEditor::create(
      editorID,
      editorTitle,
      groupName);
  }

  if( dock ) {
    KLOptionsTargetEditor *optionsEditor = (KLOptionsTargetEditor*)dock->widget();
    if( optionsEditor )
      optionsEditor->refreshKLOptions();
  }

  res = true;

  FABRIC_CATCH_END("OpenKLOptionsTargetEditorCommand::doIt");

  return res;
}

QString OpenKLOptionsTargetEditorCommand::getHelp()
{
  QMap<QString, QString> argsHelp;

  argsHelp["editorID"] = "Qt objectName of the option editor / ID of the KL option in the OptionsTargetRegistry";
  argsHelp["editorTitle"] = "Title of the widget";
  argsHelp["groupName"] = "Name of the options' group";
  
  return CommandHelpers::createHelpFromRTValArgs(
    this,
    "Open a Qt editor to edit a KL OptionsTarget",
    argsHelp);
}

QString OpenKLOptionsTargetEditorCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;

  argsDesc["editorID"] = getRTValArgValue(
    "editorID").getStringCString();

  argsDesc["editorTitle"] = getRTValArgValue(
    "editorTitle").getStringCString();

  return CommandHelpers::createHistoryDescFromArgs(
    this,
    argsDesc);
}
