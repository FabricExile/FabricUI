/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include "SHCmdHandler.h"
#include "SGAddObjectCmd.h"
#include "SGAddPropertyCmd.h"
#include "SGSetPropertyCmd.h"
#include "SGSetBooleanPropertyCmd.h"
#include "SGSetObjectPropertyCmd.h"
#include "SGSetPaintToolAttributeCmd.h"

using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;


class SHCmdHandler::WrappedCmd : public QUndoCommand {
  
  public:
    WrappedCmd(SHCmd *shCmd) 
      : QUndoCommand()
      , m_shCmd( shCmd )
      , m_didit( false ) {}

  protected:
    virtual void redo() 
    {
      try 
      {
        if(m_didit) m_shCmd->redo();
        else 
        {
          m_didit = true;
          m_shCmd->doit();
        }
        QUndoCommand::setText( m_shCmd->getDescription() );
      }
      catch (Exception e) {
        printf("SHCmdHandler::WrappedCmd error: %s\n", e.getDesc_cstr() );
      }
    }

    virtual void undo() {
      try 
      {
        assert(m_didit);
        m_shCmd->undo();
      }
      catch (Exception e) {
        printf("SHCmdHandler::WrappedCmd error: %s\n", e.getDesc_cstr() );
      }
    }

    FTL::OwnedPtr<SHCmd> m_shCmd;
    bool m_didit;
};

SHCmdHandler::SHCmdHandler(Client client, QUndoStack *qUndoStack)
  : m_stackSize(0)
  , m_client(client)
  , m_qUndoStack(qUndoStack) {
}

SHCmdHandler::~SHCmdHandler() {
}

void SHCmdHandler::registerCommands() { 
  SGAddObjectCmd addObjectCmd;
  addObjectCmd.registerCommand();

  SGAddPropertyCmd addPropertyCmd;
  addPropertyCmd.registerCommand();

  SGSetPropertyCmd setPropertyCmd;
  setPropertyCmd.registerCommand();

  SGSetBooleanPropertyCmd setBooleanPropertyCmd;
  setBooleanPropertyCmd.registerCommand();

  SGSetObjectPropertyCmd setObjectPropertyCmd;
  setObjectPropertyCmd.registerCommand();

  SGSetPaintToolAttributeCmd setPaintToolAttributeCmd;
  setPaintToolAttributeCmd.registerCommand();
}

void SHCmdHandler::onSynchronizeCommands() {
  try 
  {
    RTVal cmdManager = RTVal::Create(m_client, "LegacyCmdManager", 0, 0);
    cmdManager = cmdManager.callMethod("LegacyCmdManager", "getOrCreateLegacyCmdManager", 0, 0);

    unsigned int currentStackSize = cmdManager.callMethod("Size", "getNumCmdInUndoStack", 0, 0).getUInt32();

    // Get the number of commands already done in the KL stack
    for(unsigned int i=m_stackSize; i<currentStackSize; ++i)
    {        
      RTVal indexVal = RTVal::ConstructUInt32(m_client, i);
      RTVal cmdVal = cmdManager.callMethod("Cmd", "getCmdInUndoStack", 1, &indexVal);

      if(cmdVal.isValid() && !cmdVal.isNullObject())
      {
        SHCmd *cmd = 0;
   
        // Check if the command is registered
        int id = QMetaType::type(cmdVal.callMethod("String", "type", 0, 0).getStringCString());
        if(id != 0)
#if QT_VERSION >= 0x050000
          cmd = static_cast <SHCmd*>(QMetaType::create(id));
#else
          cmd = static_cast <SHCmd*>(QMetaType::construct(id));
#endif
          
        // Otherwise, create generic command
        else 
          cmd = new SHCmd();
 
        cmd->setFromRTVal(m_client, cmdVal);
        m_qUndoStack->push(new WrappedCmd(cmd));
      }
    }

    m_stackSize = currentStackSize;
  }

  catch(Exception e)
  {
    printf("SHCmdHandler::onSynchronizeCommands: exception: %s\n", e.getDesc_cstr());
  }
}
