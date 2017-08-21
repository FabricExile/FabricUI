//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SET_PATH_VALUE_COMMAND__
#define __UI_SET_PATH_VALUE_COMMAND__

#include "BaseRTValScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class SetPathValueCommand : public BaseRTValScriptableCommand
{
  Q_OBJECT
  
  public:
    SetPathValueCommand();

    virtual ~SetPathValueCommand();

    /// Implementation of BaseCommand.
    virtual bool canUndo();

    /// Implementation of BaseCommand.
    virtual bool doIt();

    /// Implementation of BaseCommand.
    virtual bool undoIt();

     /// Implementation of BaseCommand.
    virtual bool redoIt();

    /// Implementation of BaseCommand.
    virtual QString getHistoryDesc();

    /// Implementation of BaseCommand.
    virtual QString getHelp();

    /// Implementation of BaseCommand.
    virtual void merge(
      BaseCommand *prevCmd
      );

  private:
    FabricCore::RTVal m_prevValue;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_SET_PATH_VALUE_COMMAND__
