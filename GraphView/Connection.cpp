// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPainterPathStroker>

#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/ConnectionTarget.h>
#include <FabricUI/GraphView/FixedPort.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/MainPanel.h>
#include <FabricUI/GraphView/MouseGrabber.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/FixedPort.h>
#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/GraphView/Node.h>

#include <algorithm>
#include <cstdlib>
#include <math.h>

using namespace FabricUI::GraphView;

Connection::Connection(
  Graph * graph,
  ConnectionTarget * src,
  ConnectionTarget * dst,
  bool forceUseOfPinColor
  )
  : QObject( graph->itemGroup() )
  , QGraphicsPathItem( graph->itemGroup() )
  , m_graph( graph )
  , m_src( src )
  , m_dst( dst )
  , m_cosmeticPen( graph->cosmeticConnections() )
  , m_hovered( false )
  , m_dragging( false )
  , m_aboutToBeDeleted( false )
  , m_hasSelectedTarget( false )
{
  bool isExposedConnectionSrc = (   m_src->targetType() == TargetType_Port
                                 || m_src->targetType() == TargetType_FixedPort
                                 || m_src->targetType() == TargetType_ProxyPort );
  bool isExposedConnectionDst = (   m_dst->targetType() == TargetType_Port
                                 || m_dst->targetType() == TargetType_FixedPort
                                 || m_dst->targetType() == TargetType_ProxyPort );

  m_isExposedConnection = (   isExposedConnectionSrc
                           || isExposedConnectionDst );

  m_tooltip = "<p style='white-space:pre'>";
  {
    QString srcPortName;
    QString srcPortParent;
    QString srcPortDataType;
    QString dstPortName;
    QString dstPortParent;
    QString dstPortDataType;
    for (int i=0;i<2;i++)
    {
      ConnectionTarget *target       = (i == 0 ? src             : dst);
      QString          &portName     = (i == 0 ? srcPortName     : dstPortName);
      QString          &portParent   = (i == 0 ? srcPortParent   : dstPortParent);
      QString          &portDataType = (i == 0 ? srcPortDataType : dstPortDataType);
      switch (target->targetType())
      {
        case TargetType_Pin:
        {
          Pin &t = *(Pin *)target;
          if (!t.name()    .empty())  portName     = t.name().c_str();
          if (!t.dataType().empty())  portDataType = t.dataType().c_str();
          portParent = t.node()->name_QS() + QString(".");
        } break;
        case TargetType_Port:
        {
          Port &t = *(Port *)target;
          if (!t.name()    .empty())  portName     = t.name().c_str();
          if (!t.dataType().empty())  portDataType = t.dataType().c_str();
        } break;
        case TargetType_InstBlockPort:
        {
          InstBlockPort &t = *(InstBlockPort *)target;
          if (!t.name()    .empty())  portName     = t.name().c_str();
          if (!t.dataType().empty())  portDataType = t.dataType().c_str();
          portParent = t.instBlock()->node()->name_QS() + QString(".") + t.instBlock()->name_QS() + QString(".");
        } break;
        default:
          break;
      };
    }
    if (srcPortName    .isEmpty())  srcPortName     = "unknown";
    if (srcPortDataType.isEmpty())  srcPortDataType = "unknown";
    if (dstPortName    .isEmpty())  dstPortName     = "unknown";
    if (dstPortDataType.isEmpty())  dstPortDataType = "unknown";
    m_tooltip += " " + srcPortParent + "<font color=#0000aa>" + srcPortName + "</font> (<font color=#003300>" + srcPortDataType + "</font>)";
    m_tooltip += "  >  ";
    m_tooltip += dstPortParent + "<font color=#0000aa>" + dstPortName + "</font> (<font color=#003300>" + dstPortDataType + "</font>)" + " ";
    m_tooltip += "<p/>";
  }
  enableToolTip(m_graph->config().connectionShowTooltip);

  if(m_isExposedConnection)   m_defaultPen = m_graph->config().connectionExposePen;
  else                        m_defaultPen = m_graph->config().connectionDefaultPen;
  m_hoverPen       = m_graph->config().connectionHoverPen;
  m_clipRadius     = m_graph->config().connectionExposeRadius;
  m_shapePathWidth = qMax(1.0f, qMin(20.0f, m_graph->config().connectionClickableDistance));

  QColor color = m_graph->config().connectionColor;
  if(m_graph->config().connectionUsePinColor || forceUseOfPinColor)
  {
    for(int i=0;i<2;i++)
    {
      ConnectionTarget *target = (i == 0 ? src : dst);

      if(target->targetType() == TargetType_Pin)
      {
        color = ((Pin*)target)->color();
        break;
      }
      else if(target->targetType() == TargetType_Port)
      {
        color = ((Port*)target)->color();
      }
      else if(target->targetType() == TargetType_InstBlockPort)
      {
        color = ((InstBlockPort*)target)->color();
      }
    }
  }
  setColor(color);
  setAcceptHoverEvents(true);
  updatePen();

  setZValue(-1);

  dependencyMoved();
  dependencySelected();

  MainPanel *mainPanel = graph->mainPanel();
  QObject::connect(
    mainPanel, SIGNAL(geometryChanged()),
    this, SLOT(dependencyMoved())
    );

  if (m_isExposedConnection)
  {
    QObject::connect(
      mainPanel, SIGNAL(canvasZoomChanged(float)),
      this, SLOT(dependencyMoved())
      );
    QObject::connect(
      mainPanel, SIGNAL(canvasPanChanged(QPointF)),
      this, SLOT(dependencyMoved())
      );
  }

  for(int i=0;i<2;i++)
  {
    ConnectionTarget * target = src;
    if(i>0)
      target = dst;

    if(target->targetType() == TargetType_Pin)
    {
      Pin * pin = (Pin*)target;
      connect(
        pin, SIGNAL(drawStateChanged()),
        this, SLOT(dependencyMoved())
        );
      if ( i == 0 )
        QObject::connect(
          pin, SIGNAL(outCircleScenePositionChanged()),
          this, SLOT(dependencyMoved())
          );
      else
        QObject::connect(
          pin, SIGNAL(inCircleScenePositionChanged()),
          this, SLOT(dependencyMoved())
          );
      Node * node = pin->node();
      QObject::connect(node, SIGNAL(selectionChanged(FabricUI::GraphView::Node *, bool)), this, SLOT(dependencySelected()));
    }
    if(target->targetType() == TargetType_InstBlockPort)
    {
      InstBlockPort * instBlockPort = (InstBlockPort*)target;
      connect(
        instBlockPort, SIGNAL(drawStateChanged()),
        this, SLOT(dependencyMoved())
        );
      if ( i == 0 )
        QObject::connect(
          instBlockPort, SIGNAL(outCircleScenePositionChanged()),
          this, SLOT(dependencyMoved())
          );
      else
        QObject::connect(
          instBlockPort, SIGNAL(inCircleScenePositionChanged()),
          this, SLOT(dependencyMoved())
          );
      Node * node = instBlockPort->instBlock()->node();
      QObject::connect(node, SIGNAL(selectionChanged(FabricUI::GraphView::Node *, bool)), this, SLOT(dependencySelected()));
    }
    else if(target->targetType() == TargetType_MouseGrabber)
    {
      MouseGrabber * grabber = (MouseGrabber*)target;
      QObject::connect(grabber, SIGNAL(positionChanged(QPointF)), this, SLOT(dependencyMoved()));
    }
    else if(target->targetType() == TargetType_Port)
    {
      Port * port = (Port*)target;
      QObject::connect(port, SIGNAL(positionChanged()), this, SLOT(dependencyMoved()));
      QObject::connect(port->sidePanel(), SIGNAL(scrolled()), this, SLOT(dependencyMoved()));
    }
    else if(target->targetType() == TargetType_FixedPort)
    {
      FixedPort * fixedPort = (FixedPort*)target;
      QObject::connect(fixedPort, SIGNAL(positionChanged()), this, SLOT(dependencyMoved()));
      QObject::connect(fixedPort->sidePanel(), SIGNAL(scrolled()), this, SLOT(dependencyMoved()));
    }
  }
}

Connection::~Connection()
{
  for (int i=0;i<2;i++)
  {
    ConnectionTarget *target = (i == 0 ? m_src : m_dst);
    if (target->targetType() == TargetType_Pin)
    {
      Pin &t = *(Pin *)target;
      t.node()->setConnectionHighlighted(false);
    }
    if (target->targetType() == TargetType_InstBlockPort)
    {
      InstBlockPort &t = *(InstBlockPort *)target;
      t.instBlock()->node()->setConnectionHighlighted(false);
    }
  }
}

void Connection::setColor(QColor color)
{
  if(m_graph->config().connectionUsePinColor)
  {
    QColor defaultColor = m_graph->config().connectionColor;
    float ratio = m_graph->config().connectionNodePortColorRatio;
    float iratio = 1.0 - ratio;
    int r = (int)(float(color.red()) * ratio + float(defaultColor.red()) * iratio);
    int g = (int)(float(color.green()) * ratio + float(defaultColor.green()) * iratio);
    int b = (int)(float(color.blue()) * ratio + float(defaultColor.blue()) * iratio);
    color = QColor(r, g, b);
  }
  else
  {
    color = m_graph->config().connectionColor;
  }

  m_color = color;
  m_defaultPen.setColor(color);
  updatePen();
}

QPointF Connection::srcPoint() const
{
  if(m_aboutToBeDeleted)
    return boundingRect().topLeft();

  return mapFromScene(m_src->connectionPos(PortType_Output));
}

QPointF Connection::dstPoint() const
{
  if(m_aboutToBeDeleted)
    return boundingRect().bottomRight();

  return mapFromScene(m_dst->connectionPos(PortType_Input));
}

void Connection::invalidate()
{
  m_aboutToBeDeleted = true;
  prepareGeometryChange();
}

void Connection::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  m_hovered = true;
  updatePen();

  if (graph()->config().highlightConnectionTargets)
  {
    for (int i=0;i<2;i++)
    {
      ConnectionTarget *target = (i == 0 ? m_src : m_dst);
      if (target->targetType() == TargetType_Pin)
      {
        Pin &t = *(Pin *)target;
        t.node()->setConnectionHighlighted(m_hovered);
      }
      if (target->targetType() == TargetType_InstBlockPort)
      {
        InstBlockPort &t = *(InstBlockPort *)target;
        t.instBlock()->node()->setConnectionHighlighted(m_hovered);
      }
    }
  }

  QGraphicsPathItem::hoverEnterEvent(event);

  if (graph()->config().highlightConnectionTargets)
    graph()->update();
}

void Connection::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  m_hovered = false;
  updatePen();

  if (graph()->config().highlightConnectionTargets)
  {
    for (int i=0;i<2;i++)
    {
      ConnectionTarget *target = (i == 0 ? m_src : m_dst);
      if (target->targetType() == TargetType_Pin)
      {
        Pin &t = *(Pin *)target;
        t.node()->setConnectionHighlighted(m_hovered);
      }
      if (target->targetType() == TargetType_InstBlockPort)
      {
        InstBlockPort &t = *(InstBlockPort *)target;
        t.instBlock()->node()->setConnectionHighlighted(m_hovered);
      }
    }
  }

  QGraphicsPathItem::hoverLeaveEvent(event);

  if (graph()->config().highlightConnectionTargets)
    graph()->update();
}

void Connection::setCosmetic( bool cosmetic )
{
  m_cosmeticPen = cosmetic;
  this->updatePen();
}

void Connection::updatePen()
{
  QPen pen = m_hovered ? m_hoverPen : m_defaultPen;
  pen.setCosmetic( m_cosmeticPen );
  this->setPen( pen );
}

void Connection::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if( MainPanel::filterMousePressEvent( event ) )
    return event->ignore();

  if(!graph()->isEditable())
    return QGraphicsPathItem::mousePressEvent(event);

  if(event->button() == Qt::LeftButton)
  {
    m_dragging = true;
    QPointF pos = mapToScene(event->pos());
    qreal dInput = (pos - m_src->connectionPos(PortType_Output)).manhattanLength();
    qreal dOutput = (pos - m_dst->connectionPos(PortType_Input)).manhattanLength();
    m_draggingInput = dInput < dOutput;
    m_lastDragPoint = pos;
    event->accept();
  }
  else if(event->button() == Qt::MiddleButton)
  {
    if(graph()->config().middleClickDeletesConnections)
    {
      std::vector<Connection*> conns;
      conns.push_back(this);
      graph()->controller()->gvcDoRemoveConnections(conns);
      event->accept();
      return;
    }
    else
      QGraphicsPathItem::mousePressEvent(event);
  }
  else
    QGraphicsPathItem::mousePressEvent(event);
}

void Connection::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
  QMenu * menu = graph()->getConnectionContextMenu( this );
  if ( menu )
  {
    menu->exec( QCursor::pos() );
    menu->setParent( NULL );
    menu->deleteLater();
  }
}

void Connection::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_dragging)
  {
    QPointF scenePos = mapToScene(event->pos());
    QPointF delta = scenePos - m_lastDragPoint;

    bool isCTRL = event->modifiers().testFlag(Qt::ControlModifier);

    // todo: the disconnect threshold maybe should be a graph setting
    if(delta.manhattanLength() > 0)
    {
      // create local variables
      // since "this" might be deleted after the removeConnections call
      ConnectionTarget * src = m_src;
      ConnectionTarget * dst = m_dst;
      Graph * graph = m_graph;
      bool draggingInput = m_draggingInput;

      graph->controller()->beginInteraction();

      if(!draggingInput)
      {
        graph->constructMouseGrabber(scenePos, (Pin*)src, PortType_Input, isCTRL ? NULL : this);
      }
      else
      {
        graph->constructMouseGrabber(scenePos, (Pin*)dst, PortType_Output, isCTRL ? NULL : this);
      }
    }
  }
  else
    QGraphicsPathItem::mouseMoveEvent(event);
}

void Connection::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  float radius = m_graph->config().pinRadius;
  if (!m_dst || m_dst->isDragging())
  {
    bool isSnapped = static_cast<const MouseGrabber*>( m_dst )->targetUnderMouse() != NULL;
    if(!isSnapped)
    {
      QBrush tipBrush = painter->brush();
      tipBrush.setStyle(Qt::SolidPattern);
      tipBrush.setColor(m_color);
      painter->setBrush(tipBrush);
      painter->drawEllipse(dstPoint(), radius, radius);
      tipBrush.setStyle(Qt::NoBrush);
    }
  }

  if (!m_src || m_src->isDragging())
  {
    bool isSnapped = static_cast<const MouseGrabber*>( m_src )->targetUnderMouse() != NULL;
    if(!isSnapped)
    {
      QBrush tipBrush = painter->brush();
      tipBrush.setStyle(Qt::SolidPattern);
      tipBrush.setColor(m_color);
      painter->setBrush(tipBrush);
      painter->drawEllipse(srcPoint(), radius, radius);
      tipBrush.setStyle(Qt::NoBrush);
    }
  }

  // [FE-6836] connections of IO ports are always dimmed.
  if (m_src->path() == m_dst->path() && !m_dragging && m_src->isRealPort() && m_dst->isRealPort())
  {
    painter->setOpacity(0.15);
    QGraphicsPathItem::paint(painter, option, widget);
    painter->setOpacity(1.0);
  }

  // draw dimmed connection.
  else if (m_isExposedConnection && !m_hovered && !m_hasSelectedTarget && m_graph->config().dimConnectionLines)
  {
    painter->setOpacity(0.15);
    QGraphicsPathItem::paint(painter, option, widget);
    painter->setOpacity(0.5);
    painter->setClipPath(m_clipPath);
    QGraphicsPathItem::paint(painter, option, widget);
    painter->setClipping(false);
    painter->setOpacity(1.0);
  }

  // draw regular connection.
  else
  {
    QGraphicsPathItem::paint(painter, option, widget);
  }
}

QPainterPath Connection::shape() const
{
  return m_shapePath;
}

void Connection::dependencyMoved()
{
  QPointF srcPnt = srcPoint();
  QPointF dstPnt = dstPoint();

  // create and set the path that is
  // used by the paint() function.
  {
    QPainterPath path;

    qreal x = 0.5 * (dstPnt.x() - srcPnt.x());

    if (   m_graph->config().connectionDrawAsCurves
        || m_isExposedConnection )
    {
      if (srcPnt.x() < dstPnt.x())
      {
        path.moveTo ( srcPnt );
        path.cubicTo( srcPnt + QPointF( + x, 0 ),
                      dstPnt + QPointF( - x, 0 ),
                      dstPnt );
      }
      else
      {
        path.moveTo( srcPnt );
        path.lineTo( dstPnt );
      }
    }
    else
    {
      path.moveTo( srcPnt );
      path.lineTo( srcPnt + QPointF( + x, 0 ) );
      path.lineTo( dstPnt + QPointF( - x, 0 ) );
      path.lineTo( dstPnt );
    }
    
    setPath(path);
  }

  // create and set the path that is used
  // as the QGraphicsPathItem's shape.
  {
    QPainterPath path;

    qreal w = m_shapePathWidth;
    qreal x = 0.5 * (dstPnt.x() - srcPnt.x());

    if (   m_graph->config().connectionDrawAsCurves
        || m_isExposedConnection )
    {
      if (srcPnt.x() < dstPnt.x())
      {
        if (srcPnt.y() < dstPnt.y())
        {
          path.moveTo ( srcPnt + QPointF(       0, + w ) );
          path.cubicTo( srcPnt + QPointF( + x - w, + w ),
                        dstPnt + QPointF( - x - w, + w ),
                        dstPnt + QPointF(       0, + w ) );
          path.lineTo ( dstPnt + QPointF(       0, - w ) );
          path.cubicTo( dstPnt + QPointF( - x + w, - w ),
                        srcPnt + QPointF( + x + w, - w ),
                        srcPnt + QPointF(       0, - w ) );
        }
        else
        {
          path.moveTo ( srcPnt + QPointF(       0, + w ) );
          path.cubicTo( srcPnt + QPointF( + x + w, + w ),
                        dstPnt + QPointF( - x + w, + w ),
                        dstPnt + QPointF(       0, + w ) );
          path.lineTo ( dstPnt + QPointF(       0, - w ) );
          path.cubicTo( dstPnt + QPointF( - x - w, - w ),
                        srcPnt + QPointF( + x - w, - w ),
                        srcPnt + QPointF(       0, - w ) );
        }
      }
      else
      {
        QPointF s( dstPnt.y() - srcPnt.y(), -(dstPnt.x() - srcPnt.x()) );
        qreal len = sqrt(s.x() * s.x() + s.y() * s.y());
        if (len > 0)
        {
          s *= w / len;

          path.moveTo( srcPnt + s );
          path.lineTo( dstPnt + s );
          path.lineTo( dstPnt - s );
          path.lineTo( srcPnt - s );
        }
      }
    }
    else
    {
      if (srcPnt.x() < dstPnt.x())
      {
        if (srcPnt.y() < dstPnt.y())
        {
          path.moveTo( srcPnt + QPointF(       0, + w ) );
          path.lineTo( srcPnt + QPointF( + x - w, + w ) );
          path.lineTo( dstPnt + QPointF( - x - w, + w ) );
          path.lineTo( dstPnt + QPointF(       0, + w ) );
          path.lineTo( dstPnt + QPointF(       0, - w ) );
          path.lineTo( dstPnt + QPointF( - x + w, - w ) );
          path.lineTo( srcPnt + QPointF( + x + w, - w ) );
          path.lineTo( srcPnt + QPointF(       0, - w ) );
        }
        else
        {
          path.moveTo( srcPnt + QPointF(       0, + w ) );
          path.lineTo( srcPnt + QPointF( + x + w, + w ) );
          path.lineTo( dstPnt + QPointF( - x + w, + w ) );
          path.lineTo( dstPnt + QPointF(       0, + w ) );
          path.lineTo( dstPnt + QPointF(       0, - w ) );
          path.lineTo( dstPnt + QPointF( - x - w, - w ) );
          path.lineTo( srcPnt + QPointF( + x - w, - w ) );
          path.lineTo( srcPnt + QPointF(       0, - w ) );
        }
      }
      else
      {
        if (srcPnt.y() < dstPnt.y())
        {
          path.moveTo( srcPnt + QPointF(       0, - w ) );
          path.lineTo( srcPnt + QPointF( + x - w, - w ) );
          path.lineTo( dstPnt + QPointF( - x - w, - w ) );
          path.lineTo( dstPnt + QPointF(       0, - w ) );
          path.lineTo( dstPnt + QPointF(       0, + w ) );
          path.lineTo( dstPnt + QPointF( - x + w, + w ) );
          path.lineTo( srcPnt + QPointF( + x + w, + w ) );
          path.lineTo( srcPnt + QPointF(       0, + w ) );
        }
        else
        {
          path.moveTo( srcPnt + QPointF(       0, - w ) );
          path.lineTo( srcPnt + QPointF( + x + w, - w ) );
          path.lineTo( dstPnt + QPointF( - x + w, - w ) );
          path.lineTo( dstPnt + QPointF(       0, - w ) );
          path.lineTo( dstPnt + QPointF(       0, + w ) );
          path.lineTo( dstPnt + QPointF( - x - w, + w ) );
          path.lineTo( srcPnt + QPointF( + x - w, + w ) );
          path.lineTo( srcPnt + QPointF(       0, + w ) );
        }
      }
    }

    m_shapePath = path;
  }

  if (m_isExposedConnection)
  {
    m_clipPath = QPainterPath();
    m_clipPath.addEllipse(srcPnt, m_clipRadius, m_clipRadius);
    m_clipPath.addEllipse(dstPnt, m_clipRadius, m_clipRadius);
  }
}

void Connection::dependencySelected()
{
  bool oldHasSelectedTarget = m_hasSelectedTarget;
  m_hasSelectedTarget = false;
  if (m_src->targetType() == TargetType_Pin)
  {
    Node *node = ((Pin *)m_src)->node();
    m_hasSelectedTarget = m_hasSelectedTarget || node->selected();
  }
  if (m_src->targetType() == TargetType_InstBlockPort)
  {
    Node *node = ((InstBlockPort *)m_src)->instBlock()->node();
    m_hasSelectedTarget = m_hasSelectedTarget || node->selected();
  }
  if (m_dst->targetType() == TargetType_Pin)
  {
    Node *node = ((Pin *)m_dst)->node();
    m_hasSelectedTarget = m_hasSelectedTarget || node->selected();
  }
  if (m_dst->targetType() == TargetType_InstBlockPort)
  {
    Node *node = ((InstBlockPort *)m_dst)->instBlock()->node();
    m_hasSelectedTarget = m_hasSelectedTarget || node->selected();
  }
  if ( m_hasSelectedTarget != oldHasSelectedTarget )
    update();
}
