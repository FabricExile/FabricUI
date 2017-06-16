// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QCursor>
#include <QGraphicsView>
#include <QMimeData>

#include <FabricUI/GraphView/MainPanel.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/GraphView/SelectionRect.h>
#include <FabricUI/GraphView/Connection.h>

#include <math.h>

using namespace FabricUI::GraphView;

const float MainPanel::s_minZoom = 0.05f;
const float MainPanel::s_maxZoom = 2.00f;
const float MainPanel::s_minZoomForOne = 1.0f / 1.01f;
const float MainPanel::s_maxZoomForOne = 1.01f;

MainPanel::MainPanel(Graph * parent)
: QGraphicsWidget(parent)
{
  m_graph = parent;
  m_mouseWheelZoomState = 1.0;
  m_mouseWheelZoomRate = m_graph->config().mouseWheelZoomRate;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  setAcceptDrops( true );

  m_itemGroup = new QGraphicsWidget(this);

  m_manipulationMode = ManipulationMode_None;
  m_selectionRect = NULL;
}

Graph * MainPanel::graph()
{
  return m_graph;
}

const Graph * MainPanel::graph() const
{
  return m_graph;
}

QGraphicsWidget * MainPanel::itemGroup()
{
  return m_itemGroup;
}

const QGraphicsWidget * MainPanel::itemGroup() const
{
  return m_itemGroup;
}

float MainPanel::mouseWheelZoomRate() const
{
  return m_mouseWheelZoomRate;
}

void MainPanel::setMouseWheelZoomRate(float rate)
{
  m_mouseWheelZoomRate = rate;
}

float MainPanel::canvasZoom() const
{
  return m_mouseWheelZoomState;
}

void MainPanel::setCanvasZoom(float state, bool quiet)
{
  if(state > s_maxZoom)
    state = s_maxZoom;
  if(state < s_minZoom)
    state = s_minZoom;
  if(m_mouseWheelZoomState == state)
    return;

  m_mouseWheelZoomState = state;

  float previousState = m_itemGroup->scale();

  QGraphicsView * graphicsView = graph()->scene()->views()[0];
  if ( state > s_minZoomForOne
    && state < s_maxZoomForOne )
  {
    graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, false);
    m_itemGroup->setScale(1.0f);
  }
  else
  {
    graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);
    m_itemGroup->setScale(state);
  }

  const float cosmeticThreshold = 1.0f;
  if( state != previousState &&
    // update the connections only if the boolean argument "cosmetic" changed
    ( ( state < cosmeticThreshold ) != ( previousState < cosmeticThreshold ) )
  )
  {
    // If the scale is < 1, make the connection's pen sizes invariant
    // to zoom ( QPen::setCosmetic( true ) ). Otherwhise, their width
    // will scale with zoom.
    bool cosmetic = ( state < cosmeticThreshold );
    m_graph->setConnectionsCosmetic( cosmetic );
  }

  update();

  if(!quiet)
    emit canvasZoomChanged(m_mouseWheelZoomState);
}

QPointF MainPanel::canvasPan() const
{
  return m_itemGroup->pos();
}

void MainPanel::setCanvasPan(QPointF pos, bool quiet)
{
  m_itemGroup->setPos(pos);

  update();

  if(!quiet)
    emit canvasPanChanged(pos);
}

MainPanel::ManipulationMode MainPanel::manipulationMode() const
{
  return m_manipulationMode;
}

void MainPanel::setManipulationMode(ManipulationMode mode)
{
  m_manipulationMode = mode;
  switch (mode)
  {
    case ManipulationMode_Select:
      setCursor(Qt::ArrowCursor);
      break;
    case ManipulationMode_Pan:
    case ManipulationMode_Zoom:
      setCursor(Qt::OpenHandCursor);
      break;
    default:
      m_manipulationMode = ManipulationMode_None;
      setCursor(Qt::ArrowCursor);
      break;
  }
}

QRectF MainPanel::boundingRect() const
{
  return m_boundingRect;
}

void MainPanel::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
  QMenu * menu = graph()->getGraphContextMenu();
  if ( menu )
  {
    menu->exec( QCursor::pos() );
    menu->setParent( NULL );
    menu->deleteLater();
  }
}

bool MainPanel::filterMousePressEvent( const QGraphicsSceneMouseEvent * event )
{
  return event->modifiers().testFlag( Qt::AltModifier );
}

void MainPanel::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  // clean up the scene
  if(m_selectionRect)
  {
    scene()->removeItem(m_selectionRect);
    delete(m_selectionRect);
    m_selectionRect = NULL;
  }
  if(event->button() == Qt::LeftButton && !event->modifiers().testFlag(Qt::AltModifier))
  {
    QPointF mouseDownPos = mapToItem(m_itemGroup, mapFromScene( event->scenePos() ) );
    m_selectionRect = new SelectionRect(this, mouseDownPos);

    if(!event->modifiers().testFlag(Qt::ControlModifier) && !event->modifiers().testFlag(Qt::ShiftModifier))
      m_graph->controller()->clearSelection();
    m_ongoingSelection.clear();

    setManipulationMode( ManipulationMode_Select );
  }
  else if(   event->button() == Qt::MiddleButton
         || (event->button() == Qt::LeftButton &&  event->modifiers().testFlag(Qt::AltModifier)
                                               && !event->modifiers().testFlag(Qt::ControlModifier)
                                               && !event->modifiers().testFlag(Qt::ShiftModifier)
            )
         )
  {
    setManipulationMode( ManipulationMode_Pan );
    m_lastPanPoint = mapFromScene( event->scenePos() );
    event->accept();
  }
  else if(event->button() == Qt::RightButton &&  event->modifiers().testFlag(Qt::AltModifier)
                                             && !event->modifiers().testFlag(Qt::ControlModifier)
                                             && !event->modifiers().testFlag(Qt::ShiftModifier)
         )
  {
    setManipulationMode( ManipulationMode_Zoom );
    m_lastPanPoint = mapFromScene( event->scenePos() );
    m_mouseAltZoomState = m_mouseWheelZoomState;
  }
  else
    QGraphicsWidget::mousePressEvent(event);
}

void MainPanel::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if(manipulationMode() == ManipulationMode_Select)
  {
    QPointF dragPoint = mapToItem(m_itemGroup, mapFromScene( event->scenePos() ) );
    m_selectionRect->setDragPoint(dragPoint);

    if(!event->modifiers().testFlag(Qt::ControlModifier) && !event->modifiers().testFlag(Qt::ShiftModifier))
      m_graph->controller()->clearSelection();
    else
    {
      for(size_t i=0;i<m_ongoingSelection.size();i++)
        m_graph->controller()->selectNode(m_ongoingSelection[i], false);
    }

    m_ongoingSelection.clear();
    std::vector<Node*> nodes = m_graph->nodes();
    for(size_t i=0;i<nodes.size();i++)
    {
      bool hit = nodes[i]->collidesWithItem(m_selectionRect, Qt::IntersectsItemBoundingRect);

      if (hit && nodes[i]->isBackDropNode())
      {
        // [FE-6224]
        // backdrop nodes are only hit when the selection
        // rectangle intersects with the backdrop's border
        // or if it contains the entire backdrop.
        hit = !m_selectionRect->collidesWithItem(nodes[i], Qt::ContainsItemBoundingRect);
      }

      if (hit && !nodes[i]->selected())
      {
        m_graph->controller()->selectNode(nodes[i], true);
        m_ongoingSelection.push_back(nodes[i]);
      }
    }
  }
  else if(manipulationMode() == ManipulationMode_Pan)
  {
    QPointF pos = mapFromScene( event->scenePos() );

    QTransform xfo = m_itemGroup->transform().inverted();
    QPointF delta = xfo.map(pos) - xfo.map(m_lastPanPoint);
    m_lastPanPoint = pos;
    event->accept();

    m_graph->controller()->panCanvas(delta + canvasPan());
  }
  else if(manipulationMode() == ManipulationMode_Zoom)
  {
    QPointF delta = mapFromScene( event->scenePos() ) - m_lastPanPoint;
    float zoomFactor = powf( 1.005f, delta.x() - delta.y() );
    performZoom( m_mouseAltZoomState * zoomFactor, m_lastPanPoint );
  }
  else
    QGraphicsWidget::mouseMoveEvent(event);
}

void MainPanel::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if(manipulationMode() == ManipulationMode_Select)
  {
    prepareGeometryChange();
  
    scene()->removeItem(m_selectionRect);
    delete(m_selectionRect);
    m_selectionRect = NULL;
    setManipulationMode( ManipulationMode_None );
  }
  else if(manipulationMode() == ManipulationMode_Pan || manipulationMode() == ManipulationMode_Zoom)
  {
    setManipulationMode( ManipulationMode_None );
  }
  else
    QGraphicsWidget::mouseMoveEvent(event);
}

void MainPanel::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
  emit doubleClicked(event->modifiers());
}

void MainPanel::performZoom(
  float newZoomState,
  QPointF zoomCenter
  )
{
  if ( newZoomState > s_maxZoom )
    newZoomState = s_maxZoom;
  else if ( newZoomState < s_minZoom )
    newZoomState = s_minZoom;

  QPointF oldItemGroupPos = mapToItem( m_itemGroup, zoomCenter );
  float oldScale = m_itemGroup->scale();
  m_itemGroup->setScale( newZoomState );
  QPointF newItemPos = mapFromItem( m_itemGroup, oldItemGroupPos );
  m_itemGroup->setScale( oldScale );
  QPointF delta = newItemPos - zoomCenter;

  m_graph->controller()->zoomCanvas( newZoomState );
  m_graph->controller()->panCanvas( canvasPan() - delta );
}

void MainPanel::wheelEvent(QGraphicsSceneWheelEvent * event)
{
  if ( manipulationMode() == ManipulationMode_None )
  {
    float zoomFactor = 1.0f + float(event->delta()) * m_mouseWheelZoomRate;
    m_lastPanPoint = mapFromScene( event->scenePos() );
    performZoom( m_mouseWheelZoomState * zoomFactor, m_lastPanPoint );
  }
}

void MainPanel::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  if ( windowFrameRect() != m_boundingRect )
  {
    prepareGeometryChange();
    m_boundingRect = windowFrameRect();
  }

  QGraphicsWidget::paint(painter, option, widget);
}

void MainPanel::resizeEvent(QGraphicsSceneResizeEvent * event)
{
  QGraphicsWidget::resizeEvent(event);
  if(m_graph->sidePanel(PortType_Input))
    emit m_graph->sidePanel(PortType_Input)->scrolled();
  if(m_graph->sidePanel(PortType_Output))
    emit m_graph->sidePanel(PortType_Output)->scrolled();
}

void MainPanel::dragEnterEvent( QGraphicsSceneDragDropEvent *event )
{
  QMimeData const *mimeData = event->mimeData();
  if ( mimeData->hasText() )
  {
    event->acceptProposedAction();
    return;
  }

  QGraphicsWidget::dragEnterEvent( event );
}

void MainPanel::dropEvent( QGraphicsSceneDragDropEvent *event )
{
  QMimeData const *mimeData = event->mimeData();
  if ( mimeData->hasText() )
  {
    QString presetPath = mimeData->text();

    QPointF pos = graph()->itemGroup()->mapFromScene( event->pos() );

    graph()->controller()->gvcDoAddInstFromPreset( presetPath, pos );

    event->acceptProposedAction();

    return;
  }

  QGraphicsWidget::dropEvent( event );
}
