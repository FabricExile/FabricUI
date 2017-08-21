//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValModelItem.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace ValueEditor;
using namespace Application;
using namespace OptionsEditor;

RTValModelItem::RTValModelItem(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor* editor,
  RTVal options) 
  : BaseRTValModelItem(name, path)
{   
  FABRIC_CATCH_BEGIN();

  m_options = RTValUtil::toRTVal(options);
  m_originalOptions = m_options.clone();

  QSettings *settings = FabricApplicationStates::GetAppStates()->getSettings();
  
  // Fetching the value from the QSettings
  if(settings != 0 && settings->contains(m_path.data())) 
  {
    QString settingsValue = settings->value( 
      m_path.data() 
      ).value<QString>();

    m_options.setJSON(settingsValue.toUtf8().data());
  }

  QObject::connect(
    this,
    SIGNAL(updated()),
    editor,
    SLOT(modelUpdated())
    );

  FABRIC_CATCH_END("RTValModelItem::RTValModelItem");
}

RTValModelItem::~RTValModelItem()
{
}

QVariant RTValModelItem::getValue() 
{
  return toVariant(m_options.clone());
}

void RTValModelItem::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin) 
{
  // RTVariant::toRTVal might change the pointer in ioVal,
  // so we copy it to make sure that m_options will always point to the same place
  RTVal optionsCopy = m_options.clone();

  RTVariant::toRTVal( 
    value, 
    optionsCopy);

  m_options.assign(optionsCopy);

  QSettings *settings = FabricApplicationStates::GetAppStates()->getSettings();

  // Storing the value in the Settings
  if(settings != 0)
    settings->setValue( 
      m_path.data(),
      QString(m_options.getJSON().getStringCString())
      );

   emitModelValueChanged( getValue() );
   emit updated();
}

void RTValModelItem::resetToDefault() 
{
  setValue( 
    toVariant(m_originalOptions), 
    true, 
    getValue());
}

RTVal RTValModelItem::getRTValOptions()
{
  return RTValUtil::toKLRTVal(m_options);
}

void RTValModelItem::setRTValOptions(
  RTVal options) 
{
  RTValModelItem::setValue(
    toVariant(RTValUtil::toRTVal(options))
    );
}
