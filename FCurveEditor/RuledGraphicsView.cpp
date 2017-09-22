//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/FCurveEditor/RuledGraphicsView.h>
#include <FabricUI/FCurveEditor/Ruler.h>
#include <QGraphicsView>
#include <QLayout>
#include <QScrollBar>
#include <FabricUI/Util/QtSignalsSlots.h>

#include <cmath>
#include <qevent.h>
#include <QDebug>
#include <assert.h>
#include <QTimer>

#define RULED_GRAPHICS_VIEW_CACHE_BACKGROUND 1;

using namespace FabricUI::FCurveEditor;

class RuledGraphicsView::GraphicsView : public QGraphicsView
{
  typedef QGraphicsView Parent;
  RuledGraphicsView* m_parent;
  enum State { PANNING, SELECTING, NOTHING } m_state;
  QRectF m_selectionRect; // in scene space
  QPoint m_lastMousePos; // in widget space

#if RULED_GRAPHICS_VIEW_CACHE_BACKGROUND
  QRect m_backgroundCacheSize;
  QTransform m_backgroundCacheTransform;
  QPixmap m_backgroundCache;
#endif

public:
  GraphicsView( RuledGraphicsView* parent )
    : m_parent( parent )
    , m_state( NOTHING )
  {
    this->setDragMode( QGraphicsView::NoDrag ); // Reimplementing it ourself to support both panning and selection
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  }
  void scrollPx( QPoint delta )
  {
    QScrollBar *hBar = horizontalScrollBar();
    QScrollBar *vBar = verticalScrollBar();
    hBar->setValue( hBar->value() + ( isRightToLeft() ? delta.x() : -delta.x() ) );
    vBar->setValue( vBar->value() - delta.y() );
  }
  void drawBackgroundNoCache( QPainter* );
protected:
  // HACK ? (move the parent's handler here instead ?)
  void wheelEvent( QWheelEvent * e ) FTL_OVERRIDE { return e->ignore(); }
  void drawBackground( QPainter *, const QRectF & ) FTL_OVERRIDE;
  void mousePressEvent( QMouseEvent *event ) FTL_OVERRIDE
  {
    m_state = NOTHING;
    if( event->button() == Qt::MiddleButton || event->modifiers().testFlag( Qt::AltModifier ) )
    {
      m_lastMousePos = event->pos();
      m_state = PANNING;
      return event->accept();
    }

    Parent::mousePressEvent( event ); // passing the event to the scene
    if( event->isAccepted() )
      return;

    if( m_parent->m_rectangleSelectionEnabled &&
      event->button() == Qt::LeftButton )
    {
      m_state = SELECTING;
      m_selectionRect.setTopLeft( this->mapToScene( event->pos() ) );
      m_selectionRect.setSize( QSizeF( 0, 0 ) );
    }
  }
  void mouseMoveEvent( QMouseEvent *event ) FTL_OVERRIDE
  {
    switch( m_state )
    {
    case PANNING:
    {
      QPoint delta = event->pos() - m_lastMousePos;
      this->scrollPx( delta );
      m_lastMousePos = event->pos();
      m_parent->updateRulersRange();
    } break;
    case SELECTING:
    {
      m_selectionRect.setBottomRight( this->mapToScene( event->pos() ) );
      this->update();
    } break;
    case NOTHING: break;
    }
    Parent::mouseMoveEvent( event );
  }
  void mouseReleaseEvent( QMouseEvent *event ) FTL_OVERRIDE
  {
    if( m_state == SELECTING )
    {
      this->update();
      emit m_parent->rectangleSelectReleased( m_selectionRect, event->modifiers() );
    }
    m_state = NOTHING;
    Parent::mouseReleaseEvent( event );
  }
  void drawForeground( QPainter * p, const QRectF & r ) FTL_OVERRIDE
  {
    if( m_state == SELECTING )
    {
      QPen pen( QColor(35, 35, 35), 2.0, Qt::CustomDashLine );
      pen.setCosmetic( true );
      
      QVector<qreal> dashes;
      dashes << 1 << 2;
      pen.setDashPattern(dashes);

      p->setPen( pen );
      p->setBrush( QColor(100, 100, 100, 50) );

      p->drawRect( m_selectionRect );
    }
  }
};

QGraphicsView* RuledGraphicsView::view() { return m_view; }

class RuledGraphicsView::Ruler : public FabricUI::FCurveEditor::Ruler
{
  typedef FabricUI::FCurveEditor::Ruler Parent;
  RuledGraphicsView* m_parent;
  bool m_isVertical;

public:
  Ruler( RuledGraphicsView* parent, bool isVertical )
    : Parent( isVertical ? Ruler::Left : Ruler::Bottom )
    , m_parent( parent )
    , m_isVertical( isVertical )
  {}

protected:
  void wheelEvent( QWheelEvent * e ) FTL_OVERRIDE
  {
    m_parent->wheelEvent(
      m_isVertical ? 0 : e->delta(),
      m_isVertical ? e->delta() : 0,
      m_parent->m_view->mapToScene( m_parent->m_view->mapFromGlobal( e->globalPos() ) )
    );
    e->accept();
  }
  void enterEvent( QEvent *event ) FTL_OVERRIDE
  {
    this->setCursor( m_isVertical ? Qt::SizeVerCursor : Qt::SizeHorCursor );
  }
  void leaveEvent( QEvent *event ) FTL_OVERRIDE { this->unsetCursor(); }
};

RuledGraphicsView::RuledGraphicsView()
  : m_view( new GraphicsView( this ) )
  , m_scrollSpeed( 1 / 800.0f )
  , m_zoomOnCursor( true )
  , m_smoothZoom( false )
  , m_targetScale( QPointF( 1E2, 1E2 ) )
  , m_timer( new QTimer( this ) )
{
  this->setObjectName( "RuledGraphicsView" );

  QGridLayout* lay = new QGridLayout();
  lay->setSpacing( 0 ); lay->setMargin( 0 );

  this->enableRectangleSelection( true );

  lay->addWidget( m_view, 0, 1 );

  m_vRuler = new Ruler( this, true );
  m_vRuler->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ) );
  lay->addWidget( m_vRuler, 0, 0 );

  m_hRuler = new Ruler( this, false );
  m_hRuler->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  lay->addWidget( m_hRuler, 1, 1 );
  this->setRulersSize( 24 );

  this->setLayout( lay );

  QOBJECT_CONNECT( m_timer, SIGNAL, QTimer, timeout, ( ), this, SLOT, RuledGraphicsView, tick, ( ) );
  m_timer->setInterval( 16 );
  if( m_smoothZoom )
    m_timer->start();

  this->setTopToBottomY( false );
  this->setGridMaxSpacing( 200 );
  this->setGridMinSpacing( 20 );
  this->setGridThickness( 2 );
  this->setGridThicknessSquarred( true );
  this->setLogScale( 2 );
}

void RuledGraphicsView::setRulersSize( const int s )
{
  m_rulersSize = s;
  m_vRuler->setFixedWidth( s );
  m_hRuler->setFixedHeight( s );
}

bool RuledGraphicsView::topToBottomY() const
{
  return ( m_view->matrix().m22() >= 0 );
}

void RuledGraphicsView::setTopToBottomY( bool topToBottom )
{
  if( this->topToBottomY() != topToBottom )
  {
    this->view()->scale( 1, -1 );
    this->updateRulersRange();
  }
}

void RuledGraphicsView::wheelEvent( QWheelEvent * e )
{
  this->wheelEvent( e->delta(), e->delta(), m_view->mapToScene( m_view->mapFromGlobal( e->globalPos() ) ) );
}

void RuledGraphicsView::centeredScale( qreal x, qreal y )
{
  if( m_zoomOnCursor )
  {
    QPoint before = m_view->mapFromScene( m_scalingCenter );

    // The rule is that the scaling center must have the same pixel
    // coordinates before and after the scaling
    QPoint viewCenter = m_view->rect().center();
    QPoint scalingCenterPxOffset = m_view->mapFromScene( m_scalingCenter ) - viewCenter;
    m_view->centerOn( m_scalingCenter );
    m_view->scale( x, y );
    QPoint newScalingCenterPxOffset = m_view->mapFromScene( m_scalingCenter ) - viewCenter;
    m_view->centerOn( m_view->mapToScene( viewCenter + ( newScalingCenterPxOffset - scalingCenterPxOffset ) ) );

    QPoint after = m_view->mapFromScene( m_scalingCenter );
    QPoint diff = before - after; // correcting a 1px error that sometimes happen
    m_view->scrollPx( diff );

    assert( before == m_view->mapFromScene( m_scalingCenter ) );
  }
  else
    m_view->scale( x, y );

  updateRulersRange();
}

void RuledGraphicsView::wheelEvent( int xDelta, int yDelta, QPointF scalingCenter )
{
  m_scalingCenter = scalingCenter;
  qreal sX = ( 1 + xDelta * m_scrollSpeed );
  qreal sY = ( 1 + yDelta * m_scrollSpeed );
  if( m_smoothZoom )
  {
    m_targetScale.setX( m_targetScale.x() * sX );
    m_targetScale.setY( m_targetScale.y() * sY );
    m_timer->start();
    tick();
  }
  else
    centeredScale( sX, sY );
}

void RuledGraphicsView::fitInView( const QRectF r0, qreal margin )
{
  // dilate
  QPointF diag = r0.bottomRight() - r0.topLeft();
  assert( margin > 0 && margin < 1 );
  QRectF r = QRectF(
    r0.topLeft() - margin * diag,
    r0.bottomRight() + margin * diag
  );

  this->view()->fitInView( r );
  m_targetScale = QPointF( m_view->matrix().m11(), m_view->matrix().m22() );
  this->updateRulersRange();
}

void RuledGraphicsView::updateRulersRange()
{
  QRectF vrect = m_view->mapToScene( m_view->viewport()->geometry() ).boundingRect();
  if( m_view->matrix().m22() < 0 )
    m_vRuler->setRange( vrect.bottom(), vrect.top() );
  else
    m_vRuler->setRange( vrect.top(), vrect.bottom() );

  QRectF hrect = m_view->mapToScene( m_view->viewport()->geometry() ).boundingRect();
  m_hRuler->setRange( hrect.left(), hrect.right() );
}

void RuledGraphicsView::resizeEvent( QResizeEvent * e )
{
  QWidget::resizeEvent( e );
  this->updateRulersRange();
}

void RuledGraphicsView::tick()
{
  QPointF currentScale = QPointF( m_view->matrix().m11(), m_view->matrix().m22() );
  if(
    std::abs( std::log( currentScale.x() ) - std::log( m_targetScale.x() ) ) +
    std::abs( std::log( std::abs(currentScale.y()) ) - std::log( std::abs(m_targetScale.y()) ) )
  > 0.01 ) // If we are close enough to the target, we stop the animation
  {
    const qreal ratio = 0.2; // TODO : property
    QPointF newScale(
      ( 1 - ratio ) * currentScale.x() + ratio * m_targetScale.x(),
      ( 1 - ratio ) * currentScale.y() + ratio * m_targetScale.y()
    );
    this->centeredScale(
      newScale.x() / currentScale.x(),
      newScale.y() / currentScale.y()
    );
  }
  else
    m_timer->stop();
}

void RuledGraphicsView::GraphicsView::drawBackground( QPainter * p, const QRectF & r )
{
#if RULED_GRAPHICS_VIEW_CACHE_BACKGROUND
  const QRect wr = this->viewport()->geometry();
  const bool cacheUpToDate =
  (
    ( p->transform() == m_backgroundCacheTransform ) &&
    ( wr == m_backgroundCacheSize )
  );
  if( !cacheUpToDate )
  {
    // Updating the cache
    m_backgroundCache = QPixmap( wr.size() );
    m_backgroundCache.fill( Qt::transparent );
    QPainter cacheP( &m_backgroundCache );
    cacheP.setMatrix( p->matrix() );
    this->drawBackgroundNoCache( &cacheP );

    // Updating the indicators of cache validity
    m_backgroundCacheTransform = p->transform();
    m_backgroundCacheSize = wr;
  }
  
  p->setMatrixEnabled( false );
  p->drawPixmap( wr.topLeft(), m_backgroundCache );
  p->setMatrixEnabled( true );
#else
  this->drawBackgroundNoCache( p );
#endif
}

void RuledGraphicsView::GraphicsView::drawBackgroundNoCache( QPainter* p )
{
  QRect wr = this->viewport()->geometry(); // widget viewRect
  QRectF sr = this->mapToScene( wr ).boundingRect(); // scene viewRect

  // Grid
  // TODO : render the grid in a raster, and then tile it on the background (for better performance)
  for( int o = 0; o < 2; o++ ) // 2 orientations
  {
    const Qt::Orientation ori = o == 0 ? Qt::Vertical : Qt::Horizontal;

    qreal size = ( ori == Qt::Vertical ? sr.height() : sr.width() );
    qreal sizePx = ( ori == Qt::Vertical ? wr.height() : wr.width() );
    qreal minU = ( ori == Qt::Vertical ? sr.top() : sr.left() );
    qreal maxU = ( ori == Qt::Vertical ? sr.bottom() : sr.right() );

    const qreal logScale = m_parent->m_logScale;

    qreal minFactorFull = ( qreal( sizePx ) / m_parent->m_gridMaxSpacing ) / size;
    // getting the nearest integer (in logarithmic scale)
    qreal minFactor = std::pow( logScale, std::ceil( std::log( minFactorFull ) / std::log( logScale ) ) );
    qreal maxFactor = ( qreal( sizePx ) / m_parent->m_gridMinSpacing ) / size;
    for( qreal factor = minFactor; factor < maxFactor; factor *= logScale )
    {
      QPen pen;
      // Pen width
      {
        // We use a cosmetic pen here, because when we tried a "scene-space"
        // pen, precision errors were making thin lines be inconsistently invisible
        // for the same factor
        pen.setCosmetic( true );
        qreal pwidth = ( m_parent->m_gridThickness * minFactorFull ) / factor; // TODO : property
        if( m_parent->m_gridThicknessSquarred )
          pwidth *= ( minFactorFull / factor );
        qreal palpha = m_parent->m_gridColor.alphaF();
        if( pwidth < 1 )
        {
          palpha *= pwidth;
          pwidth = 1;
        }
        pen.setWidthF( pwidth );
        QColor c = m_parent->m_gridColor;
        c.setAlphaF( palpha );
        pen.setColor( c );
      }
      p->setPen( pen );
      for( qreal i = std::floor( factor * minU ); i < factor * maxU; i++ )
        if( ori == Qt::Horizontal )
          p->drawLine( QPointF( i / factor, sr.top() ), QPointF( i / factor, sr.bottom() ) );
        else
          p->drawLine( QPointF( sr.left(), i / factor ), QPointF( sr.right(), i / factor ) );
    }
  }
}
