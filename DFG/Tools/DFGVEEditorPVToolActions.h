//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_VEEDITOR_CREATE_PV_TOOL_ACTION__
#define __UI_DFG_VEEDITOR_CREATE_PV_TOOL_ACTION__

#include "DFGPVToolActions.h"
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>

namespace FabricUI {
namespace DFG {
 
class DFGVEEditorPVToolMenu
{
	public:
		DFGVEEditorPVToolMenu();

		~DFGVEEditorPVToolMenu();

		static QMenu* createMenu(
			QWidget *parent,
			ValueEditor::VETreeWidgetItem *veTreeItem
			);

		static QList<QAction*> createActions(
			QWidget *parent,
  		ValueEditor::VETreeWidgetItem *veTreeItem
			);

		static bool canCreate(
			ValueEditor::VETreeWidgetItem *veTreeItem
			);
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_VEEDITOR_CREATE_PV_TOOL_ACTION__
