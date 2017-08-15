//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/VarItemMetadata.h>
#include <FabricUI/ModelItems/VarModelItem.h>
#include <FabricUI/ModelItems/VarPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////

VarModelItem::VarModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName
  )
  : ItemModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    nodeName
    )
  , m_metadata( this )
{
}

VarModelItem::~VarModelItem()
{
}

FabricUI::ValueEditor::BaseModelItem *
VarModelItem::createChild( FTL::StrRef portName )
{
  return pushChild(new VarPortModelItem(
    m_dfgUICmdHandler,
    m_binding,
    m_execPath,
    m_exec,
    m_itemPath,
    portName
    ));
}

QVariant VarModelItem::getValue()
{
  try
  {
    return QVariant::fromValue( m_exec.getVarValue( m_itemPath.c_str() ) );
  }
  catch ( FabricCore::Exception e )
  {
    return QVariant();
  }
}

void VarModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  FabricCore::RTVal val = m_exec.getVarValue( m_itemPath.c_str() );
  if (FabricUI::ValueEditor::RTVariant::toRTVal( var, val ))
  {
    m_exec.setVarValue( m_itemPath.c_str(), val );
  }
}

FabricUI::ValueEditor::ItemMetadata *VarModelItem::getMetadata()
{
  return &m_metadata;
}
bool VarModelItem::shouldBeReadOnly() const
{
  try
  {
    return m_exec.hasSrcPorts( (m_itemPath + ".value").c_str() );
  }
  catch ( FabricCore::Exception e )
  {
    printf("%s\n", e.getDesc_cstr());
    return false;
  }
}

} // namespace ModelItems
} // namespace FabricUI
