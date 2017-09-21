//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include "KLOptionsTargetModelItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Commands/RTValCommandManager.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

KLOptionsTargetModelItem::KLOptionsTargetModelItem(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor* editor,
  RTVal options) 
  : RTValModelItem(
    name, 
    path, 
    editor, 
    options)
  , m_canMergeID(CommandManager::NoCanMergeID)
{   
}

KLOptionsTargetModelItem::~KLOptionsTargetModelItem()
{
}

void KLOptionsTargetModelItem::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin) 
{
  FABRIC_CATCH_BEGIN();

  RTValCommandManager *manager = qobject_cast<RTValCommandManager*>(
    CommandManager::getCommandManager());

  if(m_canMergeID == CommandManager::NoCanMergeID)
    m_canMergeID = manager->getNewCanMergeID();

  QMap<QString, RTVal> args;

  RTVal pathVal = RTVal::ConstructString(
    m_options.getContext(), 
    m_path.c_str());

  args["target"] = RTVal::Construct(
    m_options.getContext(), 
    "PathValue",
    1,
    &pathVal);

  RTVal optionsCopy = m_options.clone();
  RTVariant::toRTVal(value, optionsCopy);
  args["newValue"] = optionsCopy;

  BaseCommand* cmd = manager->createCommand(
    "setPathValue",
    args, 
    false, 
    m_canMergeID);

  if( !commit )
    cmd->blockLog();

  try
  {
    manager->doCommand( cmd, m_canMergeID );
  }

  catch(Application::FabricException &e)
  {
    Application::FabricException::Throw(
      "KLOptionsTargetModelItem::getRTValOptions",
      "",
      e.what(),
      Application::FabricException::LOG);
  }

  if( commit )
    m_canMergeID = CommandManager::NoCanMergeID;

  FABRIC_CATCH_END("KLOptionsTargetModelItem::getRTValOptions");
}
