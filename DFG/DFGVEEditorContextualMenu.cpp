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

void DFGVEEditorContextualMenu::onConstructMenu()
{
	QAction* createDFGPVToolAction = DFGVEEditorCreatePVToolAction::create(
		this,
		m_veTreeItem
		);

	if(createDFGPVToolAction)
	 	addAction(createDFGPVToolAction);

 	QAction* deleteDFGPVToolAction = DFGVEEditorDeletePVToolAction::create(
		this,
		m_veTreeItem
		);

	if(deleteDFGPVToolAction)
	 	addAction(deleteDFGPVToolAction);

 	QAction* deleteAllDFGPVToolsAction = DFGVEEditorDeleteAllPVToolsAction::create(
		this,
		m_veTreeItem
		);

	if(deleteAllDFGPVToolsAction)
	 	addAction(deleteAllDFGPVToolsAction);

 	QAction* deleteAllAndCreateDFGPVToolAction = DFGVEEditorDeleteAllAndCreatePVToolAction::create(
		this,
		m_veTreeItem
		);

	if(deleteAllAndCreateDFGPVToolAction)
	 	addAction(deleteAllAndCreateDFGPVToolAction);
}
