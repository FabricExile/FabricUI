//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "Config.h"

#include <FabricUI/Util/FabricResourcePath.h>

using namespace FTL;

#include <fstream>
#include <ctime>
#include <limits>

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

const char* VerKey_Maj = "Major";
const char* VerKey_Min = "Minor";
const char* VerKey_Rev = "Revision";
const char* VerKey_CSt = "ConfigStamp";
const char* VerKey_RtSt = "RunTimeStamp";

struct ConfigVersion
{
  uint8_t m_major, m_minor, m_revision, m_configStamp;
  int32_t m_runTimeStamp;
public:
  ConfigVersion()
    : m_major( FabricCore::GetVersionMaj() )
    , m_minor( FabricCore::GetVersionMin() )
    , m_revision( FabricCore::GetVersionRev() )
    , m_configStamp( 0 ) // Hardcoded value : modify if necessary
    , m_runTimeStamp( std::min<time_t>( std::time( NULL ), std::numeric_limits<int32_t>::max() ) )
  {}
  ConfigVersion( const FTL::JSONObject* o )
    : m_major( o->getSInt32OrDefault( VerKey_Maj, 0 ) )
    , m_minor( o->getSInt32OrDefault( VerKey_Min, 0 ) )
    , m_revision( o->getSInt32OrDefault( VerKey_Rev, 0 ) )
    , m_configStamp( o->getSInt32OrDefault( VerKey_CSt, 0 ) )
    , m_runTimeStamp( o->getSInt32OrDefault( VerKey_RtSt, 0 ) )
  {
  }
  inline bool operator<( const ConfigVersion& o ) const
  {
    return 
      ( this->m_major < o.m_major ) || ( ( this->m_major == o.m_major ) &&
        ( ( this->m_minor < o.m_minor ) || ( ( this->m_minor == o.m_minor ) &&
          ( ( this->m_revision < o.m_revision ) || ( ( this->m_revision < o.m_revision ) &&
            ( ( this->m_configStamp < o.m_configStamp ) || ( ( this->m_configStamp == o.m_configStamp ) &&
              ( this->m_runTimeStamp < o.m_runTimeStamp )
            ) )
          ) )
        ) )
      )
    ;
  }
  FTL::JSONObject* write() const
  {
    FTL::JSONObject* o = new FTL::JSONObject();
    o->insert( VerKey_Maj, new FTL::JSONSInt32( this->m_major ) );
    o->insert( VerKey_Min, new FTL::JSONSInt32( this->m_minor ) );
    o->insert( VerKey_Rev, new FTL::JSONSInt32( this->m_revision ) );
    o->insert( VerKey_CSt, new FTL::JSONSInt32( this->m_configStamp ) );
    o->insert( VerKey_RtSt, new FTL::JSONSInt32( this->m_runTimeStamp ) );
    return o;
  }
};

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

  this->open( userConfigPath );

  const char* VersionKeyStr = "ConfigVersion";

  // Retro-compatibility
  if( !m_json->has( VersionKeyStr ) )
  {
    // Detecting a previous file that we automatically wrote in Fabric 2.5.0
    // with default values that have changed since then (so we ignore these values)
    if( m_json->size() == 1 && m_json->has( "GraphView" ) )
    {
      FTL::JSONValue* gv = m_json->get( "GraphView" );
      if( gv->isObject() )
      {
        FTL::JSONObject* gvo = gv->cast<FTL::JSONObject>();
        if( gvo->size() == 1 && gvo->has( "mainPanelBackgroundColor" ) )
          gvo->clear();
      }
    }
  }

  // Writing the latest version to that ReadOnly file (this
  // is the only thing we change; aside from the malformed entries
  // that will be removed)
  {
    const char* highestVerKey = "latest";
    ConfigVersion highestVersion;
    ConfigVersion currentVersion;
    if( m_json->has( VersionKeyStr ) && m_json->get( VersionKeyStr )->isObject() )
    {
      const FTL::JSONObject* versions = m_json->getObject( VersionKeyStr );
      if( versions->has( highestVerKey ) && versions->get( highestVerKey )->isObject() )
        highestVersion = ConfigVersion( versions->getObject( highestVerKey ) );
    }
    highestVersion = std::max( highestVersion, currentVersion );

    FTL::JSONObject* versions = new FTL::JSONObject();
    versions->insert( highestVerKey, highestVersion.write() );
    versions->insert( "lastEdit", currentVersion.write() );
    m_json->replace( VersionKeyStr, versions );

    m_json->replace( "DefaultConfigPath", new FTL::JSONString( defaultConfigPath ) );
    std::ofstream file( m_fileName.data() );
    file << m_json->encode();
  }
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
