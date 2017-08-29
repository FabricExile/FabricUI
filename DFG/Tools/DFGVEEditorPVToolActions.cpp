//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include "DFGVEEditorPVToolActions.h"
#include <FabricUI/ValueEditor/ItemMetadata.h>
#include <FabricUI/ValueEditor/BaseViewItem.h>
#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/ModelItems/DFGModelItemMetadata.h>
#include <iostream>

using namespace FabricCore;
using namespace FabricUI;
using namespace DFG;
using namespace ValueEditor;

inline QString getItemPathFromItemMetaData(
  VETreeWidgetItem *veTreeItem)
{
  BaseViewItem *viewItem = veTreeItem->getViewItem();
  if(viewItem)
  {
    ItemMetadata* metadata = viewItem->getMetadata();
    return metadata->getString( 
      FabricUI::ValueEditor::ItemMetadata::VEPathKey.data() 
      );
  }      
 
  return "";
}

DFGVEEditorPVToolMenu::DFGVEEditorPVToolMenu()
{
}

DFGVEEditorPVToolMenu::~DFGVEEditorPVToolMenu()
{
}

bool DFGVEEditorPVToolMenu::canCreate(
  VETreeWidgetItem *veTreeItem)
{
  assert( veTreeItem );

  return DFGPVToolMenu::canCreate(
    getItemPathFromItemMetaData(veTreeItem)
    );
}

QMenu* DFGVEEditorPVToolMenu::createMenu(
  QWidget *parent,
  VETreeWidgetItem *veTreeItem)
{
  assert( veTreeItem );

  return DFGPVToolMenu::createMenu(
    parent,
    getItemPathFromItemMetaData(veTreeItem)
    );
}

QList<QAction*> DFGVEEditorPVToolMenu::createActions(
  QWidget *parent,
  ValueEditor::VETreeWidgetItem *veTreeItem)
{
  assert( veTreeItem );

  return DFGPVToolMenu::createActions(
    parent,
    getItemPathFromItemMetaData(veTreeItem)
    );
}
