//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValAnimXFCurveModel.h"

#include <assert.h>

using namespace FabricUI::FCurveEditor;

size_t RTValAnimXFCurveConstModel::getKeyCount() const
{
  if( !m_val.isValid() || m_val.isNullObject() )
    return 0;
  return const_cast<FabricCore::RTVal*>(&m_val)->callMethod( "UInt32", "keyframeCount", 0, NULL ).getUInt32();
}

FabricCore::RTVal RTValAnimXFCurveConstModel::idToIndex( size_t i ) const
{
  FabricCore::RTVal bRV = FabricCore::RTVal::ConstructBoolean( m_val.getContext(), true );
  const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "", "useIds", 1, &bRV );
  FabricCore::RTVal iRV = FabricCore::RTVal::ConstructUInt32( m_val.getContext(), i );
  return const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "UInt32", "getKeyIndex", 1, &iRV );
}

Key RTValAnimXFCurveConstModel::getOrderedKey( size_t index ) const
{
  const size_t argc = 2;
  FabricCore::RTVal args[argc] = {
    FabricCore::RTVal::ConstructUInt32( m_val.getContext(), index ),
    FabricCore::RTVal::Construct( m_val.getContext(), "AnimX::Keyframe", 0, NULL )
  };
  const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "Boolean", "keyframeAtIndex", argc, args );
  FabricCore::RTVal key = args[1];
  Key dst;
  dst.pos.setX( key.maybeGetMember( "time" ).getFloat64() );
  dst.pos.setY( key.maybeGetMember( "value" ).getFloat64() );
  dst.tanIn.setX( key.maybeGetMember( "tanIn" ).maybeGetMember( "x" ).getFloat64() );
  dst.tanIn.setY( key.maybeGetMember( "tanIn" ).maybeGetMember( "y" ).getFloat64() );
  dst.tanOut.setX( key.maybeGetMember( "tanOut" ).maybeGetMember( "x" ).getFloat64() );
  dst.tanOut.setY( key.maybeGetMember( "tanOut" ).maybeGetMember( "y" ).getFloat64() );
  dst.tanInType = key.maybeGetMember( "tanIn" ).maybeGetMember( "type" ).getSInt32();
  dst.tanOutType = key.maybeGetMember( "tanOut" ).maybeGetMember( "type" ).getSInt32();
  return dst;
}

Key RTValAnimXFCurveConstModel::getKey( size_t id ) const
{
  return this->getOrderedKey( this->idToIndex( id ).getUInt32() );
}

const size_t TangentTypeCount = 14+1; // see the KL code
const char* TangentTypeNames[TangentTypeCount] = 
{
  "Global",
  "Fixed",
  "Linear",
  "Flat",
  "Step",
  "Slow",
  "Fast",
  "Smooth",
  "Clamped",
  "Auto",
  "Sine",
  "Parabolic",
  "Log",
  "Plateau",
  "StepNext"
};

size_t RTValAnimXFCurveConstModel::tangentTypeCount() const
{
  return TangentTypeCount;
}

QString RTValAnimXFCurveConstModel::tangentTypeName( size_t i ) const
{
  return QString::fromUtf8( TangentTypeNames[i] );
}

const size_t InfinityTypeCount = 4 + 1;
const char* InfinityTypeNames[InfinityTypeCount] =
{
  "Constant",
  "Linear",
  "Cycle",
  "CycleRelative",
  "Oscillate"
};

size_t RTValAnimXFCurveConstModel::infinityTypeCount() const
{
  return InfinityTypeCount;
}

QString RTValAnimXFCurveConstModel::infinityTypeName( size_t i ) const
{
  return QString::fromUtf8( InfinityTypeNames[i] );
}

size_t RTValAnimXFCurveConstModel::getPreInfinityType() const
{
  if( !m_val.isValid() || m_val.isNullObject() )
    return 0;
  return const_cast<FabricCore::RTVal*>( &m_val )
    ->callMethod( "SInt32", "preInfinityType", 0, NULL ).getSInt32();
}

size_t RTValAnimXFCurveConstModel::getPostInfinityType() const
{
  if( !m_val.isValid() || m_val.isNullObject() )
    return 0;
  return const_cast<FabricCore::RTVal*>( &m_val )
    ->callMethod( "SInt32", "postInfinityType", 0, NULL ).getSInt32();
}

size_t RTValAnimXFCurveConstModel::getIndexAfterTime( qreal time ) const
{
  const size_t argc = 2;
  FabricCore::RTVal args[argc] = {
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), time ),
    FabricCore::RTVal::ConstructUInt32( m_val.getContext(), 0 )
  };
  FabricCore::RTVal b = const_cast<FabricCore::RTVal*>( &m_val )
    ->callMethod( "Boolean", "keyframeIndex", argc, args );
  assert( b.getBoolean() );
  return args[1].getUInt32();
}

qreal RTValAnimXFCurveConstModel::evaluate( qreal v ) const
{
  if( !m_val.isValid() || m_val.isNullObject() )
    return 0;
  FabricCore::RTVal time = FabricCore::RTVal::ConstructFloat64( m_val.getContext(), v );
  return const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "Float64", "evaluate", 1, &time ).getFloat64();
}

void RTValAnimXFCurveVersionedConstModel::update( bool emitChanges ) const
{
  const bool invalidVal = ( !m_val.isValid() || m_val.isNullObject() );
  size_t sVersion = invalidVal ? 0 : const_cast<FabricCore::RTVal*>( &m_val )
    ->callMethod( "UInt32", "getStructureVersion", 0, NULL ).getUInt32();
  size_t vVersion = invalidVal ? 0 : const_cast<FabricCore::RTVal*>( &m_val )
    ->callMethod( "UInt32", "getValueVersion", 0, NULL ).getUInt32();

  const bool structureChanged = ( m_lastStructureVersion != sVersion );
  const bool valueChanged = ( m_lastValueVersion != vVersion );
  const size_t lastKeyCount = m_lastKeyCount;

  m_lastStructureVersion = sVersion;
  m_lastValueVersion = vVersion;
  m_lastKeyCount = Parent::getKeyCount();

  if( !emitChanges )
    return;

  const size_t hc = Parent::getKeyCount();
  if( structureChanged )
  {
    for( size_t i = lastKeyCount; i > hc; i-- )
      emit this->keyDeleted( i-1 );
    for( size_t i = lastKeyCount; i < hc; i++ )
      emit this->keyAdded();
  }
  else
    assert( lastKeyCount == hc );
    
  if( valueChanged )
  {
    for( size_t i = 0; i < hc; i++ )
      emit this->keyMoved( i );
    emit this->infinityTypesChanged();
  }
}

inline void SetKey( FabricCore::RTVal& m_val, const FabricCore::RTVal& index, Key h )
{
  const size_t argc = 9;
  FabricCore::RTVal args[argc] =
  {
    index,
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.pos.x() ),
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.pos.y() ),
    FabricCore::RTVal::ConstructSInt32( m_val.getContext(), h.tanInType ),
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.tanIn.x() ),
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.tanIn.y() ),
    FabricCore::RTVal::ConstructSInt32( m_val.getContext(), h.tanOutType ),
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.tanOut.x() ),
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.tanOut.y() )
  };
  m_val.callMethod( "", "setKeyframe", argc, args );
}

void RTValAnimXFCurveVersionedModel::setKey( size_t i, Key h )
{
  SetKey( m_val, this->idToIndex( i ), h );
  emit this->dirty();
}

void RTValAnimXFCurveModel::setKey( size_t i, Key h )
{
  SetKey( m_val, this->idToIndex( i ), h );
  emit this->keyMoved( i );
}

inline void AddKey( FabricCore::RTVal& m_val )
{
  assert( m_val.isValid() );
  if( m_val.isNullObject() )
    m_val = FabricCore::RTVal::Create( m_val.getContext(), "AnimX::AnimCurve", 0, NULL );
  m_val.callMethod( "", "pushKeyframe", 0, NULL );
}

void RTValAnimXFCurveVersionedModel::addKey()
{
  AddKey( m_val );
  emit this->dirty();
}

void RTValAnimXFCurveModel::addKey()
{
  AddKey( m_val );
  emit this->keyAdded();
}

inline void DeleteKey( FabricCore::RTVal& m_val, FabricCore::RTVal* index )
{
  assert( m_val.isValid() );
  m_val.callMethod( "", "removeKeyframe", 1, index );
}

void RTValAnimXFCurveVersionedModel::deleteKey( size_t i )
{
  FabricCore::RTVal index = this->idToIndex( i );
  DeleteKey( m_val, &index );
}

void RTValAnimXFCurveModel::deleteKey( size_t i )
{
  FabricCore::RTVal index = this->idToIndex( i );
  DeleteKey( m_val, &index );
  emit this->keyDeleted( i );
}

void RTValAnimXFCurveVersionedModel::setPreInfinityType( size_t i )
{
  assert( m_val.isValid() );
  if( m_val.isNullObject() )
    m_val = FabricCore::RTVal::Create( m_val.getContext(), "AnimX::AnimCurve", 0, NULL );
  FabricCore::RTVal t = FabricCore::RTVal::ConstructSInt32( m_val.getContext(), i );
  m_val.callMethod( "", "setPreInfinityType", 1, &t );
  emit this->dirty();
}

void RTValAnimXFCurveVersionedModel::setPostInfinityType( size_t i )
{
  assert( m_val.isValid() );
  if( m_val.isNullObject() )
    m_val = FabricCore::RTVal::Create( m_val.getContext(), "AnimX::AnimCurve", 0, NULL );
  FabricCore::RTVal t = FabricCore::RTVal::ConstructSInt32( m_val.getContext(), i );
  m_val.callMethod( "", "setPostInfinityType", 1, &t );
  emit this->dirty();
}

void RTValAnimXFCurveModel::setPreInfinityType( size_t i )
{
  assert( m_val.isValid() );
  if( m_val.isNullObject() )
    m_val = FabricCore::RTVal::Create( m_val.getContext(), "AnimX::AnimCurve", 0, NULL );
  FabricCore::RTVal t = FabricCore::RTVal::ConstructSInt32( m_val.getContext(), i );
  m_val.callMethod( "", "setPreInfinityType", 1, &t );
  emit this->infinityTypesChanged();
}

void RTValAnimXFCurveModel::setPostInfinityType( size_t i )
{
  assert( m_val.isValid() );
  if( m_val.isNullObject() )
    m_val = FabricCore::RTVal::Create( m_val.getContext(), "AnimX::AnimCurve", 0, NULL );
  FabricCore::RTVal t = FabricCore::RTVal::ConstructSInt32( m_val.getContext(), i );
  m_val.callMethod( "", "setPostInfinityType", 1, &t );
  emit this->infinityTypesChanged();
}

void RTValAnimXFCurveVersionedModel::autoTangent( size_t id )
{
  assert( m_val.isValid() );
  FabricCore::RTVal index = this->idToIndex( id );
  m_val.callMethod( "", "autoTangent", 1, &index );
  emit this->dirty();
}

void RTValAnimXFCurveModel::autoTangent( size_t id )
{
  assert( m_val.isValid() );
  FabricCore::RTVal index = this->idToIndex( id );
  m_val.callMethod( "", "autoTangent", 1, &index );
  emit this->keyMoved( id );
}
