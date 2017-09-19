// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_PresetTreeWidget__
#define __UI_DFG_PresetTreeWidget__

#include <QWidget>
#include <QLineEdit>
#include <FabricUI/TreeView/TreeViewWidget.h>
#include <FabricUI/TreeView/TreeModel.h>
#include <FabricUI/TreeView/TreeItem.h>
#include "DFGConfig.h"
#include <SplitSearch/SplitSearch.hpp>

namespace FabricUI
{

  namespace DFG
  {
    class DFGController;

    class PresetTreeWidget : public QWidget
    {
      Q_OBJECT

    public:

      PresetTreeWidget(
        DFGController *dfgController,
        const DFGConfig & config, 
        bool showsPresets,
        bool showSearch,
        bool hideFabricDir,
        bool hideVariablesDir,
        bool hideWriteProtectedDirs,
        bool setupContextMenu
        );
      virtual ~PresetTreeWidget();

      TreeView::TreeViewWidget * getTreeView() { return m_treeView; }
      TreeView::TreeModel * getTreeModel() { return m_treeModel; }
      DFGController* getController() { return m_dfgController; }

    public slots:

      void setModelDirty(); // Will tag the model as dirty, and will schedule an update for it
      void refresh();
      void setBinding( FabricCore::DFGBinding const &binding );
      void onCustomContextMenuRequested(QPoint globalPos, FabricUI::TreeView::TreeItem * item);
      void onRowDoubleClick(const QModelIndex &item);
      void onContextMenuAction(QAction * action);

      /// Expands the tree-View to show the preset and selects it (FE-7300).
      void onExpandToAndSelectItem(QString presetPath);

    protected:
      void paintEvent( QPaintEvent * );

    private:
      
      void updatePresetPathDB();

      DFGController *m_dfgController;
      QLineEdit * m_searchEdit;
      TreeView::TreeViewWidget * m_treeView;
      TreeView::TreeModel * m_treeModel;
      FabricServices::SplitSearch::Dict m_presetPathDict;
      std::vector<std::string> m_presetPathDictSTL;
      bool m_presetDictsUpToDate;
      QString m_state;
      bool m_showsPresets;
      std::string m_contextPath;
      bool m_modelDirty;
      bool m_hideFabricDirs;
    };

  };

};

#endif // __UI_DFG_PresetTreeWidget__
