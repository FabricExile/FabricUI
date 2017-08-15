//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_VEEDITOR_CREATE_PV_TOOL_ACTION__
#define __UI_DFG_VEEDITOR_CREATE_PV_TOOL_ACTION__

#include "DFGPVToolActions.h"
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>

namespace FabricUI {
namespace DFG {
 
class DFGVEEditorCreatePVToolAction : public DFGCreatePVToolAction
{
  Q_OBJECT

	public:
	  DFGVEEditorCreatePVToolAction(
	    QObject *parent,
	  	QString const& itemPath
	    );

	  virtual ~DFGVEEditorCreatePVToolAction();

	  static QAction* create(
	    QObject *parent,
	  	ValueEditor::VETreeWidgetItem *veTreeItem
	    );
};

class DFGVEEditorDeletePVToolAction : public DFGDeletePVToolAction
{
  Q_OBJECT

	public:
	  DFGVEEditorDeletePVToolAction(
	    QObject *parent,
	  	QString const& itemPath
	    );

	  virtual ~DFGVEEditorDeletePVToolAction();

	  static QAction* create(
	    QObject *parent,
	  	ValueEditor::VETreeWidgetItem *veTreeItem
	    );
};

class DFGVEEditorDeleteAllPVToolsAction : public DFGDeleteAllPVToolsAction
{
  Q_OBJECT

	public:
	  DFGVEEditorDeleteAllPVToolsAction(
	    QObject *parent
	    );

	  virtual ~DFGVEEditorDeleteAllPVToolsAction();

	  static QAction* create(
	    QObject *parent,
 	 		ValueEditor::VETreeWidgetItem *veTreeItem
	    );
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_VEEDITOR_CREATE_PV_TOOL_ACTION__
