//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_ITEMPORTITEMMETADATA_H
#define FABRICUI_MODELITEMS_ITEMPORTITEMMETADATA_H

#include "DFGModelItemMetadata.h"
#include <FabricUI/ModelItems/ItemPortModelItem.h>

namespace FabricUI
{
  namespace ModelItems
  {

    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class ItemPortItemMetadata : public DFGModelItemMetadata
    {
    protected:

      ItemPortModelItem *m_nodePortModelItem;
      std::string m_dfgPath;

      void reportFabricCoreException( FabricCore::Exception const &e ) const
      {
        printf( "[ERROR] %s\n", e.getDesc_cstr() );
      }

    public:

      ItemPortItemMetadata( ItemPortModelItem *nodePortModelItem );

      void computeDFGPath();

      virtual const char* getString( const char* key ) const /*override*/
      {
        try
        {
          FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
          FTL::CStrRef portPath = m_nodePortModelItem->getPortPath();

          if (strcmp( "uiReadOnly", key ) == 0)
          {
            // Override disabled for connected ports
            if ( exec.hasSrcPorts( portPath.c_str() ) )
              return "1";
            if (exec.editWouldSplitFromPreset())
              return "1";
          }

          if ( key == VEPortTypeKey )
          {
            FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
            FTL::CStrRef portPath = m_nodePortModelItem->getPortPath();
            return DFGPortTypeToVEPortType(
              exec.getPortType( portPath.c_str() )
              ).c_str();
          }

          if ( key == VENotInspectableKey )
          {
            FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
            FTL::CStrRef portPath = m_nodePortModelItem->getPortPath();
            bool isNotInspectable =
              exec.getPortType( portPath.c_str() ) == FabricCore::DFGPortType_Out
                || exec.hasSrcPorts( portPath.c_str() );
            return isNotInspectable? FTL_STR("1").c_str(): FTL_STR("").c_str();
          }

          if( key == VEPathKey  )
            return m_dfgPath.data();

          return exec.getPortMetadata( portPath.c_str(), key );
        }
        catch ( FabricCore::Exception e )
        {
          reportFabricCoreException( e );
          return NULL;
        }
      }
    };
  }
}

#endif // FABRICUI_MODELITEMS_ITEMPORTITEMMETADATA_H
