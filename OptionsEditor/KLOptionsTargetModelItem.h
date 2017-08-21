//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_MODEL_ITEM__
#define __UI_KL_MODEL_ITEM__

#include "RTValModelItem.h"

namespace FabricUI {
namespace OptionsEditor {

class KLOptionsTargetModelItem : public RTValModelItem
{
  /**
    KLOptionsTargetModelItem specializes RTValModelItem 
    for simple RTVal of KL OptionsTargets.
  */    
  Q_OBJECT
  
  public:
    /// Constructs a KLOptionsTargetModelItem.
    /// \param name Name of the item.
    /// \param path Item's path.
    /// \param editor Pointor to the OptionsEditor.
    /// \param options Item's options.
    /// \param settings Pointor to the settings.
    KLOptionsTargetModelItem(
      const std::string &name,
      const std::string &path,
      BaseRTValOptionsEditor *editor,
      FabricCore::RTVal options
      );

    virtual ~KLOptionsTargetModelItem();

    /// Implementation of ValueEditor::BaseModelItem
    /// Create a command for undo-redo.
    virtual void setValue(
      QVariant value,
      bool commit = false,
      QVariant valueAtInteractionBegin = QVariant()
      );

    private:
      int m_canMergeID;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_KL_MODEL_ITEM__
