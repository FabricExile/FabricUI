// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <QApplication>
#include <QGraphicsSceneHoverEvent>

#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/GraphView/NodeLabel.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/MainPanel.h>
#include <FabricUI/GraphView/NodeHeader.h>

using namespace FabricUI::GraphView;

BackDropNode::BackDropNode(
  Graph * parent,
  FTL::CStrRef name,
  FTL::CStrRef title,
  QColor color,
  QColor titleColor
  )
  : Node(
    parent,
    NodeType_BackDrop,
    name,
    title,
    color,
    titleColor
    )
{
  m_mainWidget->setMinimumWidth(m_mainWidget->boundingRect().width() * 2.0f);
  m_mainWidget->setMinimumHeight(graph()->config().nodeMinHeight * 2.0f);
  m_mainWidget->setMaximumHeight(graph()->config().nodeMinHeight * 2.0f);

  m_minSize.setWidth(int(m_mainWidget->boundingRect().width() * 2.0f));
  m_minSize.setHeight(int(graph()->config().nodeMinHeight * 2.0f));

  m_resizeDistance = 32.0;
  m_hasCustomPointer = false;
  setAcceptHoverEvents(true);
  setZValue(-100);
}

BackDropNode::~BackDropNode()
{
}

void BackDropNode::setSize( QSizeF size )
{
  m_mainWidget->setMinimumSize( size );
  m_mainWidget->setMaximumSize( size );
}

QRectF BackDropNode::boundingRect() const
{
  // [FE-6865]
  QRectF rect = QGraphicsWidget::boundingRect().adjusted( 0, -4, 0, 4 );
  return rect.united( header()->boundingRect() ); // compensate for long header text
}

void BackDropNode::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  if(m_hasCustomPointer)
  {
    QApplication::restoreOverrideCursor();
    m_hasCustomPointer = false;
  }
  Node::hoverLeaveEvent(event);
}

void BackDropNode::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  if(m_dragging == 0)
  {
    int corner = getCorner(event->pos());

    if(!m_hasCustomPointer && (corner == 0 || corner == 3))
    {
      QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
      m_hasCustomPointer = true;
    }
    else if(!m_hasCustomPointer && (corner == 1 || corner == 2))
    {
      QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
      m_hasCustomPointer = true;
    }
    else if(m_hasCustomPointer)
    {
      QApplication::restoreOverrideCursor();
      m_hasCustomPointer = false;
    }
  }

  Node::hoverMoveEvent(event);
}

void BackDropNode::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if( MainPanel::filterMousePressEvent( event ) )
    return event->ignore();

  int corner = getCorner(event->pos());
  if(corner != -1)
  {
    m_dragging = 3 + corner;
    m_dragButton = event->button();

    m_mouseDownPos = event->scenePos();
    m_resizeOriginalPos = topLeftGraphPos();
    m_resizeOriginalSize = m_mainWidget->minimumSize();

    event->accept();
    return;
  }

  Node::mousePressEvent(event);
}

void BackDropNode::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if (m_dragging >= 3 && m_dragging <= 6)
  {
    QPointF delta = event->scenePos() - m_mouseDownPos;
    delta *= 1.0f / graph()->mainPanel()->canvasZoom();

    float gridSnapSize = (graph()->config().mainPanelGridSnap ? graph()->config().mainPanelGridSnapSize : 0);

    QPointF pos;
    QSizeF size;

    if(m_dragging == 3) // topleft
    {
      pos = m_resizeOriginalPos + delta;
      size = m_resizeOriginalSize + QSizeF( -delta.x(), -delta.y() );
    }
  
    if(m_dragging == 4) // topright
    {
      pos = m_resizeOriginalPos + QPointF( 0, delta.y() );
      size = m_resizeOriginalSize + QSizeF( +delta.x(), -delta.y() );
    }
  
    if(m_dragging == 5) // bottomleft
    {
      pos = m_resizeOriginalPos + QPointF( delta.x(), 0 );
      size = m_resizeOriginalSize + QSizeF( -delta.x(), +delta.y() );
    }
  
    if(m_dragging == 6) // bottomright
    {
      pos = m_resizeOriginalPos + QPointF( 0, 0 );
      size = m_resizeOriginalSize + QSizeF( +delta.x(), +delta.y() );
    }

    // handle case when size smaller than allowed
    if (size.width() < m_minSize.width())
    {
      if (m_dragging == 3 || m_dragging == 5)
        pos.setX(pos.x() - (m_minSize.width() - size.width()));
      size.setWidth(m_minSize.width());
    }
    if (size.height() < m_minSize.height())
    {
      if (m_dragging == 3 || m_dragging == 4)
        pos.setY(pos.y() - (m_minSize.height() - size.height()));
      size.setHeight(m_minSize.height());
    }

    // resize
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      pos,
      size,
      gridSnapSize,
      m_dragging == 3 || m_dragging == 4,
      m_dragging == 5 || m_dragging == 6,
      m_dragging == 3 || m_dragging == 5,
      m_dragging == 4 || m_dragging == 6,
      false
      );

    event->accept();
    return;
  }

  Node::mouseMoveEvent(event);
}

void BackDropNode::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if (m_dragging >= 3 && m_dragging <= 6)
  {
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      m_resizeOriginalPos,
      m_resizeOriginalSize,
      0,
      false,
      false,
      false,
      false,
      false
    );

    QPointF delta = event->scenePos() - m_mouseDownPos;
    delta *= 1.0f / graph()->mainPanel()->canvasZoom();

    float gridSnapSize = (graph()->config().mainPanelGridSnap ? graph()->config().mainPanelGridSnapSize : 0);

    QPointF pos;
    QSizeF size;

    if(m_dragging == 3) // topleft
    {
      pos = m_resizeOriginalPos + delta;
      size = m_resizeOriginalSize + QSizeF( -delta.x(), -delta.y() );
    }
  
    if(m_dragging == 4) // topright
    {
      pos = m_resizeOriginalPos + QPointF( 0, delta.y() );
      size = m_resizeOriginalSize + QSizeF( +delta.x(), -delta.y() );
    }
  
    if(m_dragging == 5) // bottomleft
    {
      pos = m_resizeOriginalPos + QPointF( delta.x(), 0 );
      size = m_resizeOriginalSize + QSizeF( -delta.x(), +delta.y() );
    }
  
    if(m_dragging == 6) // bottomright
    {
      pos = m_resizeOriginalPos + QPointF( 0, 0 );
      size = m_resizeOriginalSize + QSizeF( +delta.x(), +delta.y() );
    }
    // handle case when size smaller than allowed
    if (size.width() < m_minSize.width())
    {
      if (m_dragging == 3 || m_dragging == 5)
        pos.setX(pos.x() - (m_minSize.width() - size.width()));
      size.setWidth(m_minSize.width());
    }
    if (size.height() < m_minSize.height())
    {
      if (m_dragging == 3 || m_dragging == 4)
        pos.setY(pos.y() - (m_minSize.height() - size.height()));
      size.setHeight(m_minSize.height());
    }

    // resize
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      pos,
      size,
      gridSnapSize,
      m_dragging == 3 || m_dragging == 4,
      m_dragging == 5 || m_dragging == 6,
      m_dragging == 3 || m_dragging == 5,
      m_dragging == 4 || m_dragging == 6,
      true
      );

    m_dragging = 0;

    event->accept();
    return;
  }

  Node::mouseReleaseEvent(event);
}

void BackDropNode::appendOverlappingNodes( std::vector<Node*> &nodes ) const
{
  std::vector<Node *> all = graph()->nodes();

  QPointF topLeft = mapToScene(boundingRect().topLeft());
  QPointF bottomRight = mapToScene(boundingRect().bottomRight());
  QRectF rect(topLeft, bottomRight);

  for(size_t i=0;i<all.size();i++)
  {
    if ( all[i]->isBackDropNode() )
      continue;
    if(all[i]->selected())
      continue;

    QPointF topLeft2 = all[i]->mapToScene(all[i]->boundingRect().topLeft());
    QPointF bottomRight2 = all[i]->mapToScene(all[i]->boundingRect().bottomRight());
    QRectF rect2(topLeft2, bottomRight2);

    if(rect.contains(rect2))
      nodes.push_back(all[i]);
  }
}

int BackDropNode::getCorner(QPointF pos)
{
  QPointF topLeft = mapFromItem(m_mainWidget, m_mainWidget->boundingRect().topLeft());
  QPointF bottomRight = mapFromItem(m_mainWidget, m_mainWidget->boundingRect().bottomRight());
  QRectF rect(topLeft, bottomRight);

  if((rect.topLeft() - pos).manhattanLength() < m_resizeDistance)
    return 0;
  else if((rect.topRight() - pos).manhattanLength() < m_resizeDistance)
    return 1;
  else if((rect.bottomLeft() - pos).manhattanLength() < m_resizeDistance)
    return 2;
  else if((rect.bottomRight() - pos).manhattanLength() < m_resizeDistance)
    return 3;

  return -1;
}
