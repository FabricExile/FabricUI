//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_BINDINGMODELITEM_H
#define FABRICUI_MODELITEMS_BINDINGMODELITEM_H

#include <FabricUI/ModelItems/RootModelItem.h>

namespace FabricUI
{
namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Specialize Exec for special case exterior
    class BindingModelItem : public RootModelItem
    {
    private:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;

      FabricCore::DFGExec m_rootExec;

    public:

      BindingModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        bool showInputs = true,
        bool showOutputs = true,
        bool showIO = true
        );
      ~BindingModelItem();

      virtual bool isBinding() const /*override*/ { return true; }

      FabricUI::ValueEditor::BaseModelItem *
      createChild( FTL::StrRef name ) /*override*/;

      virtual int getNumChildren() /*override*/;

      virtual FTL::CStrRef getChildName( int i ) /*override*/;

      /////////////////////////////////////////////////////////////////////////
      // Name
      /////////////////////////////////////////////////////////////////////////

      virtual FTL::CStrRef getName() /*override*/;


      virtual bool canRename() /*override*/;

      virtual void rename( FTL::CStrRef newName ) /*override*/;

      virtual void onRenamed(
        FTL::CStrRef oldName,
        FTL::CStrRef newName
        ) /*override*/;

      /////////////////////////////////////////////////////////////////////////
      // Value
      /////////////////////////////////////////////////////////////////////////
      virtual QVariant getValue() /*override*/;


      /////////////////////////////////////////////////////////////////////////
      // Metadata
      /////////////////////////////////////////////////////////////////////////

      virtual void setMetadataImp( const char* key, 
                                const char* value, 
                                bool canUndo )/*override*/;

    protected:

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;

      bool m_showInputs;
      bool m_showOutputs;
      bool m_showIO;
    };
  }
}

#endif // FABRICUI_MODELITEMS_BINDINGMODELITEM_H
