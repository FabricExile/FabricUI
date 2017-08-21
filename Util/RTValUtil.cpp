//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "RTValUtil.h"
#include <FTL/StrRef.h>
#include <FTL/JSONEnc.h>
#include <FTL/JSONDec.h>
#include <FTL/JSONValue.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace Application;

bool RTValUtil::isKLRTVal(
  RTVal klRTVal)
{
  FABRIC_CATCH_BEGIN();
  
  return klRTVal.isWrappedRTVal();
  
  FABRIC_CATCH_END("RTValUtil::isKLRTVal");
  
  return false;
}

QString RTValUtil::getType(
  RTVal klRTVal)
{
  FABRIC_CATCH_BEGIN();
  if( klRTVal.isValid() )
    return klRTVal.callMethod(
      "String", 
      "type", 
      0, 
      0).getStringCString();

  FABRIC_CATCH_END("RTValUtil::getType");

  return "None";
}

RTVal RTValUtil::toRTVal(
  RTVal klRTVal)
{
  FABRIC_CATCH_BEGIN();

  return isKLRTVal(klRTVal)
    ? klRTVal.getUnwrappedRTVal()
    : klRTVal;

  FABRIC_CATCH_END("RTValUtil::toRTVal");

  return RTVal();
}

RTVal RTValUtil::toKLRTVal(
  RTVal rtVal)
{
  FABRIC_CATCH_BEGIN();

  return !isKLRTVal(rtVal)
    ? RTVal::ConstructWrappedRTVal(rtVal)
    : rtVal;

  FABRIC_CATCH_END("RTValUtil::toKLRTVal");

  return RTVal();
}

QString RTValUtil::toJSON(
  RTVal rtVal_)
{
  QString res;

  FABRIC_CATCH_BEGIN();

  RTVal rtVal = toRTVal(rtVal_);
  if( !rtVal.isValid() ) {
    printf( "Warning: RTValUtil::toJSON: rtVal is invalid" );
    return res;
  }

  // If the value is an Object, we have use the RTValToJSONEncoder interface, if any.
  // This is the same logic as in FabricServices::Persistence::RTValToJSONEncoder, which unfortunately is not obvious to reuse.
  if( rtVal.isObject() ) 
  {
    FabricCore::RTVal cast = FabricCore::RTVal::Construct( rtVal.getContext(), "RTValToJSONEncoder", 1, &rtVal );
    if( !cast.isValid() )
      throw FabricCore::Exception( ("KL object of type " + std::string(rtVal.getTypeNameCStr()) + " doesn't support RTValToJSONEncoder").c_str() );

    FTL::CStrRef ref;
    FabricCore::RTVal result;

    if( !cast.isNullObject() ) 
    {
      result = cast.callMethod( "String", "convertToString", 0, 0 );
      if( !result.isValid() )
        throw FabricCore::Exception( ("Calling method 'RTValToJSONEncoder::convertToString' on object of type " + std::string( rtVal.getTypeNameCStr() ) + " failed").c_str() );

      ref = result.getStringCString();
    }

    // Encode as a json friendly string (eg: add escape chars)
    std::string json;
    {
      FTL::JSONEnc<> jsonEnc( json );
      FTL::JSONStringEnc<> jsonStringEnc( jsonEnc, ref );
    }
    res = json.c_str();
  } 

  else
    res = rtVal.getJSON().getStringCString();

  FABRIC_CATCH_END("RTValUtil::toJSON");

  return res;
}

RTVal RTValUtil::fromJSON(
  Context context,
  QString const&json,
  QString const&rtValType)
{
  RTVal rtVal;
  
  FABRIC_CATCH_BEGIN();

  rtVal = RTVal::Construct(
    context,
    rtValType.toUtf8().constData(), 
    0, 
    0);

  // If the value is an Object, we have use the RTValToJSONDecoder interface, if any.
  // This is the same logic as in FabricServices::Persistence::RTValToJSONDecoder, which unfortunately is not obvious to reuse.
  if( rtVal.isObject() ) 
  {
    // Create the object (non-null)
    rtVal = RTVal::Create(
      context,
      rtValType.toUtf8().constData(),
      0,
      0 );

    // Try to decode as a String first
    std::string decodedString;
    {
      FTL::StrRef jsonStr( json.toUtf8().constData() );
      FTL::JSONStrWithLoc strWithLoc( jsonStr );
      FTL::JSONDec<FTL::JSONStrWithLoc> jsonDec( strWithLoc );
      FTL::JSONEnt<FTL::JSONStrWithLoc> jsonEnt;
      if( !jsonDec.getNext( jsonEnt )
          || jsonEnt.getType() != jsonEnt.Type_String )
        return rtVal;// Return as empty
      jsonEnt.stringAppendTo( decodedString );
    }

    FabricCore::RTVal cast = FabricCore::RTVal::Construct( context, "RTValFromJSONDecoder", 1, &rtVal );
    if( !cast.isInterface() )
      return rtVal;// Return as empty
    if( cast.isNullObject() )
      return rtVal;// Return as empty

    FabricCore::RTVal data =
      FabricCore::RTVal::ConstructString(
        context,
        decodedString.data(),
        decodedString.size()
      );

    FabricCore::RTVal result = cast.callMethod( "Boolean", "convertFromString", 1, &data );
    if( !result.isValid() || !result.getBoolean() )
      throw FabricCore::Exception( ("Error calling 'RTValFromJSONDecoder::convertFromString' on object of type " + std::string( rtVal.getTypeNameCStr() ) ).c_str() );
  } 

  else
    rtVal.setJSON( json.toUtf8().constData() );
 
  FABRIC_CATCH_END("RTValUtil::fromJSON");

  return rtVal;
}

RTVal RTValUtil::fromJSON(
  Client client,
  QString const&json,
  QString const&rtValType)
{
  return fromJSON(
    client.getContext(),
    json,
    rtValType);
}
