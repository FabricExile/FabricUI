// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#include <QGraphicsView>

#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/Exception.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/MouseGrabber.h>
#include <FabricUI/GraphView/MainPanel.h>
#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/GraphView/InfoOverlay.h>
#include <FabricUI/GraphView/FixedPort.h>

#include <FabricUI/Util/QtSignalsSlots.h>

#include <float.h>

using namespace FabricUI::GraphView;

Graph::Graph(
  QGraphicsItem * parent,
  const GraphConfig & config
  )
  : QGraphicsWidget(parent)
  , m_config( config )
  , m_cosmeticConnections( true )
{
  m_isEditable = true;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  setMinimumSize(100, 10);
  setContentsMargins(0, 0, 0, 0);

  m_controller = NULL;
  m_mouseGrabber = NULL;
  m_mainPanel = NULL;
  m_leftPanel = NULL;
  m_rightPanel = NULL;
  m_graphContextMenuCallback = NULL;
  m_nodeContextMenuCallback = NULL;
  m_pinContextMenuCallback = NULL;
  m_connectionContextMenuCallback = NULL;
  m_portContextMenuCallback = NULL;
  m_fixedPortContextMenuCallback = NULL;
  m_sidePanelContextMenuCallback = NULL;
  m_graphContextMenuCallbackUD = NULL;
  m_nodeContextMenuCallbackUD = NULL;
  m_pinContextMenuCallbackUD = NULL;
  m_connectionContextMenuCallbackUD = NULL;
  m_portContextMenuCallbackUD = NULL;
  m_fixedPortContextMenuCallbackUD = NULL;
  m_sidePanelContextMenuCallbackUD = NULL;
  m_overlayItem = NULL;
  m_nodeZValue = 3.0;
  m_backdropZValue = 1.0;
  m_connectionZValue = 2.0;
  m_centralOverlay = NULL;
  m_compsBlockedOverlay = NULL;
}

void Graph::requestSidePanelInspect(
  FabricUI::GraphView::SidePanel *sidePanel
  )
{
  emit sidePanelInspectRequested();
  clearInspection();
}

void Graph::requestMainPanelAction(
  Qt::KeyboardModifiers modifiers
  )
{
  // FE-6926  : Shift + double-clicking in an empty space "Goes up"
  if(modifiers.testFlag(Qt::ShiftModifier))
    emit goUpPressed();
}

void Graph::initialize()
{
  m_mainPanel = new MainPanel(this);
  QOBJECT_CONNECT(
    m_mainPanel, SIGNAL, MainPanel, doubleClicked, (Qt::KeyboardModifiers),
    this, SLOT, Graph, requestMainPanelAction, (Qt::KeyboardModifiers)
  );

  m_leftPanel = new SidePanel(this, PortType_Output);
  QOBJECT_CONNECT(
    m_leftPanel, SIGNAL, SidePanel, doubleClicked, (FabricUI::GraphView::SidePanel*),
    this, SLOT, Graph, requestSidePanelInspect, (FabricUI::GraphView::SidePanel*)
  );

  m_rightPanel = new SidePanel(this, PortType_Input);
  QOBJECT_CONNECT(
    m_rightPanel, SIGNAL, SidePanel, doubleClicked, (FabricUI::GraphView::SidePanel*),
    this, SLOT, Graph, requestSidePanelInspect, (FabricUI::GraphView::SidePanel*)
  );

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setOrientation(Qt::Horizontal);
  layout->addItem(m_leftPanel);
  layout->addItem(m_mainPanel);
  layout->addItem(m_rightPanel);
  setLayout(layout);
}

QGraphicsWidget * Graph::itemGroup() { return m_mainPanel->itemGroup(); }
const QGraphicsWidget * Graph::itemGroup() const { return m_mainPanel->itemGroup(); }

bool Graph::hasSidePanels() const
{
  return m_leftPanel != NULL;
}

void Graph::setEditable( bool isEditable )
{
  m_isEditable = isEditable;
  if ( hasSidePanels() ) {
    m_leftPanel->setEditable(isEditable);
    m_rightPanel->setEditable(isEditable);
  }
}

MainPanel * Graph::mainPanel()
{
  return m_mainPanel;
}

const MainPanel * Graph::mainPanel() const
{
  return m_mainPanel;
}

SidePanel * Graph::sidePanel(PortType portType)
{
  if(portType == PortType_Output)
    return m_leftPanel;
  if(portType == PortType_Input)
    return m_rightPanel;
  return NULL;
}

const SidePanel * Graph::sidePanel(PortType portType) const
{
  if(portType == PortType_Input)
    return m_leftPanel;
  if(portType == PortType_Output)
    return m_rightPanel;
  return NULL;
}

Node * Graph::addNode(Node * node, bool quiet)
{
  FTL::StrRef key = node->name();
  if(m_nodeMap.find(key) != m_nodeMap.end())
    return NULL;

  m_nodeMap.insert(std::pair<FTL::StrRef, size_t>(key, m_nodes.size()));
  m_nodes.push_back(node);

  double * zValue;
  if(node->isBackDropNode())
    zValue = &m_backdropZValue;
  else
    zValue = &m_nodeZValue;

  node->setZValue(*zValue);
  (*zValue) += 0.0001;
  if(node->bubble())
  {
    node->bubble()->setZValue(*zValue);
    (*zValue) += 0.0001;
  }

  QOBJECT_CONNECT(
    node,
    SIGNAL, Node, doubleClicked, ( FabricUI::GraphView::Node*, Qt::MouseButton, Qt::KeyboardModifiers ),
    this,
    SLOT, Graph, onNodeDoubleClicked, ( FabricUI::GraphView::Node*, Qt::MouseButton, Qt::KeyboardModifiers )
  );
  QObject::connect(node, SIGNAL(bubbleEditRequested(FabricUI::GraphView::Node*)), this, SLOT(onBubbleEditRequested(FabricUI::GraphView::Node*)));

  if(!quiet)
    emit nodeAdded(node);

  if(m_centralOverlay)
  {
    prepareGeometryChange();
    scene()->removeItem(m_centralOverlay);
    delete(m_centralOverlay);
    m_centralOverlay = NULL;
  }

  return node;
}

Node *Graph::addPlainNode(
  FTL::CStrRef name,
  FTL::CStrRef title,
  bool quiet
  )
{
  return addNode(
    new Node(
      this,
      Node::NodeType_Plain,
      name,
      title
      ),
    quiet
    );
}

Node *Graph::addInstNode(
  FTL::CStrRef name,
  FTL::CStrRef title,
  bool quiet
  )
{
  return addNode(
    new Node(
      this,
      Node::NodeType_Inst,
      name,
      title
      ),
    quiet
    );
}

Node *Graph::addBlockNode(
  FTL::CStrRef name,
  FTL::CStrRef title,
  bool quiet
  )
{
  return addNode(
    new Node(
      this,
      Node::NodeType_Block,
      name,
      title
      ),
    quiet
    );
}

BackDropNode *Graph::addBackDropNode(
  FTL::CStrRef name,
  bool quiet
  )
{
  BackDropNode *backDropNode = new BackDropNode( this, name );
  addNode( backDropNode, quiet );
  return backDropNode;
}

bool Graph::removeNode(Node * node, bool quiet)
{
  FTL::StrRef key = node->name();
  std::map<FTL::StrRef, size_t>::iterator it = m_nodeMap.find(key);
  if(it == m_nodeMap.end())
    return false;

  if ( m_mouseGrabber )
  {
    QList<ConnectionTarget *> cts;
    node->appendConnectionTargets( cts );
    for ( int i = 0; i < cts.size(); ++i )
    {
      ConnectionTarget *ct = cts[i];
      if ( m_mouseGrabber->target() == ct
        || m_mouseGrabber->targetUnderMouse() == ct )
      {
        m_mouseGrabber->performUngrab( ct );
        assert( !m_mouseGrabber );
        break;
      }
    }
  }

  for ( int i = int( node->instBlockCount() ); i--; )
    node->removeInstBlockAtIndex( i );

  for ( int i = int( node->pinCount() ); i--; )
  {
    Pin *pin = node->pin( i );
    node->removePin( pin );
  }

  controller()->beginInteraction();

  size_t index = it->second;
  m_nodes.erase(m_nodes.begin() + index);
  m_nodeMap.erase(it);

  // update the lookup
  for(it = m_nodeMap.begin(); it != m_nodeMap.end(); it++)
  {
    if(it->second > index)
      it->second--;
  }

  if(!quiet)
    emit nodeRemoved(node);

  prepareGeometryChange();
  scene()->removeItem(node);
  node->deleteLater();

  controller()->endInteraction();

  // recreate the overlay info
  if(m_nodes.size() == 0 && m_centralOverlayText.length() > 0)
    setCentralOverlayText(m_centralOverlayText);

  // [pzion 20160222] Workaround for possible bug in QGraphicsScene
  scene()->setItemIndexMethod( QGraphicsScene::NoIndex );
  scene()->setItemIndexMethod( QGraphicsScene::BspTreeIndex );

  return true;
}

std::vector<Node *> Graph::nodes() const
{
  std::vector<Node *> result;
  result.insert(result.end(), m_nodes.begin(), m_nodes.end());
  return result;
}

Node * Graph::node( FTL::StrRef name ) const
{
  std::map<FTL::StrRef, size_t>::const_iterator it = m_nodeMap.find(name);
  if(it == m_nodeMap.end())
    return NULL;
  return m_nodes[it->second];
}

QRectF Graph::selectedNodesRect() const
{
  QRectF rect;
  std::vector<Node *> nodes = selectedNodes();
  if (nodes.size() == 0)
    return rect;
  for ( size_t i = 0; i < nodes.size(); ++i )
  {
    Node *node = nodes[i];
    QRectF nodeBoundingRect = node->boundingRect();
    QPointF nodeTopLeftPos = node->topLeftGraphPos();
    rect |= nodeBoundingRect.translated( nodeTopLeftPos );
  }
  return rect;
}

std::vector<Node *> Graph::selectedNodes() const
{
  std::vector<Node *> result;
  for(size_t i=0;i<m_nodes.size();i++)
  {
    if(m_nodes[i]->selected())
      result.push_back(m_nodes[i]);
  }
  return result;
}

void Graph::selectAllNodes()
{
  for (size_t i=0;i<m_nodes.size();i++)
    m_nodes[i]->setSelected( true );
}

void Graph::clearSelection() const
{
  for (size_t i=0;i<m_nodes.size();i++)
    m_nodes[i]->setSelected( false );
}

void Graph::clearInspection() const
{
  for (size_t i=0;i<m_nodes.size();i++)
    m_nodes[i]->setInspected( false );
}

void Graph::addFixedPort( FixedPort *fixedPort )
{
  fixedPort->sidePanel()->addFixedPort( fixedPort );
}

void Graph::removeFixedPort( FixedPort *fixedPort )
{
  fixedPort->sidePanel()->removeFixedPort( fixedPort );
}

void Graph::addPort( Port *port )
{
  port->sidePanel()->addPort( port );
}

void Graph::removePort( Port *port )
{
  port->sidePanel()->removePort( port );
}

std::vector<FixedPort *> Graph::fixedPorts() const
{
  std::vector<FixedPort *> result;
  if ( hasSidePanels() )
  {
    for(unsigned int i=0;i<m_leftPanel->fixedPortCount();i++)
      result.push_back(m_leftPanel->fixedPort(i));
    for(unsigned int i=0;i<m_rightPanel->fixedPortCount();i++)
      result.push_back(m_rightPanel->fixedPort(i));
  }
  return result;
}

std::vector<Port *> Graph::ports() const
{
  std::vector<Port *> result;
  if(hasSidePanels())
  {
    for(unsigned int i=0;i<m_leftPanel->portCount();i++)
      result.push_back(m_leftPanel->port(i));
    for(unsigned int i=0;i<m_rightPanel->portCount();i++)
      result.push_back(m_rightPanel->port(i));
  }
  return result;
}

Port * Graph::port(FTL::StrRef name) const
{
  if(!hasSidePanels())
    return NULL;

  for(unsigned int i=0;i<m_leftPanel->portCount();i++)
  {
    if(name == m_leftPanel->port(i)->name())
      return m_leftPanel->port(i);
  }
  for(unsigned int i=0;i<m_rightPanel->portCount();i++)
  {
    if(name == m_rightPanel->port(i)->name())
      return m_rightPanel->port(i);
  }

  return NULL;
}

Port * Graph::nextPort(FTL::StrRef name) const
{
  if(!hasSidePanels())
    return NULL;

  for(unsigned int i=0;i<m_leftPanel->portCount();i++)
  {
    if(name == m_leftPanel->port(i)->name())
    {
      if (i + 1 < m_leftPanel->portCount())
      {
        return m_leftPanel->port(i + 1);
      }
      else
      {
        return NULL;
      }
    }
  }
  for(unsigned int i=0;i<m_rightPanel->portCount();i++)
  {
    if(name == m_rightPanel->port(i)->name())
    {
      if (i + 1 < m_rightPanel->portCount())
      {
        return m_rightPanel->port(i + 1);
      }
      else
      {
        return NULL;
      }
    }
  }

  return NULL;
}

std::vector<Port *> Graph::ports(FTL::StrRef name) const
{ 
  std::vector<Port *> result;
  if(!hasSidePanels())
    return result;

  for(unsigned int i=0;i<m_leftPanel->portCount();i++)
  {
    if(name == m_leftPanel->port(i)->name())
      result.push_back(m_leftPanel->port(i));
  }
  for(unsigned int i=0;i<m_rightPanel->portCount();i++)
  {
    if(name == m_rightPanel->port(i)->name())
      result.push_back(m_rightPanel->port(i));
  }

  return result;
}

std::vector<FixedPort *> Graph::fixedPorts(FTL::StrRef name) const
{ 
  std::vector<FixedPort *> result;
  if(!hasSidePanels())
    return result;

  for(unsigned int i=0;i<m_leftPanel->fixedPortCount();i++)
  {
    if(name == m_leftPanel->fixedPort(i)->name())
      result.push_back(m_leftPanel->fixedPort(i));
  }
  for(unsigned int i=0;i<m_rightPanel->fixedPortCount();i++)
  {
    if(name == m_rightPanel->fixedPort(i)->name())
      result.push_back(m_rightPanel->fixedPort(i));
  }

  return result;
}

std::vector<Connection *> Graph::connections() const
{
  return m_connections;
}

bool Graph::isConnected(const ConnectionTarget * target) const
{
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->dst() == target || m_connections[i]->src() == target)
      return true;
  }
  return false;
}

bool Graph::isConnectedAsSource(const ConnectionTarget * target) const
{
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->src() == target)
      return true;
  }
  return false;
}

bool Graph::isConnectedAsTarget(const ConnectionTarget * target) const
{
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->dst() == target)
      return true;
  }
  return false;
}

void Graph::updateColorForConnections(const ConnectionTarget * target) const
{
  if(!m_config.connectionUsePinColor)
    return;
  if(target == NULL)
    return;

  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->dst() == target || m_connections[i]->src() == target)
    {
      m_connections[i]->setColor(target->color());
    }
  }
}

bool Graph::connect(ConnectionTarget * source, ConnectionTarget * target)
{
  if (source->targetType() == TargetType_ProxyPort && target->targetType() == TargetType_Pin)
  {
    Pin *pinToConnectWith = static_cast<Pin *>(target);
    FTL::CStrRef pinName = pinToConnectWith->name();
    FTL::CStrRef dataType = pinToConnectWith->dataType();
    std::string metaData = controller()->gvcEncodeMetadaToPersistValue();
    controller()->gvcDoAddPort(
      QString::fromUtf8(pinName.data(), pinName.size()),
      PortType_Output,
      QString::fromUtf8(dataType.data(), dataType.size()),
      pinToConnectWith,
      QString(),
      QString::fromUtf8(metaData.data(), metaData.size())
    );
  }
  else if (target->targetType() == TargetType_ProxyPort && source->targetType() == TargetType_Pin)
  {
    Pin *pinToConnectWith = static_cast<Pin *>(source);
    FTL::CStrRef pinName = pinToConnectWith->name();
    FTL::CStrRef dataType = pinToConnectWith->dataType();
    controller()->gvcDoAddPort(
      QString::fromUtf8(pinName.data(), pinName.size()),
      PortType_Input,
      QString::fromUtf8(dataType.data(), dataType.size()),
      pinToConnectWith
    );
  }
  else if (source->targetType() == TargetType_ProxyPort && target->targetType() == TargetType_InstBlockPort)
  {
    InstBlockPort *instBlockPortToConnectWith = static_cast<InstBlockPort *>(target);
    FTL::CStrRef instBlockPortName = instBlockPortToConnectWith->name();
    FTL::CStrRef dataType = instBlockPortToConnectWith->dataType();
    std::string metaData = controller()->gvcEncodeMetadaToPersistValue();
    controller()->gvcDoAddPort(
      QString::fromUtf8(instBlockPortName.data(), instBlockPortName.size()),
      PortType_Output,
      QString::fromUtf8(dataType.data(), dataType.size()),
      instBlockPortToConnectWith,
      QString(),
      QString::fromUtf8(metaData.data(), metaData.size())
    );
  }
  else if (target->targetType() == TargetType_ProxyPort && source->targetType() == TargetType_InstBlockPort)
  {
    InstBlockPort *instBlockPortToConnectWith = static_cast<InstBlockPort *>(source);
    FTL::CStrRef instBlockPortName = instBlockPortToConnectWith->name();
    FTL::CStrRef dataType = instBlockPortToConnectWith->dataType();
    controller()->gvcDoAddPort(
      QString::fromUtf8(instBlockPortName.data(), instBlockPortName.size()),
      PortType_Input,
      QString::fromUtf8(dataType.data(), dataType.size()),
      instBlockPortToConnectWith
    );
  }
  else if (source->targetType() == TargetType_NodeHeader)
  {
    return false;
  }
  else if (target->targetType() == TargetType_NodeHeader)
  {
    return false;
  }
  else if (source->targetType() == TargetType_InstBlockHeader)
  {
    return false;
  }
  else if (target->targetType() == TargetType_InstBlockHeader)
  {
    return false;
  }
  else
  {
    std::vector<ConnectionTarget  *> sources;
    std::vector<ConnectionTarget  *> targets;
    sources.push_back(source);
    targets.push_back(target);
    controller()->gvcDoAddConnections(sources, targets);
  }
  return true;
}

Connection * Graph::addConnection(ConnectionTarget * src, ConnectionTarget * dst, bool quiet)
{
  if(src == dst)
    return NULL;

  if(src->targetType() == TargetType_MouseGrabber || dst->targetType() == TargetType_MouseGrabber)
    return NULL;

  // make sure this connection does not exist yet
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->src() == src && m_connections[i]->dst() == dst)
      return NULL;
  }

  prepareGeometryChange();
  controller()->beginInteraction();

  Connection * connection = new Connection(this, src, dst);
  m_connections.push_back(connection);
  connection->setCosmetic( m_cosmeticConnections );

  if(connection->src()->targetType() == TargetType_Pin)
  {
    Pin * pin = (Pin*)connection->src();
    pin->setDaisyChainCircleVisible(true);
    Node * node = pin->node();
    node->onConnectionsChanged();
  }
  else if ( connection->src()->targetType() == TargetType_InstBlockPort )
  {
    InstBlockPort *instBlockPort = (InstBlockPort *)connection->src();
    instBlockPort->setDaisyChainCircleVisible(true);
    InstBlock *instBlock = instBlockPort->instBlock();
    instBlock->onConnectionsChanged();
  }

  if(connection->dst()->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)connection->dst())->node();
    node->onConnectionsChanged();
  }
  else if ( connection->dst()->targetType() == TargetType_InstBlockPort )
  {
    InstBlockPort *instBlockPort = (InstBlockPort *)connection->dst();
    instBlockPort->setDaisyChainCircleVisible(true);
    InstBlock *instBlock = instBlockPort->instBlock();
    instBlock->onConnectionsChanged();
  }

  connection->setZValue(m_connectionZValue);
  m_connectionZValue += 0.0001;

  if(!quiet)
    emit connectionAdded(connection);

  controller()->endInteraction();

  return connection;
}

bool Graph::removeConnection(ConnectionTarget * src, ConnectionTarget * dst, bool quiet)
{
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->src() == src && m_connections[i]->dst() == dst)
    {
      return removeConnection(m_connections[i], quiet);
    }
  }
  return false;
}

bool Graph::removeConnection(Connection * connection, bool quiet)
{
  bool found = false;
  size_t index;
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i] == connection)
    {
      found = true;
      index = i;
      break;
    }
  }
  if(!found)
    return false;

  prepareGeometryChange();
  controller()->beginInteraction();

  Pin * daisyChainPin = NULL;
  if(connection->src()->targetType() == TargetType_Pin)
  {
    daisyChainPin = (Pin*)connection->src();
  }

  ConnectionTarget * src = connection->src();
  ConnectionTarget * dst = connection->dst();

  if(connection->dst()->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)connection->dst())->node();
    node->onConnectionsChanged();
  }

  m_connections.erase(m_connections.begin() + index);
  if(!quiet)
    emit connectionRemoved(connection);

  if(daisyChainPin)
  {
    bool found = false;
    for(size_t i=0;i<m_connections.size();i++)
    {
      if(m_connections[i]->src() == daisyChainPin)
      {
        found = true;
        break;
      }
    }
    daisyChainPin->setDaisyChainCircleVisible(found);
  }

  prepareGeometryChange();
  connection->invalidate();
  scene()->removeItem(connection);
  delete(connection);

  if(src->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)src)->node();
    node->onConnectionsChanged();
  }
  else if(src->targetType() == TargetType_InstBlockPort )
  {
    InstBlock *instBlock = ((InstBlockPort *)src)->instBlock();
    instBlock->onConnectionsChanged();
  }

  if(dst->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)dst)->node();
    node->onConnectionsChanged();
  }
  else if(dst->targetType() == TargetType_InstBlockPort )
  {
    InstBlock *instBlock = ((InstBlockPort *)dst)->instBlock();
    instBlock->onConnectionsChanged();
  }

  controller()->endInteraction();

  return true;
}

bool Graph::autoConnections()
{
  // get the selected nodes and create an array of arrays of vertical node groups.
  std::vector<Node *>              selectedNodes = Graph::selectedNodes();
  std::vector< std::vector<Node *> > nodeGroups;
  while (selectedNodes.size() > 0)
  {
    // find the index of the left-most node in selectedNodes.
    int mostLeftIdx = 0;
    for (unsigned int i=0;i<selectedNodes.size();i++)
      if (selectedNodes[i]->sceneBoundingRect().left() < selectedNodes[mostLeftIdx]->sceneBoundingRect().left())
        mostLeftIdx = i;

    // init new nodeGroup rect with the left-most node's rect
    // and add all nodes to the nodeGroup that intersect with it.
    QRectF nodeGroupRect = selectedNodes[mostLeftIdx]->sceneBoundingRect();
    nodeGroupRect.setTop   (FLT_MIN);
    nodeGroupRect.setBottom(FLT_MAX);
    std::vector<Node *> nodeGroup;
    for (int i=0;i<(int)selectedNodes.size();i++)
      if (nodeGroupRect.intersects(selectedNodes[i]->sceneBoundingRect()))
      {
        nodeGroup.push_back(selectedNodes[i]);
        nodeGroupRect |= selectedNodes[i]->sceneBoundingRect();
        selectedNodes.erase(selectedNodes.begin() + i);
        i = -1;
      }

    // sort the nodes in the group from top
    // to bottom using a simple bubble sort.
    for (unsigned int i=0;i<nodeGroup.size();i++)
      for (unsigned int j=i+1;j<nodeGroup.size();j++)
        if (nodeGroup[i]->sceneBoundingRect().center().y() > nodeGroup[j]->sceneBoundingRect().center().y())
          std::swap(nodeGroup[i], nodeGroup[j]);

    // add the result to nodeGroups.
    nodeGroups.push_back(nodeGroup);
  }

  // create the final src/dst arrays of connections.
  std::vector<ConnectionTarget *> ctSrcs;
  std::vector<ConnectionTarget *> ctDsts;
  for (unsigned int ci=0;ci+1<nodeGroups.size();ci++)
  {
    std::vector<Node *> &nodesL = nodeGroups[ci];
    std::vector<Node *> &nodesR = nodeGroups[ci + 1];

    // get usable left ports.
    std::vector<Pin *> pinsL;
    for(unsigned int i=0;i<nodesL.size();i++)
    {
      Node *node = nodesL[i];
      for(unsigned int j=0;j<node->pinCount();j++)
      {
        Pin *pin = node->pin(j);
        // skip default exec port.
        if (j == 0)
          continue;
        // skip if already connected.
        if (pin->isConnectedAsSource())
          continue;
        // use if port is output or IO.
        if (pin->portType() != PortType_Input)
          pinsL.push_back(pin);
        // use if node only has one input port except for the exec one.
        else if (node->pinCount() == 2)
            pinsL.push_back(pin);
      }
    }

    // get usable right ports.
    std::vector<Pin *> pinsR;
    for(unsigned int i=0;i<nodesR.size();i++)
    {
      Node *node = nodesR[i];
      for(unsigned int j=0;j<node->pinCount();j++)
      {
        Pin *pin = node->pin(j);
        // skip default exec port.
        if (j == 0)
          continue;
        // skip if already connected.
        if (pin->isConnectedAsTarget())
          continue;
        // use if port is input or IO.
        if (pin->portType() != PortType_Output)
          pinsR.push_back(pin);
      }
    }

    // add connectable things to the src/dst arrays.
    for (unsigned int i=0;i<pinsL.size();i++)
    {
      std::string failureReason;
      for (unsigned int j=0;j<pinsR.size();j++)
        if (!!pinsR[j] && pinsL[i]->canConnectTo(pinsR[j], failureReason))
        {
          ctSrcs.push_back(pinsL[i]);
          ctDsts.push_back(pinsR[j]);
          pinsR[j] = NULL;
          break;
        }
    }
  }

  // create final connections from src/dst arrays.
  return (ctSrcs.size() ? controller()->gvcDoAddConnections(ctSrcs, ctDsts) : true);
}

bool Graph::removeConnections()
{
  std::vector<Connection*> conns;

  // we first check if there are hovered connections.
  // If there are none, we do the regular 'disconnect
  // all ports' thing.

  for(int i=0;i<(int)m_connections.size();i++)
    if (m_connections[i]->isHovered())
        conns.push_back(m_connections[i]);

  if (!conns.size())
  {
    for(int i=0;i<(int)m_connections.size();i++)
    {
      ConnectionTarget *srcCT = m_connections[i]->src();
      bool srcIsSelected = srcCT && srcCT->selected();

      ConnectionTarget *dstCT = m_connections[i]->dst();
      bool dstIsSelected = dstCT && dstCT->selected();

      if (srcIsSelected != dstIsSelected)
        conns.push_back(m_connections[i]);
    }
  }

  return (conns.size() ? controller()->gvcDoRemoveConnections(conns) : true);
}

MouseGrabber * Graph::constructMouseGrabber(QPointF pos, ConnectionTarget * target, PortType portType, Connection *connectionPrevious)
{
  if(!m_isEditable)
    return NULL;
  m_mouseGrabber = MouseGrabber::construct(this, pos, target, portType, connectionPrevious);
  return m_mouseGrabber;
}

MouseGrabber * Graph::getMouseGrabber()
{
  return m_mouseGrabber;
}

void Graph::resetMouseGrabber()
{
  m_mouseGrabber = NULL;
}

void Graph::setConnectionsCosmetic( bool cosmetic )
{
  m_cosmeticConnections = cosmetic;
  const std::vector<Connection*> connections = this->connections();
  for( size_t i = 0; i < connections.size(); i++ )
    connections[i]->setCosmetic( m_cosmeticConnections );
  if( getMouseGrabber() )
  {
    Connection* connection = getMouseGrabber()->connection();
    if( connection )
      connection->setCosmetic( m_cosmeticConnections );
  }
}

void Graph::exposeAllPorts(bool exposeUnconnectedInputs, bool exposeUnconnectedOutputs)
{
  if (!exposeUnconnectedInputs && !exposeUnconnectedOutputs)
    return;

  std::vector<Node *> nodes = selectedNodes();

  // sort the nodes from top
  // to bottom via bubble sort.
  for (unsigned int i = 0; i<nodes.size(); i++)
    for (unsigned int j = i + 1; j<nodes.size(); j++)
      if (nodes[i]->sceneBoundingRect().center().y() > nodes[j]->sceneBoundingRect().center().y())
        std::swap(nodes[i], nodes[j]);


  // do it.
  for (size_t i=0;i<nodes.size();i++)
  {
    Node *node = nodes[i];

    if (node == NULL)
      return;

    if (exposeUnconnectedInputs)
    {
      ConnectionTarget *source = (ConnectionTarget *)m_leftPanel->m_proxyPort;
      for (unsigned int j = 0; j<node->pinCount(); j++)
      {
        Pin *pin = node->pin(j);
        // skip default exec port.
        if (j == 0)
          continue;
        // skip if already connected.
        if (pin->isConnectedAsTarget())
          continue;
        // we have a candiate.
        if (pin->portType() != PortType_Output)
        {
          ConnectionTarget *target = pin;
          connect(source, target);
        }
      }
    }

    if (exposeUnconnectedOutputs)
    {
      ConnectionTarget *target = (ConnectionTarget *)m_rightPanel->m_proxyPort;
      for (unsigned int j = 0; j<node->pinCount(); j++)
      {
        Pin *pin = node->pin(j);
        // skip default exec port.
        if (j == 0)
          continue;
        // skip if already connected.
        if (pin->isConnectedAsSource())
          continue;
        // we have a candidate.
        if (pin->portType() != PortType_Input)
        {
          ConnectionTarget *source = pin;
          connect(source, target);
        }
      }
    }
  }
}

void Graph::updateOverlays(float width, float height)
{
  if(m_overlayItem != NULL)
  {
    QSizeF overlaySize = m_overlayItem->boundingRect().size();

    if ( 3 * overlaySize.width() > width
      || 3 * overlaySize.height() > height )
    {
      m_overlayItem->hide();
    }
    else
    {
      m_overlayItem->show();

      QPointF pos = m_overlayPos;
      if(pos.x() < 0.0f) 
      {
        if(m_rightPanel)
          pos.setX(width + pos.x() - m_rightPanel->rect().width());
        else
          pos.setX(width + pos.x());
      }
      else if(m_leftPanel)
      {
        pos.setX(pos.x() + m_leftPanel->rect().width());
      }
      
      if(pos.y() < 0.0) pos.setY(height + pos.y());

      m_overlayItem->setPos(pos);
    }
  }

  if(m_centralOverlay != NULL)
  {
    QPointF pos;
    pos.setX(-m_centralOverlay->minimumWidth() * 0.5 + m_mainPanel->size().width() * 0.5);
    pos.setY(-m_centralOverlay->minimumHeight() * 0.5 + m_mainPanel->size().height() * 0.5);
    m_centralOverlay->setPos(pos);
  }

  if (m_compsBlockedOverlay)
  {
    QPointF pos;
    pos.setX(-m_compsBlockedOverlay->minimumWidth()  + m_mainPanel->size().width()  - 10);
    pos.setY(-m_compsBlockedOverlay->minimumHeight() + m_mainPanel->size().height() - 10);
    m_compsBlockedOverlay->setPos(pos);
  }
}

void Graph::setupBackgroundOverlay(QPointF pos, QString filePath)
{
  m_overlayPos = pos;
  m_overlayPixmap = QPixmap(filePath);  
  if(m_overlayItem == NULL)
    m_overlayItem = new QGraphicsPixmapItem(m_overlayPixmap, this);
  else
    m_overlayItem->setPixmap(m_overlayPixmap);
  m_overlayItem->setZValue(2000);
  updateOverlays(rect().width(), rect().height());
}

void Graph::setCentralOverlayText(QString text)
{
  if(m_centralOverlay)
    return;
  m_centralOverlay = new InfoOverlay(this, text, m_config);
  m_centralOverlayText = text;

  float height = rect().height();
  if(scene())
  {
    if(scene()->views().count() > 0)
    {
      QGraphicsView * view = scene()->views()[0];
      height = (float)view->size().height();
    }
  }
 
  updateOverlays(rect().width(), height);
}

void Graph::setCompsBlockedOverlayVisibility(bool state)
{
  if (!m_compsBlockedOverlay)
    m_compsBlockedOverlay = new InfoOverlay(this, "Graph Compilations Disabled", m_config);
  m_compsBlockedOverlay->setVisible(state);
  updateOverlays(rect().width(), rect().height());
}

void Graph::onNodeDoubleClicked(FabricUI::GraphView::Node * node, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
  if ( modifiers.testFlag( Qt::ShiftModifier ) )
    emit nodeEditRequested( node );
  else
    emit nodeInspectRequested( node );
}

void Graph::onBubbleEditRequested(FabricUI::GraphView::Node * node)
{
  emit bubbleEditRequested(node);
}

void Graph::setGraphContextMenuCallback(Graph::GraphContextMenuCallback callback, void * userData)
{
  m_graphContextMenuCallback = callback;
  m_graphContextMenuCallbackUD = userData;
}

void Graph::setNodeContextMenuCallback(Graph::NodeContextMenuCallback callback, void * userData)
{
  m_nodeContextMenuCallback = callback;
  m_nodeContextMenuCallbackUD = userData;
}

void Graph::setPinContextMenuCallback(Graph::PinContextMenuCallback callback, void * userData)
{
  m_pinContextMenuCallback = callback;
  m_pinContextMenuCallbackUD = userData;
}

void Graph::setConnectionContextMenuCallback(Graph::ConnectionContextMenuCallback callback, void * userData)
{
  m_connectionContextMenuCallback = callback;
  m_connectionContextMenuCallbackUD = userData;
}

void Graph::setPortContextMenuCallback(Graph::PortContextMenuCallback callback, void * userData)
{
  m_portContextMenuCallback = callback;
  m_portContextMenuCallbackUD = userData;
}

void Graph::setFixedPortContextMenuCallback(Graph::FixedPortContextMenuCallback callback, void * userData)
{
  m_fixedPortContextMenuCallback = callback;
  m_fixedPortContextMenuCallbackUD = userData;
}

void Graph::setSidePanelContextMenuCallback(Graph::SidePanelContextMenuCallback callback, void * userData)
{
  m_sidePanelContextMenuCallback = callback;
  m_sidePanelContextMenuCallbackUD = userData;
}

QMenu* Graph::getGraphContextMenu()
{
  if(!m_graphContextMenuCallback)
    return NULL;
  return (*m_graphContextMenuCallback)(this, m_graphContextMenuCallbackUD);
}

QMenu* Graph::getNodeContextMenu(Node * node)
{
  if(!m_nodeContextMenuCallback)
    return NULL;
  return (*m_nodeContextMenuCallback)(node, m_nodeContextMenuCallbackUD);
}

QMenu *Graph::getPinContextMenu(Pin * pin)
{
  if(!m_pinContextMenuCallback)
    return NULL;
  return (*m_pinContextMenuCallback)(pin, m_pinContextMenuCallbackUD);
}

QMenu *Graph::getConnectionContextMenu(Connection * connection)
{
  if(!m_connectionContextMenuCallback)
    return NULL;
  return (*m_connectionContextMenuCallback)(connection, m_connectionContextMenuCallbackUD);
}

QMenu *Graph::getPortContextMenu(Port * port)
{
  if(!m_portContextMenuCallback)
    return NULL;
  return (*m_portContextMenuCallback)(port, m_portContextMenuCallbackUD);
}

QMenu *Graph::getFixedPortContextMenu( FixedPort *fixedPort )
{
  if ( !m_fixedPortContextMenuCallback )
    return NULL;
  return (*m_fixedPortContextMenuCallback)(fixedPort, m_fixedPortContextMenuCallbackUD);
}

QMenu *Graph::getSidePanelContextMenu(SidePanel * sidePanel)
{
  if(!m_sidePanelContextMenuCallback)
    return NULL;
  return (*m_sidePanelContextMenuCallback)(sidePanel, m_sidePanelContextMenuCallbackUD);
}

void Graph::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  QGraphicsWidget::paint(painter, option, widget);
}

Node *Graph::renameNode( FTL::StrRef oldName, FTL::StrRef newName )
{
  std::map<FTL::StrRef, size_t>::iterator it = m_nodeMap.find( oldName );
  if ( it != m_nodeMap.end() )
  {
    size_t index = it->second;
    Node *node = m_nodes[index];
    m_nodeMap.erase( it );
    node->m_name = newName;
    m_nodeMap.insert(
      std::pair<FTL::StrRef, size_t>(
        node->m_name, index
        )
      );
    return node;
  }
  else return 0;
}

void Graph::removeConnectionsForConnectionTarget( ConnectionTarget *target )
{
  for ( int i = m_connections.size(); i--; )
  {
    Connection * con = m_connections[i];
    if ( con->src() == target || con->dst() == target )
    {
      m_connections.erase( m_connections.begin() + i );
      break;
    }
  }
}
