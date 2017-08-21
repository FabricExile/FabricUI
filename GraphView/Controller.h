// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Controller__
#define __UI_GraphView_Controller__

#include <QObject>
#include <QString>
#include <QPointF>
#include <QSizeF>
#include <QColor>

#include "PortType.h"

#include <vector>

#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>

class QMenu;

namespace FabricServices
{
  namespace Commands
  {
    class CompoundCommand;
  }
}


namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;
    class Node;
    class NodeHeader;
    class NodeHeaderButton;
    class BackDropNode;
    class Pin;
    class Port;
    class Connection;
    class ConnectionTarget;
    class InstBlock;

    class Controller : public QObject
    {
      Q_OBJECT
      
    public:

      Controller(Graph * graph);
      virtual ~Controller();

      Graph * graph();
      const Graph * graph() const;
      void setGraph(Graph * graph);

      virtual bool beginInteraction();
      virtual bool endInteraction();

      virtual std::string gvcDoCopy(
        ) = 0;

      virtual void gvcDoPaste(
        bool mapPositionToMouseCursor = true
        ) = 0;

      virtual bool gvcDoRemoveNodes(
        FTL::ArrayRef<GraphView::Node *> nodes
        ) = 0;

      virtual bool gvcDoAddConnections(
        std::vector<ConnectionTarget  *> const &srcs,
        std::vector<ConnectionTarget  *> const &dsts
        ) = 0;

      virtual bool gvcDoRemoveConnections(
        std::vector<ConnectionTarget  *> const &srcs,
        std::vector<ConnectionTarget  *> const &dsts
        ) = 0;

      virtual bool gvcDoAddInstFromPreset(
        QString presetPath,
        QPointF pos
        ) = 0;

      virtual void gvcDoAddPort(
        QString desiredPortName,
        PortType portType,
        QString typeSpec = QString(),
        ConnectionTarget *connectWith = 0,
        QString extDep = QString(),
        QString metaData = QString()
        ) = 0;

      virtual void gvcDoRenameExecPort(
        QString oldName,
        QString desiredPortName,
        QString execPath = "" // Path from the current Exec, to the edited Exec
      ) = 0;

      virtual void gvcDoRenameNode(
        Node* node,
        QString newName
      ) = 0;
      
      virtual void gvcDoSetNodeCommentExpanded(
        Node *node,
        bool expanded
        ) = 0;

      virtual void gvcDoMoveNode(
        Node const *node,
        QPointF const &nodeOriginalPos,
        QSizeF const &nodeOriginalSize,
        QPointF delta,
        float gridSnapSize,
        std::vector<QPointF> const &nodeSnapPositions,
        qreal nodeSnapDistance,
        std::vector<qreal> const &portSnapPositionsSrcY,
        std::vector<qreal> const &portSnapPositionsDstY,
        qreal portSnapDistance,
        bool allowUndo
        ) = 0;

      virtual void gvcDoMoveNodes(
        std::vector<Node *> const &nodes,
        std::vector<QPointF> const &nodesOriginalPos,
        QPointF delta,
        float gridSnapSize,
        bool allowUndo
        ) = 0;

      virtual void gvcDoResizeBackDropNode(
        GraphView::BackDropNode *backDropNode,
        QPointF newTopLeftPos,
        QSizeF newSize,
        float gridSnapSize,
        bool gridSnapTop,
        bool gridSnapBottom,
        bool gridSnapLeft,
        bool gridSnapRight,
        bool allowUndo
        ) = 0;

      virtual void gvcDoMoveExecPort(
        QString srcName,
        QString dstName
        ) = 0;

      virtual QString gvcGetCurrentExecPath() = 0;
      virtual bool gvcCurrentExecIsInstBlockExec() = 0;

      virtual bool selectNode(Node * node, bool state);
      virtual bool clearSelection();
      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool frameAndFitNodes( FTL::ArrayRef<Node *> nodes );
      virtual bool frameSelectedNodes();
      virtual bool frameAllNodes();
      virtual bool nodesAreVisible( FTL::ArrayRef<Node *> nodes );
      virtual bool allNodesAreVisible();
      virtual bool canConnectTo(
        char const *pathA,
        char const *pathB,
        std::string &failureReason
        ) const;

      bool gvcDoRemoveConnections(const std::vector<Connection*> & conns);

      virtual QMenu* gvcCreateNodeHeaderMenu( Node * node, ConnectionTarget * other, PortType nodeRole ) { return NULL; }
      virtual QMenu* gvcCreateInstBlockHeaderMenu( InstBlock *instBlock, ConnectionTarget *other, PortType nodeRole ) { return NULL; }
      virtual std::string gvcEncodeMetadaToPersistValue() { return ""; }

    virtual void collapseNodes(int state, const std::vector<Node*> & nodes);
    virtual void collapseSelectedNodes(int state);
    
    public slots:
      virtual void onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button);

    private:

      Graph * m_graph;
      unsigned int m_interactionBracket;
      FabricServices::Commands::CompoundCommand * m_compound;
    };

  };

};

#endif // __UI_GraphView_Controller__
