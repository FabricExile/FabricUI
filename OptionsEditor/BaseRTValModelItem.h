//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_MODEL_ITEM__
#define __UI_BASE_MODEL_ITEM__

#include <string>
#include <FabricCore.h>
#include <FabricUI/ValueEditor/BaseModelItem.h>

namespace FabricUI {
namespace OptionsEditor {

class BaseRTValModelItem : public ValueEditor::BaseModelItem 
{
  /**
    BaseRTValModelItem is the base class for any options item.
  */  
  Q_OBJECT
  
  public:
    /// Constructs a model item.
    /// \param name Name of the item.
    /// \param path Path of the item.
    BaseRTValModelItem(
      const std::string &name,
      const std::string &path
      );

    virtual ~BaseRTValModelItem();
 
    /// Implementation of BaseModelItem
    virtual bool hasDefault();

    /// Implementation of BaseModelItem
    virtual FTL::CStrRef getName();

    /// Get the path of the item (./.../item/child/...)
    FTL::CStrRef getPath();

    /// Sets the dictionary of options.
    virtual void setRTValOptions(
      FabricCore::RTVal options
      ) = 0;

    /// Gets the dictionary of options.
    virtual FabricCore::RTVal getRTValOptions() = 0;

  protected:
    /// Name of the item.
    std::string m_name;
    /// Path of the item.
    std::string m_path;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_BASE_MODEL_ITEM__
