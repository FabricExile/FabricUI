//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_VARITEMMETADATA_H
#define FABRICUI_MODELITEMS_VARITEMMETADATA_H

#include "DFGModelItemMetadata.h"

namespace FabricUI {
namespace ModelItems {

class VarModelItem;

class VarItemMetadata : public DFGModelItemMetadata
{
public:

  VarItemMetadata( VarModelItem *varModelItem )
  : m_varModelItem(varModelItem)
  {}

  virtual const char* getString( const char* key ) const; /*override*/

private:
  VarModelItem * m_varModelItem;
};

} // namespace ModelItems
} // namespace FabricUI

#endif // FABRICUI_MODELITEMS_VARITEMMETADATA_H
