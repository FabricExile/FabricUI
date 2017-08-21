// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/MainPanel.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/InstBlockHeader.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/MouseGrabber.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/NodeHeader.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/PinCircle.h>
#include <FabricUI/GraphView/FixedPort.h>

#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QAction>
#include <QApplication>

#include <math.h>

using namespace FabricUI::GraphView;

MouseGrabber::MouseGrabber(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType, Connection *connectionPrevious)
: ConnectionTarget(parent->itemGroup())
  , m_lastSidePanel( NULL )
{
  m_connectionPos = mousePos;
  m_target = target;
  m_target->setHighlighted( true );
  m_otherPortType = portType;
  m_targetUnderMouse = NULL;
  m_connectionPrevious = connectionPrevious;

  const GraphConfig & config = parent->config();
  m_radius = config.mouseGrabberRadius;

  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  setPreferredSize(QSizeF(diameter() * 2.0, diameter() * 2.0));
  setPos(-diameter(), -diameter());
  setWindowFrameMargins(0, 0, 0, 0);

  QPointF localPos = parent->itemGroup()->mapFromScene(m_connectionPos);
  setZValue(-1);
  setTransform(QTransform::fromTranslate(localPos.x(), localPos.y()), false);
  grabMouse();

  if(m_otherPortType == PortType_Input)
    m_connection = new Connection(parent, m_target, this, true /* forceUseOfPinColor */);
  else
    m_connection = new Connection(parent, this, m_target, true /* forceUseOfPinColor */);
}

MouseGrabber::~MouseGrabber()
{
  if ( m_target )
    m_target->setHighlighted( false );
}

MouseGrabber * MouseGrabber::construct(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType, Connection *connectionPrevious)
{
  switch ( target->targetType() )
  {
    case TargetType_NodeHeader:
    {
      NodeHeader * header = static_cast<NodeHeader *>( target );
      Node * node = header->node();

      QMenu * menu = parent->controller()->gvcCreateNodeHeaderMenu(node, NULL, portType);
      if(menu == NULL)
        return NULL;

      QString name = "";

      if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
      {
        QList<QAction *> actions = menu->actions();
        if(actions.size() > 0)
        {
          QAction *action = actions[0];
          if(action->isEnabled())
          {
            name = action->data().toString();
            if (name != "exec")
              name = "";
          }
        }
      }

      if(name == "")
      {
        QAction * action = menu->exec(QCursor::pos());
        if(action == NULL)
          return NULL;

        name = action->data().toString();
      }

      if(name == "")
        return NULL;

      Pin * pin = node->pin(name.toUtf8().constData());
      if(!pin)
        return NULL;

      return construct(parent, mousePos, pin, portType, connectionPrevious);
    }
    break;

    case TargetType_InstBlockHeader:
    {
      InstBlockHeader * header = static_cast<InstBlockHeader *>( target );
      InstBlock * instBlock = header->instBlock();

      QMenu * menu = parent->controller()->gvcCreateInstBlockHeaderMenu(instBlock, NULL, portType);
      if(menu == NULL)
        return NULL;

      QString name = "";

      if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
      {
        QList<QAction *> actions = menu->actions();
        if(actions.size() > 0)
        {
          QAction *action = actions[0];
          if(action->isEnabled())
          {
            name = action->data().toString();
            if (name != "exec")
              name = "";
          }
        }
      }

      if(name == "")
      {
        QAction * action = menu->exec(QCursor::pos());
        if(action == NULL)
          return NULL;

        name = action->data().toString();
      }

      if(name == "")
        return NULL;

      InstBlockPort * instBlockPort =
        instBlock->instBlockPort(name.toUtf8().constData());
      if(!instBlockPort)
        return NULL;

      return construct(parent, mousePos, instBlockPort, portType, connectionPrevious);
    }
    break;

    default:
      return new MouseGrabber(parent, mousePos, target, portType, connectionPrevious);
  }
}

float MouseGrabber::radius() const
{
  return m_radius;
}

float MouseGrabber::diameter() const
{
  return m_radius * 2.0f;
}

ConnectionTarget * MouseGrabber::target()
{
  return m_target;
}

const ConnectionTarget * MouseGrabber::target() const
{
  return m_target;
}

Graph * MouseGrabber::graph()
{
  return target()->graph();
}

const Graph * MouseGrabber::graph() const
{
  return target()->graph();
}

QColor MouseGrabber::color() const
{
  return m_target->color();
}

QPointF MouseGrabber::connectionPos(PortType pType) const
{
  if(m_targetUnderMouse)
    return m_targetUnderMouse->connectionPos(m_otherPortType);
  return m_connectionPos;
}

void MouseGrabber::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  m_connectionPos = mapToScene(event->pos());
  QPointF mousePos = mapToItem(graph()->itemGroup(), event->pos());
  setTransform(QTransform::fromTranslate(mousePos.x(), mousePos.y()), false);

  emit positionChanged(mousePos);

  QList<QGraphicsItem *> items = collidingItems(Qt::IntersectsItemBoundingRect);

  bool isDraggingPortInSidePanel = false;
  if( m_lastSidePanel != NULL )
    m_lastSidePanel->onDraggingPortLeave();
  m_lastSidePanel = NULL;

  ConnectionTarget * newTargetUnderMouse = NULL;
  ConnectionTarget * prevTargetUnderMouse = m_targetUnderMouse;

  MainPanel * mainPanel = graph()->mainPanel();
  float zoom = mainPanel->canvasZoom();
  float distance = diameter() * zoom;
  for(int i=0;i<items.count();i++)
  {
    if(items[i]->type() == QGraphicsItemType_PinCircle)
    {
      PinCircle * pinCircle = (PinCircle *)items[i];
      ConnectionTarget * target = pinCircle->target();
      std::string failureReason;
      bool success = false;
      if(pinCircle->portType() == PortType_Input)
        success = m_target->canConnectTo(target, failureReason);
      else
        success = target->canConnectTo(m_target, failureReason);
      if(success)
      {
        QPointF diff = (pinCircle->centerInSceneCoords() - m_connectionPos); // use m_connectionPos so we're working in scene space
        float newDistanceSquared = diff.x() * diff.x() + diff.y() * diff.y(); // is good enough to compare distances (not need for the expensive sqrt)
        float newDistance = sqrt( newDistanceSquared );
        if(newDistance <= distance)
        {
          distance = newDistance;
          newTargetUnderMouse = target;
          QToolTip::hideText();
        }
      }
      else
      {
        if(failureReason.length() > 0)
        {
          QString failureReasonQString = failureReason.c_str();
          if(!QToolTip::isVisible() || QToolTip::text() != failureReasonQString)
          {
            QGraphicsView * view = scene()->views()[0];
            if(view)
            {
              // map the position
              QPoint widgetPos = view->mapFromScene( QPoint(
                int( m_connectionPos.x() ), int( m_connectionPos.y() ) ) );
              QPoint globalPos = view->mapToGlobal(widgetPos);

              // map the exit rect. if the mouse leaves the area the tooltip will disappear
              QRectF circleRect = pinCircle->boundingRect();
              QPointF circleTopLeft = circleRect.topLeft();
              QPointF circleBottomRight = circleRect.bottomRight();
              QPointF sceneCircleTopLeft = pinCircle->mapToScene(circleTopLeft);
              QPointF sceneCircleBottomRight = pinCircle->mapToScene(circleBottomRight);
              QPoint widgetCircleTopLeft =
                view->mapFromScene( QPoint( int( sceneCircleTopLeft.x() ),
                                            int( sceneCircleTopLeft.y() ) ) );
              QPoint widgetCircleBottomRight = view->mapFromScene(
                QPoint( int( sceneCircleBottomRight.x() ),
                        int( sceneCircleBottomRight.y() ) ) );

              QRect rect(widgetCircleTopLeft, widgetCircleBottomRight);
              if(rect.contains(widgetPos))
                QToolTip::showText(globalPos, failureReasonQString, view, rect);
              else
                QToolTip::hideText();
            }
          }
        }
      }
    }
    else if (
      items[i]->type() == QGraphicsItemType_SidePanel &&
      target()->targetType() == TargetType_Port
      )
    {
      Port* port = (Port*)target();
      SidePanel* sidePanel = (SidePanel*)items[i];
      if (
        port->allowEdits() // can it be re-ordered ?
        && port->sidePanel() == sidePanel
      )
      {
        sidePanel->onDraggingPort( event, port );
        isDraggingPortInSidePanel = true;
        m_lastSidePanel = sidePanel;
      }
    }
  }

  // changing the cursor to "simulate" QDrag
  if ( isDraggingPortInSidePanel )
    setCursor( Qt::ClosedHandCursor );
  else
    setCursor( Qt::ArrowCursor );

  if(newTargetUnderMouse == NULL && prevTargetUnderMouse != NULL)
  {
    prevTargetUnderMouse->setHighlighted(false);
    prevTargetUnderMouse = NULL;
  }
  else
  {
    if(prevTargetUnderMouse != NULL && prevTargetUnderMouse != newTargetUnderMouse)
      prevTargetUnderMouse->setHighlighted(false);
    if(newTargetUnderMouse)
      newTargetUnderMouse->setHighlighted(true);
  }

  m_targetUnderMouse = newTargetUnderMouse;
}

void MouseGrabber::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  bool ungrab = false;

  if( m_lastSidePanel )
    m_lastSidePanel->onDroppingPort();

  if (m_connectionPrevious) // [FE-8122]
  {
    std::vector<Connection *> conns;
    conns.push_back(m_connectionPrevious);
    graph()->controller()->gvcDoRemoveConnections(conns);
    m_connectionPrevious = NULL;
  }

  if(m_targetUnderMouse)
  {
    m_targetUnderMouse->setHighlighted(false);

    ConnectionTarget * source;
    ConnectionTarget * target;

    if(m_otherPortType == PortType_Input)
    {
      source = m_target;
      target = m_targetUnderMouse;
    }
    else
    {
      source = m_targetUnderMouse;
      target = m_target;
    }

    invokeConnect(source, target);

    // [FE-6842]
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
      bool forceUseOfPinColor = true;
      if (m_target->targetType() == TargetType_Pin)
      {
        Pin  *p    = static_cast<Pin *>( m_target );
        Node *node = p->node();
        Pin  *next = p;
        while (true)
        {
          next = node->nextPin(next->name());
          if (!next || next->portType() == p->portType())
            break;
        }
        if (next)
        {
          m_target = next;
          m_connection->invalidate();
          graph()->scene()->removeItem(m_connection);
          m_connection->deleteLater();
          if (m_otherPortType == PortType_Input)
            m_connection = new Connection(graph(), m_target, this, forceUseOfPinColor);
          else
            m_connection = new Connection(graph(), this, m_target, forceUseOfPinColor);
        }
        else
          ungrab = true;
      }
      else if (m_target->targetType() == TargetType_Port)
      {
        Port *p    = static_cast<Port *>( m_target );
        Port *next = p;
        while (true)
        {
          next = graph()->nextPort(next->name());
          if (!next || next->portType() == p->portType())
            break;
        }
        if (next)
        {
          m_target = next;
          m_connection->invalidate();
          graph()->scene()->removeItem(m_connection);
          m_connection->deleteLater();
          if (m_otherPortType == PortType_Input)
            m_connection = new Connection(graph(), m_target, this, forceUseOfPinColor);
          else
            m_connection = new Connection(graph(), this, m_target, forceUseOfPinColor);
        }
        else
          ungrab = true;
      }
    }
  }

  if(ungrab || (!event->modifiers().testFlag(Qt::ShiftModifier) && !event->modifiers().testFlag(Qt::ControlModifier)))
    performUngrab( 0 );

  if(!event->modifiers().testFlag(Qt::ShiftModifier))
  {
    // begin interaction is issued by connection +/ PinCircle
    graph()->controller()->endInteraction();
  }
}

void MouseGrabber::performUngrab( ConnectionTarget *fromCT )
{
  prepareGeometryChange();
  ungrabMouse();
  QGraphicsScene * scene = graph()->scene();
  graph()->resetMouseGrabber();
  m_connection->invalidate();
  scene->removeItem(m_connection);

  // [FE-8406] When deleting a Node while dragging a connection from it
  // the code in ~Connection() uses the Node. Thus, we have to delete the connection
  // first and then delete the Node (see FabricUI::GraphView::Graph::removeNode)
  delete m_connection;

  // m_connection->setParent(this);
  scene->removeItem(this);
  if ( m_target == fromCT )
    m_target = 0;
  if ( m_targetUnderMouse == fromCT )
    m_targetUnderMouse = 0;
  this->deleteLater();
}

void MouseGrabber::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  if(m_targetUnderMouse == NULL)
  { 
    QBrush outlineBrush = painter->brush();
    outlineBrush.setStyle(Qt::NoBrush);
    QColor color = m_connection->color();
    color.setAlpha(85);
    painter->setBrush(outlineBrush);
    painter->setPen(color);
    painter->drawEllipse(windowFrameRect().center(), diameter(), diameter());
  }
  QGraphicsWidget::paint(painter, option, widget);
}

void MouseGrabber::invokeConnect(ConnectionTarget * source, ConnectionTarget * target)
{
  // graph()->connect() handles all cases except the "node header" ones,
  // so if the function returns false we take care of the header cases here.
  if (!graph()->connect(source, target))
  {
    if (source->targetType() == TargetType_NodeHeader)
    {
      NodeHeader * header = static_cast<NodeHeader *>(source);
      Node * node = header->node();

      QPoint globalPos = QCursor::pos();
      invokeNodeHeaderMenu(node, target, PortType_Input, globalPos);
    }
    else if (target->targetType() == TargetType_NodeHeader)
    {
      NodeHeader * header = static_cast<NodeHeader *>(target);
      Node * node = header->node();

      QPoint globalPos = QCursor::pos();
      invokeNodeHeaderMenu(node, source, PortType_Output, globalPos);
    }
    else if (source->targetType() == TargetType_InstBlockHeader)
    {
      InstBlockHeader * header = static_cast<InstBlockHeader *>(source);
      InstBlock * instBlock = header->instBlock();

      QPoint globalPos = QCursor::pos();
      invokeInstBlockHeaderMenu(instBlock, target, PortType_Input, globalPos);
    }
    else if (target->targetType() == TargetType_InstBlockHeader)
    {
      InstBlockHeader * header = static_cast<InstBlockHeader *>(target);
      InstBlock * instBlock = header->instBlock();

      QPoint globalPos = QCursor::pos();
      invokeInstBlockHeaderMenu(instBlock, source, PortType_Output, globalPos);
    }
  }
}

void MouseGrabber::invokeNodeHeaderMenu(Node * node, ConnectionTarget * other, PortType nodeRole, QPoint pos)
{
  QMenu * menu = graph()->controller()->gvcCreateNodeHeaderMenu(node, other, nodeRole);
  if(menu == NULL)
    return;

  QString name = "";

  if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
  {
    QList<QAction *> actions = menu->actions();
    if(actions.size() > 0)
    {
      QAction *action = actions[0];
      if(action->isEnabled())
      {
        name = action->data().toString();
        if (name != "exec")
          name = "";
      }
    }
  }

  if(name == "")
  {
    QAction * action = menu->exec(pos);
    if(action == NULL)
      return;

    name = action->data().toString();
  }

  if(name == "")
    return;

  Pin * pin = node->pin(name.toUtf8().constData());
  if(!pin)
    return;

  if(other == NULL)
  {
    // this is invoked from MouseGrabber::construct
    other = pin;
  }
  else if(nodeRole == PortType_Output)
    invokeConnect(other, pin);
  else
    invokeConnect(pin, other);
}

void MouseGrabber::invokeInstBlockHeaderMenu(
  InstBlock * instBlock,
  ConnectionTarget * other,
  PortType instBlockRole,
  QPoint pos
  )
{
  QMenu * menu = graph()->controller()->gvcCreateInstBlockHeaderMenu(instBlock, other, instBlockRole);
  if(menu == NULL)
    return;

  QString name = "";

  if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
  {
    QList<QAction *> actions = menu->actions();
    if(actions.size() > 0)
    {
      QAction *action = actions[0];
      if(action->isEnabled())
      {
        name = action->data().toString();
        if (name != "exec")
          name = "";
      }
    }
  }

  if(name == "")
  {
    QAction * action = menu->exec(pos);
    if(action == NULL)
      return;

    name = action->data().toString();
  }

  if(name == "")
    return;

  InstBlockPort * instBlockPort = instBlock->instBlockPort(name.toUtf8().constData());
  if(!instBlockPort)
    return;

  if(other == NULL)
  {
    // this is invoked from MouseGrabber::construct
    other = instBlockPort;
  }
  else if(instBlockRole == PortType_Output)
    invokeConnect(other, instBlockPort);
  else
    invokeConnect(instBlockPort, other);
}
