
#include <FabricUI/FCurveEditor/Ruler.h>

#include <cmath>
#include <QPainter>
#include <QDebug>

using namespace FabricUI::FCurveEditor;

Ruler::Ruler( Orientation o )
  : m_orientation( o )
  , m_majorGradsPixelSpacing( 800 )
  , m_majToMinGradsRatio( 8.0 )
  , m_logScale( 2 )
  , m_penColor( QColor( 128, 128, 128 ) )
  , m_penTextColor( QColor( 255, 255, 255 ) )
  , m_majorPenWidth( 2 )
  , m_minorPenWidth( 1 )
  , m_majGradToBreadthRatio( 0.5 )
  , m_minGradToBreadthRatio( 0.1 )
  , m_textBreadthPos( 0.25 )
{
  this->setObjectName( "Ruler" );
}

void Ruler::setRange( qreal start, qreal end )
{
  m_start = start;
  m_end = end;
  this->update();
}

inline qreal Map(
  qreal v, // value to be mapped
  qreal minB, qreal maxB, // original range
  qreal minA, qreal maxA // new range
)
{
  return minA + ( v - minB ) * ( maxA - minA ) / ( maxB - minB );
}

inline bool IsVertical( Ruler::Orientation o ) { return !( o == Ruler::Top || o == Ruler::Bottom ); }

inline qreal Map(
  qreal v, // value to be mapped
  Ruler::Orientation o,
  qreal min, qreal max,
  QRect r
)
{
  return IsVertical( o ) ?
    Map( v, min, max, r.top(), r.bottom() ) :
    Map( v, min, max, r.left(), r.right() )
  ;
}

inline void DrawLine( QPainter* p, qreal pos, Ruler::Orientation o, qreal endRPos, const QRect& r )
{
  switch( o )
  {
  case Ruler::Right:
    p->drawLine( QPointF( endRPos * r.left() + ( 1 - endRPos ) * r.right(), pos ), QPointF( r.right(), pos ) );
    break;
  case Ruler::Left:
    p->drawLine( QPointF( endRPos * r.right() + ( 1 - endRPos ) * r.left(), pos ), QPointF( r.left(), pos ) );
    break;
  case Ruler::Top:
    p->drawLine( QPointF( pos, r.top() ), QPointF( pos, ( 1 - endRPos ) * r.top() + endRPos * r.bottom() ) );
    break;
  case Ruler::Bottom:
    p->drawLine( QPointF( pos, r.bottom() ), QPointF( pos, ( 1 - endRPos ) * r.bottom() + endRPos * r.top() ) );
    break;
  }
}

inline void DrawText( QPainter* p, qreal pos, Ruler::Orientation o, qreal endRPos, const QRect& r, const QString& txt )
{
  switch( o )
  {
  case Ruler::Right:
    p->rotate( 90 );
    p->drawText( QPointF( pos, -( endRPos * r.left() + ( 1 - endRPos ) * r.right() ) ), txt );
    p->rotate( -90 );
    break;
  case Ruler::Left:
    p->rotate( 90 );
    p->drawText( QPointF( pos, -( endRPos * r.right() + ( 1 - endRPos ) * r.left() ) ), txt );
    p->rotate( -90 );
    break;
  case Ruler::Top:
    p->drawText( QPointF( pos, endRPos * r.bottom() + ( 1 - endRPos ) * r.top() ), txt );
    break;
  case Ruler::Bottom:
    p->drawText( QPointF( pos, endRPos * r.top() + ( 1 - endRPos ) * r.bottom() ), txt );
    break;
  }
}

void Ruler::paintEvent( QPaintEvent * e )
{
  Parent::paintEvent( e );

  if( m_start == m_end )
    return;

  const QRect r = this->contentsRect();
  const size_t bigSide = ( IsVertical( m_orientation ) ? r.height() : r.width() );
  qreal bigFactor = std::pow( m_logScale,
    std::ceil( -std::log( std::abs( m_end - m_start ) * m_majorGradsPixelSpacing / bigSide ) / std::log( m_logScale ) ) ) * 4;
  qreal smallFactor = bigFactor * m_majToMinGradsRatio;
  {
    QPainter p( this );
    QPen pen;
    pen.setColor( m_penColor );
    pen.setWidthF( m_majorPenWidth );
    p.setPen( pen );
    qreal minV = std::min( m_start, m_end );
    qreal maxV = std::max( m_start, m_end );
    for( qreal i = std::floor( minV * bigFactor ); i < bigFactor * maxV; i++ )
    {
      qreal xs = i / bigFactor;
      qreal xw = Map( xs, m_orientation, m_start, m_end, r );
      DrawLine( &p, xw, m_orientation, m_majGradToBreadthRatio, r );
      pen.setColor( m_penTextColor );
      p.setPen( pen );
      DrawText( &p, xw + 5, m_orientation, m_textBreadthPos, r, QString::number( xs ) );
      pen.setColor( m_penColor );
      p.setPen( pen );
    }
    pen.setWidthF( m_minorPenWidth );
    p.setPen( pen );
    for( qreal i = std::floor( minV * smallFactor ); i < smallFactor * maxV; i++ )
    {
      qreal xs = i / smallFactor;
      qreal xw = Map( xs, m_orientation, m_start, m_end, r );
      DrawLine( &p, xw, m_orientation, m_minGradToBreadthRatio, r );
    }
  }
}
