//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_DFG_DFGVEEDITOROWNER_H
#define FABRICUI_DFG_DFGVEEDITOROWNER_H

#include <FabricUI/ValueEditor/VEEditorOwner.h>
#include <FabricUI/DFG/DFGNotifier.h>
#include <QTreeWidgetItem>

class BaseModelItem;

namespace FTL {
  class JSONArray;
}

namespace FabricUI {

  namespace DFG {
    class DFGController;
    class DFGWidget;
  }
  namespace GraphView {
    class Node;
    class Graph;
    class InstBlock;
  }
  namespace ModelItems {
    class BindingModelItem;
    class ItemModelItem;
    class RootModelItem;
  }

  namespace ValueEditor {
    class VETreeWidget;
  }

  namespace DFG 
  {
    class DFGVEEditorOwner;

    class DFGVEEditorOwner_NotifProxy : public QObject
    {
    public:
      
      DFGVEEditorOwner_NotifProxy(
        DFGVEEditorOwner *dst,
        QObject *parent
        )
        : QObject( parent )
        , m_dst( dst )
        {}

      virtual ~DFGVEEditorOwner_NotifProxy() {}

    protected:

      DFGVEEditorOwner *m_dst;
    };

    class DFGVEEditorOwner_BindingNotifProxy :
      public DFGVEEditorOwner_NotifProxy
    {
      Q_OBJECT

    public:

      DFGVEEditorOwner_BindingNotifProxy(
        DFGVEEditorOwner *dst,
        QObject *parent
        )
        : DFGVEEditorOwner_NotifProxy( dst, parent )
        {}

    public slots:

      void onBindingArgValueChanged(
        unsigned index,
        FTL::CStrRef name
        );

      void onBindingArgInserted(
        unsigned index,
        FTL::CStrRef name,
        FTL::CStrRef type
        );

      void onBindingArgRenamed(
        unsigned argIndex,
        FTL::CStrRef oldArgName,
        FTL::CStrRef newArgName
        );

      void onBindingArgRemoved(
        unsigned index,
        FTL::CStrRef name
        );

      void onBindingArgTypeChanged(
        unsigned index,
        FTL::CStrRef name,
        FTL::CStrRef newType
        );

      void onBindingArgsReordered(
        FTL::ArrayRef<unsigned> newOrder
        );
    };

    class DFGVEEditorOwner : public ValueEditor::VEEditorOwner
    {
      Q_OBJECT

    public:

      DFGVEEditorOwner( DFGWidget * dfgWidget );
      ~DFGVEEditorOwner();

      virtual void initConnections();

      void onBindingArgValueChanged( unsigned index, FTL::CStrRef name );

      void onBindingArgInserted(
        unsigned index,
        FTL::CStrRef name,
        FTL::CStrRef type
        );

      void onBindingArgRenamed(
        unsigned argIndex,
        FTL::CStrRef oldArgName,
        FTL::CStrRef newArgName
        );

      void onBindingArgRemoved(
        unsigned index,
        FTL::CStrRef name
        );

      void onBindingArgTypeChanged(
        unsigned index,
        FTL::CStrRef name,
        FTL::CStrRef newType
        );

      void onBindingArgsReordered(
        FTL::ArrayRef<unsigned> newOrder
        );

    public slots :

      virtual void onOutputsChanged(); // Call after each evaluation

    protected slots:

      void onControllerBindingChanged(
        FabricCore::DFGBinding const &binding
        );

      virtual void onSidePanelInspectRequested();

      virtual void onCustomContextMenu(
        const QPoint &point
        );

      void onNodeInspectRequested(
        FabricUI::GraphView::Node *node
        );

      void onExecNodePortInserted(
        FTL::CStrRef nodeName,
        unsigned portIndex,
        FTL::CStrRef portName
        );

      void onExecNodePortRenamed(
        FTL::CStrRef nodeName,
        unsigned portIndex,
        FTL::CStrRef oldNodePortName,
        FTL::CStrRef newNodePortName
        );

      void onExecNodePortRemoved(
        FTL::CStrRef nodeName,
        unsigned portIndex,
        FTL::CStrRef portName
        );

      void onExecNodePortsReordered(
        FTL::CStrRef nodeName,
        FTL::ArrayRef<unsigned> newOrder
        );

      void onExecPortMetadataChanged(
        FTL::CStrRef portName,
        FTL::CStrRef key,
        FTL::CStrRef value
        );

      void onExecNodeRemoved(
        FTL::CStrRef nodeName
        );

      void onExecNodeRenamed(
        FTL::CStrRef oldNodeName,
        FTL::CStrRef newNodeName
        );

      void onExecPortsConnectedOrDisconnected(
        FTL::CStrRef srcPortPath,
        FTL::CStrRef dstPortPath
        );

      void onExecPortDefaultValuesChanged(
        FTL::CStrRef portName
        );

      void onExecNodePortDefaultValuesChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName
        );

      void onExecNodePortResolvedTypeChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedTypeName
        );

      void onExecRefVarPathChanged(
        FTL::CStrRef refName,
        FTL::CStrRef newVarPath
        );

      // InstBlocks

      void onInstBlockRenamed(
        FTL::CStrRef instName,
        FTL::CStrRef oldBlockName,
        FTL::CStrRef newBlockName
        );

      void onInstBlockRemoved(
        FTL::CStrRef instName,
        FTL::CStrRef blockName
        );

      // InstBlockPorts

      void onInstBlockPortInserted(
        FTL::CStrRef instName,
        FTL::CStrRef blockName,
        unsigned portIndex,
        FTL::CStrRef portName
        );

      void onInstBlockPortRenamed(
        FTL::CStrRef instName,
        FTL::CStrRef blockName,
        unsigned portIndex,
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName
        );

      void onInstBlockPortRemoved(
        FTL::CStrRef instName,
        FTL::CStrRef blockName,
        unsigned portIndex,
        FTL::CStrRef portName
        );

      void onInstBlockPortDefaultValuesChanged(
        FTL::CStrRef instName,
        FTL::CStrRef blockName,
        FTL::CStrRef portName
        );

      void onInstBlockPortResolvedTypeChanged(
        FTL::CStrRef instName,
        FTL::CStrRef blockName,
        FTL::CStrRef portName,
        FTL::CStrRef newResolveTypeName
        );

      void onInstBlockPortsReordered(
        FTL::CStrRef instName,
        FTL::CStrRef blockName,
        FTL::ArrayRef<unsigned> newOrder
        );

      void onItemOveredChanged( 
        QTreeWidgetItem *oldItem,
        QTreeWidgetItem *newItem
        );

      virtual void onStructureChanged() {};

      virtual void onGraphSet(FabricUI::GraphView::Graph * graph);

      FabricUI::DFG::DFGWidget * getDfgWidget();
      FabricUI::DFG::DFGController * getDFGController();

    private:

      void setModelRoot(
        FabricUI::DFG::DFGController *dfgController,
        FabricUI::ModelItems::BindingModelItem *bindingModelItem
        );
      void setModelRoot(
        FabricCore::DFGExec exec,
        FTL::CStrRef nodeName,
        FabricUI::ModelItems::ItemModelItem *nodeModelItem
        );

      DFG::DFGWidget * m_dfgWidget;
      FabricUI::GraphView::Graph * m_setGraph;
      QSharedPointer<DFG::DFGNotifier> m_notifier;
      QSharedPointer<DFG::DFGNotifier> m_subNotifier;
      DFGVEEditorOwner_NotifProxy *m_notifProxy;
    };
}
}

#endif // FABRICUI_DFG_DFGVEEDITOROWNER_H
