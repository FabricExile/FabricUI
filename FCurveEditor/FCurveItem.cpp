
#include <FabricUI/FCurveEditor/FCurveItem.h>
#include <FabricUI/FCurveEditor/FCurveEditorScene.h>
#include <FTL/Config.h>
#include <FabricUI/Util/QtSignalsSlots.h>

#include <QStyleOption>
#include <QPainter>
#include <QGraphicsSceneEvent>

#include <cmath>
#include <limits>

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

inline qreal len2( const QPointF& v ) { return v.x() * v.x() + v.y() * v.y(); }

class FCurveItem::FCurveShape : public QGraphicsItem
{
  const FCurveItem* m_parent;
  typedef QGraphicsItem Parent;
  mutable QRectF m_boundingRect;
  mutable bool m_boundingRectDirty;

  void updateBoundingRect() const
  {
    QPointF topLeft = QPointF( 1, 1 ) * std::numeric_limits<qreal>::max();
    QPointF botRight = QPointF( 1, 1 ) * ( -std::numeric_limits<qreal>::max() );
    if( m_parent->m_curve != NULL )
    {
      size_t hc = m_parent->m_curve->getKeyCount();
      if( hc <= 1 )
      {
        QPointF pos;
        if( hc == 1 )
          pos = m_parent->m_curve->getKey( 0 ).pos;
        m_boundingRect = QRectF( -1 + pos.x(), -1 + pos.y(), 2, 2 );
        return;
      }
      for( size_t i = 0; i < hc; i++ )
      {
        Key h = m_parent->m_curve->getKey( i );
        topLeft = min( topLeft, h.pos );
        botRight = max( botRight, h.pos );
        topLeft.setY( std::min( topLeft.y(), h.pos.y() - h.tanIn.y() ) );
        topLeft.setY( std::min( topLeft.y(), h.pos.y() + h.tanOut.y() ) );
        botRight.setY( std::max( botRight.y(), h.pos.y() - h.tanIn.y() ) );
        botRight.setY( std::max( botRight.y(), h.pos.y() + h.tanOut.y() ) );
      }
    }
    m_boundingRect = QRectF( topLeft, botRight );
    m_boundingRectDirty = false;
  }

  inline void updateBoundingRectIfDirty() const
  {
    if( m_boundingRectDirty )
      this->updateBoundingRect();
  }

  QRectF getSelectedKeysBoundingRect() const
  {
    if( m_parent->m_curve != NULL )
    {
      if( !this->m_parent->m_selectedKeys.empty() )
      {
        QPointF topLeft = QPointF( 1, 1 ) * std::numeric_limits<qreal>::max();
        QPointF botRight = QPointF( 1, 1 ) * ( -std::numeric_limits<qreal>::max() );
        for( std::set<size_t>::const_iterator it = this->m_parent->m_selectedKeys.begin(); it != this->m_parent->m_selectedKeys.end(); it++ )
        {
          Key h = m_parent->m_curve->getKey( *it );
          topLeft = min( topLeft, h.pos );
          botRight = max( botRight, h.pos );
          topLeft.setY( std::min( topLeft.y(), h.pos.y() - h.tanIn.y() ) );
          topLeft.setY( std::min( topLeft.y(), h.pos.y() + h.tanOut.y() ) );
          botRight.setY( std::max( botRight.y(), h.pos.y() - h.tanIn.y() ) );
          botRight.setY( std::max( botRight.y(), h.pos.y() + h.tanOut.y() ) );
        }
        return QRectF( topLeft, botRight );
      }
    }
    return QRectF();
  }

public:
  FCurveShape( const FCurveItem* parent )
    : m_parent( parent )
    , m_boundingRectDirty( true )
  {
    this->updateBoundingRect();
  }

  QRectF boundingRect() const FTL_OVERRIDE
  {
    this->updateBoundingRectIfDirty();
    QRectF r = m_boundingRect;
    const qreal w = r.width();
    r.setLeft( r.left() - 100 * w );
    r.setRight( r.right() + 100 * w );
    const qreal h = r.height();
    if( h > 0 )
    {
      r.setTop( r.top() - 100 * h );
      r.setBottom( r.bottom() + 100 * h );
    }
    return r;
  }

  inline QRectF keysBoundingRect() const
  {
    this->updateBoundingRectIfDirty();
    return m_boundingRect;
  }

  inline void setBoundingRectDirty()
  {
    this->prepareGeometryChange();
    m_boundingRectDirty = true;
    this->update();
  }

  inline QRectF selectedKeysBoundingRect() const
  {
    return this->getSelectedKeysBoundingRect();
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

      QColor inactiveColor( 80, 98, 110 );
      QColor activeColor( 102, 125, 140 );

      if( !this->m_parent->m_selectedKeys.empty() ) {
        pen.setColor( activeColor );        
      }
      else {
        pen.setColor( inactiveColor );        
      }

      painter->setPen( pen );
      const size_t n = widget->width() / 1;// 8;
      qreal step = er.width() / n;
      if( step == 0 )
        return;

      // drawing the FCurve as line segments
      if( m_parent->m_curve->getKeyCount() <= 1 )
        painter->drawLine(
          QPointF( er.left(), m_parent->m_curve->evaluate( er.left() ) ),
          QPointF( er.right(), m_parent->m_curve->evaluate( er.right() ) )
        );
      else
      {
        // from the left of the screen, to the first key
        size_t firstKey = m_parent->m_curve->getIndexAfterTime( er.left() );
        qreal firstKeyTime = m_parent->m_curve->getOrderedKey( firstKey ).pos.x();
        for( size_t i = 0; true; i++ )
        {
          qreal x1 = er.left() + ( i * er.width() ) / n;
          if( x1 > firstKeyTime )
            break;
          qreal x2 = std::min( er.left() + ( ( i + 1 ) * er.width() ) / n, firstKeyTime );
          painter->drawLine(
            QPointF( x1, m_parent->m_curve->evaluate( x1 ) ),
            QPointF( x2, m_parent->m_curve->evaluate( x2 ) )
          );
        }

        size_t lastKey = m_parent->m_curve->getIndexAfterTime( er.right() );
        qreal lastKeyTime = m_parent->m_curve->getOrderedKey( lastKey ).pos.x();

        // drawing all the middle keys
        if( lastKey > 0 )
        {
          for( size_t i = firstKey; i < lastKey; i++ )
          {
            qreal leftTime = m_parent->m_curve->getOrderedKey( i ).pos.x();
            qreal rightTime = m_parent->m_curve->getOrderedKey( i + 1 ).pos.x();
            for( qreal x = leftTime; x < rightTime; x += step )
            {
              qreal x2 = std::min( rightTime, x + step );
              painter->drawLine(
                QPointF( x, m_parent->m_curve->evaluate( x ) ),
                QPointF( x2, m_parent->m_curve->evaluate( x2 ) )
              );
            }
          }
        }

        // from the last key to the right of the screen
        for( qreal x = lastKeyTime; x < er.right(); x += step )
        {
          qreal x2 = std::min( er.right(), x + step );
          painter->drawLine(
            QPointF( x, m_parent->m_curve->evaluate( x ) ),
            QPointF( x2, m_parent->m_curve->evaluate( x2 ) )
          );
        }
      }
    }
  }
};

QRectF FCurveItem::keysBoundingRect() const { return m_curveShape->keysBoundingRect(); }

QRectF FCurveItem::selectedKeysBoundingRect() const { 
  return m_curveShape->selectedKeysBoundingRect(); 
}

const qreal TangentToSpaceRatio = 3;

class FCurveItem::KeyWidget : public QGraphicsWidget
{
  FCurveItem* m_parent;
  size_t m_index;

  class Tangent
  {
    bool m_inNotOut;
    QGraphicsLineItem* m_line;

    class End : public QGraphicsEllipseItem
    {
      KeyWidget* m_parent;
      Tangent* m_tangent;
      bool m_selected;
      typedef QGraphicsEllipseItem Parent;
      inline void updateColor()
      {
        this->setPen( Qt::NoPen );
        this->setBrush( m_selected ? QColor( 39, 168, 223 ) : QColor( 255, 255, 255 ) );
      }
    public:
      QGraphicsWidget* m_posW; // Used for its position
      End( KeyWidget* parent, Tangent* tangent )
        : QGraphicsEllipseItem()
        , m_parent( parent )
        , m_tangent( tangent )
        , m_posW( new QGraphicsWidget( parent ) )
      {
        this->setParentItem( m_posW );
        this->setRect( QRectF( -4, -4, 8, 8 ) );
        this->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
        this->setSelected( false );
        this->setAcceptHoverEvents( true );
      }
      inline void setEndSelected( bool s ) { m_selected = s; this->updateColor(); }
    protected:
      void mousePressEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
      {
        emit m_parent->m_parent->interactionBegin();
        m_parent->m_parent->editKey( m_parent->m_index, m_tangent->m_inNotOut ? TAN_IN : TAN_OUT );
      }
      void mouseMoveEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
      {
        AbstractFCurveModel* curve = m_parent->m_parent->m_curve;
        const size_t index = m_parent->m_index;
        Key h = curve->getKey( index );

        const qreal crossProd = std::abs( h.tanIn.x() * h.tanOut.y() - h.tanIn.y() * h.tanOut.x() );
        const qreal l2In = len2( h.tanIn );
        const qreal l2Out = len2( h.tanOut );
        const bool splitTangents = // moving tagents independently ?
          event->modifiers().testFlag( Qt::ShiftModifier ) ||
          ( crossProd * crossProd > 0.001 * l2In * l2Out ) ||
          l2In == 0 || l2Out == 0
        ;
        if( m_tangent->m_inNotOut )
        {
          h.tanIn = -( event->scenePos() - m_parent->scenePos() ) * TangentToSpaceRatio;
          if( h.tanIn.x() < 0 )
            h.tanIn.setX( 0 );
          if( !splitTangents )
            h.tanOut = h.tanIn * sqrt( l2Out / l2In );
        }
        else
        {
          h.tanOut = ( event->scenePos() - m_parent->scenePos() ) * TangentToSpaceRatio;
          if( h.tanOut.x() < 0 )
            h.tanOut.setX( 0 );
          if( !splitTangents )
            h.tanIn = h.tanOut * sqrt( l2In / l2Out );
        }
        curve->setKey( index, h );
        event->widget()->repaint();
      }
      void hoverEnterEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE { this->setCursor( Qt::SizeAllCursor ); }
      void hoverLeaveEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE { this->unsetCursor(); }
      void mouseReleaseEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
      {
        Parent::mouseReleaseEvent( event );
        emit m_parent->m_parent->interactionEnd();
      }
    };
    End* m_end;

  public:
    Tangent( KeyWidget* parent, bool inNotOut )
      : m_inNotOut( inNotOut )
      , m_line( new QGraphicsLineItem( parent ) )
      , m_end( new End( parent, this ) )
    {
      QPen linePen; linePen.setCosmetic( true );
      linePen.setColor( QColor( 255, 255, 255, 128 ) );
      linePen.setWidthF( 2 );
      m_line->setPen( linePen );
    }

    void setValue( const Key& h )
    {
      const QPointF p = ( m_inNotOut ? -h.tanIn : h.tanOut ) / TangentToSpaceRatio;
      m_line->setLine( QLineF( QPointF( 0, 0 ), p ) );
      m_end->m_posW->setPos( p );
    }

    inline void setVisible( bool visible )
    {
      m_line->setVisible( visible );
      m_end->setVisible( visible );
    }

    inline void setSelected( bool s ) { m_end->setEndSelected( s ); }
  };
  Tangent m_inT, m_outT;

  class Center : public QGraphicsRectItem
  {
    KeyWidget* m_parent;
    typedef QGraphicsRectItem Parent;
    bool m_selected;
    bool m_hovered;
    bool m_selectOnRelease;
    void updateColor()
    {
      this->setPen( Qt::NoPen );
      this->setBrush( 
        ( m_hovered && m_parent->m_parent->m_scene->mode() != ADD ) ? QColor( 191, 191, 191 ) :
        m_selected ? QColor( 39, 168, 223 )
        : QColor( 128, 128, 128 )
      );
    }
  public:
    Center( KeyWidget* parent )
      : QGraphicsRectItem( parent )
      , m_parent( parent )
      , m_selected( false )
      , m_hovered( false )
    {
      this->setRect( QRectF( -4, -4, 8, 8 ) );
      this->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
      this->setAcceptHoverEvents( true );
      this->updateColor();
    };
    void setKeySelected( bool selected )
    {
      m_selected = selected;
      this->updateColor();
    }
  protected:
    void mousePressEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
    {
      if( m_parent->m_parent->m_scene->mode() == REMOVE )
      {
        m_parent->m_parent->m_curve->deleteKey( m_parent->m_index );
        return;
      }

      bool shift = event->modifiers().testFlag( Qt::ShiftModifier );
      bool ctrl = event->modifiers().testFlag( Qt::ControlModifier );
      m_selectOnRelease = !shift && !ctrl;
      if( !m_selected && !shift && !ctrl )
        m_parent->m_parent->clearKeySelection();
      if( m_selected && ctrl )
        m_parent->m_parent->removeKeyFromSelection( m_parent->m_index );
      else
        m_parent->m_parent->addKeyToSelection( m_parent->m_index );
      emit m_parent->m_parent->interactionBegin();
      this->setCursor( Qt::SizeAllCursor );
    }
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
    {
      AbstractFCurveModel* curve = m_parent->m_parent->m_curve;
      const size_t index = m_parent->m_index;
      const Key h = curve->getKey( index );
      m_selectOnRelease = false;
      m_parent->m_parent->moveSelectedKeys( event->scenePos() - h.pos );
      event->widget()->repaint();
    }
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
    {
      Parent::mouseReleaseEvent( event );
      if( m_selectOnRelease )
      {
        m_parent->m_parent->clearKeySelection();
        m_parent->m_parent->addKeyToSelection( m_parent->m_index );
        m_parent->m_parent->editKey( m_parent->m_index );
      }
      emit m_parent->m_parent->interactionEnd();
      this->unsetCursor();
    }
    void hoverEnterEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE
    {
      m_hovered = true;
      this->updateColor();
    }
    void hoverLeaveEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE
    {
      m_hovered = false;
      this->updateColor();
    }
  };
  Center* m_center;

public:
  KeyWidget( FCurveItem* parent, size_t index )
    : QGraphicsWidget( parent )
    , m_parent( parent )
    , m_index( index )
    , m_inT( this, true )
    , m_outT( this, false )
    , m_center( new Center( this ) )
  {
    this->setEditState( NOTHING );
  }
  void setValue( const Key& h )
  {
    this->setPos( h.pos );
    m_inT.setValue( h );
    m_outT.setValue( h );
  }
  inline void setIndex( size_t i ) { this->m_index = i; }

  inline void setEditState( KeyProp p )
  {
    const bool visible = ( p != NOTHING );
    m_inT.setVisible( visible );
    m_outT.setVisible( visible );
    m_center->setKeySelected( p == POSITION );
    m_inT.setSelected( p == TAN_IN );
    m_outT.setSelected( p == TAN_OUT );
  }

  inline void setKeySelected( bool selected )
  {
    m_center->setKeySelected( selected );
    if( !selected )
      this->setEditState( NOTHING );
  }
};

FCurveItem::FCurveItem( FCurveEditorScene* scene )
  : m_scene( scene )
  , m_curve( NULL )
  , m_curveShape( new FCurveShape( this ) )
{
  m_curveShape->setParentItem( this );
}

void FCurveItem::clearKeySelection()
{
  for( std::set<size_t>::const_iterator it = m_selectedKeys.begin(); it != m_selectedKeys.end(); it++ )
    m_keys[*it]->setKeySelected( false );
  m_selectedKeys.clear();
  emit this->selectionChanged();
}

void FCurveItem::addKeyToSelection( size_t i )
{
  if( !m_selectedKeys.empty() )
  {
    m_keys[*m_selectedKeys.begin()]->setEditState( NOTHING );
    m_keys[*m_selectedKeys.begin()]->setKeySelected( true );
  }
  m_selectedKeys.insert( i );
  m_keys[i]->setKeySelected( true );
  if( m_selectedKeys.size() == 1 )
    this->editKey( *m_selectedKeys.begin() );
  this->selectionChanged();
}

void FCurveItem::removeKeyFromSelection( size_t i )
{
  m_selectedKeys.erase( i );
  m_keys[i]->setKeySelected( false );
  if( m_selectedKeys.size() == 1 )
    this->editKey( *m_selectedKeys.begin(), POSITION );
  this->selectionChanged();
}

void FCurveItem::rectangleSelect( const QRectF& r, Qt::KeyboardModifiers m )
{
  bool shift = m.testFlag( Qt::ShiftModifier );
  bool ctrl = m.testFlag( Qt::ControlModifier );
  if( m_scene->mode() == REMOVE )
  {
    shift = false;
    ctrl = false;
  }
  else
  if( m_scene->mode() == SELECT )
  {
    if( !shift && !ctrl )
      this->clearKeySelection();
  }
  else
    assert( false );

  for( size_t i = 0; i < m_keys.size(); i++ )
    if( !shift && ctrl ) {
      if( r.contains( m_keys[i]->scenePos() ) )
      this->removeKeyFromSelection( i );
    }
    else {
      if( r.contains( m_keys[i]->scenePos() ) )
        this->addKeyToSelection( i );
    }

  if( m_scene->mode() == REMOVE )
    this->deleteSelectedKeys();
}

void FCurveItem::editKey( size_t i, KeyProp p )
{
  m_keys[i]->setEditState( p );
  m_editedKeyProp = p;
  emit this->editedKeyPropChanged();
}

void FCurveItem::selectAllKeys()
{
  for( size_t i = 0; i < m_curve->getKeyCount(); i++ )
    this->addKeyToSelection( i );
}

void FCurveItem::deleteSelectedKeys()
{
  if( m_selectedKeys.empty() )
    return;
  else
  if( m_selectedKeys.size() == 1 )
  {
    size_t index = *m_selectedKeys.begin();
    this->clearKeySelection();
    m_curve->deleteKey( index );
  }
  else
  {
    std::vector<size_t> orderedIndices;
    for( std::set<size_t>::const_iterator it = m_selectedKeys.begin(); it != m_selectedKeys.end(); it++ )
      orderedIndices.push_back( *it );
    this->clearKeySelection();
    std::sort( orderedIndices.begin(), orderedIndices.end() );
    m_curve->deleteKeys( orderedIndices.data(), orderedIndices.size() );
  }
}

void FCurveItem::moveSelectedKeys( QPointF delta )
{
  if( m_selectedKeys.empty() )
    return;
  else
  if( m_selectedKeys.size() == 1 )
  {
    const size_t index = *m_selectedKeys.begin();
    Key h = m_curve->getKey( index );
    h.pos += delta;
    m_curve->setKey( index, h );
  }
  else
  {
    // TODO (optimisation) : cache that vector<size_t> to avoid reallocation
    std::vector<size_t> indices;
    for( std::set<size_t>::const_iterator it = m_selectedKeys.begin(); it != m_selectedKeys.end(); it++ )
      indices.push_back( *it );
    m_curve->moveKeys( indices.data(), indices.size(), delta );
  }
}

void FCurveItem::addKey( size_t i )
{
  KeyWidget* w = new KeyWidget( this, i );
  w->setValue( m_curve->getKey( i ) );
  m_keys.push_back( w );
}

void FCurveItem::onKeyAdded()
{
  this->addKey( m_keys.size() );
  m_curveShape->setBoundingRectDirty();
}

void FCurveItem::onKeyDeleted( size_t i )
{
  this->clearKeySelection();

  delete m_keys[i];
  for( size_t j = i; j < m_keys.size() - 1; j++ )
  {
    m_keys[j] = m_keys[j + 1];
    m_keys[j]->setIndex( j );
  }
  m_keys.resize( m_keys.size() - 1 );

  m_curveShape->setBoundingRectDirty();
}

void FCurveItem::onKeyMoved( size_t i )
{
  assert( i < m_keys.size() );
  assert( m_keys.size() == m_curve->getKeyCount() );
  m_keys[i]->setValue( m_curve->getKey( i ) );
  m_curveShape->setBoundingRectDirty();
  if( m_selectedKeys.find( i ) != m_selectedKeys.end() )
    emit this->editedKeyValueChanged();
}

void FCurveItem::onInfinityTypesChanged()
{
  m_curveShape->update();
  this->onDirty();
}

void FCurveItem::setCurve( AbstractFCurveModel* curve )
{
  assert( curve != m_curve );
  m_curve = curve;
  QOBJECT_CONNECT( m_curve, SIGNAL, AbstractFCurveModel, keyMoved, ( size_t ), this, SLOT, FCurveItem, onKeyMoved, ( size_t ) );
  QOBJECT_CONNECT( m_curve, SIGNAL, AbstractFCurveModel, keyAdded, (), this, SLOT, FCurveItem, onKeyAdded, () );
  QOBJECT_CONNECT( m_curve, SIGNAL, AbstractFCurveModel, keyDeleted, ( size_t ), this, SLOT, FCurveItem, onKeyDeleted, ( size_t ) );
  QOBJECT_CONNECT( m_curve, SIGNAL, AbstractFCurveModel, infinityTypesChanged, ( ), this, SLOT, FCurveItem, onInfinityTypesChanged, ( ) );
  QOBJECT_CONNECT( m_curve, SIGNAL, AbstractFCurveModel, dirty, ( ), this, SLOT, FCurveItem, onDirty, ( ) );

  // Clearing previous keys
  this->clearKeySelection();
  for( std::vector<KeyWidget*>::const_iterator it = m_keys.begin(); it < m_keys.end(); it++ )
    delete *it;
  m_keys.clear();

  m_curve->init();

  size_t hc = m_curve->getKeyCount();
  for( size_t i = 0; i < hc; i++ )
    this->addKey( i );

  m_curveShape->setBoundingRectDirty();
}

void FCurveItem::paint( QPainter * p, const QStyleOptionGraphicsItem * s, QWidget * w )
{
  m_curve->update();
}
