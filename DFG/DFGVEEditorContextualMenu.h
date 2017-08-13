//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_DFG_DFGVEEDITORCONTEXTUALMENU_H
#define FABRICUI_DFG_DFGVEEDITORCONTEXTUALMENU_H

#include <QMenu>
#include <QPoint>
#include <QWidget>
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>

namespace FabricUI {
namespace DFG {

class DFGVEEditorContextualMenu : public QMenu
{
	Q_OBJECT

	public:
		DFGVEEditorContextualMenu(
			QWidget *parent,
			ValueEditor::VETreeWidgetItem *veTreeItem
			);

		~DFGVEEditorContextualMenu();

		static void create(
			QWidget *parent,
			QPoint const& point,
			ValueEditor::VETreeWidgetItem *veTreeItem
			);

	protected slots:
		virtual void onConstructMenu();

	private:
		ValueEditor::VETreeWidgetItem *m_veTreeItem;
};

} // namespace DFG
} // namespace FabricUI

#endif // FABRICUI_DFG_DFGVEEDITORCONTEXTUALMENU_H
