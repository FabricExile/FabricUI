//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

const char FabricUI::OptionsEditor::OptionsEditorHelpers::pathSeparator = '/';
const char FabricUI::OptionsEditor::OptionsEditorHelpers::arraySeparator = '_';

using namespace FabricUI;
using namespace OptionsEditor;
using namespace FabricCore;
using namespace Util;

inline RTVal GetKLSingleOption(
  int optIndex,
  QList<QString> singleOptionPaths,
  RTVal &options) 
{ 
  FABRIC_CATCH_BEGIN();

  options = RTValUtil::toRTVal(options);
  QString optionName = singleOptionPaths[optIndex];

  RTVal key = RTVal::ConstructString(
    options.getContext(),
    optionName.toUtf8().constData()); 

  if(options.isDict()) 
  {
    RTVal childrenOptions = RTValUtil::toRTVal(options.getDictElement(key)); 
    if(childrenOptions.isDict() || childrenOptions.isArray())
      return GetKLSingleOption(
        optIndex+1, 
        singleOptionPaths, 
        childrenOptions);

    else
      return childrenOptions;
  }

  else if(options.isArray()) 
  {
    int sepIndex = optionName.lastIndexOf(OptionsEditorHelpers::arraySeparator);
    int arrayIndex = optionName.mid(sepIndex+1).toInt();
    RTVal childrenOptions = RTValUtil::toRTVal(options.getArrayElementRef(arrayIndex)); 

    if(childrenOptions.isDict() || childrenOptions.isArray())
      GetKLSingleOption(
        optIndex+1, 
        singleOptionPaths, 
        childrenOptions);

    else
      return childrenOptions;
  }

  else
    return RTValUtil::toRTVal(options);

  FABRIC_CATCH_END("OptionsEditorHelpers::GetKLSingleOption");

  return RTVal();
}

inline void SetKLSingleOption(
  int optIndex,
  QList<QString> singleOptionPaths,
  RTVal singleOption,
  RTVal options) 
{ 
  FABRIC_CATCH_BEGIN();

  options = RTValUtil::toRTVal(options);

  QString optionName = singleOptionPaths[optIndex];

  RTVal key = RTVal::ConstructString(
    singleOption.getContext(),
    optionName.toUtf8().constData()); 

  if(options.isDict()) 
  {
    RTVal childrenOptions = RTValUtil::toRTVal(options.getDictElement(key)); 
    if(childrenOptions.isDict() || childrenOptions.isArray())
      SetKLSingleOption(
        optIndex+1, 
        singleOptionPaths, 
        singleOption,
        childrenOptions);

    else
      options.setDictElement(
        key, 
        RTValUtil::toKLRTVal(singleOption)
        );
  }

  else if(options.isArray()) 
  {
    int sepIndex = optionName.lastIndexOf(OptionsEditorHelpers::arraySeparator);
    int arrayIndex = optionName.mid(sepIndex+1).toInt();
    RTVal childrenOptions = RTValUtil::toRTVal(options.getArrayElementRef(arrayIndex)); 

    if(childrenOptions.isDict() || childrenOptions.isArray())
      SetKLSingleOption(
        optIndex+1, 
        singleOptionPaths, 
        singleOption,
        childrenOptions);

    else
      options.setArrayElement(
        arrayIndex, 
        RTValUtil::toKLRTVal(singleOption)
        );
  }

  else
    options = singleOption;

  FABRIC_CATCH_END("OptionsEditorHelpers::SetKLSingleOption");
}
  
RTVal OptionsEditorHelpers::getKLOptionsTargetRegistry() 
{
  FABRIC_CATCH_BEGIN();

  RTVal appOptionsTargetRegistry = RTVal::Construct(
    Application::FabricApplicationStates::GetAppStates()->getContext(),
    "AppOptionsTargetRegistry",
    0, 0);

  return appOptionsTargetRegistry.callMethod(
    "OptionsTargetRegistry",
    "getOptionsTargetRegistry",
    0, 0);

  FABRIC_CATCH_END("OptionsEditorHelpers::getKLOptionsTargetRegistry");

  return RTVal();
}
 
RTVal OptionsEditorHelpers::getKLOptionsTargetOptions(
  QString registryID) 
{
  FABRIC_CATCH_BEGIN();

  RTVal registryIDVal = RTVal::ConstructString(
    Application::FabricApplicationStates::GetAppStates()->getContext(),
    registryID.toUtf8().constData());
  
  return getKLOptionsTargetRegistry().callMethod(
    "RTVal", 
    "getTargetOptions",
    1,
    &registryIDVal);

  FABRIC_CATCH_END("OptionsEditorHelpers::getKLOptionsTargetRegistry");

  return RTVal();
}

RTVal OptionsEditorHelpers::getKLOptionsTargetSingleOption(
  QString path) 
{ 
  FABRIC_CATCH_BEGIN();

  int index = path.indexOf(pathSeparator);
  QString registryID = path.midRef(0, index).toUtf8().constData();
  RTVal options = getKLOptionsTargetOptions(registryID);

  return GetKLSingleOption(
    1,
    path.split(pathSeparator),
    options);

  FABRIC_CATCH_END("OptionsEditorHelpers::getKLOptionsTargetSingleOption");

  return RTVal();
}

void OptionsEditorHelpers::setKLOptionsTargetSingleOption(
  QString path,
  RTVal singleOption) 
{ 
  FABRIC_CATCH_BEGIN();

  int index = path.indexOf(pathSeparator);
  QString registryID = path.midRef(0, index).toUtf8().constData();

  RTVal options = getKLOptionsTargetOptions(registryID);

  SetKLSingleOption(
    1,
    path.split(pathSeparator),
    singleOption,
    options);

  RTVal args[2] = 
  {
    RTVal::ConstructString(
      options.getContext(), 
      registryID.toUtf8().constData())
    ,
    RTVal::Construct(
      options.getContext(), 
      "RTVal",
      1,
      &options)
  };

  getKLOptionsTargetRegistry().callMethod(
    "", 
    "setTargetOptions",
    2,
    args);

  FABRIC_CATCH_END("OptionsEditorHelpers::setKLOptionsTargetSingleOption");
}
