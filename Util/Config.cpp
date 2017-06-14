//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "Config.h"

#include <FabricUI/Util/FabricResourcePath.h>

using namespace FTL;

#include <fstream>

#include <QColor>
#include <QFont>

#ifdef NO_FABRIC_CORE // HACK
namespace FabricCore { inline char const * GetFabricUserDir() { return ""; } }
#else
#include <FabricCore.h>
#endif

namespace FabricUI {
namespace Util {

Config::Config( const FTL::StrRef fileName, Access access )
  : ConfigSection()
{
  setAccess( access );
  open( fileName );
}

void Config::open( const FTL::StrRef fileName )
{
  m_fileName = fileName;

  if ( m_json != NULL ) { delete m_json; m_json = NULL; }

  // If WriteOnly, read the file, but don't use its values (see getOrCreateValue<>)
  //if( getAccess() != WriteOnly )
  {
    std::ifstream file( fileName.data() );
    if ( file.is_open() )
    {
      m_content = std::string(
        std::istreambuf_iterator<char>( file ),
        std::istreambuf_iterator<char>()
      );
      if ( !m_content.empty() )
      {
        try
        {
          JSONStrWithLoc content( m_content );
          m_json = JSONObject::Decode( content );
          return;
        }
        catch ( FTL::JSONException e)
        {
          printf(
            "Error in %s, malformed JSON : %s\n",
            fileName.data(),
            e.getDescCStr()
          );
        }
      }
    }
  }

  // If there is no readable JSON, create a new one
  m_json = new JSONObject();
}

Config::Config()
  : ConfigSection()
{
  // The default config is only used to show the default values
  // TODO : We might not have write permissions to this directory
  std::string defaultConfigPath = FTL::PathJoin( FabricResourcesDirPath(), "default.config.json" );
  Config* defaultCfg = new Config(
    defaultConfigPath
    , WriteOnly
  );
  m_previousSection = defaultCfg;

  // The user config is readonly : we only read the values that the user has defined
  this->setAccess( ReadOnly );
  std::string userConfigPath = FTL::PathJoin( FabricCore::GetFabricUserDir(), "user.config.json" );

  // [FE-7891] Creating a sample file if none exists
  if ( !std::ifstream( userConfigPath.data() ).is_open() )
  {
    std::ofstream file( userConfigPath.data() );
    // TODO : instead of writing the sample file like this, we could read it
    // from the disk (see how it's done in Config::open above)
    file << "// This file will override the default settings that are located in " << defaultConfigPath << std::endl;
    file << "{" << std::endl;
    file << "  \"GraphView\" : {" << std::endl;
    file << "    \"mainPanelBackgroundColor\" : {" << std::endl;
    file << "      \"r\" : 68," << std::endl;
    file << "      \"g\" : 68," << std::endl;
    file << "      \"b\" : 68," << std::endl;
    file << "      \"a\" : 255" << std::endl;
    file << "    }" << std::endl;
    file << "  }" << std::endl;
    file << "}" << std::endl;
  }
  this->open( userConfigPath );
}

Config::~Config()
{
  // If ReadOnly, don't write the file
  // TODO : we might want to still write it, in which
  // case malformed entries will disappear, but well-formed
  // entries will remain the same if getOrCreateValue<>
  // doesn't allow modifying them, as expected
  if( getAccess() != ReadOnly )
  {
    std::ofstream file( m_fileName.data() );
    file << m_json->encode();
    delete m_json;
  }
  if( m_previousSection != NULL )
    delete m_previousSection;
}

void ConfigSection::setAccess( Access access )
{
  m_access = access;
}

ConfigSection& ConfigSection::getOrCreateSection( const FTL::StrRef name )
{
  if ( m_sections.find( name ) == m_sections.end() )
  {
    // If there is no child section and there is a previous section, return its child section
    if ( !m_json->has( name ) && m_previousSection != NULL )
      return m_previousSection->getOrCreateSection( name );

    // Else read it from the JSON or create an empty one
    ConfigSection* newSection = new ConfigSection();
    newSection->setAccess( this->getAccess() );
    m_sections[name] = newSection;
    newSection->m_json = m_json->has( name ) ?
      m_json->get( name )->cast<JSONObject>() : new JSONObject();
    m_json->insert( name, newSection->m_json );

    // Link the child section of the previous section to this new child
    if ( m_previousSection != NULL )
      m_sections[name]->m_previousSection = &m_previousSection->getOrCreateSection( name );
  }
  return *m_sections[name];
}

// bool

template<>
JSONValue* ConfigSection::createValue( const bool v ) const
{
  return new JSONBoolean( v );
}

template<>
bool ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getBooleanValue();
}

// int

template<>
JSONValue* ConfigSection::createValue( const int v ) const
{
  return new JSONSInt32( v );
}

template<>
int ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getSInt32Value();
}

// unsigned int

template<>
JSONValue* ConfigSection::createValue( const unsigned int v ) const
{
  return new JSONSInt32( v );
}

template<>
unsigned int ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getSInt32Value();
}

// double

template<>
JSONValue* ConfigSection::createValue( const double v ) const
{
  return new JSONFloat64( v );
}

template<>
double ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getFloat64Value();
}

// float

template<>
JSONValue* ConfigSection::createValue( const float v ) const
{
  return new JSONFloat64( v );
}

template<>
float ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getFloat64Value();
}

// QString

template<>
JSONValue* ConfigSection::createValue( const QString v ) const
{
  return new JSONString( StrRef( v.toUtf8().data(), v.toUtf8().size() ) );
}

template<>
QString ConfigSection::getValue( const JSONValue* entry ) const
{
  StrRef v = entry->getStringValue();
  return QString::fromUtf8( v.data(), v.size() );
}

// QColor

template<>
JSONValue* ConfigSection::createValue( const QColor v ) const
{
  JSONObject* color = new JSONObject();
  color->insert( "r", new JSONSInt32( v.red() ) );
  color->insert( "g", new JSONSInt32( v.green() ) );
  color->insert( "b", new JSONSInt32( v.blue() ) );
  color->insert( "a", new JSONSInt32( v.alpha() ) );
  return color;
}

template<>
QColor ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  return QColor(
    obj->getSInt32( "r" ),
    obj->getSInt32( "g" ),
    obj->getSInt32( "b" ),
    obj->has( "a" ) ? obj->getSInt32( "a" ) : 255
  );
}

// QFont

template<>
JSONValue* ConfigSection::createValue( const QFont v ) const
{
  JSONObject* font = new JSONObject();

  // mandatory parameters (see the constructor in getValue, below)
  font->insert( "family", createValue( v.family() ) );
  font->insert( "pointSize", createValue( v.pointSize() ) );
  font->insert( "weight", createValue( v.weight() ) );

  if ( v.pixelSize() > 0 ) font->insert( "pixelSize", createValue( v.pixelSize() ) );
  if( v.pointSizeF() > 0 ) font->insert( "pointSizeF", createValue( v.pointSizeF() ) );
  font->insert( "styleHint", createValue<int>( v.styleHint() ) );
  font->insert( "hintingPreference", createValue<int>( v.hintingPreference() ) );
  return font;
}

template<>
QFont ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  QFont result = QFont(
    getValue<QString>( obj->get( "family" ) ),
    obj->getSInt32( "pointSize" ),
    obj->getSInt32( "weight" )
  );

  if ( obj->has( "pixelSize" ) )
    result.setPixelSize( obj->getSInt32( "pixelSize" ) );

  if( obj->has( "pointSizeF" ) )
    result.setPointSizeF( obj->getFloat64( "pointSizeF" ) );

  if ( obj->has( "styleHint" ) )
    result.setStyleHint( QFont::StyleHint( obj->getSInt32( "styleHint" ) ) );

  if( obj->has( "hintingPrefrence" ) )
    result.setHintingPreference( QFont::HintingPreference( obj->getSInt32(  "hintingPrefrence" ) ) );

  return result;
}

}} // namespace FabricUI::Util
