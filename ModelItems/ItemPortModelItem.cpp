//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/ItemPortItemMetadata.h>
#include <FabricUI/ModelItems/ItemPortModelItem.h>
#include <FabricUI/ModelItems/RootModelItem.h>
#include <QStringList>
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricApplicationStates.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
ItemPortModelItem::ItemPortModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef itemPath,
  FTL::StrRef portName
  )
  : m_dfgUICmdHandler( dfgUICmdHandler )
  , m_binding( binding )
  , m_execPath( execPath )
  , m_exec( exec )
  , m_itemPath( itemPath )
  , m_portName( portName )
  , m_metadata( NULL )
{
  updatePortPath();
}

ItemPortModelItem::~ItemPortModelItem()
{
  delete m_metadata;
}

void ItemPortModelItem::updatePortPath()
{
  m_portPath = m_itemPath;
  m_portPath += '.';
  m_portPath += m_portName;
  if( m_metadata != NULL )
  {
    m_metadata->computeDFGPath();
    emit this->metadataChanged();
  }
}

FabricUI::ValueEditor::ItemMetadata* ItemPortModelItem::getMetadata()
{
  if ( !m_metadata )
    m_metadata = new ItemPortItemMetadata( this );

  return m_metadata;
}

void ItemPortModelItem::setMetadataImp( const char* key, const char* value, bool canUndo ) /**/
{
  m_exec.setPortMetadata( m_portPath.c_str(), key, value, canUndo );
}

QVariant ItemPortModelItem::getValue()
{
  try
  {
    // TODO: Find a way to show values of connected ports
    if ( m_exec.hasSrcPorts( m_portPath.c_str() ) )
      return QVariant();

    // If we have a resolved type, allow getting the default val
    const char* ctype = m_exec.getPortResolvedType( m_portPath.c_str() );
    if (ctype != NULL)
    {
      FabricCore::RTVal rtVal = 
        m_exec.getPortResolvedDefaultValue( m_portPath.c_str(), ctype );
      assert( rtVal.isValid() );
      return QVariant::fromValue<FabricCore::RTVal>( rtVal.copy() );
    }
  }
  catch ( FabricCore::Exception e )
  {
    reportFabricCoreException( e );
  }
  return QVariant();
}

void ItemPortModelItem::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  try
  {
    if ( m_exec.hasSrcPorts( m_portPath.c_str() ) )
      return;

    const char* resolvedTypeName =
      m_exec.getPortResolvedType( m_portPath.c_str() );
    assert( resolvedTypeName );
    if ( !resolvedTypeName )
      return;

    FabricCore::DFGHost host = m_binding.getHost();
    FabricCore::Context context = host.getContext();
    FabricCore::RTVal rtVal =
      FabricCore::RTVal::Construct( context, resolvedTypeName, 0, 0 );

    if ( commit )
    {
      if ( valueAtInteractionBegin.isValid() )
      {
        m_binding.suspendDirtyNotifs();

        FabricUI::ValueEditor::RTVariant::toRTVal( valueAtInteractionBegin, rtVal );
        m_exec.setPortDefaultValue(
          m_portPath.c_str(),
          rtVal,
          false // canUndo
          );

        FabricUI::ValueEditor::RTVariant::toRTVal( value, rtVal );
        m_dfgUICmdHandler->dfgDoSetPortDefaultValue(
          m_binding,
          QString::fromUtf8( m_execPath.data(), m_execPath.size() ),
          m_exec,
          QString::fromUtf8( m_portPath.data(), m_portPath.size() ),
          rtVal
          );

        m_binding.resumeDirtyNotifs();
      }
      else
      {
        FabricCore::DFGNotifBracket _( host );
        FabricUI::ValueEditor::RTVariant::toRTVal( value, rtVal );
        m_dfgUICmdHandler->dfgDoSetPortDefaultValue(
          m_binding,
          QString::fromUtf8( m_execPath.data(), m_execPath.size() ),
          m_exec,
          QString::fromUtf8( m_portPath.data(), m_portPath.size() ),
          rtVal
          );
      }
    }
    else
    {
      FabricCore::DFGNotifBracket _( host );
      FabricUI::ValueEditor::RTVariant::toRTVal( value, rtVal );
      m_exec.setPortDefaultValue(
        m_portPath.c_str(),
        rtVal,
        false // canUndo
        );
    }
  }
  catch ( FabricCore::Exception e )
  {
    reportFabricCoreException( e );
  }
}

QString ItemPortModelItem::getCommandName() 
{
  return "setPortDefaultValue";
}

FabricCore::RTVal ItemPortModelItem::getCommandArgs() 
{
  FabricCore::RTVal cmdArgs;

  try
  {    
    FabricCore::Client client =  Application::FabricApplicationStates::GetAppStates()->getClient();
  
    cmdArgs = FabricCore::RTVal::ConstructDict(
      client, 
      "String",
      "RTVal"
    );

    cmdArgs.setDictElement(
      FabricCore::RTVal::ConstructString(
        client, 
        "execPath"
        ), 
      Util::RTValUtil::toKLRTVal(
        FabricCore::RTVal::ConstructString(
          client, 
          getExecPath().c_str()
          )
        )
      );

    cmdArgs.setDictElement(
      FabricCore::RTVal::ConstructString(
        client, 
        "nodeName"
        ), 
      Util::RTValUtil::toKLRTVal(
        FabricCore::RTVal::ConstructString(
          client, 
          getItemPath().c_str()
          )
        )
      );

    cmdArgs.setDictElement(
      FabricCore::RTVal::ConstructString(
        client, 
        "portName"
        ), 
      Util::RTValUtil::toKLRTVal(
        FabricCore::RTVal::ConstructString(
          client, 
          getPortName().c_str()
          )
        )
      );
  }
  
  catch(FabricCore::Exception &e)
  {
    printf(
      "ItemPortModelItem::getCommandArgs: exception: %s\n", 
      e.getDesc_cstr());
  }

  return cmdArgs;
}

void ItemPortModelItem::onItemRenamed(
  FTL::CStrRef oldItemPath,
  FTL::CStrRef newItemPath
  )
{
  assert( m_itemPath == oldItemPath );

  m_itemPath = newItemPath;

  updatePortPath();
}

bool ItemPortModelItem::hasDefault()
{
  // Disabled the "resetToDefault" feature, because it currently
  // doesn't work : DFGExec::getPortDefaultValue returns the same
  // value that we modify in ItemPortModelItem::setValue
  // TODO : re-enable
  return false;

  // If we have a resolved type, allow getting the default val
  //const char* ctype = m_exec.getPortResolvedType(m_portPath.c_str());
  //return (ctype != NULL);
}

void ItemPortModelItem::resetToDefault()
{
  //#pragma message("Fix instance values for non-arg ports")
  //// If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getPortResolvedType(m_portPath.c_str());
  if (ctype != NULL)
  {
    // TODO : get the original (value when we opened the graph), instead
    FabricCore::RTVal val =
      m_exec.getPortDefaultValue(m_portPath.c_str(), ctype);
    if (val.isValid())
      onViewValueChanged(QVariant::fromValue(val));
  }
}

void ItemPortModelItem::reportFabricCoreException( FabricCore::Exception const &e )
{
  printf( "[ERROR] %s\n", e.getDesc_cstr() );
}

} // namespace ModelItems
} // namespace FabricUI
