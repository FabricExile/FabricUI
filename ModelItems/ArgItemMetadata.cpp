//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/ArgItemMetadata.h>

using namespace FabricUI::ModelItems;

ArgItemMetadata::ArgItemMetadata( ArgModelItem *argModelItem )
  : m_argModelItem( argModelItem )
{
  this->computeDFGPath();
}

void ArgItemMetadata::computeDFGPath()
{
  QString bindingId = QString::number( 
  	m_argModelItem->getBinding().getBindingID() 
  	);

  QString portPath = m_argModelItem->getArgName().c_str();

  m_dfgPath = bindingId + "." + portPath;
}
