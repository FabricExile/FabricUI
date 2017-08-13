//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QMap>
#include <assert.h>
#include "DFGVEEditorContextualMenu.h"
#include <FabricUI/DFG/Tools/DFGVEEditorCreatePVToolAction.h>
#include <iostream>

using namespace FabricUI;
using namespace DFG;
using namespace ValueEditor;

DFGVEEditorContextualMenu::DFGVEEditorContextualMenu(
	QWidget *parent,
	VETreeWidgetItem *veTreeItem)
	: QMenu(parent),
	m_veTreeItem(veTreeItem)
{
  connect(
  	this, 
  	SIGNAL(aboutToShow()), 
  	this, 
  	SLOT(onConstructMenu())
  	);
}

DFGVEEditorContextualMenu::~DFGVEEditorContextualMenu()
{
}

void DFGVEEditorContextualMenu::create(
	QWidget *parent,
	QPoint const& point,
	VETreeWidgetItem *veTreeItem)
{
	assert( veTreeItem );
  std::cout << "DFGVEEditorContextualMenu::create 1 " << std::endl;

  DFGVEEditorContextualMenu *menu = new DFGVEEditorContextualMenu( 
    parent,
    veTreeItem
    );

  //menu->onConstructMenu();
  menu->exec(point);

  std::cout << "DFGVEEditorContextualMenu::create 2 " << std::endl;
}

void DFGVEEditorContextualMenu::onConstructMenu()
{
  std::cout << "DFGVEEditorContextualMenu::onConstructMenu 1 " << std::endl;

	QAction* toolAction = DFGVEEditorCreatePVToolAction::create(
		this,
		m_veTreeItem
		);

	if(toolAction)
	 	addAction(toolAction);
  
  std::cout << "DFGVEEditorContextualMenu::onConstructMenu 2 " << std::endl;
}
