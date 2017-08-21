//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/ModelItems/ArgModelItem.h>
#include <FabricUI/ModelItems/BindingModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
BindingModelItem::BindingModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  bool showInputs,
  bool showOutputs,
  bool showIO
  )
  : m_dfgUICmdHandler( dfgUICmdHandler )
  , m_binding( binding )
  , m_rootExec( binding.getExec() )
  , m_showInputs(showInputs)
  , m_showOutputs(showOutputs)
  , m_showIO(showIO)
{
  assert( m_binding.isValid() );
  assert( m_rootExec.isValid() );
}

BindingModelItem::~BindingModelItem()
{
}


int BindingModelItem::getNumChildren()
{
  int count = 0;
  for(unsigned int j=0;j<m_rootExec.getExecPortCount();j++)
  {
    FabricCore::DFGPortType portType = m_rootExec.getExecPortType(j);
    if((portType == FabricCore::DFGPortType_In) && !m_showInputs)
      continue;
    if((portType == FabricCore::DFGPortType_Out) && !m_showOutputs)
      continue;
    if((portType == FabricCore::DFGPortType_IO) && !m_showIO)
      continue;
    count++;
  }
  return count;
}

FTL::CStrRef BindingModelItem::getChildName( int i )
{
  int index = 0;
  for(unsigned int j=0;j<m_rootExec.getExecPortCount();j++)
  {
    FabricCore::DFGPortType portType = m_rootExec.getExecPortType(j);
    if((portType == FabricCore::DFGPortType_In) && !m_showInputs)
      continue;
    if((portType == FabricCore::DFGPortType_Out) && !m_showOutputs)
      continue;
    if((portType == FabricCore::DFGPortType_IO) && !m_showIO)
      continue;
    if(index == i)
      return m_rootExec.getExecPortName(j);
    index++;
  }
  return FTL::CStrRef();
}

FabricUI::ValueEditor::BaseModelItem *
BindingModelItem::createChild( FTL::StrRef name ) /**/
{
  return pushChild(new ArgModelItem(
    m_dfgUICmdHandler,
    m_binding,
    name
    ));
}

FTL::CStrRef BindingModelItem::getName()
{
  return FTL_STR("<Root>");
}

bool BindingModelItem::canRename()
{
  return false;
}

void BindingModelItem::rename( FTL::CStrRef newName )
{
  assert( false );
}

void BindingModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( false );
}

void BindingModelItem::setMetadataImp( 
  const char* key, 
  const char* value, 
  bool canUndo ) /**/
{
  m_rootExec.setMetadata( key, value, canUndo );
}

QVariant BindingModelItem::getValue()
{
  return QVariant();
}

void BindingModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  if (commit)
  {
    QByteArray asciiArr = var.toString().toLatin1();
    m_rootExec.setTitle( asciiArr.data() );
    emitModelValueChanged(var);
  }
}

} // namespace ModelItems
} // namespace FabricUI
