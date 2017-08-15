//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_VEEDITOR_CREATE_PV_TOOL_ACTION__
#define __UI_DFG_VEEDITOR_CREATE_PV_TOOL_ACTION__

#include <QMenu>
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
	    QString const& text,
	  	QString const& itemPath
	    );

	  virtual ~DFGVEEditorCreatePVToolAction();

	  static QAction* create(
	    QObject *parent,
	    QString const& text,
	  	ValueEditor::VETreeWidgetItem *veTreeItem
	    );
};

class DFGVEEditorDeletePVToolAction : public DFGDeletePVToolAction
{
  Q_OBJECT

	public:
	  DFGVEEditorDeletePVToolAction(
	    QObject *parent,
	    QString const& text,
	  	QString const& itemPath
	    );

	  virtual ~DFGVEEditorDeletePVToolAction();

	  static QAction* create(
	    QObject *parent,
	    QString const& text,
	  	ValueEditor::VETreeWidgetItem *veTreeItem
	    );
};

class DFGVEEditorDeleteAllPVToolsAction : public DFGDeleteAllPVToolsAction
{
  Q_OBJECT

	public:
	  DFGVEEditorDeleteAllPVToolsAction(
	    QObject *parent,
	    QString const& text
	    );

	  virtual ~DFGVEEditorDeleteAllPVToolsAction();

	  static QAction* create(
	    QObject *parent,
	    QString const& text,
 	 		ValueEditor::VETreeWidgetItem *veTreeItem
	    );
};

class DFGVEEditorDeleteAllAndCreatePVToolAction : public DFGDeleteAllAndCreatePVToolAction
{
  Q_OBJECT

	public:
	  DFGVEEditorDeleteAllAndCreatePVToolAction(
	    QObject *parent,
	    QString const& text,
	  	QString const& itemPath
	    );

	  virtual ~DFGVEEditorDeleteAllAndCreatePVToolAction();

	  static QAction* create(
	    QObject *parent,
	    QString const& text,
 	 		ValueEditor::VETreeWidgetItem *veTreeItem
	    );
};

class DFGVEEditorPVToolMenu : public QMenu
{
	Q_OBJECT

	public:
		DFGVEEditorPVToolMenu(
			QWidget *parent,
			ValueEditor::VETreeWidgetItem *veTreeItem
			);

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

		protected slots:
			virtual void onConstructMenu();

	private:
		ValueEditor::VETreeWidgetItem *m_veTreeItem;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_VEEDITOR_CREATE_PV_TOOL_ACTION__
