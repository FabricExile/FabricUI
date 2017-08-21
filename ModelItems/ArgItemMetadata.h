//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_ARGITEMMETADATA_H
#define FABRICUI_MODELITEMS_ARGITEMMETADATA_H

#include "DFGModelItemMetadata.h"

#include <FabricUI/ModelItems/ArgModelItem.h>

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{

  namespace ModelItems
  {

    class ArgItemMetadata : public DFGModelItemMetadata
    {
    private:

      ArgModelItem *m_argModelItem;
      std::string m_bindingId, m_portPath;

    public:

      ArgItemMetadata( ArgModelItem *argModelItem );

      void computeDFGPath();

      virtual const char* getString( const char* key ) const /*override*/
      {
        if ( key == FTL_STR("vePortType") )
        {
          FabricCore::DFGExec rootExec = m_argModelItem->getRootExec();
          FTL::CStrRef argName = m_argModelItem->getArgName();
          return DFGPortTypeToVEPortType(
            rootExec.getExecPortType( argName.c_str() )
            ).c_str();
        }

        if( key == DFGModelItemMetadata::VEDFGBindingIdKey )
          return m_bindingId.data();

        if( key == DFGModelItemMetadata::VEDFGPortPathKey )
          return m_portPath.data();

        FabricCore::DFGExec rootExec = m_argModelItem->getRootExec();
        FTL::CStrRef argName = m_argModelItem->getArgName();
        return rootExec.getExecPortMetadata( argName.c_str(), key );
      }

    };

  }
}

#endif // FABRICUI_MODELITEMS_ARGITEMMETADATA_H
