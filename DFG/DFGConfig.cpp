//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGConfig.h"
#include <CodeCompletion/KLTypeDesc.h>
#include <FabricUI/Util/Config.h>

using namespace FabricServices;
using namespace FabricUI::DFG;

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
  REGISTER_DATA_TYPE_COLOR( "Geometry", QColor( 51, 1, 106 ) );
  REGISTER_DATA_TYPE_COLOR( "Lines", QColor( 51, 1, 106 ) );
  REGISTER_DATA_TYPE_COLOR( "Curves", QColor( 51, 1, 106 ) );
  REGISTER_DATA_TYPE_COLOR( "Curve", QColor( 51, 1, 106 ) );
  REGISTER_DATA_TYPE_COLOR( "Points", QColor( 51, 1, 106 ) );
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
