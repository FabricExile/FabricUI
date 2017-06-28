
#include <FabricUI/FCurveEditor/FCurveItem.h>
#include <FTL/Config.h>

#include <QStyleOption>
#include <QPainter>
#include <QGraphicsSceneEvent>

#include <QDebug>
#include <assert.h>

using namespace FabricUI::FCurveEditor;

inline QPointF min( const QPointF& a, const QPointF& b )
{
  return QPointF(
    std::min( a.x(), b.x() ),
    std::min( a.y(), b.y() )
  );
}

inline QPointF max( const QPointF& a, const QPointF& b )
{
  return QPointF(
    std::max( a.x(), b.x() ),
    std::max( a.y(), b.y() )
  );
}

class FCurveItem::FCurveShape : public QGraphicsItem
{
  const FCurveItem* m_parent;
  typedef QGraphicsItem Parent;

public:
  FCurveShape( const FCurveItem* parent )
    : m_parent( parent )
  {
  }

  QRectF boundingRect() const FTL_OVERRIDE
  {
    // TODO : cache and update when handles are moved
    QPointF topLeft = QPointF( 1, 1 ) * std::numeric_limits<qreal>::max();
    QPointF botRight = QPointF( 1, 1 ) * (-std::numeric_limits<qreal>::max());
    if( m_parent->m_curve != NULL )
    {
      size_t hc = m_parent->m_curve->getHandleCount();
      if( hc == 0 )
        return QRectF( -1, -1, 2, 2 );
      for( size_t i = 0; i < hc; i++ )
      {
        Handle h = m_parent->m_curve->getHandle( i );
        topLeft = min( topLeft, h.pos );
        botRight = max( botRight, h.pos );
        topLeft.setY( std::min( topLeft.y(), h.pos.y() - h.tanIn.y() ) );
        topLeft.setY( std::min( topLeft.y(), h.pos.y() + h.tanOut.y() ) );
        botRight.setY( std::max( botRight.y(), h.pos.y() - h.tanIn.y() ) );
        botRight.setY( std::max( botRight.y(), h.pos.y() + h.tanOut.y() ) );
      }
    }
    QRectF dst( topLeft, botRight );
    return dst;
  }

  void paint(
    QPainter * painter,
    const QStyleOptionGraphicsItem * option,
    QWidget * widget
  ) FTL_OVERRIDE
  {
    if( m_parent->m_curve != NULL )
    {
      const QRectF er = painter->transform().inverted()
        .mapRect( QRectF( widget->geometry() ) ).intersected( this->boundingRect() );
      QPen pen; pen.setCosmetic( true );
      pen.setWidthF( 2 );
      pen.setColor( QColor( 0, 255, 255 ) );
      painter->setPen( pen );
      const size_t n = widget->width() / 8;
      for( size_t i = 0; i < n; i++ )
      {
        qreal x1 = er.left() + ( i * er.width() ) / n;
        qreal x2 = er.left() + ( ( i + 1 ) * er.width() ) / n;
        painter->drawLine(
          QPointF( x1, m_parent->m_curve->evaluate( x1 ) ),
          QPointF( x2, m_parent->m_curve->evaluate( x2 ) )
        );
      }
      //painter->drawRect( this->boundingRect() );
    }
  }
};

class FCurveItem::HandleWidget : public QGraphicsWidget
{
  FCurveItem* m_parent;
  size_t m_index;

  class Center : public QGraphicsRectItem
  {
    HandleWidget* m_parent;
  public:
    Center( HandleWidget* parent )
      : QGraphicsRectItem( parent )
      , m_parent( parent )
    {
      this->setRect( QRectF( -4, -4, 8, 8 ) );
      this->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
      this->setBrush( QColor( 255, 255, 255 ) );
      this->setAcceptHoverEvents( true );
    };
  protected:
    void mousePressEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE {}
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
    {
      AbstractFCurveModel* curve = m_parent->m_parent->m_curve;
      const size_t index = m_parent->m_index;
      Handle h = curve->getHandle( index );
      h.pos = event->scenePos();
      curve->setHandle( index, h );
    }
    void hoverEnterEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE { this->setCursor( Qt::CrossCursor ); }
    void hoverLeaveEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE { this->unsetCursor(); }
  };
  Center* m_center;

  class Tangent
  {
    bool m_inNotOut;
    QGraphicsLineItem* m_line;

    class End : public QGraphicsEllipseItem
    {
      HandleWidget* m_parent;
      Tangent* m_tangent;
    public:
      QGraphicsWidget* m_posW; // Used for its position
      End( HandleWidget* parent, Tangent* tangent )
        : QGraphicsEllipseItem()
        , m_parent( parent )
        , m_tangent( tangent )
        , m_posW( new QGraphicsWidget( parent ) )
      {
        this->setParentItem( m_posW );
        this->setRect( QRectF( -4, -4, 8, 8 ) );
        this->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
        this->setBrush( QColor( 255, 255, 255 ) );
        this->setAcceptHoverEvents( true );
      }
    protected:
      void mousePressEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE {}
      void mouseMoveEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
      {
        AbstractFCurveModel* curve = m_parent->m_parent->m_curve;
        const size_t index = m_parent->m_index;
        Handle h = curve->getHandle( index );
        if( m_tangent->m_inNotOut )
          h.tanIn = -( event->scenePos() - m_parent->scenePos() );
        else
          h.tanOut = ( event->scenePos() - m_parent->scenePos() );
        curve->setHandle( index, h );
      }
      void hoverEnterEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE { this->setCursor( Qt::CrossCursor ); }
      void hoverLeaveEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE { this->unsetCursor(); }
    };
    End* m_end;

  public:
    Tangent( HandleWidget* parent, bool inNotOut )
      : m_inNotOut( inNotOut )
      , m_line( new QGraphicsLineItem( parent ) )
      , m_end( new End( parent, this ) )
    {
      QPen linePen; linePen.setCosmetic( true );
      linePen.setColor( QColor( 255, 255, 255, 128 ) );
      linePen.setWidthF( 2 );
      m_line->setPen( linePen );
    }

    void setValue( const Handle& h )
    {
      const QPointF p = m_inNotOut ? -h.tanIn : h.tanOut;
      m_line->setLine( QLineF( QPointF( 0, 0 ), p ) );
      m_end->m_posW->setPos( p );
    }
  };
  Tangent m_inT, m_outT;

public:
  HandleWidget( FCurveItem* parent, size_t index )
    : QGraphicsWidget( parent )
    , m_parent( parent )
    , m_index( index )
    , m_center( new Center( this ) )
    , m_inT( this, true )
    , m_outT( this, false )
  {
  }
  void setValue( const Handle& h )
  {
    this->setPos( h.pos );
    m_inT.setValue( h );
    m_outT.setValue( h );
  }
};

FCurveItem::FCurveItem()
  : m_curve( NULL )
  , m_curveShape( new FCurveShape( this ) )
{
  m_curveShape->setParentItem( this );
}

void FCurveItem::addHandle( size_t i )
{
  HandleWidget* w = new HandleWidget( this, i );
  w->setValue( m_curve->getHandle( i ) );
  m_handles.push_back( w );
}

void FCurveItem::onHandleAdded()
{
  this->addHandle( m_curve->getHandleCount() - 1 );
}

void FCurveItem::onHandleMoved( size_t i )
{
  assert( i < m_handles.size() );
  assert( m_handles.size() == m_curve->getHandleCount() );
  m_handles[i]->setValue( m_curve->getHandle( i ) );
  m_curveShape->update();
}

void FCurveItem::setCurve( AbstractFCurveModel* curve )
{
  assert( curve != m_curve );
  m_curve = curve;
  //connect( m_curve, &AbstractFCurveModel::handleMoved, this, &FCurveEditor::onHandleMoved );
  QObject::connect( m_curve, SIGNAL( handleMoved( size_t ) ), this, SLOT( onHandleMoved( size_t ) ) );
  QObject::connect( m_curve, SIGNAL( handleAdded() ), this, SLOT( onHandleAdded() ) );

  // Clearing previous handles
  for( std::vector<HandleWidget*>::const_iterator it = m_handles.begin(); it < m_handles.end(); it++ )
    delete *it;
  m_handles.clear();

  size_t hc = m_curve->getHandleCount();
  for( size_t i = 0; i < hc; i++ )
    this->addHandle( i );
}
