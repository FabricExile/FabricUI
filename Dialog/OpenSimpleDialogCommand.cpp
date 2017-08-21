//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QCursor>
#include "SimpleDialog.h"
#include <FabricUI/Util/QtUtil.h>
#include "OpenSimpleDialogCommand.h"
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace Dialog;
using namespace Commands;
using namespace Application;

OpenSimpleDialogCommand::OpenSimpleDialogCommand() 
  : BaseScriptableCommand()
{
  FABRIC_CATCH_BEGIN();

  declareArg("title", false, "");
  declareArg("text", false, "");

  FABRIC_CATCH_END("OpenSimpleDialogCommand::OpenSimpleDialogCommand");
}

OpenSimpleDialogCommand::~OpenSimpleDialogCommand() 
{
}

bool OpenSimpleDialogCommand::canUndo()
{
  return false;
}

bool OpenSimpleDialogCommand::canLog()
{
  return false;
}

bool OpenSimpleDialogCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  QMainWindow *mainWindow = QtUtil::getMainWindow();

  SimpleDialog *dialog = new SimpleDialog(
    getArg("title"), 
    getArg("text"), 
    mainWindow);

  QPoint pos = QCursor::pos();
  dialog->move(pos.x(), pos.y());

  dialog->show();
  return true;

  FABRIC_CATCH_END("OpenSimpleDialogCommand::doIt");

  return false;
}

QString OpenSimpleDialogCommand::getHelp()
{
  FABRIC_CATCH_BEGIN();

  QMap<QString, QString> argsHelp;
  argsHelp["title"] = "Title of the Dialog widget";
  argsHelp["text"] = "Text to display";

  return CommandHelpers::createHelpFromArgs(
    this,
    "Open a Simple dialog placed at the current mouse position",
    argsHelp);

  FABRIC_CATCH_END("OpenSimpleDialogCommand::getHelp");

  return "";
}
