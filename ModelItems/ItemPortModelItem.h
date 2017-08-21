//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_ITEMPORTMODELITEM_H
#define FABRICUI_MODELITEMS_ITEMPORTMODELITEM_H

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FTL/StrRef.h>

namespace FabricUI
{

namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {

    class ItemPortItemMetadata;

    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing ports
    class ItemPortModelItem : public FabricUI::ValueEditor::BaseModelItem
    {
    protected:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      std::string m_itemPath;
      std::string m_portName;
    
      std::string m_portPath;

      ItemPortItemMetadata *m_metadata;

    public:

      ItemPortModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef itemPath,
        FTL::StrRef portName
        );
      ~ItemPortModelItem();

      FabricCore::DFGExec getExec()
        { return m_exec; }
      inline const FabricCore::DFGBinding& getBinding() const
        { return m_binding; }
      FTL::CStrRef getItemPath()
        { return m_itemPath; }
      FTL::CStrRef getExecPath()
        { return m_execPath; }
      FTL::CStrRef getPortName()
        { return m_portName; }
      FTL::CStrRef getPortPath()
        { return m_portPath; }

      void onItemRenamed(
        FTL::CStrRef oldItemPath,
        FTL::CStrRef newItemPath
        );

      /////////////////////////////////////////////////////////////////////////
      // MetadatagetWidgetCommandArgs
      /////////////////////////////////////////////////////////////////////////

      virtual FabricUI::ValueEditor::ItemMetadata* getMetadata();
      virtual void setMetadataImp( const char* key,
                                const char* value,
                                bool canUndo )/*override*/;


      /////////////////////////////////////////////////////////////////////////
      // Value
      /////////////////////////////////////////////////////////////////////////

      virtual QVariant getValue();

      virtual QString getCommandName();
      
      virtual FabricCore::RTVal getCommandArgs();
            
    protected:

      void updatePortPath();

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;

      bool hasDefault() FTL_OVERRIDE;
      void resetToDefault() FTL_OVERRIDE;

      void reportFabricCoreException( FabricCore::Exception const &e );
    };
  }
}

#endif // FABRICUI_MODELITEMS_ITEMPORTMODELITEM_H
