//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "QVariantRTVal.h"
#include "Vec4ViewItem.h"
#include "ViewItemFactory.h"
#include "VEDoubleSpinBox.h"

#include <assert.h>
#include <FabricUI/Util/UIRange.h>
#include <QVariant>
#include <QBoxLayout>
#include <QLineEdit>
#include <QFrame>

using namespace FabricUI::ValueEditor;

Vec4ViewItem::Vec4ViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseComplexViewItem( name, metadata )
  , m_vec4dValue( getQVariantRTValValue<QVector4D>( value ) )
{
  m_widget = new QFrame;
  m_widget->setObjectName( "Vec4Item" );

  m_xSpinBox = new VEDoubleSpinBox;
  m_xSpinBox->setObjectName( "VELeft" );
  m_xSpinBox->setValue( m_vec4dValue.x() );
  m_ySpinBox = new VEDoubleSpinBox;
  m_ySpinBox->setObjectName( "VEMiddle" );
  m_ySpinBox->setValue( m_vec4dValue.y() );
  m_zSpinBox = new VEDoubleSpinBox;
  m_zSpinBox->setObjectName( "VEMiddle" );
  m_zSpinBox->setValue( m_vec4dValue.z() );
  m_tSpinBox = new VEDoubleSpinBox;
  m_tSpinBox->setObjectName( "VERight" );
  m_tSpinBox->setValue( m_vec4dValue.w() );
  
  connect(
    m_xSpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_xSpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_xSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onXSpinBoxValueChanged(double))
    );
  
  connect(
    m_ySpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_ySpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_ySpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onYSpinBoxValueChanged(double))
    );
  
  connect(
    m_zSpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_zSpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_zSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onZSpinBoxValueChanged(double))
    );
  
  connect(
    m_tSpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_tSpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_tSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onTSpinBoxValueChanged(double))
    );

  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_xSpinBox );
  layout->addWidget( m_ySpinBox );
  layout->addWidget( m_zSpinBox );
  layout->addWidget( m_tSpinBox );
  layout->addStretch();

  metadataChanged();
}

Vec4ViewItem::~Vec4ViewItem()
{
}

QWidget *Vec4ViewItem::getWidget()
{
  return m_widget;
}

void Vec4ViewItem::onModelValueChanged( QVariant const &value )
{
  QVector4D vec4d = m_vec4dValue;
  m_vec4dValue = getQVariantRTValValue<QVector4D>( value );

  if ( vec4d.x() != m_vec4dValue.x() )
  {
    m_xSpinBox->setValue( m_vec4dValue.x() );
    routeModelValueChanged( 0, QVariant( m_vec4dValue.x() ) );
  }
  if ( vec4d.y() != m_vec4dValue.y() )
  {
    m_ySpinBox->setValue( m_vec4dValue.y() );
    routeModelValueChanged( 1, QVariant( m_vec4dValue.y() ) );
  }
  if ( vec4d.z() != m_vec4dValue.z() )
  {
    m_zSpinBox->setValue( m_vec4dValue.z() );
    routeModelValueChanged( 2, QVariant( m_vec4dValue.z() ) );
  }
  if ( vec4d.w() != m_vec4dValue.w() )
  {
    m_tSpinBox->setValue( m_vec4dValue.w() );
    routeModelValueChanged( 3, QVariant( m_vec4dValue.w() ) );
  }
}

void Vec4ViewItem::onXSpinBoxValueChanged( double value )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setX( value );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onYSpinBoxValueChanged( double value )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setY( value );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onZSpinBoxValueChanged( double value )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setZ( value );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onTSpinBoxValueChanged( double value )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setW( value );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onChildViewValueChanged(
  int index,
  QVariant value
  )
{
  QVector4D vec4d = m_vec4dValue;
  switch ( index )
  {
    case 0:
      vec4d.setX( value.toDouble() );
      break;
    case 1:
      vec4d.setY( value.toDouble() );
      break;
    case 2:
      vec4d.setZ( value.toDouble() );
      break;
    case 3:
      vec4d.setW( value.toDouble() );
      break;
    default:
      assert( false );
      break;
  }
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::doAppendChildViewItems(QList<BaseViewItem *>& items)
{
  ViewItemFactory* factory = ViewItemFactory::GetInstance();

  BaseViewItem *children[4];
  children[0] = factory->createViewItem( "X", QVariant( m_vec4dValue.x() ), &m_metadata );
  children[1] = factory->createViewItem( "Y", QVariant( m_vec4dValue.y() ), &m_metadata );
  children[2] = factory->createViewItem( "Z", QVariant( m_vec4dValue.z() ), &m_metadata );
  children[3] = factory->createViewItem( "T", QVariant( m_vec4dValue.w() ), &m_metadata );
  for ( int i = 0; i < 4; ++i )
  {
    connectChild( i, children[i] );
    items.append( children[i] );
  }
}

void Vec4ViewItem::metadataChanged()
{
  FTL::StrRef uiRangeString = m_metadata.getString( "uiHardRange" );
  
  double minValue, maxValue;
  if ( FabricUI::DecodeUIRange( uiRangeString, minValue, maxValue ) )
  {
    m_xSpinBox->setRange( minValue, maxValue );
    m_ySpinBox->setRange( minValue, maxValue );
    m_zSpinBox->setRange( minValue, maxValue );
    m_tSpinBox->setRange( minValue, maxValue );
  }
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* Vec4ViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
{
  if ( RTVariant::isType<QVector4D>( value ) )
    return new Vec4ViewItem( name, value, metadata );
  else
    return 0;
}

const int Vec4ViewItem::Priority = 3;
