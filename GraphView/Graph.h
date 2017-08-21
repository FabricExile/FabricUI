// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Graph__
#define __UI_GraphView_Graph__

#include <QGraphicsWidget>
#include <QGraphicsScene>
#include <QColor>
#include <QPen>
#include <QMenu>

#include <FTL/StrRef.h>

#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/PortType.h>
#include <FabricUI/Util/QString_Conversion.h>

namespace FabricUI
{

  namespace GraphView
  {

    class Controller;
    class BackDropNode;
    class Node;
    class Pin;
    class Port;
    class FixedPort;
    class Connection;
    class ConnectionTarget;
    class MouseGrabber;
    class MainPanel;
    class SidePanel;
    class InfoOverlay;

    class Graph : public QGraphicsWidget
    {
      Q_OBJECT

      friend class Controller;
      friend class RemoveNodeCommand;
      friend class AddConnectionCommand;
      friend class RemoveConnectionCommand;
      friend class Node;
      friend class MouseGrabber;

    public:

      Graph(
        QGraphicsItem * parent,
        const GraphConfig & config = GraphConfig()
        );
      virtual ~Graph() {}

      virtual void initialize();

      GraphConfig const &config() const
        { return m_config; }
      GraphConfig &config()
        { return m_config; }

      QGraphicsWidget * itemGroup();
      const QGraphicsWidget * itemGroup() const;

      Controller *controller()
        { return m_controller; }
      Controller const *controller() const
        { return m_controller; }
      void setController( Controller * c )
        { m_controller = c; }

      bool isEditable() const { return m_isEditable; }
      void setEditable( bool isEditable );

      MainPanel * mainPanel();
      const MainPanel * mainPanel() const;
      bool hasSidePanels() const;
      SidePanel * sidePanel(PortType portType);
      const SidePanel * sidePanel(PortType portType) const;

      // nodes
      virtual std::vector<Node *> nodes() const;
      virtual Node * node( FTL::StrRef name ) const;
      Node * node( QString const &name ) const
      {
        QByteArray nameUtf8 = name.toUtf8();
        return node( FTL::CStrRef( nameUtf8.constData() ) );
      }
      virtual Node * nodeFromPath( FTL::StrRef path ) const
        { return node( path ); }
      Node *renameNode( FTL::StrRef oldName, FTL::StrRef newName );

      virtual QRectF selectedNodesRect() const;
      virtual std::vector<Node *> selectedNodes() const;
      void selectAllNodes();
      void clearSelection() const;
      void clearInspection() const;

      // ports
      std::vector<Port *> ports() const;
      Port *port(FTL::StrRef name) const;
      Port *nextPort(FTL::StrRef name) const;
      std::vector<Port *> ports(FTL::StrRef name) const;

      // connections
      virtual std::vector<Connection *> connections() const;
      virtual bool isConnected(const ConnectionTarget * target) const;
      virtual bool isConnectedAsSource(const ConnectionTarget * target) const;
      virtual bool isConnectedAsTarget(const ConnectionTarget * target) const;
      virtual void updateColorForConnections(const ConnectionTarget * target) const;

      // context menus
      // menus are consumed by the graph, so they are destroyed after use.
      typedef QMenu* (*GraphContextMenuCallback)(Graph*, void*);
      typedef QMenu* (*NodeContextMenuCallback)(Node*, void*);
      typedef QMenu* (*PinContextMenuCallback)(Pin*, void*);
      typedef QMenu* (*ConnectionContextMenuCallback)(Connection*, void*);
      typedef QMenu* (*PortContextMenuCallback)(Port*, void*);
      typedef QMenu* (*FixedPortContextMenuCallback)(FixedPort*, void*);
      typedef QMenu* (*SidePanelContextMenuCallback)(SidePanel*, void*);
      virtual void setGraphContextMenuCallback(GraphContextMenuCallback callback, void * userData = NULL);
      virtual void setNodeContextMenuCallback(NodeContextMenuCallback callback, void * userData = NULL);
      virtual void setPinContextMenuCallback(PinContextMenuCallback callback, void * userData = NULL);
      virtual void setConnectionContextMenuCallback(ConnectionContextMenuCallback callback, void * userData = NULL);
      virtual void setPortContextMenuCallback(PortContextMenuCallback callback, void * userData = NULL);
      virtual void setFixedPortContextMenuCallback(FixedPortContextMenuCallback callback, void * userData = NULL);
      virtual void setSidePanelContextMenuCallback(SidePanelContextMenuCallback callback, void * userData = NULL);
      virtual QMenu* getGraphContextMenu();
      virtual QMenu* getNodeContextMenu(Node * node);
      virtual QMenu* getPinContextMenu(Pin * pin);
      virtual QMenu* getConnectionContextMenu(Connection * connection);
      virtual QMenu* getPortContextMenu(Port * Port);
      virtual QMenu* getFixedPortContextMenu(FixedPort * Port);
      virtual QMenu* getSidePanelContextMenu(SidePanel * sidePanel);

      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

      MouseGrabber * constructMouseGrabber(QPointF pos, ConnectionTarget * target, PortType portType, Connection *connectionPrevious);
      MouseGrabber * getMouseGrabber();

      // interaction
      virtual Node * addNode(Node * node, bool quiet = false);
      Node *addInstNode(
        FTL::CStrRef name,
        FTL::CStrRef preset,
        bool quiet = false
        );
      Node *addBlockNode(
        FTL::CStrRef name,
        FTL::CStrRef preset,
        bool quiet = false
        );
      Node *addPlainNode(
        FTL::CStrRef name,
        FTL::CStrRef preset,
        bool quiet = false
        );
      virtual BackDropNode * addBackDropNode( FTL::CStrRef name, bool quiet = false );
      virtual bool removeNode(Node * node, bool quiet = false);

      void addFixedPort( FixedPort *fixedPort );
      std::vector<FixedPort *> fixedPorts() const;
      std::vector<FixedPort *> fixedPorts( FTL::StrRef name ) const;
      void removeFixedPort( FixedPort *fixedPort );

      void addPort( Port *port );
      void removePort( Port *port );

      virtual Connection * addConnection(ConnectionTarget * src, ConnectionTarget * dst, bool quiet = false);
      virtual bool removeConnection(ConnectionTarget * src, ConnectionTarget * dst, bool quiet = false);
      virtual bool removeConnection(Connection * connection, bool quiet = false);
      virtual bool autoConnections();
      virtual bool removeConnections();
      virtual void resetMouseGrabber();
      void setConnectionsCosmetic( bool cosmetic );
      inline bool cosmeticConnections() const { return m_cosmeticConnections; }
      bool connect(ConnectionTarget * source, ConnectionTarget * target);
      void exposeAllPorts(bool exposeUnconnectedInputs, bool exposeUnconnectedOutputs);

      void updateOverlays(float width, float height);
      void setupBackgroundOverlay(QPointF pos, QString filePath);
      void setCentralOverlayText(QString text);
      void setCompsBlockedOverlayVisibility(bool state);

      void removeConnectionsForConnectionTarget( ConnectionTarget *target );

    public slots:

      void onNodeDoubleClicked(FabricUI::GraphView::Node * node, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
      void requestSidePanelInspect(FabricUI::GraphView::SidePanel *sidePanel);
      void requestMainPanelAction(Qt::KeyboardModifiers modifiers);
      void onBubbleEditRequested(FabricUI::GraphView::Node * node);

    signals:

      void graphChanged(FabricUI::GraphView::Graph * graph, QString path);
      void nodeAdded(FabricUI::GraphView::Node * node);
      void nodeRemoved(FabricUI::GraphView::Node * node);
      void nodeSelected(FabricUI::GraphView::Node * node);
      void nodeDeselected(FabricUI::GraphView::Node * node);
      void nodeMoved(FabricUI::GraphView::Node * node, QPointF pos);
      void nodeInspectRequested(FabricUI::GraphView::Node *);
      void nodeEditRequested(FabricUI::GraphView::Node *);
      void sidePanelInspectRequested();
      void connectionAdded(FabricUI::GraphView::Connection * connection);
      void connectionRemoved(FabricUI::GraphView::Connection * connection);
      void bubbleEditRequested(FabricUI::GraphView::Node * node);
      // FE-6926  : Shift + double-clicking in an empty space "Goes up"
      void goUpPressed();

    private:

      GraphConfig m_config;
      Controller * m_controller;
      std::vector<Node *> m_nodes;
      std::map<FTL::StrRef, size_t> m_nodeMap;
      std::vector<Connection *> m_connections;
      MouseGrabber * m_mouseGrabber;
      MainPanel * m_mainPanel;
      SidePanel * m_leftPanel;
      SidePanel * m_rightPanel;
      GraphContextMenuCallback m_graphContextMenuCallback;
      NodeContextMenuCallback m_nodeContextMenuCallback;
      PinContextMenuCallback m_pinContextMenuCallback;
      ConnectionContextMenuCallback m_connectionContextMenuCallback;
      PortContextMenuCallback m_portContextMenuCallback;
      FixedPortContextMenuCallback m_fixedPortContextMenuCallback;
      SidePanelContextMenuCallback m_sidePanelContextMenuCallback;
      void * m_graphContextMenuCallbackUD;
      void * m_nodeContextMenuCallbackUD;
      void * m_pinContextMenuCallbackUD;
      void * m_connectionContextMenuCallbackUD;
      void * m_portContextMenuCallbackUD;
      void * m_fixedPortContextMenuCallbackUD;
      void * m_sidePanelContextMenuCallbackUD;
      InfoOverlay * m_centralOverlay;
      QString m_centralOverlayText;
      InfoOverlay * m_compsBlockedOverlay;
      bool m_isEditable;
      QPointF m_overlayPos;
      QPixmap m_overlayPixmap;
      QGraphicsPixmapItem * m_overlayItem;
      double m_nodeZValue;
      double m_backdropZValue;
      double m_connectionZValue;
      bool m_cosmeticConnections;
    };

  };

};

#endif // __UI_GraphView_Graph__
