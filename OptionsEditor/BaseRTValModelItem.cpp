//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseRTValModelItem.h"
#include "OptionsEditorHelpers.h"

using namespace FabricUI;
using namespace ValueEditor;
using namespace OptionsEditor;
 
BaseRTValModelItem::BaseRTValModelItem(
  const std::string &name,
  const std::string &path) 
  : BaseModelItem()
  , m_name(name)
{
	// Don't add the separator at
	// the begining of the path.
 	m_path = (path.length() > 0)
		?	path + OptionsEditorHelpers::pathSeparator + name
		: name;
}

BaseRTValModelItem::~BaseRTValModelItem()
{
}

bool BaseRTValModelItem::hasDefault() 
{ 
  return true; 
}

FTL::CStrRef BaseRTValModelItem::getName() 
{ 
  return m_name; 
}

FTL::CStrRef BaseRTValModelItem::getPath() 
{ 
  return m_path; 
}
