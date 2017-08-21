#include "AnimXFCurveModel.h"

#include <assert.h>

using namespace FabricUI::FCurveEditor;
using namespace adsk;

inline const char* TangentTypeName( const TangentType& t )
{
  switch( t )
  {
  case TangentType::Global: return "Global";
  case TangentType::Fixed: return "Fixed";
  case TangentType::Linear: return "Linear";
  case TangentType::Flat: return "Flat";
  case TangentType::Step: return "Step";
  case TangentType::Slow: return "Slow";
  case TangentType::Fast: return "Fast";
  case TangentType::Smooth: return "Smooth";
  case TangentType::Clamped: return "Clamped";
  case TangentType::Auto: return "Auto";
  case TangentType::Sine: return "Sine";
  case TangentType::Parabolic: return "Parabolic";
  case TangentType::Log: return "Log";
  case TangentType::Plateau: return "Plateau";
  case TangentType::StepNext: return "StepNext";
  }
  assert( false );
  return "";
}

inline size_t TangentTypeIndex( const TangentType& t )
{
  switch( t )
  {
  case TangentType::Global: return 0;
  case TangentType::Fixed: return 1;
  case TangentType::Linear: return 2;
  case TangentType::Flat: return 3;
  case TangentType::Step: return 4;
  case TangentType::Slow: return 5;
  case TangentType::Fast: return 6;
  case TangentType::Smooth: return 7;
  case TangentType::Clamped: return 8;
  case TangentType::Auto: return 9;
  case TangentType::Sine: return 10;
  case TangentType::Parabolic: return 11;
  case TangentType::Log: return 12;
  case TangentType::Plateau: return 13;
  case TangentType::StepNext: return 14;
  }
  assert( false );
  return 0;
}

const TangentType TangentTypes[] =
{
  TangentType::Global,
  TangentType::Fixed,
  TangentType::Linear,
  TangentType::Flat,
  TangentType::Step,
  TangentType::Slow,
  TangentType::Fast,
  TangentType::Smooth,
  TangentType::Clamped,
  TangentType::Auto,
  TangentType::Sine,
  TangentType::Parabolic,
  TangentType::Log,
  TangentType::Plateau,
  TangentType::StepNext
};

inline TangentType TangentTypeFromIndex( size_t i ) { return TangentTypes[i]; }

QString AnimxFCurveModel::tangentTypeName( size_t i ) const
{
  assert( i < this->tangentTypeCount() );
  return QString::fromUtf8( TangentTypeName( TangentType( i ) ) );
}

inline const char* InfinityTypeName( const InfinityType& t )
{
  switch( t )
  {
  case InfinityType::Constant: return "Constant";
  case InfinityType::Cycle: return "Cycle";
  case InfinityType::CycleRelative: return "CycleRelative";
  case InfinityType::Linear: return "Linear";
  case InfinityType::Oscillate: return "Oscillate";
  }
  assert( false );
  return "";
}

inline size_t InfinityTypeIndex( const InfinityType& t )
{
  switch( t )
  {
  case InfinityType::Constant: return 0;
  case InfinityType::Cycle: return 1;
  case InfinityType::CycleRelative: return 2;
  case InfinityType::Linear: return 3;
  case InfinityType::Oscillate: return 4;
  }
  assert( false );
  return 0;
}

const InfinityType InfinityTypes[] =
{
  InfinityType::Constant,
  InfinityType::Cycle,
  InfinityType::CycleRelative,
  InfinityType::Linear,
  InfinityType::Oscillate,
};

inline InfinityType InfinityTypeFromIndex( size_t i ) { return InfinityTypes[i]; }

QString AnimxFCurveModel::infinityTypeName( size_t i ) const
{
  return InfinityTypeName( InfinityTypeFromIndex( i ) );
}

AnimxFCurveModel::AnimxFCurveModel()
  : m_preInfType( InfinityType::Constant )
  , m_postInfType( InfinityType::Constant )
{
}

size_t AnimxFCurveModel::getPreInfinityType() const
{
  return InfinityTypeIndex( m_preInfType );
}

size_t AnimxFCurveModel::getPostInfinityType() const
{
  return InfinityTypeIndex( m_postInfType );
}

void AnimxFCurveModel::setPreInfinityType( size_t i )
{
  InfinityType t = InfinityTypeFromIndex( i );
  if( t != m_preInfType )
  {
    m_preInfType = t;
    emit this->infinityTypesChanged();
  }
}

void AnimxFCurveModel::setPostInfinityType( size_t i )
{
  InfinityType t = InfinityTypeFromIndex( i );
  if( t != m_postInfType )
  {
    m_postInfType = t;
    emit this->infinityTypesChanged();
  }
}

Key AnimxFCurveModel::getOrderedKey( size_t index ) const
{
  assert( index < m_keys.size() );
  const Keyframe& key = m_keys[index].key;
  Key dst;
  dst.pos = QPointF( key.time, key.value );
  dst.tanIn = QPointF( key.tanIn.x, key.tanIn.y );
  dst.tanOut = QPointF( key.tanOut.x, key.tanOut.y );
  dst.tanInType = TangentTypeIndex( key.tanIn.type );
  dst.tanOutType = TangentTypeIndex( key.tanOut.type );
  return dst;
}

Key AnimxFCurveModel::getKey( size_t uiId ) const
{
  assert( uiId < m_keys.size() );
  return this->getOrderedKey( m_uiIdToIndex[uiId] );
}

void AnimxFCurveModel::setKey( size_t uiId, Key h )
{
  assert( uiId < m_keys.size() );
  const size_t i = m_uiIdToIndex[uiId];
  Keyframe& key = m_keys[i].key;
  key.time = h.pos.x();
  key.value = h.pos.y();
  key.tanIn.x = h.tanIn.x();
  key.tanIn.y = h.tanIn.y();
  key.tanOut.x = h.tanOut.x();
  key.tanOut.y = h.tanOut.y();
  key.tanIn.type = TangentTypeFromIndex( h.tanInType );
  key.tanOut.type = TangentTypeFromIndex( h.tanOutType );
  key.linearInterpolation = false; // HACK/TODO
  key.index = i;

  // Reordering the keys
#if 1
  {
    size_t disorderIndex = i;
    while( disorderIndex > 0 && m_keys[disorderIndex - 1].key.time > m_keys[disorderIndex].key.time )
    {
      this->swap( disorderIndex - 1, disorderIndex );
      disorderIndex--;
    }
    while( disorderIndex < m_keys.size() - 1 && m_keys[disorderIndex].key.time > m_keys[disorderIndex + 1].key.time )
    {
      this->swap( disorderIndex, disorderIndex + 1 );
      disorderIndex++;
    }
  }
#else
  { // doesn't scale well with the number of key (only for debug)
    std::sort( m_keys.begin(), m_keys.end(), []( const UIKey& a, const UIKey& b ) { return a.key.time < b.key.time; } );
    for( size_t i = 0; i < m_keys.size(); i++ )
      m_uiIdToIndex[m_keys[i].uiId] = i;
  }
#endif

  emit this->keyMoved( uiId );
}

void AnimxFCurveModel::addKey()
{
  UIKey key;
  key.key = adsk::Keyframe();
  key.uiId = m_keys.size();
  m_keys.push_back( key );
  m_uiIdToIndex.push_back( m_keys.size()-1 );
  emit this->keyAdded();
}

void AnimxFCurveModel::deleteKey( size_t uiId )
{
  size_t index = m_uiIdToIndex[uiId];

  // TODO : more efficient way to delete keys ?

  // Shifting the local indices
  for( size_t i = index; i < m_keys.size()-1; i++ )
  {
    m_keys[i] = m_keys[i + 1];
    m_keys[i].key.index--;
    m_uiIdToIndex[m_keys[i].uiId] = i;
  }
  m_keys.resize( m_keys.size() - 1 );

  // Shifting the UI indices
  for( size_t i = uiId; i < m_uiIdToIndex.size() - 1; i++ )
  {
    m_uiIdToIndex[i] = m_uiIdToIndex[i + 1];
    m_keys[m_uiIdToIndex[i]].uiId = i;
  }
  m_uiIdToIndex.resize( m_uiIdToIndex.size() - 1 );

  emit this->keyDeleted( uiId );
}


void AnimxFCurveModel::autoTangent( size_t uiId )
{
  if( m_keys.size() == 1 )
    return;
  const size_t index = m_uiIdToIndex[uiId];
  for( size_t dir = 0; dir <= 1; dir++ )
    adsk::autoTangent(
      dir == 0,
      m_keys[index].key,
      ( index > 0 ? &m_keys[index - 1].key : NULL ),
      ( index < m_keys.size()-1 ? &m_keys[index + 1].key : NULL ),
      m_keys[index].key.curveInterpolationMethod(this->isWeighted()),
      ( dir == 0 ? m_keys[index].key.tanIn.x : m_keys[index].key.tanOut.x ),
      ( dir == 0 ? m_keys[index].key.tanIn.y : m_keys[index].key.tanOut.y )
    );
  emit this->keyMoved( uiId );
}

qreal AnimxFCurveModel::evaluate( qreal v ) const
{
  return evaluateCurve( v, *this );
}

bool AnimxFCurveModel::keyframeAtIndex( int i, adsk::Keyframe& k ) const
{
  if( i >= 0 && i < m_keys.size() )
  {
    k = m_keys[i].key;
    return true;
  }
  return false;
}

bool AnimxFCurveModel::keyframe( double time, adsk::Keyframe& key ) const
{
  if( m_keys.size() == 0 )
    return false;

  // TODO : implement this efficiently (binary search tree ?)
  size_t closestI = m_keys.size() - 1;
  double closestT = INFINITY;
  for( size_t i = 0; i < m_keys.size(); i++ )
  {
    if( m_keys[i].key.time >= time && m_keys[i].key.time < closestT )
    {
      closestI = i;
      closestT = m_keys[i].key.time;
    }
  }
  key = m_keys[closestI].key;
  return true;
}

size_t AnimxFCurveModel::getIndexAfterTime( qreal time ) const
{
  adsk::Keyframe key;
  bool valid = this->keyframe( time, key );
  assert( valid );
  return key.index;
}

bool AnimxFCurveModel::first( adsk::Keyframe& k ) const
{
  if( m_keys.size() > 0 )
  {
    k = m_keys[0].key;
    return true;
  }
  return false;
}

bool AnimxFCurveModel::last( adsk::Keyframe& k ) const
{
  if( m_keys.size() > 0 )
  {
    k = m_keys[m_keys.size()-1].key;
    return true;
  }
  return false;
}
