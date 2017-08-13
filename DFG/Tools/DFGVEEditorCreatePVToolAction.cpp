//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include "DFGVEEditorCreatePVToolAction.h"
#include <FabricUI/Tools/PathValueTool.h>
#include <FabricUI/ValueEditor/ItemMetadata.h>
#include <FabricUI/ValueEditor/BaseViewItem.h>
#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/ModelItems/DFGModelItemMetadata.h>
#include <iostream>

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
 
QAction* DFGVEEditorCreatePVToolAction::create(
	QObject *parent,
  VETreeWidgetItem *veTreeItem)
{ 
  FABRIC_CATCH_BEGIN();

  std::cout << "DFGVEEditorCreatePVToolAction::create 1 " << std::endl;

	BaseModelItem *modelItem = 0;
  BaseViewItem *viewItem = veTreeItem->getViewItem();
  
  if(viewItem)
  {
    std::cout << "DFGVEEditorCreatePVToolAction::create 2 " << std::endl;
    modelItem = viewItem->getModelItem();
  
    if(modelItem)
    {
      std::cout << "DFGVEEditorCreatePVToolAction::create 3 " << std::endl;
      ItemMetadata* metadata = modelItem->getMetadata();

      QString bindingId = metadata->getString(
        ModelItems::DFGModelItemMetadata::VEDFGBindingIdKey.data() 
        );

      QString portPath = metadata->getString(
        ModelItems::DFGModelItemMetadata::VEDFGPortPathKey.data() 
        );

      if(portPath.mid(0, 1) == ".")
        portPath = portPath.mid(1);

      QString itemPath = bindingId + "." + portPath;
      
      RTVal pathValueTool = PathValueTool::getTool(itemPath);
      bool pathValueToolIsValid = pathValueTool.isValid() && !pathValueTool.isNullObject();

      std::cout 
        << "DFGVEEditorCreatePVToolAction::create 4 "
        << " portPath " 
        << itemPath.toUtf8().constData()
        << " pathValueToolIsValid " 
        << pathValueToolIsValid
        << std::endl;
      
      if(!pathValueToolIsValid && PathValueTool::canCreateTool(itemPath))
      {
        std::cout << "DFGVEEditorCreatePVToolAction::create 5 " << std::endl;

        return new DFGVEEditorCreatePVToolAction(
          parent,
          itemPath);
      }
    }
  }      

  std::cout << "DFGVEEditorCreatePVToolAction::create 6 " << std::endl;

  FABRIC_CATCH_END("DFGVEEditorCreatePVToolAction::create");

  return 0;
}
