//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValFCurveViewItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FabricUI/FCurveEditor/Models/DFG/DFGAnimXFCurveModel.h>
#include <FabricUI/ValueEditor/ItemMetadata.h>
#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ModelItems/DFGModelItemMetadata.h>
#include <FabricUI/Util/QtSignalsSlots.h>
#include <FabricUI/Util/LoadPixmap.h>

#include <FabricUI/Commands/CommandManager.h>
#include <FabricUI/Commands/KLCommandRegistry.h> // HACK: remove

#include <QPushButton>
#include <QDialog>
#include <QLayout>

#include <assert.h>
#include <QDebug>

using namespace FabricUI::ValueEditor;
using namespace FabricUI::FCurveEditor;

class RTValFCurveViewItem::Editor : public FabricUI::FCurveEditor::FCurveEditor
{
  typedef FabricUI::FCurveEditor::FCurveEditor Parent;
  RTValFCurveViewItem* m_parent;
  QPushButton* m_expandButton;
  void updateExpandButtonSize()
  {
    QRect cr = this->contentsRect();
    m_expandButton->setGeometry( QRect(
      cr.right() - 8 - m_expandButton->width(),
      cr.top() + 8,
      m_expandButton->width(),
      m_expandButton->height()
    ) );
    this->setVEPos( QPoint( -8 - m_expandButton->width() - 8, 8 ) );
  }
public:

  Editor( RTValFCurveViewItem* parent )
    : m_parent( parent )
    , m_expandButton( new QPushButton( this ) )
  {
    m_expandButton->setObjectName( "expand" );
    m_expandButton->resize( 32, 32 );
    QOBJECT_CONNECT( m_expandButton, SIGNAL, QPushButton, released, ( ), m_parent, SLOT, RTValFCurveViewItem, expand, ( ) );
    this->updateExpandButtonSize();
  }

  void resizeEvent( QResizeEvent * e ) FTL_OVERRIDE
  {
    Parent::resizeEvent( e );
    this->updateExpandButtonSize();
    this->frameAllKeys();
  }

  ~Editor() { delete this->model(); }
};

class RTValFCurveViewItem::ExpandedDialog : public QWidget
{
  FabricUI::FCurveEditor::FCurveEditor* m_editor;
public:
  ExpandedDialog( Editor* parent )
    : QWidget( parent, Qt::Window )
    , m_editor( new FabricUI::FCurveEditor::FCurveEditor() )
  {
    this->setObjectName( "ExpandedDialog" );

    QHBoxLayout* m_lay = new QHBoxLayout();
    m_lay->setMargin( 0 ); m_lay->setSpacing( 0 );
    m_lay->addWidget( m_editor );
    this->setLayout( m_lay );
  }
  inline FabricUI::FCurveEditor::FCurveEditor* editor() { return m_editor; }
};

void SetPath( RTValAnimXFCurveDFGController* model, const ItemMetadata* metadata )
{
  // const char* bindingId = metadata->getString( FabricUI::ModelItems::DFGModelItemMetadata::VEDFGBindingIdKey.data() );
  // const char* portPath = metadata->getString( FabricUI::ModelItems::DFGModelItemMetadata::VEDFGPortPathKey.data() );
  //model->setPath( bindingId, portPath );

  const char* path = metadata->getString( FabricUI::ValueEditor::ItemMetadata::VEPathKey.data() );
  model->setPath( path );
}

RTValFCurveViewItem::RTValFCurveViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
) : BaseViewItem( name, metadata )
  , m_model( new FabricUI::FCurveEditor::RTValAnimXFCurveDFGController() )
  , m_editor( new Editor( this ) )
  , m_expandedDialog( new ExpandedDialog( m_editor ) )
{
  m_editor->setModel( m_model );

  //m_expandedDialog->editor()->setModel( m_model ); // 2 different scenes
  m_expandedDialog->editor()->deriveFrom( m_editor ); // sharing the same scene

  this->onModelValueChanged( value );
  m_editor->setToolBarEnabled( false );
  m_expandedDialog->editor()->setToolBarEnabled( true );

  m_editor->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed ) );
  m_editor->setFixedHeight( 175 );
  m_editor->frameAllKeys( );

  //connect( m_model, SIGNAL( keyAdded() ), this, SLOT( onViewValueChanged() ) );
  //connect( m_model, SIGNAL( keyMoved( size_t ) ), this, SLOT( onViewValueChanged() ) );
  //connect( m_editor, SIGNAL( interactionBegin() ), this, SIGNAL( interactionBegin() ) );
  //connect( m_editor, SIGNAL( interactionEnd() ), this, SLOT( emitInteractionEnd() ) );

  for( size_t i = 0; i < 2; i++ )
  {
    FabricUI::FCurveEditor::FCurveEditor* editor =
      ( i == 0 ? m_editor : m_expandedDialog->editor() );
    QOBJECT_CONNECT(
      editor, SIGNAL, FabricUI::FCurveEditor::FCurveEditor, interactionBegin, ( ),
      m_model, SLOT, FabricUI::FCurveEditor::RTValAnimXFCurveDFGController, onInteractionBegin, ( )
    );
    QOBJECT_CONNECT(
      editor, SIGNAL, FabricUI::FCurveEditor::FCurveEditor, interactionEnd, ( ),
      m_model, SLOT, FabricUI::FCurveEditor::RTValAnimXFCurveDFGController, onInteractionEnd, ( )
    );
  }

  //const char* portPath = metadata->getString( FabricUI::ModelItems::DFGModelItemMetadata::VEDFGPortPathKey.data() );
  SetPath( m_model, metadata );
  const char* portPath = metadata->getString( FabricUI::ValueEditor::ItemMetadata::VEPathKey.data() );

  m_expandedDialog->setWindowTitle( "AnimX::AnimCurve <" + QString::fromUtf8( portPath ) + ">" );
}

void RTValFCurveViewItem::expand()
{
  m_expandedDialog->show();
  m_expandedDialog->editor()->frameAllKeys( );

  // HACK : sometimes the layout of the widgets in the ToolBar is
  // wrong (they are on top of each other). Calling this should refresh it.
  m_expandedDialog->editor()->setToolBarEnabled( false );
  m_expandedDialog->editor()->setToolBarEnabled( true );
}

RTValFCurveViewItem::~RTValFCurveViewItem()
{
}

void RTValFCurveViewItem::onViewValueChanged()
{
  //emit this->viewValueChanged( QVariant::fromValue<FabricCore::RTVal>( m_model->value() ) );
}

void RTValFCurveViewItem::setBaseModelItem( BaseModelItem* item )
{
  Parent::setBaseModelItem( item );
  if( this->getModelItem() != NULL )
  {
    // TODO : disconnect from previous model item (but BaseModelItem doesn't seem
    // to do it, so maybe we shouldn't)
    QOBJECT_CONNECT(
      this->getModelItem(), SIGNAL, BaseModelItem, metadataChanged, ( ),
      this, SLOT, RTValFCurveViewItem, onMetadataChanged, ( )
    );
  }
}

void RTValFCurveViewItem::onMetadataChanged()
{
  SetPath( m_model, &m_metadata );
}

QWidget* RTValFCurveViewItem::getWidget()
{
  return m_editor;
}

void RTValFCurveViewItem::onModelValueChanged( QVariant const & v )
{
  FabricCore::RTVal rtval = v.value<FabricCore::RTVal>();
  m_model->setValue( rtval );
  m_model->update();
  m_model->emitDirty();
}

BaseViewItem * RTValFCurveViewItem::CreateItem(
  QString const& name,
  QVariant const& value,
  ItemMetadata* metaData
)
{
  if( isRTVal( value ) )
  {
    const FabricCore::RTVal& val = value.value<FabricCore::RTVal>();
    if( val.isValid() )
    {
      const char* rtype = val.getTypeNameCStr();
      if( strcmp( rtype, "AnimX::AnimCurve" ) == 0 )
        return new RTValFCurveViewItem( name, value, metaData );
    }
  }
  return NULL;
}

const int RTValFCurveViewItem::Priority = 3;
