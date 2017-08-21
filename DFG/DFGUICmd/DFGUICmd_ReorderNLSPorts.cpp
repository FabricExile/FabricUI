//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_ReorderNLSPorts.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_ReorderNLSPorts::appendDesc( QString &desc )
{
  desc += "Reordered locals [";

  for ( int i = 0; i < m_indices.size(); ++i )
  {
    if ( i > 0 )
      desc += ",";
    desc += QString::number( m_indices[i] );
  }

  desc += "]";
}

void DFGUICmd_ReorderNLSPorts::invoke( unsigned &coreUndoCount )
{
  std::vector<unsigned> indices;
  indices.reserve( m_indices.size() );
  foreach ( int index, m_indices )
    indices.push_back( unsigned( index ) );

  invoke(
    m_itemPath.toUtf8().constData(),
    indices,
    coreUndoCount
    );
}

void DFGUICmd_ReorderNLSPorts::invoke(
  FTL::CStrRef itemPath,
  FTL::ArrayRef<unsigned> indices,
  unsigned &coreUndoCount
  )
{
  getExec().reorderNLSPorts( itemPath.c_str(), indices.size(), &indices[0] );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
