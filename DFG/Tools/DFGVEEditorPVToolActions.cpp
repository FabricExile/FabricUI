//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include "DFGVEEditorPVToolActions.h"
#include <FabricUI/Tools/PathValueTool.h>
#include <FabricUI/ValueEditor/ItemMetadata.h>
#include <FabricUI/ValueEditor/BaseViewItem.h>
#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/ModelItems/DFGModelItemMetadata.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace DFG;
using namespace Tools;
using namespace ValueEditor;

DFGVEEditorCreatePVToolAction::DFGVEEditorCreatePVToolAction(
  QObject *parent,
	QString const& itemPath)
  : DFGCreatePVToolAction(parent, itemPath)
{
}

DFGVEEditorCreatePVToolAction::~DFGVEEditorCreatePVToolAction()
{
}
 
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

      if(portPath.mid(0, 1) == ".")
        portPath = portPath.mid(1);

      return bindingId + "." + portPath;
    }
  }      
 
  return "";
}

QAction* DFGVEEditorCreatePVToolAction::create(
	QObject *parent,
  VETreeWidgetItem *veTreeItem)
{ 
  FABRIC_CATCH_BEGIN();

	QString itemPath = getItemPathFromItemMetaData(
    veTreeItem);
  
  if(!itemPath.isEmpty())
  {
    RTVal pathValueTool = PathValueTool::getTool(itemPath);
    bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

    if(!pathValueToolIsValid && PathValueTool::canCreateTool(itemPath))
      return new DFGVEEditorCreatePVToolAction(
        parent,
        itemPath);
  }      

  FABRIC_CATCH_END("DFGVEEditorCreatePVToolAction::create");

  return 0;
}

DFGVEEditorDeletePVToolAction::DFGVEEditorDeletePVToolAction(
  QObject *parent,
  QString const& itemPath)
  : DFGDeletePVToolAction(parent, itemPath)
{
}

DFGVEEditorDeletePVToolAction::~DFGVEEditorDeletePVToolAction()
{
}
 
QAction* DFGVEEditorDeletePVToolAction::create(
  QObject *parent,
  VETreeWidgetItem *veTreeItem)
{ 
  FABRIC_CATCH_BEGIN();

  QString itemPath = getItemPathFromItemMetaData(
    veTreeItem);
  
  if(!itemPath.isEmpty())
  {
    RTVal pathValueTool = PathValueTool::getTool(itemPath);
    bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

    if(pathValueToolIsValid)
      return new DFGVEEditorDeletePVToolAction(
        parent,
        itemPath);
  }      

  FABRIC_CATCH_END("DFGVEEditorCreatePVToolAction::create");

  return 0;
}

DFGVEEditorDeleteAllPVToolsAction::DFGVEEditorDeleteAllPVToolsAction(
  QObject *parent)
  : DFGDeleteAllPVToolsAction(parent)
{
}

DFGVEEditorDeleteAllPVToolsAction::~DFGVEEditorDeleteAllPVToolsAction()
{
}
 
QAction* DFGVEEditorDeleteAllPVToolsAction::create(
  QObject *parent,
  VETreeWidgetItem *veTreeItem)
{ 
  FABRIC_CATCH_BEGIN();

  QString itemPath = getItemPathFromItemMetaData(
    veTreeItem);
  
  if(!itemPath.isEmpty())
  {
    RTVal pathValueTool = PathValueTool::getTool(itemPath);
    bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

    if(pathValueToolIsValid)
      return new DFGVEEditorDeleteAllPVToolsAction(
        parent);
  }      

  FABRIC_CATCH_END("DFGVEEditorCreatePVToolAction::create");

  return 0;
}
