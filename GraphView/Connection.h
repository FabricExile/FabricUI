// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Connection__
#define __UI_GraphView_Connection__

#include <QGraphicsPathItem>
#include <QColor>
#include <QPen>
#include <QPointF>
#include <QRectF>

#include "GraphicItemTypes.h"
#include <FTL/Config.h>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;
    class ConnectionTarget;

    class Connection : public QObject, public QGraphicsPathItem
    {
      Q_OBJECT

    public:

      Connection(
        Graph * parent,
        ConnectionTarget * src,
        ConnectionTarget * dst,
        bool forceUseOfPinColor = false
        );

      ~Connection();

      virtual int type() const { return QGraphicsItemType_Connection; }

      Graph *graph()
        { return m_graph; }
      Graph const *graph() const
        { return m_graph; }

      ConnectionTarget *src()
        { return m_src; }
      ConnectionTarget const *src() const
        { return m_src; }
        
      ConnectionTarget *dst()
        { return m_dst; }
      ConnectionTarget const *dst() const
        { return m_dst; }

      virtual QColor color() const
        { return m_color; }
      virtual void setColor(QColor color);

      QPointF srcPoint() const;
      QPointF dstPoint() const;

      virtual void invalidate();
      
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
      virtual QPainterPath shape() const;

      bool isHovered()  { return m_hovered; }

      void enableToolTip(bool state)
        { setToolTip(state ? m_tooltip : QString()); }

      void setCosmetic( bool );

    public slots:

      virtual void dependencyMoved();
      virtual void dependencySelected();

    protected:
      void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) FTL_OVERRIDE;

    private:

      Graph * const m_graph;
      ConnectionTarget * const m_src;
      ConnectionTarget * const m_dst;

      QColor m_color;
      QPen m_defaultPen;
      float m_shapePathWidth;
      QPen m_hoverPen;
      bool m_cosmeticPen;
      bool m_hovered;
      void updatePen();

      bool m_dragging;
      bool m_draggingInput; // or Output
      QPointF m_lastDragPoint;
      bool m_aboutToBeDeleted;
      bool m_isExposedConnection;
      bool m_hasSelectedTarget;
      float m_clipRadius;
      QPainterPath m_clipPath;
      QPainterPath m_shapePath;

      QString m_tooltip;
    };

  };

};

#endif // __UI_GraphView_Connection__
