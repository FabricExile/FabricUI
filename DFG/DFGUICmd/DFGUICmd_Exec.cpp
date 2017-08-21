//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

QStringList DFGUICmd_Exec::adjustNewItems(
  FabricCore::String const &newItemNamesJSON,
  QPointF targetPos,
  unsigned &coreUndoCount
  )
{
  char const *newItemNamesJSONCStr;
  uint32_t newItemNamesJSONSize;
  newItemNamesJSON.getCStrAndSize( newItemNamesJSONCStr, newItemNamesJSONSize );
  FTL::StrRef newItemNamesStr( newItemNamesJSONCStr, newItemNamesJSONSize );
  FTL::JSONStr newItemNamesJSONStr( newItemNamesStr );
  FTL::OwnedPtr<FTL::JSONArray const> newItemNamesJA(
    FTL::JSONValue::Decode<FTL::JSONStr>(
      newItemNamesJSONStr
      )->cast<FTL::JSONArray>()
    );

  size_t count = newItemNamesJA->size();
  std::vector<FTL::CStrRef> newItemNames;
  newItemNames.reserve( count );
  for ( size_t i = 0; i < count; ++i )
  {
    FTL::JSONValue const *newNodeNameJV = newItemNamesJA->get( i );
    newItemNames.push_back( newNodeNameJV->getStringValue() );
  }

  if ( count > 0 )
  {
    std::vector<QPointF> newTopLeftPoss;
    newTopLeftPoss.reserve( newItemNames.size() );
    for ( size_t i = 0; i < count; ++i )
      newTopLeftPoss.push_back(
        getNodeUIGraphPos( newItemNames[i] )
        );

    QPointF avgTopLeftPos;
    for ( size_t i = 0; i < count; ++i )
      avgTopLeftPos += newTopLeftPoss[i];
    avgTopLeftPos /= count;

    QPointF delta = targetPos - avgTopLeftPos;

    for ( size_t i = 0; i < count; ++i )
      newTopLeftPoss[i] += delta;

    moveNodes(
      newItemNames,
      newTopLeftPoss,
      coreUndoCount
      );
  }

  QStringList newItemNames_Qt;
  newItemNames_Qt.reserve( newItemNames.size() );
  for ( size_t i = 0; i < newItemNames.size(); ++i )
  {
    FTL::CStrRef newNodeName = newItemNames[i];
    newItemNames_Qt.append(
      QString::fromUtf8( newNodeName.data(), newNodeName.size() )
      );
  }
  return newItemNames_Qt;
}

void DFGUICmd_Exec::moveNodes(
  FTL::ArrayRef<FTL::CStrRef> itemNames,
  FTL::ArrayRef<QPointF> newTopLeftPoss,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGExec &exec = getExec();

  for ( size_t i = 0; i < itemNames.size(); ++i )
  {
    FTL::CStrRef itemName = itemNames[i];
    if(newTopLeftPoss.size() == i)
      break;
    
    QPointF newTopLeftPos = newTopLeftPoss[i];

    std::string json;
    {
      FTL::JSONEnc<std::string> je( json, FTL::JSONFormat::Packed() );
      FTL::JSONObjectEnc<std::string> joe( je );
      {
        FTL::JSONEnc<std::string> xJE( joe, FTL_STR("x") );
        FTL::JSONFloat64Enc<std::string> xJFE( xJE, newTopLeftPos.x() );
      }
      {
        FTL::JSONEnc<std::string> yJE( joe, FTL_STR("y") );
        FTL::JSONFloat64Enc<std::string> yJFE( yJE, newTopLeftPos.y() );
      }
    }
    exec.setItemMetadata(
      itemName.c_str(),
      "uiGraphPos",
      json.c_str(),
      true, // canUndo
      false // shouldSplitFromPreset
      );
    ++coreUndoCount;
  }
}

QPointF DFGUICmd_Exec::getNodeUIGraphPos( 
  FTL::CStrRef itemName
  )
{
  FabricCore::DFGExec &exec = getExec();

  QPointF result;
  FTL::CStrRef uiGraphPosJSON =
    exec.getItemMetadata( itemName.c_str(), "uiGraphPos" );
  if ( !uiGraphPosJSON.empty() )
  {
    FTL::JSONStr uiGraphPosJSONStr( uiGraphPosJSON );
    FTL::OwnedPtr<FTL::JSONValue const> uiGraphPosJV(
      FTL::JSONValue::Decode( uiGraphPosJSONStr )
      );

    if ( FTL::JSONObject const *uiGraphPosJO =
      uiGraphPosJV->maybeCast<FTL::JSONObject>() )
    {
      if ( FTL::JSONValue const *xJV = uiGraphPosJO->maybeGet( "x" ) )
      {
        switch ( xJV->getType() )
        {
          case FTL::JSONValue::Type_SInt32:
            result.setX( xJV->getSInt32Value() );
            break;

          case FTL::JSONValue::Type_Float64:
            result.setX( xJV->getFloat64Value() );
            break;

          default: break;
        }
      }

      if ( FTL::JSONValue const *yJV = uiGraphPosJO->maybeGet( "y" ) )
      {
        switch ( yJV->getType() )
        {
          case FTL::JSONValue::Type_SInt32:
            result.setY( yJV->getSInt32Value() );
            break;

          case FTL::JSONValue::Type_Float64:
            result.setY( yJV->getFloat64Value() );
            break;

          default: break;
        }
      }
    }
  }
  return result;
}

FABRIC_UI_DFG_NAMESPACE_END
