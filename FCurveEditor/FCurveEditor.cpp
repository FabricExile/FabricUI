//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FabricUI/FCurveEditor/RuledGraphicsView.h>
#include <FabricUI/FCurveEditor/FCurveItem.h>
#include <FabricUI/FCurveEditor/FCurveEditorScene.h>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QToolBar>
#include <QPushButton>
#include <QSplitter>
#include <QComboBox>
#include <QMenu>
#include <QGraphicsSceneEvent>

#include <QDebug>
#include <assert.h>

#include <FabricUI/Util/QtSignalsSlots.h>

using namespace FabricUI::FCurveEditor;

AbstractAction::AbstractAction( QObject* parent )
  : QAction( parent )
{
  QOBJECT_CONNECT(
    this, SIGNAL, QAction, triggered, ( bool ),
    this, SLOT, AbstractAction, onTriggered, ( )
  );
}

class FCurveEditor::KeyValueEditor : public QFrame
{
  FCurveEditor* m_parent;

  class FloatEditor : public QWidget
  {
    KeyValueEditor* m_parent;
    QLabel* m_label;
    QLineEdit* m_edit;
  public:
    FloatEditor( KeyValueEditor* parent, bool isXNotY )
      : QWidget( parent )
      , m_parent( parent )
      , m_label( new QLabel( isXNotY ? "X:" : "Y:" ) )
      , m_edit( new QLineEdit() )
    {
      this->setObjectName( "FloatEditor" );

      QHBoxLayout* m_layout = new QHBoxLayout();
      m_layout->setMargin( 0 );
      m_layout->addWidget( m_label );
      m_layout->addWidget( m_edit );
      this->setLayout( m_layout );
      this->set( 0 );

      if( isXNotY )
        QOBJECT_CONNECT( m_edit, SIGNAL, QLineEdit, editingFinished, ( ), m_parent->m_parent, SLOT, FCurveEditor, veXEditFinished, ( ) );
      else
        QOBJECT_CONNECT( m_edit, SIGNAL, QLineEdit, editingFinished, ( ), m_parent->m_parent, SLOT, FCurveEditor, veYEditFinished, ( ) );
    }
    inline void set( qreal v )
    {
      m_edit->setText( QString::number( v ) );
      m_edit->setCursorPosition( 0 );
    }
    inline void clear() { m_edit->clear(); }
    inline QString get() const { return m_edit->text(); }
  };

  class TangentTypeEditor : public QComboBox
  {
    AbstractFCurveModel* m_model;
  public:

    TangentTypeEditor()
      : m_model( NULL )
    {
      this->setObjectName( "TangentTypeEditor" );
      this->setToolTip( "Tangent Type" );
    }

    void setModel( AbstractFCurveModel* model )
    {
      if( model != m_model )
      {
        this->clear();
        for( size_t i = 0; i < model->tangentTypeCount(); i++ )
          this->addItem( model->tangentTypeName( i ) );
        m_model = model;
      }
    }
  };

public:
  FloatEditor* m_x;
  FloatEditor* m_y;
  TangentTypeEditor* m_tanType;

  KeyValueEditor( FCurveEditor* parent )
    : QFrame( parent )
    , m_parent( parent )
    , m_x( new FloatEditor( this, true ) )
    , m_y( new FloatEditor( this, false ) )
    , m_tanType( new TangentTypeEditor() )
  {
    this->setObjectName( "KeyValueEditor" );

    QHBoxLayout* m_layout = new QHBoxLayout();
    m_layout->setContentsMargins( QMargins( 8, 2, 8, 2 ) );
    m_layout->setSpacing( 8 );
    m_layout->addWidget( m_x );
    m_layout->addWidget( m_y );
    m_layout->addWidget( m_tanType );
    QOBJECT_CONNECT_OVERLOADED( m_tanType, SIGNAL, QComboBox, currentIndexChanged, ( int ),, parent, SLOT, FCurveEditor, veTanTypeEditFinished, ( ), );
    this->setLayout( m_layout );
    this->resize( 200, 32 );
  }
};

void FCurveEditor::veEditFinished( bool isXNotY )
{
  const QString text = ( isXNotY ? m_keyValueEditor->m_x->get() : m_keyValueEditor->m_y->get() );
  bool ok;
  const qreal v = text.toDouble( &ok );
  if( !ok )
    this->onEditedKeysChanged(); // clear what the user entered, and fetch the model values
  else
  {
    assert( !m_scene->curveItem()->selectedKeys().empty() );
    if( m_scene->curveItem()->selectedKeys().size() == 1 )
    {
      size_t editedKey = *m_scene->curveItem()->selectedKeys().begin();
      Key h = m_scene->curveItem()->curve()->getKey( editedKey );
      QPointF* p = NULL;
      switch( m_scene->curveItem()->editedKeyProp() )
      {
      case FCurveItem::POSITION: p = &h.pos; break;
      case FCurveItem::TAN_IN: p = &h.tanIn; break;
      case FCurveItem::TAN_OUT: p = &h.tanOut; break;
      case FCurveItem::NOTHING: assert( false ); break;
      }
      if( isXNotY )
        p->setX( v );
      else
        p->setY( v );
      m_scene->curveItem()->curve()->setKey( editedKey, h );
    }
    else
    {
      for( std::set<size_t>::const_iterator it = m_scene->curveItem()->selectedKeys().begin();
        it != m_scene->curveItem()->selectedKeys().end(); it++ )
      {
        assert( !isXNotY ); // several keys shouldn't have the same time value, so the field should be disabled then
        Key h = m_scene->curveItem()->curve()->getKey( *it );
        h.pos.setY( v );
        m_scene->curveItem()->curve()->setKey( *it, h );
      }
    }
  }
}

void FCurveEditor::veTanTypeEditFinished()
{
  assert( !m_scene->curveItem()->selectedKeys().empty() );
  assert( m_scene->curveItem()->selectedKeys().size() == 1 );
  {
    size_t editedKey = *m_scene->curveItem()->selectedKeys().begin();
    Key h = m_scene->curveItem()->curve()->getKey( editedKey );
    FCurveItem::KeyProp prop = m_scene->curveItem()->editedKeyProp();
    if( prop == FCurveItem::TAN_IN || prop == FCurveItem::TAN_OUT )
    {
      size_t& t = ( prop == FCurveItem::TAN_IN ? h.tanInType : h.tanOutType );
      size_t newT = m_keyValueEditor->m_tanType->currentIndex();
      if( t != newT )
      {
        t = newT;
        m_scene->curveItem()->curve()->setKey( editedKey, h );
      }
    }
  }
}

const char* ModeNames[MODE_COUNT] =
{
  "Select",
  "Add",
  "Remove"
};

const Qt::KeyboardModifier ModeToggleModifier = Qt::ShiftModifier;

Qt::Key ModeKeys[MODE_COUNT] =
{
  Qt::Key_1,
  Qt::Key_2,
  Qt::Key_3
};
const Qt::Key SnapToCurveKey = Qt::Key_C;

inline QKeySequence GetModeToggleShortcut( Mode m )
{
  return QKeySequence( ModeToggleModifier ).toString() + QKeySequence( ModeKeys[m] ).toString();
}

class FCurveEditor::SetInfinityTypeAction : public AbstractAction
{
  FCurveEditor* m_parent;
  size_t m_type;
public:
  enum Direction { PRE, POST, DIRCOUNT };
  Direction m_direction;

  SetInfinityTypeAction( FCurveEditor* parent, Direction direction, size_t type )
    : AbstractAction( parent )
    , m_parent( parent )
    , m_type( type )
    , m_direction( direction )
  {
  }

  void onTriggered() FTL_OVERRIDE
  {
    if( m_direction == PRE )
      m_parent->m_scene->curveItem()->curve()->setPreInfinityType( m_type );
    if( m_direction == POST )
      m_parent->m_scene->curveItem()->curve()->setPostInfinityType( m_type );
  }

  void setValue( bool v )
  {
    this->setEnabled( !v );
    this->setChecked( v );
  }
};

class FCurveEditor::ToolBar : public QWidget
{
  FCurveEditor* m_parent;
  QHBoxLayout* m_layout;
  QPushButton* m_buttons[MODE_COUNT];
  Mode m_previousMode; // MODE_COUNT if not in a temporary mode

public:
  QPushButton* m_snapToCurveButton;
  std::vector<SetInfinityTypeAction*> m_infTypes[SetInfinityTypeAction::DIRCOUNT];
  QAction* m_snapToCurveAction;
  QAction* m_modeActions[MODE_COUNT];

  ToolBar( FCurveEditor* parent )
    : m_parent( parent )
    , m_layout( new QHBoxLayout() )
    , m_previousMode( MODE_COUNT )
  {
    this->setObjectName( "ToolBar" );
    this->setMinimumHeight( 40 );

    m_layout->setAlignment( Qt::AlignLeft );
    m_layout->setMargin( 8 );
    m_layout->setContentsMargins( 8, 2, 2, 2 );

    const size_t buttonSize = 26;

    for( int m = 0; m < MODE_COUNT; m++ )
    {
      m_buttons[m] = new QPushButton();
      QPushButton* bt = m_buttons[m];
      bt->setObjectName( ModeNames[m] );
      bt->setFixedSize( QSize( buttonSize, buttonSize ) );
      bt->setCheckable( true );
      m_layout->addWidget( bt );
      QString actionName = QString::fromUtf8( ModeNames[m] ) + " Keys Mode";
      bt->setToolTip( actionName + " [Press (" + QKeySequence(ModeToggleModifier).toString() + ")" + QKeySequence(ModeKeys[m]).toString() + "]" );
      m_modeActions[m] = new QAction( actionName, m_parent );
      QAction* action = m_modeActions[m];
      action->setCheckable( true );
      action->setShortcut( GetModeToggleShortcut( Mode(m) ) );
      action->setShortcutContext( Qt::WidgetWithChildrenShortcut );
      m_parent->addAction( action );
      QOBJECT_CONNECT( action, SIGNAL, QAction, triggered, ( bool ), bt, SIGNAL, QPushButton, released, ( ) );
    }

    QOBJECT_CONNECT( m_buttons[SELECT], SIGNAL, QPushButton, released, ( ), m_parent, SLOT, FCurveEditor, setModeSelect, ( ) );
    QOBJECT_CONNECT( m_buttons[ADD], SIGNAL, QPushButton, released, ( ), m_parent, SLOT, FCurveEditor, setModeAdd, ( ) );
    QOBJECT_CONNECT( m_buttons[REMOVE], SIGNAL, QPushButton, released, ( ), m_parent, SLOT, FCurveEditor, setModeRemove, ( ) );

    m_layout->addWidget( new QSplitter( Qt::Vertical ) );

    m_snapToCurveButton = new QPushButton();
    m_snapToCurveButton->setObjectName( "SnapToCurve" );
    m_snapToCurveButton->setFixedSize( QSize( buttonSize, buttonSize ) );
    m_snapToCurveButton->setCheckable( true );
    m_snapToCurveButton->setToolTip( "Snap to Curve [Press (" + QKeySequence( ModeToggleModifier ).toString() + ")"
      + QKeySequence( SnapToCurveKey ).toString() + "]" );
    m_snapToCurveAction = new QAction( "Snap to Curve", m_parent );
    m_snapToCurveAction->setCheckable( true );
    m_snapToCurveAction->setShortcut( QKeySequence( ModeToggleModifier ).toString() + QKeySequence( SnapToCurveKey ).toString() );
    m_snapToCurveAction->setShortcutContext( Qt::WidgetWithChildrenShortcut );
    m_parent->addAction( m_snapToCurveAction );
    QOBJECT_CONNECT( m_snapToCurveAction, SIGNAL, QAction, triggered, ( bool ), m_snapToCurveButton, SLOT, QPushButton, toggle, ( ) );
    QOBJECT_CONNECT( m_snapToCurveButton, SIGNAL, QPushButton, toggled, ( bool ), m_parent, SLOT, FCurveEditor, setSnapToCurveFromButton, ( ) );
    m_layout->addWidget( m_snapToCurveButton );

    this->setLayout( m_layout );
  }

  void setMode( Mode m )
  {
    for( int i = 0; i < MODE_COUNT; i++ )
    {
      m_buttons[i]->setChecked( i == m );
      m_modeActions[i]->setChecked( i == m );
      m_modeActions[i]->setEnabled( i != m );
    }
    if( m == m_previousMode )
      m_previousMode = MODE_COUNT;
    m_snapToCurveButton->setVisible( m == ADD );
    m_snapToCurveAction->setEnabled( m == ADD );
  }

  inline void setTemporaryMode( Mode m )
  {
    Mode previousMode = m_parent->m_scene->mode();
    if( m != previousMode )
    {
      if( !inTemporaryMode() )
        m_previousMode = previousMode;
      m_parent->m_scene->setMode( m );
    }
  }
  inline bool inTemporaryMode() const { return m_previousMode < MODE_COUNT; }
  inline Mode getPreviousMode() const { assert( inTemporaryMode() ); return m_previousMode; }
};

FCurveEditor::FCurveEditor()
  : m_rview( new RuledGraphicsView() )
  , m_owningScene( true )
  , m_scene( NULL )
  , m_toolBar( new ToolBar( this ) )
{
  this->setObjectName( "FCurveEditor" );
  this->setContextMenuPolicy(Qt::CustomContextMenu);

  QVBoxLayout* m_layout = new QVBoxLayout();
  m_layout->setMargin( 0 ); m_layout->setSpacing( 0 );
  m_layout->addWidget( m_toolBar );
  m_layout->addWidget( m_rview );
  this->setLayout( m_layout );
  m_keyValueEditor = new KeyValueEditor( this );

  QOBJECT_CONNECT(
    this, SIGNAL, FCurveEditor, customContextMenuRequested, ( const QPoint& ),
    this, SLOT, FCurveEditor, showContextMenu, ( const QPoint& ) );

  QOBJECT_CONNECT(
    m_rview, SIGNAL, RuledGraphicsView, rectangleSelectReleased, ( const QRectF&, Qt::KeyboardModifiers ),
    this, SLOT, FCurveEditor, onRectangleSelectReleased, ( const QRectF&, Qt::KeyboardModifiers )
  );

#define DEFINE_FCE_ACTION_NOSHORTCUT( member, strName, slot ) \
  member = new QAction( strName, this ); \
  QOBJECT_CONNECT( member, SIGNAL, QAction, triggered, (), this, SLOT, FCurveEditor, slot, () ); \
  this->addAction( member );

#define DEFINE_FCE_ACTION( member, strName, slot, shortcut ) \
  DEFINE_FCE_ACTION_NOSHORTCUT( member, strName, slot ) \
  member->setShortcutContext( Qt::WidgetWithChildrenShortcut ); \
  member->setShortcut( shortcut );

  DEFINE_FCE_ACTION( m_keysSelectAllAction, "Select all keys", onSelectAllKeys, Qt::CTRL + Qt::Key_A )
  DEFINE_FCE_ACTION( m_keysDeselectAllAction, "Deselect all keys", onDeselectAllKeys, Qt::CTRL + Qt::SHIFT + Qt::Key_A )
  DEFINE_FCE_ACTION( m_keysFrameAllAction, "Frame All Keys", onFrameAllKeys, Qt::Key_A )
  DEFINE_FCE_ACTION( m_keysFrameSelectedAction, "Frame Selected Keys", onFrameSelectedKeys, Qt::Key_F )
  DEFINE_FCE_ACTION( m_keysDeleteAction, "Delete selected Keys", onDeleteSelectedKeys, Qt::Key_Delete )
  DEFINE_FCE_ACTION_NOSHORTCUT( m_autoTangentsAction, "Auto Tangents", onAutoTangents )
  DEFINE_FCE_ACTION_NOSHORTCUT( m_tangentsZeroSlopeAction, "Zero-slope Tangents", onTangentsZeroSlope )
  DEFINE_FCE_ACTION_NOSHORTCUT( m_presetRampIn, "Ramp In", onPresetRampIn )
  DEFINE_FCE_ACTION_NOSHORTCUT( m_presetRampOut, "Ramp Out", onPresetRampOut )
  DEFINE_FCE_ACTION_NOSHORTCUT( m_presetSmoothStep, "Smooth Step", onPresetSmoothStep )
  DEFINE_FCE_ACTION_NOSHORTCUT( m_clearAction, "Clear all keys", onClearAllKeys )
}

void FCurveEditor::resizeEvent( QResizeEvent * e )
{
  Parent::resizeEvent( e );
  this->updateVEPos();
}

void FCurveEditor::updateVEPos()
{
  if( !this->toolBarEnabled() )
  {
    m_keyValueEditor->setGeometry( QRect(
      ( m_vePos.x() < 0 ?
        this->rect().right() + m_vePos.x() - m_keyValueEditor->width() :
        this->rect().left() + m_vePos.x()
        ),
        ( m_vePos.y() < 0 ?
          this->rect().bottom() + m_vePos.y() - m_keyValueEditor->height() :
          this->rect().top() + m_vePos.y()
          ),
      m_keyValueEditor->width(),
      m_keyValueEditor->height()
    ) );
  }
}

void FCurveEditor::setToolBarEnabled( bool enabled )
{
  if( this->toolBarEnabled() != enabled )
  {
    if( enabled )
    {
      m_toolBar->setVisible( true );
      m_toolBar->layout()->addWidget( m_keyValueEditor );
      m_toolBar->layout()->setAlignment( m_keyValueEditor, Qt::AlignRight );
    }
    else
    {
      m_toolBar->setVisible( false );
      m_keyValueEditor->setParent( this );
      this->updateVEPos();
    }
    m_toolbarEnabled = enabled;
  }
}

void FCurveEditor::onModeChanged()
{
  Mode m = m_scene->mode();
  assert( m < MODE_COUNT );
  m_toolBar->setMode( m );
  m_rview->enableRectangleSelection( m != ADD );
}

void FCurveEditor::onSnapToCurveChanged()
{
  m_toolBar->m_snapToCurveAction->setChecked( m_scene->snapToCurve() );
  m_toolBar->m_snapToCurveButton->setChecked( m_scene->snapToCurve() );
}

void FCurveEditor::setSnapToCurveFromButton()
{
  m_scene->setSnapToCurve( m_toolBar->m_snapToCurveButton->isChecked() );
}

void FCurveEditor::onInfinityTypesChanged()
{
  AbstractFCurveModel* curve = m_scene->curveItem()->curve();
  for( size_t t = 0; t < curve->infinityTypeCount(); t++ )
  {
    m_toolBar->m_infTypes[SetInfinityTypeAction::PRE][t]->setValue( t == curve->getPreInfinityType() );
    m_toolBar->m_infTypes[SetInfinityTypeAction::POST][t]->setValue( t == curve->getPostInfinityType() );
  }
}

void FCurveEditor::onSelectionChanged()
{
  const bool empty = m_scene->curveItem()->selectedKeys().empty();
  m_keysDeselectAllAction->setEnabled( !empty );
  m_keysFrameSelectedAction->setEnabled( !empty );
  m_keysDeleteAction->setEnabled( !empty );
  m_keysSelectAllAction->setEnabled( m_scene->curveItem()->selectedKeys().size() < m_scene->curveItem()->curve()->getKeyCount() );
  m_autoTangentsAction->setEnabled( !empty );
  m_tangentsZeroSlopeAction->setEnabled( !empty );

  if( empty )
    m_keyValueEditor->setVisible( false );
  else
  {
    if( m_scene->curveItem()->selectedKeys().size() == 1 )
    {
      this->onEditedKeysChanged();
    }
    else
    {
      m_keyValueEditor->m_tanType->setVisible( false );
      m_keyValueEditor->m_x->setVisible( false );
      m_keyValueEditor->m_x->clear();
      m_keyValueEditor->m_y->clear();
    }
    m_keyValueEditor->setVisible( true );
  }
}

void FCurveEditor::onEditedKeysChanged()
{
  if( m_scene->curveItem()->selectedKeys().size() == 1 )
  {
    Key h = m_scene->curveItem()->curve()->getKey( *m_scene->curveItem()->selectedKeys().begin() );
    QPointF p;
    switch( m_scene->curveItem()->editedKeyProp() )
    {
    case FCurveItem::POSITION: p = h.pos; break;
    case FCurveItem::TAN_IN: p = h.tanIn; break;
    case FCurveItem::TAN_OUT: p = h.tanOut; break;
    case FCurveItem::NOTHING: assert( false ); break;
    }
    m_keyValueEditor->m_x->set( p.x() );
    m_keyValueEditor->m_x->setVisible( true );
    m_keyValueEditor->m_y->set( p.y() );
    m_keyValueEditor->m_tanType->setModel( m_scene->curveItem()->curve() );
    if(
      m_scene->curveItem()->editedKeyProp() == FCurveItem::TAN_IN ||
      m_scene->curveItem()->editedKeyProp() == FCurveItem::TAN_OUT
      )
    {
      m_keyValueEditor->m_tanType->setVisible( true );
      m_keyValueEditor->m_tanType->setCurrentIndex(
        int( m_scene->curveItem()->editedKeyProp() == FCurveItem::TAN_IN ? h.tanInType : h.tanOutType ) );
    }
    else
      m_keyValueEditor->m_tanType->setVisible( false );
  }
}

void FCurveEditor::onRectangleSelectReleased( const QRectF& r, Qt::KeyboardModifiers m )
{
  m_scene->curveItem()->rectangleSelect( r, m );
}

void FCurveEditor::onFrameAllKeys()
{
  this->frameAllKeys();
}

void FCurveEditor::onFrameSelectedKeys()
{
  this->frameSelectedKeys();
}

void FCurveEditor::onDeleteSelectedKeys()
{
  m_scene->curveItem()->deleteSelectedKeys();
}

void FCurveEditor::onSelectAllKeys()
{
  m_scene->curveItem()->selectAllKeys();
}

void FCurveEditor::onDeselectAllKeys()
{
  m_scene->curveItem()->clearKeySelection();
}

void AddPreset( AbstractFCurveModel* model,
  QPointF inP,
  QPointF outP,
  QPointF inT,
  QPointF outT
)
{
  Key in, out;
  in.pos = inP;
  in.tanIn = QPointF( 1, 0 );
  in.tanOut = inT;
  in.tanInType = 0;
  in.tanOutType = 0;
  out.pos = outP;
  out.tanIn = outT;
  out.tanOut = QPointF( 1, 0 );
  out.tanInType = 0;
  out.tanOutType = 0;
  model->addKey( in );
  model->addKey( out );
}

void FCurveEditor::onPresetRampIn()
{
  this->onClearAllKeys();
  AddPreset( m_scene->curveItem()->curve(), QPointF( 0, 0 ), QPointF( 1, 1 ), QPointF( 1, 1 ), QPointF( 1, 1 ) );
}

void FCurveEditor::onPresetRampOut()
{
  this->onClearAllKeys();
  AddPreset( m_scene->curveItem()->curve(), QPointF( 0, 1 ), QPointF( 1, 0 ), QPointF( 1, -1 ), QPointF( 1, -1 ) );
}

void FCurveEditor::onPresetSmoothStep()
{
  this->onClearAllKeys();
  AddPreset( m_scene->curveItem()->curve(), QPointF( 0, 0 ), QPointF( 1, 1 ), QPointF( 1, 0 ), QPointF( 1, 0 ) );
}

void FCurveEditor::onClearAllKeys()
{
  this->onSelectAllKeys();
  this->onDeleteSelectedKeys();
}

void FCurveEditor::onAutoTangents()
{
  const std::set<size_t>& selected = m_scene->curveItem()->selectedKeys();
  for( std::set<size_t>::const_iterator it = selected.begin(); it != selected.end(); it++ )
    m_scene->curveItem()->curve()->autoTangent( *it );
}

void FCurveEditor::onTangentsZeroSlope()
{
  const std::set<size_t>& selected = m_scene->curveItem()->selectedKeys();
  for( std::set<size_t>::const_iterator it = selected.begin(); it != selected.end(); it++ )
  {
    Key key = m_scene->curveItem()->curve()->getKey( *it );
    key.tanIn.setY( 0 ); key.tanOut.setY( 0 );
    m_scene->curveItem()->curve()->setKey( *it, key );
  }
}

void FCurveEditor::deleteOwnedScene()
{
  if( m_scene != NULL && m_owningScene )
    delete m_scene;
}

FCurveEditor::~FCurveEditor()
{
  this->deleteOwnedScene();
}

void FCurveEditor::setModel( AbstractFCurveModel* model )
{
  this->deleteOwnedScene();
  m_scene = new FCurveEditorScene( model );
  m_owningScene = true;
  this->linkToScene();
}

void FCurveEditor::linkToScene()
{
  m_rview->view()->setScene( m_scene );

  AbstractFCurveModel* curve = m_scene->curveItem()->curve();

  QOBJECT_CONNECT( m_scene, SIGNAL, FCurveEditorScene, interactionBegin, ( ), this, SIGNAL, FCurveEditor, interactionBegin, ( ) );
  QOBJECT_CONNECT( m_scene, SIGNAL, FCurveEditorScene, interactionEnd, ( ), this, SIGNAL, FCurveEditor, interactionEnd, ( ) );
  QOBJECT_CONNECT( m_scene, SIGNAL, FCurveEditorScene, modeChanged, ( ), this, SLOT, FCurveEditor, onModeChanged, ( ) );
  QOBJECT_CONNECT( m_scene, SIGNAL, FCurveEditorScene, snapToCurveChanged, ( ), this, SLOT, FCurveEditor, onSnapToCurveChanged, ( ) );
  QOBJECT_CONNECT(
    m_scene->curveItem(), SIGNAL, FCurveItem, selectionChanged, ( ),
    this, SLOT, FCurveEditor, onSelectionChanged, ( )
  );
  QOBJECT_CONNECT(
    m_scene->curveItem(), SIGNAL, FCurveItem, editedKeyValueChanged, ( ),
    this, SLOT, FCurveEditor, onEditedKeysChanged, ( )
  );
  QOBJECT_CONNECT(
    m_scene->curveItem(), SIGNAL, FCurveItem, editedKeyPropChanged, ( ),
    this, SLOT, FCurveEditor, onEditedKeysChanged, ( )
  );
  QOBJECT_CONNECT(
    curve, SIGNAL, AbstractFCurveModel, infinityTypesChanged, ( ),
    this, SLOT, FCurveEditor, onInfinityTypesChanged, ( )
  );

  m_rview->fitInView( m_scene->curveItem()->keysBoundingRect() );

  for( size_t i = 0; i < SetInfinityTypeAction::DIRCOUNT; i++ )
  {
    std::vector<SetInfinityTypeAction*>* actions = &(m_toolBar->m_infTypes[i]);
    for( size_t j = 0; j < actions->size(); j++ )
      ( *actions )[j]->deleteLater();
    actions->clear();
    for( size_t j = 0; j < curve->infinityTypeCount(); j++ )
    {
      SetInfinityTypeAction* action = new SetInfinityTypeAction( this, SetInfinityTypeAction::Direction( i ), j );
      action->setCheckable( true );
      action->setText( curve->infinityTypeName( j ) );
      actions->push_back( action );
    }
  }
  this->onInfinityTypesChanged();

  this->setVEPos( QPoint( -20, 20 ) );
  this->setToolBarEnabled( true );
  this->onModeChanged();
  this->onSelectionChanged();
}

void FCurveEditor::deriveFrom( FCurveEditor* other )
{
  this->deleteOwnedScene();
  m_scene = other->m_scene;
  m_owningScene = false;
  this->linkToScene();
}

void FCurveEditor::frameAllKeys()
{
  const size_t kc = m_scene->curveItem()->curve()->getKeyCount();
  if( kc == 0 )
    m_rview->fitInView( QRectF( 0, 0, 1, 1 ) );
  else
  {
    if( kc == 1 )
      m_rview->view()->centerOn( m_scene->curveItem()->curve()->getKey( 0 ).pos );
    else
    {
      QRectF rect = m_scene->curveItem()->keysBoundingRect();
      assert( rect.isValid() );
      m_rview->fitInView( rect );
    }
  }
}

void FCurveEditor::frameSelectedKeys()
{
  if( !m_scene->curveItem()->selectedKeys().empty() )
  {
    if( m_scene->curveItem()->selectedKeys().size() == 1 )
      m_rview->view()->centerOn( m_scene->curveItem()->curve()->getKey( *m_scene->curveItem()->selectedKeys().begin() ).pos );
    else
    {
      QRectF rect = m_scene->curveItem()->selectedKeysBoundingRect();
      assert( rect.isValid() );
      m_rview->fitInView( rect );
    }
  }
}

void FCurveEditor::showContextMenu(const QPoint &pos)
{
  QMenu contextMenu("Context menu", this);
  
  for( int i = 0; i < MODE_COUNT; i++ )
    contextMenu.addAction( m_toolBar->m_modeActions[i] );
  contextMenu.addSeparator();

  // Keys Menu
  QMenu keysMenu( "Keys", this );

  contextMenu.addMenu(&keysMenu);
  keysMenu.addAction(this->m_keysSelectAllAction);
  keysMenu.addAction(this->m_keysDeselectAllAction);
  keysMenu.addAction(this->m_keysDeleteAction);
  keysMenu.addSeparator();
  keysMenu.addAction(this->m_keysFrameAllAction);
  keysMenu.addAction(this->m_keysFrameSelectedAction);
  keysMenu.addSeparator();
  keysMenu.addAction(this->m_autoTangentsAction);
  keysMenu.addAction(this->m_tangentsZeroSlopeAction);
  keysMenu.addAction( m_toolBar->m_snapToCurveAction );
  keysMenu.addSeparator();
  keysMenu.addAction( m_clearAction );

  // Curve Menu
  QMenu curveMenu( "Curve", this );
  contextMenu.addMenu( &curveMenu );

  // Presets Menu
  QMenu presetsMenu( "Presets", this );
  curveMenu.addMenu( &presetsMenu );
  presetsMenu.addAction( m_presetRampIn );
  presetsMenu.addAction( m_presetRampOut );
  presetsMenu.addAction( m_presetSmoothStep );

  // Infinity Types
  QMenu preInfMenu( "Pre Infinity", this );
  QMenu postInfMenu( "Post Infinity", this );
  {
    for( int i = 0; i < SetInfinityTypeAction::DIRCOUNT; i++ )
    {
      const std::vector<SetInfinityTypeAction*>* actions = &(m_toolBar->m_infTypes[i]);
      QMenu* menu = NULL;
      switch( i )
      {
      case 0: menu = &preInfMenu; break;
      case 1: menu = &postInfMenu; break;
      }
      for( size_t j = 0; j < actions->size(); j++ )
        menu->addAction( (*actions)[j] );
      curveMenu.addMenu( menu );
    }
  }

  contextMenu.addSeparator();

  contextMenu.exec(this->mapToGlobal(pos));
}

void FCurveEditor::keyPressEvent( QKeyEvent * e )
{
  for( int m = 0; m < MODE_COUNT; m++ )
    if( e->key() == ModeKeys[m] )
      m_toolBar->setTemporaryMode( Mode(m) );
  Parent::keyPressEvent( e );
}

void FCurveEditor::keyReleaseEvent( QKeyEvent * e )
{
  if( e->key() == ModeKeys[m_scene->mode()] )
    if( m_toolBar->inTemporaryMode() )
      m_scene->setMode( m_toolBar->getPreviousMode() );
  Parent::keyReleaseEvent( e );
}

AbstractFCurveModel* FCurveEditor::model() { return m_scene->curveItem()->curve(); }

void FCurveEditor::setModeSelect() { m_scene->setMode( SELECT ); }
void FCurveEditor::setModeAdd() { m_scene->setMode( ADD ); }
void FCurveEditor::setModeRemove() { m_scene->setMode( REMOVE ); }
