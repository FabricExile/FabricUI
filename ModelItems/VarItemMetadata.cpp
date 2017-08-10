//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "VarItemMetadata.h"
#include "VarModelItem.h"

namespace FabricUI {
namespace ModelItems {

const char* VarItemMetadata::getString( const char* key ) const /*override*/
{
  if ( key == FTL_STR("vePortType") )
    return FTL_STR("IO").c_str();
  if ( key == FTL_STR("vePortType") )
    return FTL_STR("IO").c_str();

  if ( key == FTL_STR("uiReadOnly") )
  {
    if(m_varModelItem->shouldBeReadOnly())
      return "1";
  }
  return 0;
}

}
}
