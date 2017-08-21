//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_MODEL_ITEM__
#define __UI_RTVAL_MODEL_ITEM__

#include "BaseRTValModelItem.h"
#include "BaseRTValOptionsEditor.h"

namespace FabricUI {
namespace OptionsEditor {

class RTValModelItem : public BaseRTValModelItem 
{
  /**
    RTValModelItem specializes BaseRTValModelItem 
    for RTVal of basic type (leaf).
  */    
  Q_OBJECT
  
  public:
    /// Constructs a RTValModelItem.
    /// \param name Name of the item.
    /// \param path Item's path.
    /// \param editor Pointor to the OptionsEditor.
    /// \param options Item's options.
    RTValModelItem(
      const std::string &name,
      const std::string &path,
      BaseRTValOptionsEditor *editor,
      FabricCore::RTVal options
      );

    virtual ~RTValModelItem();

    /// Implementation of ValueEditor::BaseModelItem
    virtual QVariant getValue();
  
    /// Implementation of ValueEditor::BaseModelItem
    virtual void setValue(
      QVariant value,
      bool commit = false,
      QVariant valueAtInteractionBegin = QVariant()
      );

    /// Implementation of ValueEditor::BaseModelItem
    virtual void resetToDefault();

    /// Implementation of BaseRTValModelItem.
    virtual FabricCore::RTVal getRTValOptions();

    /// Implementation of BaseRTValModelItem.
    virtual void setRTValOptions(
      FabricCore::RTVal options
      );

  signals:
    /// Emitted when the value of one option has changed.
    void updated();
    
  protected:
    /// Current value
    FabricCore::RTVal m_options; 
    /// Current value
    FabricCore::RTVal m_originalOptions; 
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_RTVAL_MODEL_ITEM__
