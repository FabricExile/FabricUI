//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "FabricException.h"
#include "FabricApplicationStates.h"

namespace FabricUI {
  namespace Commands {
    void ResetCommandStatesOnNewClient();
  }
}

using namespace FabricUI;
using namespace FabricCore;
using namespace Application;

bool FabricApplicationStates::s_instanceFlag = false;
FabricApplicationStates* FabricApplicationStates::s_appStates = 0;

FabricApplicationStates::FabricApplicationStates(
  Client client,
  QSettings *settings)
{
  // We accept a new FabricApplicationStates only if the client changes (which can happen with our unit tests)
  if(s_appStates != 0 && s_appStates->m_client.getContext() == client.getContext() )
    FabricException::Throw(
      "FabricApplicationStates::FabricApplicationStates",
      "the singleton has been set already");

  m_client = client;
  m_settings = settings;

  s_appStates = this;
  s_instanceFlag = true;

  Commands::ResetCommandStatesOnNewClient();
}

FabricApplicationStates::~FabricApplicationStates()
{
  if( s_appStates == this )
    s_appStates = 0;
  s_instanceFlag = false;
}

FabricApplicationStates* FabricApplicationStates::GetAppStates() 
{
  if(!s_appStates)
    FabricException::Throw(
      "FabricApplicationStates::GetAppStates",
      "the singleton is null");

  return s_appStates;
}
 
QSettings* FabricApplicationStates::getSettings()
{
  return m_settings;
}

Client FabricApplicationStates::getClient()
{
  return m_client;
}

Context FabricApplicationStates::getContext()
{
  return m_client.getContext();
}
      
