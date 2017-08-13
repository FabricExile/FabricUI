//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QMap>
#include <assert.h>
#include "DFGVEEditorContextualMenu.h"
#include <FabricUI/DFG/Tools/DFGVEEditorCreatePVToolAction.h>

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

  DFGVEEditorContextualMenu *menu = new DFGVEEditorContextualMenu( 
    parent,
    veTreeItem
    );

  menu->exec(
    parent->mapToGlobal(point)
    );
}

void DFGVEEditorContextualMenu::onConstructMenu()
{
	QAction* toolAction = DFGVEEditorCreatePVToolAction::create(
		this,
		m_veTreeItem
		);

	if(toolAction)
	 	addAction(toolAction);
}
