//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_ARRAY_MODEL_ITEM__
#define __UI_RTVAL_ARRAY_MODEL_ITEM__

#include "BaseRTValModelItem.h"
#include "BaseRTValOptionsEditor.h"

namespace FabricUI {
namespace OptionsEditor {

class RTValArrayModelItem : public BaseRTValModelItem 
{
  /**
    RTValArrayModelItem specializes BaseRTValModelItem 
    for RTVals arrays.
  */  
  Q_OBJECT
  
  public:
    /// Constructs a RTValArrayModelItem.
    /// \param name Name of the item.
    /// \param path Item's path.
    /// \param editor Pointor to the OptionsEditor.
    /// \param options Item's options.
    RTValArrayModelItem(
      const std::string &name,
      const std::string &path,
      BaseRTValOptionsEditor *editor,
      FabricCore::RTVal options
      );

    virtual ~RTValArrayModelItem();

    /// Implementation of ValueEditor::BaseModelItem
    virtual int getNumChildren();

    /// Implementation of ValueEditor::BaseModelItem
    virtual ValueEditor::BaseModelItem* getChild(
      FTL::StrRef childName, 
      bool doCreate
      );

    /// Implementation of ValueEditor::BaseModelItem
    virtual ValueEditor::BaseModelItem* getChild(
      int index, 
      bool doCreate
      );
    
    /// Implementation of ValueEditor::BaseModelItem
    virtual void resetToDefault();

    /// Implementation of BaseRTValModelItem.
    virtual FabricCore::RTVal getRTValOptions();

    /// Implementation of BaseRTValModelItem.
    /// Throws an error if 'options' isn't a RTVal array.  
    virtual void setRTValOptions(
      FabricCore::RTVal options
      );

  private:  
    /// Lists of the keys.
    std::vector<std::string> m_keys;
    /// Dictionary [key, value]
    std::map<std::string, BaseRTValModelItem*> m_children;
    FabricCore::RTVal m_options;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_RTVAL_ARRAY_MODEL_ITEM__
