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

using namespace FabricCore;
using namespace FabricUI;
using namespace DFG;
using namespace ValueEditor;

inline QString getItemPathFromItemMetaData(
  VETreeWidgetItem *veTreeItem)
{
  BaseModelItem *modelItem = 0;
  BaseViewItem *viewItem = veTreeItem->getViewItem();
  
  if(viewItem)
  {
    modelItem = viewItem->getModelItem();

    if(modelItem)
    {
      ItemMetadata* metadata = modelItem->getMetadata();

      QString bindingId = metadata->getString(
        ModelItems::DFGModelItemMetadata::VEDFGBindingIdKey.data() 
        );

      QString portPath = metadata->getString(
        ModelItems::DFGModelItemMetadata::VEDFGPortPathKey.data() 
        );

      // FE-8736 : if the current executable is the root
      // The path has the form '.node.port' or , remove the first '.'
      if(portPath.mid(0, 1) == ".")
        portPath = portPath.mid(1);

      return bindingId + "." + portPath;
    }
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
