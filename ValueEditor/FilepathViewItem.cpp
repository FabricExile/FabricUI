//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "FilepathViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"
#include "VELineEdit.h"

#include <QVariant>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>

using namespace FabricUI::ValueEditor;

inline QString ToQString( const QVariant& var );
FabricCore::RTVal ToFilePath( FabricCore::RTVal& val, const QString& text );

FilepathViewItem::FilepathViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
  , m_val(value.value<FabricCore::RTVal>())
  , m_isOpenFile(true)
{
  m_edit = new VELineEdit;
  m_edit->setObjectName( "VELeft" );
  m_edit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

  QPushButton* browseButton = new QPushButton;
  browseButton->setObjectName( "VERight" );
  browseButton->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );

  onModelValueChanged( value );

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_edit );
  layout->addWidget( browseButton );

  m_widget = new QWidget;
  m_widget->setObjectName( "VEFilepathViewItem" );
  m_widget->setLayout( layout );

  connect(
    m_edit, SIGNAL( textModified( QString ) ),
    this, SLOT( onTextModified( QString ) )
    );
  connect( 
    browseButton, SIGNAL( clicked() ), 
    this, SLOT( doBrowse() ) );

  metadataChanged();
}

FilepathViewItem::~FilepathViewItem()
{
}

QWidget *FilepathViewItem::getWidget()
{
  return m_widget;
}

void FilepathViewItem::metadataChanged()
{
  if (m_metadata.has( "uiFileTypeFilter" ))
    m_filter = m_metadata.getString( "uiFileTypeFilter" );
  else
    m_filter = QString();

  if (m_metadata.has( "uiIsOpenFile" ))
  {
    QString uiIsOpenFile = m_metadata.getString( "uiIsOpenFile" );
    m_isOpenFile = (uiIsOpenFile == "true");
  }
  else
    m_isOpenFile = true;
}

void FilepathViewItem::onModelValueChanged( QVariant const &v )
{
  m_edit->setText( ToQString(v) );
}

QString FilepathViewItem::m_lastFolder = QString();

void FilepathViewItem::doBrowse()
{
  QString fileName;
  if(m_isOpenFile)
  {
    fileName = QFileDialog::getOpenFileName( m_widget,
                                           tr( "Open File" ), 
                                           m_lastFolder, 
                                           m_filter);
  }
  else
  {
    fileName = QFileDialog::getSaveFileName( m_widget,
                                           tr( "Save File" ), 
                                           m_lastFolder, 
                                           m_filter);
  }
  if (!fileName.isEmpty())
  {
    m_lastFolder = fileName;
    // Update the string widget
    m_edit->setText( fileName );
    // Update the model
    onTextModified( fileName );
  }
}

void FilepathViewItem::onTextModified( QString text )
{
  m_val = ToFilePath( m_val, text );
  emit viewValueChanged(
    QVariant::fromValue<FabricCore::RTVal>( m_val )
    );
}

//////////////////////////////////////////////////////////////////////////
//

inline QString ToQString( const QVariant& var )
{
  if (var.isNull())
    return "";
  FabricCore::RTVal val = var.value<FabricCore::RTVal>();
  const FabricCore::RTVal pathStr = val.callMethod( "String", "string", 0, NULL );
  const char* asCStr = pathStr.getStringCString();
  return asCStr;
}

FabricCore::RTVal ToFilePath( FabricCore::RTVal& val, const QString& text )
{
  FabricCore::RTVal asString = FabricCore::RTVal::ConstructString(
    val.getContext(),
    text.toUtf8().constData() );

  return FabricCore::RTVal::Create(
    val.getContext(),
    "FilePath",
    1,
    &asString );
}


//////////////////////////////////////////////////////////////////////////
// 

BaseViewItem *FilepathViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (isRTVal(value))
  {
    const FabricCore::RTVal& val = value.value<FabricCore::RTVal>( );
    if (val.isValid())
    {
      const char* rtype = val.getTypeNameCStr();
      if (strcmp( rtype, "FilePath" ) == 0)
        return new FilepathViewItem( name, value, metaData );
    }
  }
  return NULL;
}

const int FilepathViewItem::Priority = 3;
