// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_SidePanel__
#define __UI_GraphView_SidePanel__

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QPen>
#include <vector>

#include <FTL/StrRef.h>

#include "PortType.h"
#include <FabricUI/GraphView/GraphicItemTypes.h>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;
    class Port;
    class FixedPort;
    class ProxyPort;
    class ConnectionTarget;
    class SidePanelItemGroup;
    class TextContainer;

    class SidePanel : public QGraphicsWidget
    {
      Q_OBJECT

      friend class Graph;
      friend class MainPanel;

    public:

      SidePanel(Graph * parent, PortType portType, QColor color = QColor());

      virtual int type() const { return QGraphicsItemType_SidePanel; }

      Graph * graph();
      const Graph * graph() const;
      QGraphicsWidget * itemGroup();
      const QGraphicsWidget * itemGroup() const;

      QString path() const;
      QString name() const;
      QColor color() const;
      PortType portType() const;
      
      unsigned int fixedPortCount() const
        { return m_fixedPorts.size(); }
      FixedPort *fixedPort( unsigned index )
        { return m_fixedPorts[index]; }
      FixedPort *fixedPort( FTL::StrRef name );
      
      unsigned int portCount() const
        { return m_ports.size(); }
      Port *port( unsigned index )
        { return m_ports[index]; }
      Port *port( FTL::StrRef name );

      ConnectionTarget *getConnectionTarget( FTL::StrRef name );

      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mousePressEvent( QGraphicsSceneMouseEvent* ) FTL_OVERRIDE;
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
      virtual void wheelEvent(QGraphicsSceneWheelEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

      virtual void resizeEvent(QGraphicsSceneResizeEvent * event);

      void onDraggingPort( const QGraphicsSceneMouseEvent* event, Port* draggedPort );
      void onDraggingPortLeave();
      void onDroppingPort();

      void addFixedPort( FixedPort *fixedPort );
      void removeFixedPort( FixedPort *fixedPort );
      void reorderFixedPorts( QStringList names );

      void addPort( Port *port );
      void removePort( Port *port );
      void reorderPorts( QStringList names );

      void setEditable( bool isEditable );

      void scroll(float delta);
      void updateItemGroupScroll(float height = 0.0f);

    signals:
      void doubleClicked(FabricUI::GraphView::SidePanel *);
      void scrolled();

    protected:
      void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) FTL_OVERRIDE;

    private slots:
      void onItemGroupResized();

    private:

      void resetLayout();

      Graph * m_graph;
      QColor m_color;
      QPen m_pen;
      PortType m_portType;
      bool m_requiresToSendSignalsForPorts;
      SidePanelItemGroup * m_itemGroup;
      float m_itemGroupScroll;

      ProxyPort* m_proxyPort;
      TextContainer * m_proxyPortDummy;
      std::vector<FixedPort*> m_fixedPorts;
      std::vector<Port*> m_ports;

      QString m_dragSrcPortName;
      QString m_dragDstPortName;
      qreal m_dragDstY;
    };

  };

};

#endif // __UI_GraphView_SidePanel__
