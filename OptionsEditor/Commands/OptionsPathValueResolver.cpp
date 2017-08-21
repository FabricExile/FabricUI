//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QDockWidget>
#include <QStringList>
#include <FabricUI/Util/QtUtil.h>
#include "../OptionsEditorHelpers.h"
#include "../BaseRTValOptionsEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include "OptionsPathValueResolver.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace Application;
using namespace OptionsEditor;

OptionsPathValueResolver::OptionsPathValueResolver()
  : BasePathValueResolver()
{
}

OptionsPathValueResolver::~OptionsPathValueResolver()
{
}

bool OptionsPathValueResolver::hasOptions(
  RTVal pathValue)
{
  bool hasOption = false;

  FABRIC_CATCH_BEGIN();

  pathValue = RTValUtil::toRTVal(
    pathValue);

  QString path = pathValue.maybeGetMember(
    "path").getStringCString();

  int index = path.indexOf(OptionsEditorHelpers::pathSeparator);
  
  RTVal id = RTVal::ConstructString(
    pathValue.getContext(),
    path.midRef(0, index).toUtf8().constData()
    );

  RTVal optionsTarget = OptionsEditorHelpers::getKLOptionsTargetRegistry().callMethod(
    "OptionsTarget",
    "getTarget",
    1,
    &id);

  hasOption = optionsTarget.isValid() && !optionsTarget.isNullObject();

  FABRIC_CATCH_END("OptionsPathValueResolver::hasOptions");

  return hasOption;
}

bool OptionsPathValueResolver::knownPath(
  RTVal pathValue)
{
  return/* !m_evalContextID.isEmpty()
    ? BasePathValueResolver::knownPath(pathValue) && hasOptions(pathValue)
    :*/ hasOptions(pathValue);
}

QString OptionsPathValueResolver::getType(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  getValue(pathValue);

  return RTValUtil::getType(
    RTValUtil::toRTVal(pathValue.maybeGetMember("value"))
    );

  FABRIC_CATCH_END("OptionsPathValueResolver::getType");

  return "";
}

void OptionsPathValueResolver::getValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  pathValue = RTValUtil::toRTVal(
    pathValue);

  QString path = pathValue.maybeGetMember(
    "path").getStringCString();

  RTVal singleOption = OptionsEditorHelpers::getKLOptionsTargetSingleOption(
    path);

  pathValue.setMember(
    "value", 
    singleOption);

  FABRIC_CATCH_END("OptionsPathValueResolver::getValue");
}

void OptionsPathValueResolver::setValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  pathValue = RTValUtil::toRTVal(
    pathValue);

  QString path = pathValue.maybeGetMember(
    "path").getStringCString();
  
  RTVal option = pathValue.maybeGetMember(
    "value");

  OptionsEditorHelpers::setKLOptionsTargetSingleOption(
    path,
    option);
  
  int index = path.indexOf(OptionsEditorHelpers::pathSeparator);
  
  QDockWidget *dock = QtUtil::getDockWidget(
    path.midRef(0, index).toUtf8().constData());

  if(dock == 0)
    FabricException::Throw(
      "OptionsPathValueResolver::setValue",
      "dock is null");

  BaseRTValOptionsEditor *editor = qobject_cast<BaseRTValOptionsEditor*>(dock->widget());

  if(editor == 0)
    FabricException::Throw(
      "OptionsPathValueResolver::setValue",
      "editor is null");
  
  editor->updateModel();

  FABRIC_CATCH_END("OptionsPathValueResolver::setValue");
}
