// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_MouseGrabber__
#define __UI_GraphView_MouseGrabber__

#include <QAction>
#include <QGraphicsWidget>
#include <QToolTip>
#include "ConnectionTarget.h"
#include "PortType.h"

namespace FabricUI
{
  namespace GraphView
  {
    // forward declarations
    class Connection;
    class ConnectionTarget;
    class Graph;
    class InstBlock;
    class Node;
    class SidePanel;

    class MouseGrabber : public ConnectionTarget
    {
      Q_OBJECT

    public:

      MouseGrabber(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType, Connection *connectionPrevious);
      ~MouseGrabber();

      static MouseGrabber * construct(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType, Connection *connectionPrevious);

      float radius() const;
      float diameter() const;
      ConnectionTarget * target();
      const ConnectionTarget * target() const;
      ConnectionTarget *targetUnderMouse() const
        { return m_targetUnderMouse; }
      Graph * graph();
      const Graph * graph() const;
      virtual QColor color() const;
      inline Connection * connection() const { return m_connection; }

      virtual std::string path() const { return "***ASSERT***"; }

      virtual bool canConnectTo(
        ConnectionTarget * other,
        std::string &failureReason
        ) const
        { return false; }

      virtual TargetType targetType() const { return TargetType_MouseGrabber; }
      virtual QPointF connectionPos(PortType pType) const;

      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

      // these don't do anything in this case
      virtual bool highlighted() const { return false; }
      virtual void setHighlighted(bool state = true) {}

      void performUngrab( ConnectionTarget *fromCT );

    signals:

      void positionChanged(QPointF);

    protected:

      PinCircle * findPinCircle( QPointF pos ) FTL_OVERRIDE { return NULL; }

    private:

      void showToolTip();
      void invokeConnect(ConnectionTarget * source, ConnectionTarget * target);

      void invokeNodeHeaderMenu(
        Node * node,
        ConnectionTarget * other,
        PortType nodeRole,
        QPoint pos
        );
      void invokeInstBlockHeaderMenu(
        InstBlock * instBlock,
        ConnectionTarget * other,
        PortType instBlockRole,
        QPoint pos
        );

      QPointF m_connectionPos;
      ConnectionTarget * m_target;
      PortType m_otherPortType;
      float m_radius;
      Connection * m_connection;
      Connection * m_connectionPrevious;
      ConnectionTarget * m_targetUnderMouse;
      SidePanel* m_lastSidePanel;
    };

  };

};

#endif // __UI_GraphView_MouseGrabber__
