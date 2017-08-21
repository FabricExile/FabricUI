//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandManager.h"
#include "CommandRegistry.h"
#include "PathValueResolverRegistry.h"

namespace FabricUI {
  namespace Commands {

    // Cleans up static app data.
    // Normally this shouldn't be required, however unit tests are running multiple apps sequencially in the same process.
    void ResetCommandStatesOnNewClient() {
      if( CommandManager::isInitalized() )
        CommandManager::getCommandManager()->clear();
      if( CommandRegistry::isInitalized() )
        CommandRegistry::getCommandRegistry()->clear();
      PathValueResolverRegistry::getRegistry()->clear();
    }

  } // namespace Commands
} // namespace FabricUI
