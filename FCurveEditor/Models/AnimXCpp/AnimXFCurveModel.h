//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_ANIMXFCURVEMODEL_H
#define FABRICUI_FCURVEEDITOR_ANIMXFCURVEMODEL_H

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <FTL/Config.h>

#include <animx.h>
#include <vector>
namespace FabricUI
{
namespace FCurveEditor
{

class AnimxFCurveModel : public AbstractFCurveModel, public adsk::ICurve
{
  Q_OBJECT

  struct UIKey
  {
    adsk::Keyframe key;
    size_t uiId;
  };
  std::vector<UIKey> m_keys;
  std::vector<size_t> m_uiIdToIndex;
  adsk::InfinityType m_preInfType, m_postInfType;

  inline void swap( const size_t a, const size_t b )
  {
    UIKey tmp = m_keys[a];
    m_keys[a] = m_keys[b];
    m_keys[b] = tmp;
    m_keys[a].key.index = a;
    m_keys[b].key.index = b;
    m_uiIdToIndex[m_keys[a].uiId] = a;
    m_uiIdToIndex[m_keys[b].uiId] = b;
  }

public:

  AnimxFCurveModel();

  // AbstractFCurveModel
  size_t getKeyCount() const FTL_OVERRIDE { return m_keys.size(); }
  Key getKey( size_t uiId ) const FTL_OVERRIDE;
  Key getOrderedKey( size_t ) const FTL_OVERRIDE;
  size_t getIndexAfterTime( qreal ) const FTL_OVERRIDE;
  void setKey( size_t uiId, Key ) FTL_OVERRIDE;
  void addKey() FTL_OVERRIDE;
  void deleteKey( size_t uiId ) FTL_OVERRIDE;
  void autoTangent( size_t ) FTL_OVERRIDE;
  qreal evaluate( qreal v ) const FTL_OVERRIDE;
  size_t tangentTypeCount() const FTL_OVERRIDE { return size_t(adsk::TangentType::StepNext) + 1; }
  QString tangentTypeName( size_t i ) const FTL_OVERRIDE;
  size_t infinityTypeCount() const FTL_OVERRIDE { return size_t( adsk::InfinityType::Oscillate ) + 1; }
  QString infinityTypeName( size_t i ) const FTL_OVERRIDE;
  size_t getPreInfinityType() const FTL_OVERRIDE;
  size_t getPostInfinityType() const FTL_OVERRIDE;
  void setPreInfinityType( size_t ) FTL_OVERRIDE;
  void setPostInfinityType( size_t ) FTL_OVERRIDE;

  // adsk::ICurve
  bool keyframeAtIndex( int, adsk::Keyframe& ) const FTL_OVERRIDE;
  bool keyframe( double, adsk::Keyframe& ) const FTL_OVERRIDE;
  bool first( adsk::Keyframe& ) const FTL_OVERRIDE;
  bool last( adsk::Keyframe& ) const FTL_OVERRIDE;
  adsk::InfinityType preInfinityType() const FTL_OVERRIDE { return m_preInfType; }
  adsk::InfinityType postInfinityType() const FTL_OVERRIDE { return m_postInfType; }
  bool isWeighted() const FTL_OVERRIDE { return true; } // TODO
  unsigned int keyframeCount() const FTL_OVERRIDE { return m_keys.size(); }
  bool isStatic() const FTL_OVERRIDE { return false; } // TODO
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_ANIMXFCURVEMODEL_H
