//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_PATH_VALUE_RESOLVER__
#define __UI_OPTIONS_PATH_VALUE_RESOLVER__

#include <FabricUI/Commands/BasePathValueResolver.h>

namespace FabricUI {
namespace OptionsEditor {

class OptionsPathValueResolver : public Commands::BasePathValueResolver
{
  /**
    OptionsPathValueResolver  
  */  
  Q_OBJECT

  public:
    OptionsPathValueResolver();

    virtual ~OptionsPathValueResolver();
      
    /// Implementation of BasePathValueResolver.
    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );
 
    /// Implementation of BasePathValueResolver.
    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual void setValue(
      FabricCore::RTVal pathValue
      );

  private:
    bool hasOptions(
      FabricCore::RTVal pathValue
      );
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_OPTIONS_PATH_VALUE_RESOLVER__
