//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QMap>
#include <assert.h>
#include "DFGVEEditorContextualMenu.h"
#include <FabricUI/DFG/Tools/DFGVEEditorPVToolActions.h>

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

bool DFGVEEditorContextualMenu::canCreate(
	VETreeWidgetItem *veTreeItem)
{
	assert( veTreeItem );

  return DFGVEEditorPVToolMenu::canCreate(veTreeItem);
}

void DFGVEEditorContextualMenu::onConstructMenu()
{
	// Don't create a sub menu since it's the only one
  QAction* action;
	foreach(action, DFGVEEditorPVToolMenu::createActions(this, m_veTreeItem))
    addAction(action);
}
