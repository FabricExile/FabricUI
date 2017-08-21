//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>

using namespace FabricUI::FCurveEditor;

void AbstractFCurveModel::moveKeys( const size_t* indices, const size_t nbIndices, QPointF delta )
{
  for( size_t i = 0; i < nbIndices; i++ )
  {
    Key h = getKey( indices[i] );
    h.pos += delta;
    this->setKey( indices[i], h );
  }
}

void AbstractFCurveModel::deleteKeys( const size_t* indices, const size_t nbIndices )
{
  for( size_t i = nbIndices; i > 0; i-- )
    this->deleteKey( indices[i-1] );
}
