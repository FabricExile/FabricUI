//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_FABRIC_APPLICATION_STATES__
#define __UI_FABRIC_APPLICATION_STATES__

#include <QSettings>
#include <FabricCore.h>
 
namespace FabricUI {
namespace Application {

class FabricApplicationStates  
{
  /**
    FabricApplicationStates singleton provides a global access
    to the FabricCore context/client and application Settings 
    that are shared by all the user-interface components.
    
    Warning: creating a FabricApplicationStates is interpreted as an application init or reset!

    Usage:
    - Create the singleton : new FabricApplicationStates(client, settings); 
    - Get the singleton : FabricApplicationStates* states = FabricApplicationStates::GetAppStates();
  */

  public:
    FabricApplicationStates(
      FabricCore::Client client,
      QSettings *settings = 0
      );

    virtual ~FabricApplicationStates();

    /// Gets the App states singleton.
    /// Throws an error if the singleton has not been crated.
    static FabricApplicationStates* GetAppStates();
    
    /// Gets the FabricCore context.
    virtual FabricCore::Context getContext();
        
    /// Gets the FabricCore client.
    virtual FabricCore::Client getClient();

    QSettings* getSettings();

  private:
    /// Reference to the core client.
    FabricCore::Client m_client;
    /// Pointor to tha app settings.
    QSettings *m_settings;

    /// ApplicationStates singleton, set from Constructor.
    static FabricApplicationStates *s_appStates;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
};

} // namespace Application
} // namespace FabricUI

#endif // __UI_FABRIC_APPLICATION_STATES__
