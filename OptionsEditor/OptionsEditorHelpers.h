//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_EDITOR_HELPERS__
#define __UI_OPTIONS_EDITOR_HELPERS__

#include <QString>
#include <FabricCore.h>
    
namespace FabricUI {
namespace OptionsEditor {
 
class OptionsEditorHelpers 
{
  /**
    Set of helpers for Options editor.
  */  

  public:
    static const char pathSeparator;
    static const char arraySeparator;

    static FabricCore::RTVal getKLOptionsTargetRegistry();

    /// Gets the options of a KL OptionsTarget.
    /// \param registryID ID of the KL OptionsEditorTarget in registry.
    static FabricCore::RTVal getKLOptionsTargetOptions(
      QString registryID
      );

    /// Sets a single option of a KL OptionsTarget.
    /// \param registryID ID of the KL OptionsEditorTarget in registry.
    /// \param singleOptionPath Path of the single options if the options hierarchy.
    /// \param singleOption Single option.
    static void setKLOptionsTargetSingleOption(
      QString singleOptionPath,
      FabricCore::RTVal singleOption
      ); 

    /// Sets a single option of a KL OptionsTarget.
    /// \param registryID ID of the KL OptionsEditorTarget in registry.
    /// \param singleOptionPath Path of the single options if the options hierarchy.
    /// \param singleOption Single option.
    static FabricCore::RTVal getKLOptionsTargetSingleOption(
      QString singleOptionPath
      ); 
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPTIONS_EDITOR_HELPERS__
