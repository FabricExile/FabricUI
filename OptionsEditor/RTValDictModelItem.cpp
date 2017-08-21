//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "RTValDictModelItem.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace ValueEditor; 
using namespace Application;
using namespace OptionsEditor;

RTValDictModelItem::RTValDictModelItem(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor *editor,
  RTVal options) 
  : BaseRTValModelItem(name, path)
{
  FABRIC_CATCH_BEGIN();

  m_options = options;
  
  RTVal keys = m_options.getDictKeys();
  for(unsigned i = 0; i < keys.getArraySize(); i++) 
  {
    RTVal key = keys.getArrayElementRef(i); 
    RTVal childrenOptions = m_options.getDictElement(key); 
    std::string childName = key.getStringCString();

    BaseRTValModelItem* item = editor->constructModel(
      childName,
      m_path,
      editor,
      childrenOptions);

    m_children[childName] = item;
    m_keys.push_back(childName);
  }

  FABRIC_CATCH_END("RTValDictModelItem::RTValDictModelItem");
}
 
RTValDictModelItem::~RTValDictModelItem() 
{
}

int RTValDictModelItem::getNumChildren() 
{ 
  return m_children.size(); 
}

BaseModelItem* RTValDictModelItem::getChild(
  FTL::StrRef childName, 
  bool doCreate) 
{ 
  return m_children[childName.data()]; 
}

BaseModelItem* RTValDictModelItem::getChild(
  int index, 
  bool doCreate) 
{ 
  return m_children[m_keys[index]]; 
}

void RTValDictModelItem::resetToDefault() 
{
  std::map<std::string, BaseRTValModelItem*>::iterator it;
  for (it = m_children.begin(); it != m_children.end(); it++) 
    it->second->resetToDefault();
}

RTVal RTValDictModelItem::getRTValOptions()
{
  FABRIC_CATCH_BEGIN();

  std::map<std::string, BaseRTValModelItem*>::iterator it;
  for(it = m_children.begin(); it != m_children.end(); it++) 
  {
    RTVal key = RTVal::ConstructString(
      FabricApplicationStates::GetAppStates()->getContext(),
      it->first.data());

    BaseRTValModelItem *child = (BaseRTValModelItem *)it->second;

    m_options.setDictElement(
      key,
      RTValUtil::toKLRTVal(child->getRTValOptions())
      );
  }
  
  FABRIC_CATCH_END("RTValDictModelItem::getRTValOptions");

  return m_options;
}

void RTValDictModelItem::setRTValOptions(
  RTVal options) 
{
  FABRIC_CATCH_BEGIN();

  m_options = RTValUtil::toRTVal(options);

  if(!m_options.isDict())
    FabricException::Throw(
      "RTValDictModelItem::setRTValOptions",
      "Options is not a dictionay" 
      );

  std::map<std::string, BaseRTValModelItem*>::iterator it;
  for(it=m_children.begin(); it!=m_children.end(); it++) 
  {
    RTVal key = RTVal::ConstructString(
      m_options.getContext(),
      it->first.data());

    RTVal childrenOptions = m_options.getDictElement(
      key); 

    BaseRTValModelItem *child = (BaseRTValModelItem *)it->second;
     
    child->setRTValOptions(childrenOptions);
  }

  FABRIC_CATCH_END("RTValDictModelItem::setRTValOptions");
}
