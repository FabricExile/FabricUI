//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND_MANAGER__
#define __UI_KL_COMMAND_MANAGER__
 
#include "RTValCommandManager.h"

namespace FabricUI {
namespace Commands {

class KLCommandManager : public RTValCommandManager
{
  /**
    KLCommandManager has a reference to the KL command manager so it can create KL commands. 
    However, the C++ manager isn't automatically synchronized with the KL manager. If commands 
    are added to the KL manager directly, the C++ manager needs explicitly to synchronize with it. 
    
    Two scenarios are possible. First, the KL manager creates KL commands. Specials C++ commands 
    (KLCommand-KLScriptableCommand) that wrap KL commands are created. Secondly, C++ commands are 
    asked to be created frrom KL. An 'AppCommand' command  is created in KL. When the two managers 
    are synchronized, the C++ manager detects the KL 'AppCommand', and creates the real C++ command.
    
    The method `synchronizeKL` must be call to synchronize the managers.
  */
  Q_OBJECT

  public:
    KLCommandManager();

    virtual ~KLCommandManager();
  
    /// Implementation of CommandManager
    virtual void clear();
   
    /// Implementation of CommandManager
    virtual QString getContent(
      bool withArgs = true
      );

    /// Implementation of CommandManager
    virtual int getNewCanMergeID();
    
  public slots:
    /// Synchronizes with the KL manager.
    /// To call after KL commands' been created in KL.
    void synchronizeKL();

  protected:
    /// Implementation of CommandManager
    virtual void clearRedoStack();
   
  private:
    /// Creates a C++ commands 
    void createAppCommand(
      FabricCore::RTVal appCmd
      );

    /// Creates a C++ commands 
    void doKLCommand(
      FabricCore::RTVal klCmd
      );
};
 
} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_MANAGER__
