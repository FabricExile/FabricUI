/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include "SHCmd.h"
using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;
 
SHCmd::SHCmd() 
  : m_state(State_New)
  , m_coreUndoCount(0) {
}

SHCmd::~SHCmd() {
}

void SHCmd::registerCommand() {
}

void SHCmd::setFromRTVal(FabricCore::Client client, FabricCore::RTVal cmd) {
  try 
  {
    m_client = client;
    m_description = QString(cmd.callMethod("String", "type", 0, 0).getStringCString());
  }
  catch(FabricCore::Exception e)
  {
    printf("SHCmd::setFromRTVal: exception: %s\n", e.getDesc_cstr());
  }
}
        
QString SHCmd::getDescription() { 
  assert( m_state != State_New ); 
  return m_description; 
}

void SHCmd::doit() {
  assert(m_state == State_New);
  ++m_coreUndoCount;
  m_state = State_Done;
}

void SHCmd::undo() {
  assert(m_state == State_Done || m_state == State_Redone);
  try 
  {
    m_state = State_Undone;
    FabricCore::RTVal cmdManager = RTVal::Create(m_client, "LegacyCmdManager", 0, 0);
    cmdManager = cmdManager.callMethod("LegacyCmdManager", "getOrCreateLegacyCmdManager", 0, 0);
    
    for(unsigned i=0; i<m_coreUndoCount; ++i)
      cmdManager.callMethod("Boolean", "undo", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHCmd::undo: exception: %s\n", e.getDesc_cstr());
  }
}

void SHCmd::redo() {
  assert(m_state = State_Undone);
  try 
  {
    m_state = State_Redone;
    FabricCore::RTVal cmdManager = RTVal::Create(m_client, "LegacyCmdManager", 0, 0);
    cmdManager = cmdManager.callMethod("LegacyCmdManager", "getOrCreateLegacyCmdManager", 0, 0);
    
    for(unsigned i=0; i<m_coreUndoCount; ++i)
      cmdManager.callMethod("Boolean", "redo", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHCmd::redo: exception: %s\n", e.getDesc_cstr());
  }
}
