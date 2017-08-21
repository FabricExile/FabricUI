//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGConfig.h"
#include <CodeCompletion/KLTypeDesc.h>
#include <FabricUI/Util/Config.h>

using namespace FabricServices;
using namespace FabricUI::DFG;
using namespace FTL;

namespace FabricUI {
namespace Util {

template<>
JSONValue* ConfigSection::createValue( const DFGConfig::PredefinedPort v ) const
{
  JSONObject* dst = new JSONObject();
  dst->insert( "name", createValue( v.name ) );
  dst->insert( "typeSpec", createValue( v.typeSpec ) );
  if( !v.portName.isEmpty() )
    dst->insert( "portName", createValue( v.portName ) );
  if( !v.extDep.isEmpty() )
    dst->insert( "extDep", createValue( v.extDep ) );
  if( !v.metaData.isEmpty() )
    dst->insert( "metaData", createValue( v.metaData ) );
  return dst;
}

template<>
DFGConfig::PredefinedPort ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  DFGConfig::PredefinedPort dst(
    getValue<QString>( obj->get( "name" ) ),
    getValue<QString>( obj->get( "typeSpec" ) )
  );
  if( obj->has( "portName" ) )
    dst.portName = getValue<QString>( obj->get( "portName" ) );
  if( obj->has( "extDep" ) )
    dst.extDep = getValue<QString>( obj->get( "extDep" ) );
  if( obj->has( "metaData" ) )
    dst.metaData = getValue<QString>( obj->get( "metaData" ) );
  return dst;
}

// TODO : make the vector getter/setters generic template functions
template<>
JSONValue* ConfigSection::createValue( const std::vector<DFGConfig::PredefinedPort> v ) const
{
  JSONArray* array = new JSONArray();
  for( std::vector<DFGConfig::PredefinedPort>::const_iterator it = v.begin(); it != v.end(); it++ )
    array->push_back( createValue<DFGConfig::PredefinedPort>( *it ) );
  return array;
}

template<>
std::vector<DFGConfig::PredefinedPort> ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONArray* array = entry->cast<JSONArray>();
  std::vector<DFGConfig::PredefinedPort> dst;
  for( JSONArray::const_iterator it = array->begin(); it != array->end(); it++ )
    dst.push_back( getValue<DFGConfig::PredefinedPort>( *it ) );
  return dst;
}

} // namespace FabricUI
} // namespace Util

DFGConfig::DFGConfig()
: defaultFont("Roboto", 10),
  fixedFont("Roboto Mono", 10),
  logFont("Roboto Mono", 10)
  , searchQueryFont( QFont("Roboto Mono", 10, QFont::Bold) )
  , searchResultsFont( QFont("Roboto Mono", 10) )
  , searchHelpFont( QFont("Roboto Mono", 10, QFont::Bold) )
{
  Util::Config rootConfig;
  Util::ConfigSection& cfg = rootConfig.getOrCreateSection( "DFG" );

#define GET_PARAMETER( parameter, defaultValue ) \
  parameter = cfg.getOrCreateValue( #parameter, defaultValue )

  // defaultFont.setBold(true);
  fixedFont.setStyleHint( QFont::TypeWriter );
  logFont.setStyleHint( QFont::TypeWriter );

#define GET_FONT( font ) \
  GET_PARAMETER( font, font )

  GET_FONT( defaultFont );
  GET_FONT( fixedFont );
  GET_FONT( logFont );
  GET_FONT( searchQueryFont );
  GET_FONT( searchResultsFont );
  GET_FONT( searchHelpFont );

  GET_PARAMETER( defaultWindowColor, QColor( 85, 88, 92 ) );
  GET_PARAMETER( defaultBackgroundColor, QColor( 156, 174, 187 ) );
  GET_PARAMETER( defaultFontColor, QColor( 31, 43, 55, 255 ) );
  GET_PARAMETER( searchBackgroundColor, QColor( 135, 135, 135 ) );
  GET_PARAMETER( searchHighlightColor, QColor( 137, 181, 202 ) );
  GET_PARAMETER( searchCursorColor, QColor( 220, 220, 220 ) );
  GET_PARAMETER( searchFontColor, QColor( 0, 0, 0 ) );
  GET_PARAMETER( varNodeDefaultColor, QColor( 216, 140, 106 ) );
  GET_PARAMETER( varLabelDefaultColor, QColor( 190, 93, 90 ) );

  predefinedPorts.push_back( PredefinedPort( "Integer", "Integer", "i" ) );
  predefinedPorts.push_back( PredefinedPort( "Integer [0-100]", "Integer", "i", "", "{ \"uiRange\" : \"(0, 100)\" }" ) );
  predefinedPorts.push_back( PredefinedPort( "Scalar", "Scalar", "v" ) );
  predefinedPorts.push_back( PredefinedPort( "Scalar [0.0-1.0]", "Scalar", "v", "", "{ \"uiRange\" : \"(0.0, 1.0)\" }" ) );
  predefinedPorts.push_back( PredefinedPort( "Vec3", "Vec3", "vec" ) );
  predefinedPorts.push_back( PredefinedPort( "Color", "Color", "col" ) );
  predefinedPorts.push_back( PredefinedPort( "FCurve", "AnimX::AnimCurve", "curve", "AnimX" ) );

  GET_PARAMETER( predefinedPorts, predefinedPorts );

  GET_PARAMETER( klEditorConfig.codeBackgroundColor, defaultFontColor );
  GET_PARAMETER( klEditorConfig.codeFontColor, defaultBackgroundColor );

  Util::ConfigSection& dataTypes = cfg.getOrCreateSection( "dataTypes" );

#define REGISTER_DATA_TYPE_COLOR( typeName, defaultColor ) \
  registerDataTypeColor( typeName, dataTypes.getOrCreateValue( typeName, defaultColor ) )

  REGISTER_DATA_TYPE_COLOR( "", QColor( 40, 40, 40 ) );
  REGISTER_DATA_TYPE_COLOR( "Boolean", QColor( 240, 78, 35 ) );
  REGISTER_DATA_TYPE_COLOR( "Scalar", QColor( 128, 195, 66 ) );
  REGISTER_DATA_TYPE_COLOR( "Float32", QColor( 128, 195, 66 ) );
  REGISTER_DATA_TYPE_COLOR( "Float64", QColor( 128, 195, 66 ) );
  REGISTER_DATA_TYPE_COLOR( "Byte", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "Integer", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "SInt8", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "UInt8", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "SInt16", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "UInt16", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "SInt32", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "UInt32", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "SInt64", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "UInt64", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "Index", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "Size", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "Count", QColor( 0, 153, 100 ) );
  REGISTER_DATA_TYPE_COLOR( "String", QColor( 166, 36, 0 ) );
  REGISTER_DATA_TYPE_COLOR( "Execute", QColor( "#60D4E7" ) );

  // todo: to be removed
  // these colors should be defined as metadata above the KL code
  // that defines these types (in their extension)
  REGISTER_DATA_TYPE_COLOR( "Regex", QColor( 134, 55, 41 ) );
  REGISTER_DATA_TYPE_COLOR( "Vec2", QColor( 255, 242, 0 ) );
  REGISTER_DATA_TYPE_COLOR( "Vec3", QColor( 255, 242, 0 ) );
  REGISTER_DATA_TYPE_COLOR( "Vec4", QColor( 255, 242, 0 ) );
  REGISTER_DATA_TYPE_COLOR( "Mat22", QColor( 249, 157, 28 ) );
  REGISTER_DATA_TYPE_COLOR( "Mat33", QColor( 249, 157, 28 ) );
  REGISTER_DATA_TYPE_COLOR( "Mat44", QColor( 249, 157, 28 ) );
  REGISTER_DATA_TYPE_COLOR( "Xfo", QColor( 249, 157, 28 ) );
  REGISTER_DATA_TYPE_COLOR( "Quat", QColor( 0, 191, 232 ) );
  REGISTER_DATA_TYPE_COLOR( "Euler", QColor( 0, 191, 232 ) );
  REGISTER_DATA_TYPE_COLOR( "RotationOrder", QColor( 0, 191, 232 ) );
  REGISTER_DATA_TYPE_COLOR( "Color", QColor( 255, 0, 0 ) );
  REGISTER_DATA_TYPE_COLOR( "RGB", QColor( 255, 0, 0 ) );
  REGISTER_DATA_TYPE_COLOR( "RGBA", QColor( 255, 0, 0 ) );
  REGISTER_DATA_TYPE_COLOR( "ARGB", QColor( 255, 0, 0 ) );
  REGISTER_DATA_TYPE_COLOR( "Complex", QColor( "#E30761" ) );
  REGISTER_DATA_TYPE_COLOR( "Geometry", QColor( 110, 58, 166 ) );
  REGISTER_DATA_TYPE_COLOR( "Lines", QColor( 110, 58, 166 ) );
  REGISTER_DATA_TYPE_COLOR( "Curves", QColor( 110, 58, 166 ) );
  REGISTER_DATA_TYPE_COLOR( "Curve", QColor( 110, 58, 166 ) );
  REGISTER_DATA_TYPE_COLOR( "Points", QColor( 110, 58, 166 ) );
  REGISTER_DATA_TYPE_COLOR( "PolygonMesh", QColor( 110, 58, 166 ) );
  REGISTER_DATA_TYPE_COLOR( "ImporterObject", QColor( 206, 165, 151 ) );
  REGISTER_DATA_TYPE_COLOR( "ImporterContext", QColor( 206, 165, 151 ) );
}

void DFGConfig::registerDataTypeColor(FTL::StrRef dataType, QColor color)
{
  std::string baseType = CodeCompletion::KLTypeDesc(dataType).getBaseType();
  std::map<std::string, QColor>::iterator it = colorForDataType.find(baseType);
  if(it != colorForDataType.end())
    it->second = color;
  colorForDataType.insert(std::pair<std::string, QColor>(baseType, color));
}

QColor DFGConfig::getColorForDataType(FTL::StrRef dataType, FabricCore::DFGExec * exec, char const * portName)
{
  if(dataType.size() > 0)
  {
    if(dataType.data()[0] == '$')
      return QColor(0, 0, 0);
    std::string baseType = CodeCompletion::KLTypeDesc(dataType.data()).getBaseType();
    std::map<std::string, QColor>::iterator it = colorForDataType.find(baseType);

    if(it == colorForDataType.end() && exec != NULL && portName != NULL)
    {
      QString uiColor = exec->getExecPortMetadata(portName, "uiColor");
      if(uiColor.length() > 0)
      {
        FabricCore::Variant uiColorVar = FabricCore::Variant::CreateFromJSON(uiColor.toUtf8().constData());
        const FabricCore::Variant * rVar = uiColorVar.getDictValue("r");
        const FabricCore::Variant * gVar = uiColorVar.getDictValue("g");
        const FabricCore::Variant * bVar = uiColorVar.getDictValue("b");

        if(rVar && gVar && bVar)
        {
          QColor color(rVar->getSInt32(), gVar->getSInt32(), bVar->getSInt32());
          registerDataTypeColor(dataType, color);
          return color;
        }
      }
    }

    if(it != colorForDataType.end())
      return it->second;
    return QColor(150, 150, 150);
  }
  return QColor(0, 0, 0);
}
