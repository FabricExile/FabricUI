//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/ArgItemMetadata.h>

#include <QString>

using namespace FabricUI::ModelItems;

ArgItemMetadata::ArgItemMetadata( ArgModelItem *argModelItem )
  : m_argModelItem( argModelItem )
{
  this->computeDFGPath();
}

void ArgItemMetadata::computeDFGPath()
{
  m_bindingId = QString::number( m_argModelItem->getBinding().getBindingID() ).toUtf8().data();

  m_portPath = m_argModelItem->getArgName();

  m_dfgPath = m_bindingId + "." + m_portPath;
}
