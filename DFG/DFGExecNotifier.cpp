//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGExecNotifier.h"
#include <FTL/JSONValue.h>
#include <stdio.h>

namespace FabricUI {
namespace DFG {

DFGExecNotifier::HandlerMap const &DFGExecNotifier::GetHandlerMap()
{
  static HandlerMap handlerMap;
  if ( handlerMap.empty() )
  {
    handlerMap[FTL_STR("execBlockInserted")] = &DFGExecNotifier::handler_execBlockInserted;
    handlerMap[FTL_STR("execBlockMetadataChanged")] = &DFGExecNotifier::handler_execBlockMetadataChanged;
    handlerMap[FTL_STR("execBlockPortDefaultValuesChanged")] = &DFGExecNotifier::handler_execBlockPortDefaultValuesChanged;
    handlerMap[FTL_STR("execBlockPortInserted")] = &DFGExecNotifier::handler_execBlockPortInserted;
    handlerMap[FTL_STR("execBlockPortOutsidePortTypeChanged")] = &DFGExecNotifier::handler_execBlockPortOutsidePortTypeChanged;
    handlerMap[FTL_STR("execBlockPortRemoved")] = &DFGExecNotifier::handler_execBlockPortRemoved;
    handlerMap[FTL_STR("execBlockPortRenamed")] = &DFGExecNotifier::handler_execBlockPortRenamed;
    handlerMap[FTL_STR("execBlockPortResolvedTypeChanged")] = &DFGExecNotifier::handler_execBlockPortResolvedTypeChanged;
    handlerMap[FTL_STR("execBlockPortsReordered")] = &DFGExecNotifier::handler_execBlockPortsReordered;
    handlerMap[FTL_STR("execBlockPortTypeSpecChanged")] = &DFGExecNotifier::handler_execBlockPortTypeSpecChanged;
    handlerMap[FTL_STR("execBlockRemoved")] = &DFGExecNotifier::handler_execBlockRemoved;
    handlerMap[FTL_STR("execBlockRenamed")] = &DFGExecNotifier::handler_execBlockRenamed;
    handlerMap[FTL_STR("execDidAttachPreset")] = &DFGExecNotifier::handler_execDidAttachPreset;
    handlerMap[FTL_STR("execEditWouldSplitFromPresetMayHaveChanged")] = &DFGExecNotifier::handler_execEditWouldSplitFromPresetMayHaveChanged;
    handlerMap[FTL_STR("execFixedPortInserted")] = &DFGExecNotifier::handler_execFixedPortInserted;
    handlerMap[FTL_STR("execFixedPortRemoved")] = &DFGExecNotifier::handler_execFixedPortRemoved;
    handlerMap[FTL_STR("execFixedPortRenamed")] = &DFGExecNotifier::handler_execFixedPortRenamed;
    handlerMap[FTL_STR("execFixedPortResolvedTypeChanged")] = &DFGExecNotifier::handler_execFixedPortResolvedTypeChanged;
    handlerMap[FTL_STR("execFixedPortsReordered")] = &DFGExecNotifier::handler_execFixedPortsReordered;
    handlerMap[FTL_STR("execMetadataChanged")] = &DFGExecNotifier::handler_execMetadataChanged;
    handlerMap[FTL_STR("execPortDefaultValuesChanged")] = &DFGExecNotifier::handler_execPortDefaultValuesChanged;
    handlerMap[FTL_STR("execPortInserted")] = &DFGExecNotifier::handler_execPortInserted;
    handlerMap[FTL_STR("execPortMetadataChanged")] = &DFGExecNotifier::handler_execPortMetadataChanged;
    handlerMap[FTL_STR("execPortRemoved")] = &DFGExecNotifier::handler_execPortRemoved;
    handlerMap[FTL_STR("execPortRenamed")] = &DFGExecNotifier::handler_execPortRenamed;
    handlerMap[FTL_STR("execPortResolvedTypeChanged")] = &DFGExecNotifier::handler_execPortResolvedTypeChanged;
    handlerMap[FTL_STR("execPortsReordered")] = &DFGExecNotifier::handler_execPortsReordered;
    handlerMap[FTL_STR("execPortTypeChanged")] = &DFGExecNotifier::handler_execPortTypeChanged;
    handlerMap[FTL_STR("execPortTypeSpecChanged")] = &DFGExecNotifier::handler_execPortTypeSpecChanged;
    handlerMap[FTL_STR("execTitleChanged")] = &DFGExecNotifier::handler_execTitleChanged;
    handlerMap[FTL_STR("execWillDetachPreset")] = &DFGExecNotifier::handler_execWillDetachPreset;
    handlerMap[FTL_STR("extDepsChanged")] = &DFGExecNotifier::handler_extDepsChanged;
    handlerMap[FTL_STR("extDepAdded")] = &DFGExecNotifier::handler_extDepAdded;
    handlerMap[FTL_STR("funcCodeChanged")] = &DFGExecNotifier::handler_funcCodeChanged;
    handlerMap[FTL_STR("instBlockExecEditWouldSplitFromPresetMayHaveChanged")] = &DFGExecNotifier::handler_instBlockExecEditWouldSplitFromPresetMayHaveChanged;
    handlerMap[FTL_STR("instBlockInserted")] = &DFGExecNotifier::handler_instBlockInserted;
    handlerMap[FTL_STR("instBlockPortDefaultValuesChanged")] = &DFGExecNotifier::handler_instBlockPortDefaultValuesChanged;
    handlerMap[FTL_STR("instBlockPortInserted")] = &DFGExecNotifier::handler_instBlockPortInserted;
    handlerMap[FTL_STR("instBlockPortRemoved")] = &DFGExecNotifier::handler_instBlockPortRemoved;
    handlerMap[FTL_STR("instBlockPortRenamed")] = &DFGExecNotifier::handler_instBlockPortRenamed;
    handlerMap[FTL_STR("instBlockPortResolvedTypeChanged")] = &DFGExecNotifier::handler_instBlockPortResolvedTypeChanged;
    handlerMap[FTL_STR("instBlockPortsReordered")] = &DFGExecNotifier::handler_instBlockPortsReordered;
    handlerMap[FTL_STR("instBlockRemoved")] = &DFGExecNotifier::handler_instBlockRemoved;
    handlerMap[FTL_STR("instBlockRenamed")] = &DFGExecNotifier::handler_instBlockRenamed;
    handlerMap[FTL_STR("instExecDidAttachPreset")] = &DFGExecNotifier::handler_instExecDidAttachPreset;
    handlerMap[FTL_STR("instExecEditWouldSplitFromPresetMayHaveChanged")] = &DFGExecNotifier::handler_instExecEditWouldSplitFromPresetMayHaveChanged;
    handlerMap[FTL_STR("instExecTitleChanged")] = &DFGExecNotifier::handler_instExecTitleChanged;
    handlerMap[FTL_STR("nodeInserted")] = &DFGExecNotifier::handler_nodeInserted;
    handlerMap[FTL_STR("nodeMetadataChanged")] = &DFGExecNotifier::handler_nodeMetadataChanged;
    handlerMap[FTL_STR("nodePortDefaultValuesChanged")] = &DFGExecNotifier::handler_nodePortDefaultValuesChanged;
    handlerMap[FTL_STR("nodePortInserted")] = &DFGExecNotifier::handler_nodePortInserted;
    handlerMap[FTL_STR("nodePortMetadataChanged")] = &DFGExecNotifier::handler_nodePortMetadataChanged;
    handlerMap[FTL_STR("nodePortRemoved")] = &DFGExecNotifier::handler_nodePortRemoved;
    handlerMap[FTL_STR("nodePortRenamed")] = &DFGExecNotifier::handler_nodePortRenamed;
    handlerMap[FTL_STR("nodePortResolvedTypeChanged")] = &DFGExecNotifier::handler_nodePortResolvedTypeChanged;
    handlerMap[FTL_STR("nodePortsReordered")] = &DFGExecNotifier::handler_nodePortsReordered;
    handlerMap[FTL_STR("nodePortTypeChanged")] = &DFGExecNotifier::handler_nodePortTypeChanged;
    handlerMap[FTL_STR("nodeRemoved")] = &DFGExecNotifier::handler_nodeRemoved;
    handlerMap[FTL_STR("nodeRenamed")] = &DFGExecNotifier::handler_nodeRenamed;
    handlerMap[FTL_STR("portsConnected")] = &DFGExecNotifier::handler_portsConnected;
    handlerMap[FTL_STR("portsDisconnected")] = &DFGExecNotifier::handler_portsDisconnected;
    handlerMap[FTL_STR("refVarPathChanged")] = &DFGExecNotifier::handler_refVarPathChanged;
    handlerMap[FTL_STR("removedFromOwner")] = &DFGExecNotifier::handler_removedFromOwner;
    handlerMap[FTL_STR("nlsPortInserted")] = &DFGExecNotifier::handler_nlsPortInserted;
    handlerMap[FTL_STR("nlsPortMetadataChanged")] = &DFGExecNotifier::handler_nlsPortMetadataChanged;
    handlerMap[FTL_STR("nlsPortRemoved")] = &DFGExecNotifier::handler_nlsPortRemoved;
    handlerMap[FTL_STR("nlsPortRenamed")] = &DFGExecNotifier::handler_nlsPortRenamed;
    handlerMap[FTL_STR("nlsPortResolvedTypeChanged")] = &DFGExecNotifier::handler_nlsPortResolvedTypeChanged;
    handlerMap[FTL_STR("nlsPortTypeSpecChanged")] = &DFGExecNotifier::handler_nlsPortTypeSpecChanged;
    handlerMap[FTL_STR("nlsPortsReordered")] = &DFGExecNotifier::handler_nlsPortsReordered;
  }
  return handlerMap;
}

DFGExecNotifier::~DFGExecNotifier()
{
  try
  {
    m_view.invalidate();
  }
  catch ( FabricCore::Exception e )
  {
    // [andrew 20160320] an exception will be thrown here if the Core
    // is destroyed before the Qt widgets
  }
}

void DFGExecNotifier::handle( FTL::CStrRef jsonStr )
{
  try
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );
    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
      );
    FTL::CStrRef descStr = jsonObject->getString( FTL_STR("desc") );

    HandlerMap const &handlerMap = GetHandlerMap();
    HandlerMap::const_iterator it = handlerMap.find( descStr );
    if ( it != handlerMap.end() )
    {
      Handler handler = it->second;
      (this->*handler)( jsonObject.get() );
    }
    else
    {
      fprintf(
        stderr,
        "DFGExecNotifier::viewCallback: warning: unhandled notification:\n%s\n",
        jsonStr.c_str()
        );
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "DFGExecNotifier::viewCallback: caught Core exception: %s\n  while handling notification: %s\n",
      e.getDesc_cstr(),
      jsonStr.c_str()
      );
  }
  catch ( FTL::JSONException e )
  {
    printf(
      "DFGExecNotifier::viewCallback: caught FTL::JSONException: %s\n  while handling notification: %s",
      e.getDescCStr(),
      jsonStr.c_str()
      );
  }
}

static FabricCore::DFGPortType PortTypeStrToDFGPortType( FTL::StrRef portTypeStr )
{
  if ( portTypeStr == FTL_STR("Out") )
    return FabricCore::DFGPortType_Out;
  else if ( portTypeStr == FTL_STR("IO") )
    return FabricCore::DFGPortType_IO;
  else
    return FabricCore::DFGPortType_In;
}

void DFGExecNotifier::handler_instBlockInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef blockName =
    jsonObject->getObject(
      FTL_STR("blockDesc")
      )->getString( FTL_STR("name") );

  emit instBlockInserted( instName, blockName );
}

void DFGExecNotifier::handler_instBlockRemoved( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );

  emit instBlockRemoved( instName, blockName );
}

void DFGExecNotifier::handler_instBlockPortInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  unsigned portIndex = jsonObject->getSInt32( FTL_STR("portIndex") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit instBlockPortInserted( instName, blockName, portIndex, portName );
}

void DFGExecNotifier::handler_instBlockPortRenamed( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  unsigned portIndex = jsonObject->getSInt32( FTL_STR("portIndex") );
  FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
  FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("portName") );

  emit instBlockPortRenamed(
    instName,
    blockName,
    portIndex,
    oldPortName,
    newPortName
    );
}

void DFGExecNotifier::handler_instBlockPortRemoved( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  unsigned portIndex = jsonObject->getSInt32( FTL_STR("portIndex") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit instBlockPortRemoved( instName, blockName, portIndex, portName );
}

void DFGExecNotifier::handler_execBlockPortInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  unsigned portIndex = jsonObject->getSInt32( FTL_STR("portIndex") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::JSONObject const *portDesc =
    jsonObject->getObject( FTL_STR("portDesc") );

  emit execBlockPortInserted(
    blockName,
    portIndex,
    portName,
    portDesc
    );
}

void DFGExecNotifier::handler_execBlockPortRenamed( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  unsigned portIndex = jsonObject->getSInt32( FTL_STR("portIndex") );
  FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
  FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("portName") );

  emit execBlockPortRenamed(
    blockName,
    portIndex,
    oldPortName,
    newPortName
    );
}

void DFGExecNotifier::handler_execBlockPortRemoved( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  unsigned portIndex = jsonObject->getSInt32( FTL_STR("portIndex") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit execBlockPortRemoved( blockName, portIndex, portName );
}

void DFGExecNotifier::handler_execBlockInserted( FTL::JSONObject const *jsonObject )
{
  unsigned blockIndex = jsonObject->getSInt32( FTL_STR("blockIndex") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );

  emit execBlockInserted( blockIndex, blockName );
}

void DFGExecNotifier::handler_execBlockRemoved( FTL::JSONObject const *jsonObject )
{
  unsigned blockIndex = jsonObject->getSInt32( FTL_STR("blockIndex") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );

  emit execBlockRemoved( blockIndex, blockName );
}

void DFGExecNotifier::handler_execBlockMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit execBlockMetadataChanged( blockName, key, value );
}

void DFGExecNotifier::handler_nodeInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );

  emit nodeInserted( nodeName );
}

void DFGExecNotifier::handler_nodeRenamed( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef oldNodeName = jsonObject->getString( FTL_STR("oldNodeName") );
  FTL::CStrRef newNodeName = jsonObject->getString( FTL_STR("nodeName") );

  emit nodeRenamed( oldNodeName, newNodeName );
}

void DFGExecNotifier::handler_execBlockRenamed( FTL::JSONObject const *jsonObject )
{
  unsigned blockIndex = jsonObject->getSInt32( FTL_STR("blockIndex") );
  FTL::CStrRef oldBlockName = jsonObject->getString( FTL_STR("oldBlockName") );
  FTL::CStrRef newBlockName = jsonObject->getString( FTL_STR("blockName") );

  emit execBlockRenamed( blockIndex, oldBlockName, newBlockName );
}

void DFGExecNotifier::handler_instBlockRenamed( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef oldBlockName = jsonObject->getString( FTL_STR("oldBlockName") );
  FTL::CStrRef newBlockName = jsonObject->getString( FTL_STR("blockName") );

  emit instBlockRenamed( instName, oldBlockName, newBlockName );
}

void DFGExecNotifier::handler_nodeRemoved( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );

  emit nodeRemoved( nodeName );
}

void DFGExecNotifier::handler_nodePortInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit nodePortInserted( nodeName, portIndex, portName );
}

void DFGExecNotifier::handler_nodePortRenamed( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
  FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("portName") );

  emit nodePortRenamed( nodeName, portIndex, oldPortName, newPortName );
}

void DFGExecNotifier::handler_nodePortRemoved( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit nodePortRemoved( nodeName, portIndex, portName );
}

void DFGExecNotifier::handler_execPortInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::JSONObject const *portDesc =
    jsonObject->getObject( FTL_STR("portDesc") );

  emit execPortInserted( portIndex, portName, portDesc );
}

void DFGExecNotifier::handler_execFixedPortInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::JSONObject const *portDesc =
    jsonObject->getObject( FTL_STR("portDesc") );

  emit execFixedPortInserted( portIndex, portName, portDesc );
}

void DFGExecNotifier::handler_execPortRenamed( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
  FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("portName") );

  emit execPortRenamed( portIndex, oldPortName, newPortName );
}

void DFGExecNotifier::handler_execFixedPortRenamed( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
  FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("portName") );

  emit execFixedPortRenamed( portIndex, oldPortName, newPortName );
}

void DFGExecNotifier::handler_execPortRemoved( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit execPortRemoved( portIndex, portName );
}

void DFGExecNotifier::handler_execFixedPortRemoved( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit execFixedPortRemoved( portIndex, portName );
}

void DFGExecNotifier::handler_execPortTypeSpecChanged( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newTypeSpec = jsonObject->getString( FTL_STR("newTypeSpec") );

  emit execPortTypeSpecChanged( portIndex, portName, newTypeSpec );
}

void DFGExecNotifier::handler_execPortsReordered( FTL::JSONObject const *jsonObject )
{
  FTL::JSONArray const *newOrderJSONArray =
    jsonObject->getArray( FTL_STR("newOrder") );
  size_t newOrderSize = newOrderJSONArray->size();
  unsigned *newOrderData =
    (unsigned *)alloca( newOrderSize * sizeof( unsigned ) );
  for ( size_t i = 0; i < newOrderSize; ++i )
    newOrderData[i] = unsigned( newOrderJSONArray->getSInt32( i ) );

  emit execPortsReordered(
    FTL::ArrayRef<unsigned>( newOrderData, newOrderSize )
    );
}

void DFGExecNotifier::handler_execFixedPortsReordered( FTL::JSONObject const *jsonObject )
{
  FTL::JSONArray const *newOrderJSONArray =
    jsonObject->getArray( FTL_STR("newOrder") );
  size_t newOrderSize = newOrderJSONArray->size();
  unsigned *newOrderData =
    (unsigned *)alloca( newOrderSize * sizeof( unsigned ) );
  for ( size_t i = 0; i < newOrderSize; ++i )
    newOrderData[i] = unsigned( newOrderJSONArray->getSInt32( i ) );

  emit execFixedPortsReordered(
    FTL::ArrayRef<unsigned>( newOrderData, newOrderSize )
    );
}

void DFGExecNotifier::handler_execBlockPortsReordered( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  FTL::JSONArray const *newOrderJSONArray =
    jsonObject->getArray( FTL_STR("newOrder") );
  size_t newOrderSize = newOrderJSONArray->size();
  unsigned *newOrderData =
    (unsigned *)alloca( newOrderSize * sizeof( unsigned ) );
  for ( size_t i = 0; i < newOrderSize; ++i )
    newOrderData[i] = unsigned( newOrderJSONArray->getSInt32( i ) );

  emit execBlockPortsReordered(
    blockName,
    FTL::ArrayRef<unsigned>( newOrderData, newOrderSize )
    );
}

void DFGExecNotifier::handler_nodePortsReordered( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::JSONArray const *newOrderJSONArray =
    jsonObject->getArray( FTL_STR("newOrder") );
  size_t newOrderSize = newOrderJSONArray->size();
  unsigned *newOrderData =
    (unsigned *)alloca( newOrderSize * sizeof( unsigned ) );
  for ( size_t i = 0; i < newOrderSize; ++i )
    newOrderData[i] = unsigned( newOrderJSONArray->getSInt32( i ) );

  emit nodePortsReordered(
    nodeName,
    FTL::ArrayRef<unsigned>( newOrderData, newOrderSize )
    );
}

void DFGExecNotifier::handler_instBlockPortsReordered( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  FTL::JSONArray const *newOrderJSONArray =
    jsonObject->getArray( FTL_STR("newOrder") );
  size_t newOrderSize = newOrderJSONArray->size();
  unsigned *newOrderData =
    (unsigned *)alloca( newOrderSize * sizeof( unsigned ) );
  for ( size_t i = 0; i < newOrderSize; ++i )
    newOrderData[i] = unsigned( newOrderJSONArray->getSInt32( i ) );

  emit instBlockPortsReordered(
    instName,
    blockName,
    FTL::ArrayRef<unsigned>( newOrderData, newOrderSize )
    );
}

void DFGExecNotifier::handler_nodePortResolvedTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

  emit nodePortResolvedTypeChanged( nodeName, portName, newResolvedTypeName );
}

void DFGExecNotifier::handler_nodePortDefaultValuesChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit nodePortDefaultValuesChanged( nodeName, portName );
}

void DFGExecNotifier::handler_execBlockPortDefaultValuesChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit execBlockPortDefaultValuesChanged( blockName, portName );
}

void DFGExecNotifier::handler_instBlockPortDefaultValuesChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit instBlockPortDefaultValuesChanged( instName, blockName, portName );
}

void DFGExecNotifier::handler_execBlockPortTypeSpecChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  unsigned portIndex = jsonObject->getSInt32( FTL_STR("portIndex") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newTypeSpec = jsonObject->getStringOrEmpty( FTL_STR("newTypeSpec") );

  emit execBlockPortTypeSpecChanged( blockName, portIndex, portName, newTypeSpec );
}

void DFGExecNotifier::handler_execBlockPortResolvedTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

  emit execBlockPortResolvedTypeChanged( blockName, portName, newResolvedTypeName );
}

void DFGExecNotifier::handler_execBlockPortOutsidePortTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  unsigned portIndex = jsonObject->getSInt32( FTL_STR("portIndex") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FabricCore::DFGPortType newOutsidePortType =
    PortTypeStrToDFGPortType(
      jsonObject->getStringOrEmpty( FTL_STR("newOutsidePortType") )
      );

  emit execBlockPortOutsidePortTypeChanged(
    blockName, portIndex, portName, newOutsidePortType
    );
}

void DFGExecNotifier::handler_instBlockPortResolvedTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

  emit instBlockPortResolvedTypeChanged( instName, blockName, portName, newResolvedTypeName );
}

void DFGExecNotifier::handler_execPortResolvedTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

  emit execPortResolvedTypeChanged( portName, newResolvedTypeName );
}

void DFGExecNotifier::handler_execFixedPortResolvedTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

  emit execFixedPortResolvedTypeChanged( portName, newResolvedTypeName );
}

void DFGExecNotifier::handler_execEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject )
{
  emit editWouldSplitFromPresetMayHaveChanged();
}

void DFGExecNotifier::handler_instExecEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );

  emit instExecEditWouldSplitFromPresetMayHaveChanged( instName );
}

void DFGExecNotifier::handler_instBlockExecEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef blockName = jsonObject->getString( FTL_STR("blockName") );

  emit instBlockExecEditWouldSplitFromPresetMayHaveChanged( instName, blockName );
}

void DFGExecNotifier::handler_execMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit metadataChanged( key, value );
}

void DFGExecNotifier::handler_execTitleChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef title = jsonObject->getString( FTL_STR("title") );

  emit titleChanged( title );
}

void DFGExecNotifier::handler_execPortDefaultValuesChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit execPortDefaultValuesChanged( portName );
}

void DFGExecNotifier::handler_instExecTitleChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef execTitle = jsonObject->getString( FTL_STR("execTitle") );

  emit instExecTitleChanged( instName, execTitle );
}

void DFGExecNotifier::handler_nodeMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit nodeMetadataChanged( nodeName, key, value );
}

void DFGExecNotifier::handler_execPortMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit execPortMetadataChanged( portName, key, value );
}

void DFGExecNotifier::handler_nodePortMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit nodePortMetadataChanged( nodeName, portName, key, value );
}

void DFGExecNotifier::handler_portsConnected( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef srcPortPath = jsonObject->getString( FTL_STR("srcPath") );
  FTL::CStrRef dstPortPath = jsonObject->getString( FTL_STR("dstPath") );

  emit portsConnected( srcPortPath, dstPortPath );
}

void DFGExecNotifier::handler_portsDisconnected( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef srcPortPath = jsonObject->getString( FTL_STR("srcPath") );
  FTL::CStrRef dstPortPath = jsonObject->getString( FTL_STR("dstPath") );

  emit portsDisconnected( srcPortPath, dstPortPath );
}

void DFGExecNotifier::handler_refVarPathChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef refName = jsonObject->getString( FTL_STR("refName") );
  FTL::CStrRef newVarPath = jsonObject->getString( FTL_STR("newVarPath") );

  emit refVarPathChanged( refName, newVarPath );
}

void DFGExecNotifier::handler_removedFromOwner( FTL::JSONObject const *jsonObject )
{
  emit removedFromOwner();
}

void DFGExecNotifier::handler_funcCodeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef code = jsonObject->getString( FTL_STR("code") );

  emit funcCodeChanged( code );
}

void DFGExecNotifier::handler_nodePortTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newNodePortType = jsonObject->getString( FTL_STR("newNodePortType") );

  emit nodePortTypeChanged( nodeName, portName, newNodePortType );
}

void DFGExecNotifier::handler_execPortTypeChanged( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = jsonObject->getSInt32( FTL_STR("portIndex") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FabricCore::DFGPortType newPortType = 
    PortTypeStrToDFGPortType( jsonObject->getString( FTL_STR("newExecPortType") ) );

  emit execPortTypeChanged( portIndex, portName, newPortType );
}

void DFGExecNotifier::handler_instExecDidAttachPreset( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );

  emit instExecDidAttachPreset( nodeName, presetFilePath );
}

void DFGExecNotifier::handler_execDidAttachPreset( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );

  emit didAttachPreset( presetFilePath );
}

void DFGExecNotifier::handler_execWillDetachPreset( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );

  emit willDetachPreset( presetFilePath );
}

void DFGExecNotifier::handler_extDepsChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef extDeps = jsonObject->getString( FTL_STR("extDeps") );

  emit extDepsChanged( extDeps );
}

void DFGExecNotifier::handler_extDepAdded( FTL::JSONObject const *jsonObject )
{
  // TODO
}

void DFGExecNotifier::handler_nlsPortInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::JSONObject const *portDesc =
    jsonObject->getObject( FTL_STR("portDesc") );

  emit nlsPortInserted( portIndex, portName, portDesc );
}

void DFGExecNotifier::handler_nlsPortRenamed( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
  FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("portName") );

  emit nlsPortRenamed( portIndex, oldPortName, newPortName );
}

void DFGExecNotifier::handler_nlsPortRemoved( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit nlsPortRemoved( portIndex, portName );
}

void DFGExecNotifier::handler_nlsPortTypeSpecChanged( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newTypeSpec = jsonObject->getString( FTL_STR("newTypeSpec") );

  emit nlsPortTypeSpecChanged( portIndex, portName, newTypeSpec );
}

void DFGExecNotifier::handler_nlsPortsReordered( FTL::JSONObject const *jsonObject )
{
  FTL::JSONArray const *newOrderJSONArray =
    jsonObject->getArray( FTL_STR("newOrder") );
  size_t newOrderSize = newOrderJSONArray->size();
  unsigned *newOrderData =
    (unsigned *)alloca( newOrderSize * sizeof( unsigned ) );
  for ( size_t i = 0; i < newOrderSize; ++i )
    newOrderData[i] = unsigned( newOrderJSONArray->getSInt32( i ) );

  emit nlsPortsReordered(
    FTL::ArrayRef<unsigned>( newOrderData, newOrderSize )
    );
}

void DFGExecNotifier::handler_nlsPortResolvedTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

  emit nlsPortResolvedTypeChanged( portName, newResolvedTypeName );
}

void DFGExecNotifier::handler_nlsPortMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit nlsPortMetadataChanged( portName, key, value );
}

} // namespace DFG
} // namespace FabricUI
