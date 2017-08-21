/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_MANIPULATION_TOOL__
#define __UI_MANIPULATION_TOOL__

#include <QEvent>
#include <FabricCore.h>
#include <Commands/Command.h>
 
namespace FabricUI {
namespace Viewports {

class ManipulationCmd : FabricServices::Commands::Command
{    
  public:
    ManipulationCmd(); 

    virtual ~ManipulationCmd(); 
    
    virtual const char* getName() const;
    
    virtual const char* getShortDesc() const;
    
    virtual const char* getFullDesc() const;

    static void setStaticRTValCommands(
      FabricCore::RTVal commands
      );

  protected:
    virtual bool invoke();
    
    virtual bool undo();

    // We set the static commands pointer, and then construct the command. 
    static FabricCore::RTVal s_rtval_commands;

  private:
    FabricCore::RTVal m_rtval_commands;
};

class ManipulationTool 
{
  public:
    ManipulationTool();

    ~ManipulationTool();

    /// Activates/deactivates the tools.
    void setActive( 
      bool active 
      );

    /// Checks if the manips are active.
    bool isActive();

    /// Sents the event to the KL 
    /// manipualtion framework.
    bool onEvent(
      FabricCore::RTVal klevent, 
      bool &redrawRequested, 
      QString &portManipulationRequested
      );
    
    /// Gets the last manipulated value.
    /// Depreciated
    FabricCore::RTVal getLastManipVal();

  private:
    bool m_active;
    FabricCore::RTVal m_lastToolValue;
    FabricCore::RTVal m_lastManipValue;
    FabricCore::RTVal m_eventDispatcher;
};

} // namespace Viewports
} // namespace FabricUI

#endif // __UI_MANIPULATION_TOOL__
