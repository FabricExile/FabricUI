//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include "DFGVEEditorPVToolActions.h"
#include <FabricUI/Tools/ToolManager.h>
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
  QString const& text,
	QString const& itemPath)
  : DFGCreatePVToolAction(parent, "DFGVEEditorCreatePVToolAction", text, itemPath)
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
  QString const& text,
  VETreeWidgetItem *veTreeItem)
{ 
  FABRIC_CATCH_BEGIN();

	QString itemPath = getItemPathFromItemMetaData(
    veTreeItem);
  
  if(!itemPath.isEmpty())
  {
    RTVal pathValueTool = ToolManager::getPathValueTool(itemPath);
    bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

    if(!pathValueToolIsValid && ToolManager::canCreatePathValueTool(itemPath))
      return new DFGVEEditorCreatePVToolAction(
        parent,
        text,
        itemPath);
  }      

  FABRIC_CATCH_END("DFGVEEditorCreatePVToolAction::create");

  return 0;
}

DFGVEEditorDeletePVToolAction::DFGVEEditorDeletePVToolAction(
  QObject *parent,
  QString const& text,
  QString const& itemPath)
  : DFGDeletePVToolAction(parent, "DFGVEEditorDeletePVToolAction", text, itemPath)
{
}

DFGVEEditorDeletePVToolAction::~DFGVEEditorDeletePVToolAction()
{
}
 
QAction* DFGVEEditorDeletePVToolAction::create(
  QObject *parent,
  QString const& text,
  VETreeWidgetItem *veTreeItem)
{ 
  FABRIC_CATCH_BEGIN();

  QString itemPath = getItemPathFromItemMetaData(
    veTreeItem);
  
  if(!itemPath.isEmpty())
  {
    RTVal pathValueTool = ToolManager::getPathValueTool(itemPath);
    bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

    if(pathValueToolIsValid)
      return new DFGVEEditorDeletePVToolAction(
        parent,
        text,
        itemPath);
  }      

  FABRIC_CATCH_END("DFGVEEditorCreatePVToolAction::create");

  return 0;
}

DFGVEEditorDeleteAllPVToolsAction::DFGVEEditorDeleteAllPVToolsAction(
  QObject *parent,
  QString const& text)
  : DFGDeleteAllPVToolsAction(parent, "DFGVEEditorDeleteAllPVToolsAction", text)
{
}

DFGVEEditorDeleteAllPVToolsAction::~DFGVEEditorDeleteAllPVToolsAction()
{
}
 
QAction* DFGVEEditorDeleteAllPVToolsAction::create(
  QObject *parent,
  QString const& text,
  VETreeWidgetItem *veTreeItem)
{ 
  FABRIC_CATCH_BEGIN();

  QString itemPath = getItemPathFromItemMetaData(
    veTreeItem);
  
  if(!itemPath.isEmpty())
  {
    RTVal pathValueTool = ToolManager::getPathValueTool(itemPath);
    bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

    if(pathValueToolIsValid)
      return new DFGVEEditorDeleteAllPVToolsAction(
        parent,
        text);
  }      

  FABRIC_CATCH_END("DFGVEEditorCreatePVToolAction::create");

  return 0;
}

DFGVEEditorDeleteAllAndCreatePVToolAction::DFGVEEditorDeleteAllAndCreatePVToolAction(
  QObject *parent,
  QString const& text,
  QString const& itemPath)
  : DFGDeleteAllAndCreatePVToolAction(parent, "DFGVEEditorDeleteAllAndCreatePVToolAction", text, itemPath)
{
}

DFGVEEditorDeleteAllAndCreatePVToolAction::~DFGVEEditorDeleteAllAndCreatePVToolAction()
{
}
 
QAction* DFGVEEditorDeleteAllAndCreatePVToolAction::create(
  QObject *parent,
  QString const& text,
  VETreeWidgetItem *veTreeItem)
{ 
  FABRIC_CATCH_BEGIN();

  QString itemPath = getItemPathFromItemMetaData(
    veTreeItem);
  
  if(!itemPath.isEmpty())
  {
    RTVal pathValueTool = ToolManager::getPathValueTool(itemPath);
    bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

    if(!pathValueToolIsValid && ToolManager::canCreatePathValueTool(itemPath))
      return new DFGVEEditorDeleteAllAndCreatePVToolAction(
        parent,
        text,
        itemPath);
  }      

  FABRIC_CATCH_END("DFGVEEditorDeleteAllAndCreatePVToolAction::create");

  return 0;
}

DFGVEEditorPVToolMenu::DFGVEEditorPVToolMenu(
  QWidget *parent,
  VETreeWidgetItem *veTreeItem)
  : QMenu(parent),
  m_veTreeItem(veTreeItem)
{
  QObject::connect(
    this, 
    SIGNAL(aboutToShow()), 
    this, 
    SLOT(onConstructMenu())
    );
}

DFGVEEditorPVToolMenu::~DFGVEEditorPVToolMenu()
{
}

bool DFGVEEditorPVToolMenu::canCreate(
  VETreeWidgetItem *veTreeItem)
{
  FABRIC_CATCH_BEGIN();

  QString itemPath = getItemPathFromItemMetaData(
    veTreeItem);
  
  if(!itemPath.isEmpty())
    return ToolManager::canCreatePathValueTool(itemPath); 

  FABRIC_CATCH_END("DFGVEEditorPVToolMenu::canCreate");

  return 0;
}

QMenu* DFGVEEditorPVToolMenu::createMenu(
  QWidget *parent,
  VETreeWidgetItem *veTreeItem)
{
  assert( veTreeItem );

  return new DFGVEEditorPVToolMenu( 
    parent,
    veTreeItem
    );
}

void DFGVEEditorPVToolMenu::onConstructMenu()
{
  QAction* action;
  foreach(action, createActions(this, m_veTreeItem))
    addAction(action);
}

QList<QAction*> DFGVEEditorPVToolMenu::createActions(
  QWidget *parent,
  ValueEditor::VETreeWidgetItem *veTreeItem)
{
  QList<QAction*> actions;

  // QAction* createDFGPVToolAction = DFGVEEditorCreatePVToolAction::create(
  //   parent,
  //   "Edit With Tool With Others",
  //   veTreeItem
  //   );
  
  // if(createDFGPVToolAction)
  //   actions.append(createDFGPVToolAction);


  // QAction* deleteAllDFGPVToolsAction = DFGVEEditorDeleteAllPVToolsAction::create(
  //   parent,
  //   "Close All Tools",
  //   veTreeItem
  //   );
 
  // if(deleteAllDFGPVToolsAction)
  //   actions.append(deleteAllDFGPVToolsAction);
  
  QAction* deleteAllAndCreateDFGPVToolAction = DFGVEEditorDeleteAllAndCreatePVToolAction::create(
    parent,
    "Edit With Tool",
    veTreeItem
    );

  if(deleteAllAndCreateDFGPVToolAction)
    actions.append(deleteAllAndCreateDFGPVToolAction);

  QAction* deleteDFGPVToolAction = DFGVEEditorDeletePVToolAction::create(
    parent,
    "Close tool",
    veTreeItem
    );

  if(deleteDFGPVToolAction)
    actions.append(deleteDFGPVToolAction);
  

  return actions;
}