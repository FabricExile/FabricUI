
#include <FabricUI/FCurveEditor/FCurveEditorScene.h>
#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <FabricUI/FCurveEditor/FCurveItem.h>

#include <FabricUI/Util/QtSignalsSlots.h>

#include <QWidget>
#include <QGraphicsSceneEvent>

using namespace FabricUI::FCurveEditor;

FCurveEditorScene::FCurveEditorScene( AbstractFCurveModel* model )
  : m_curveItem( new FCurveItem( this ) )
  , m_draggedSnappedKey( NULL )
  , m_isDraggingKey( false )
  , m_snapToCurve( false )
  , m_clickState( RELEASED )
{
  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, interactionBegin, ( ), this, SIGNAL, FCurveEditorScene, interactionBegin, ( ) );
  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, interactionEnd, ( ), this, SIGNAL, FCurveEditorScene, interactionEnd, ( ) );

  m_curveItem->setCurve( model );
  this->addItem( m_curveItem );

  this->setSceneRect( QRectF( -1E8, -1E8, 2 * 1E8, 2 * 1E8 ) );
  this->setMode( SELECT );
}

void FCurveEditorScene::setMode( Mode m )
{
  m_mode = m;
  if( m_mode == REMOVE )
    m_curveItem->clearKeySelection();
  emit this->modeChanged();
}

class FCurveEditorScene::DraggedKey : public QGraphicsRectItem
{
public:
  QGraphicsLineItem* m_vertLine;

  DraggedKey( FCurveEditorScene * parent )
  {
    {
      QPen linePen;
      linePen.setColor( QColor( 255, 255, 255, 64 ) );
      linePen.setCosmetic( true );
      m_vertLine = new QGraphicsLineItem();
      m_vertLine->setPen( linePen );
      parent->addItem( m_vertLine );
    }

    parent->addItem( this );
    this->setRect( QRectF( -4, -4, 8, 8 ) );
    this->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
    this->setPen( Qt::NoPen );
    this->setBrush( QColor( 39, 168, 223 ) );
  }

  ~DraggedKey() { delete m_vertLine; }
};

void FCurveEditorScene::setSnapToCurve( bool snap )
{
  if( snap != m_snapToCurve )
  {
    m_snapToCurve = snap;
    if( m_isDraggingKey )
    {
      AbstractFCurveModel* curve = this->curveItem()->curve();
      if( m_snapToCurve )
      {
        this->updateDraggedSnappedPos( curve->getKey( curve->getKeyCount() - 1 ).pos );
        m_curveItem->deleteSelectedKeys();
      }
      else
      {
        this->addKeyAtPos( m_draggedSnappedKey->scenePos() );
      }
      curve->update();
    }
    this->updateDraggedSnappedKey();
    emit this->snapToCurveChanged();
  }
}

// returns true if the key is displayed, false otherwise
bool FCurveEditorScene::updateDraggedSnappedKey()
{
  if( m_isDraggingKey && m_snapToCurve )
  {
    if( m_draggedSnappedKey == NULL )
      m_draggedSnappedKey = new DraggedKey( this );
    return true;
  }
  else
  {
    if( m_draggedSnappedKey != NULL )
    {
      delete m_draggedSnappedKey;
      m_draggedSnappedKey = NULL;
    }
    return false;
  }
}

void FCurveEditorScene::addKeyAtPos( QPointF scenePos )
{
  Key h; h.pos = scenePos;
  h.tanIn = QPointF( 1, 0 ); h.tanOut = h.tanIn;
  h.tanInType = 0; h.tanOutType = 0;
  m_curveItem->curve()->addKey( h, true );
  size_t index = m_curveItem->curve()->getKeyCount() - 1;
  m_curveItem->curve()->update();
  m_curveItem->addKeyToSelection( index );
}

void FCurveEditorScene::mousePressEvent( QGraphicsSceneMouseEvent * e )
{
  if( this->mode() == ADD && e->button() == Qt::LeftButton )
  {
    m_curveItem->clearKeySelection();
    if( !m_snapToCurve )
    {
      // Adding a new Key
      this->addKeyAtPos( e->scenePos() );
    }
    m_isDraggingKey = true;
    this->updateDraggedSnappedPos( e->scenePos() );
  }
  else
    Parent::mousePressEvent( e );
  m_clickState = CLICKED;
}

void FCurveEditorScene::updateDraggedSnappedPos( QPointF scenePos )
{
  if( this->updateDraggedSnappedKey() )
  {
    qreal x = scenePos.x();
    m_draggedSnappedKey->setPos( QPointF(
      x,
      m_curveItem->curve()->evaluate( x )
    ) );
    m_draggedSnappedKey->m_vertLine->setLine(
      x,
      this->sceneRect().top(),
      x,
      this->sceneRect().bottom()
    );
    this->update();
  }
}

void FCurveEditorScene::mouseMoveEvent( QGraphicsSceneMouseEvent * e )
{
  Parent::mouseMoveEvent( e );
  if( m_isDraggingKey )
  {
    if( this->updateDraggedSnappedKey() )
    {
      this->updateDraggedSnappedPos( e->scenePos() );
    }
    else
    {
      if( m_clickState == CLICKED )
        emit this->interactionBegin();

      size_t index = m_curveItem->curve()->getKeyCount() - 1;
      Key k = m_curveItem->curve()->getKey( index );
      k.pos = e->scenePos();
      m_curveItem->curve()->setKey( index, k );
      m_curveItem->curve()->autoTangent( index );
      e->widget()->repaint();
    }
  }
  m_clickState = DRAGGING;
}

void FCurveEditorScene::mouseReleaseEvent( QGraphicsSceneMouseEvent * e )
{
  Parent::mouseReleaseEvent( e );
  if( m_isDraggingKey )
  {
    if( m_snapToCurve )
      this->addKeyAtPos( QPointF(
        e->scenePos().x(),
        m_curveItem->curve()->evaluate( e->scenePos().x() )
      ) );
    m_isDraggingKey = false;
    this->updateDraggedSnappedKey();
    if( !m_snapToCurve && m_clickState == DRAGGING )
      emit this->interactionEnd();
  }
  m_clickState = RELEASED;
}
