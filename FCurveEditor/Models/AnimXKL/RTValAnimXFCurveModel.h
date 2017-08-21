//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEMODEL_H
#define FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEMODEL_H

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <FTL/Config.h>
#include <FabricCore.h>

namespace FabricUI
{
namespace FCurveEditor
{

class RTValAnimXFCurveConstModel : public AbstractFCurveModel
{
  Q_OBJECT

protected:
  // TODO : also use a pointer directly to the KL data, for better performance ?
  FabricCore::RTVal m_val;

  FabricCore::RTVal idToIndex( size_t ) const;

public:
  size_t getKeyCount() const FTL_OVERRIDE;
  Key getKey( size_t ) const FTL_OVERRIDE;
  Key getOrderedKey( size_t ) const FTL_OVERRIDE;
  size_t getIndexAfterTime( qreal ) const FTL_OVERRIDE;
  qreal evaluate( qreal v ) const FTL_OVERRIDE;
  size_t tangentTypeCount() const FTL_OVERRIDE;
  QString tangentTypeName( size_t i ) const FTL_OVERRIDE;
  size_t infinityTypeCount() const FTL_OVERRIDE;
  QString infinityTypeName( size_t i ) const FTL_OVERRIDE;
  size_t getPreInfinityType() const FTL_OVERRIDE;
  size_t getPostInfinityType() const FTL_OVERRIDE;

  inline void setValue( FabricCore::RTVal v ) { m_val = v; }
  inline FabricCore::RTVal value() { return m_val; }
};

class RTValAnimXFCurveVersionedConstModel : public RTValAnimXFCurveConstModel
{
  Q_OBJECT

  typedef RTValAnimXFCurveConstModel Parent;

protected:
  mutable size_t m_lastKeyCount;
  mutable size_t m_lastStructureVersion;
  mutable size_t m_lastValueVersion;

  void update( bool emitChanges ) const;

public:
  void init() FTL_OVERRIDE { this->update( false ); }
  void update() FTL_OVERRIDE { this->update( true ); }

  RTValAnimXFCurveVersionedConstModel()
    : m_lastKeyCount( 0 )
    , m_lastStructureVersion( 0 )
    , m_lastValueVersion( 0 )
  {}
};

class RTValAnimXFCurveVersionedModel : public RTValAnimXFCurveVersionedConstModel
{
  Q_OBJECT

public:
  void setKey( size_t, Key ) FTL_OVERRIDE;
  void addKey() FTL_OVERRIDE;
  void deleteKey( size_t ) FTL_OVERRIDE;
  void setPreInfinityType( size_t ) FTL_OVERRIDE;
  void setPostInfinityType( size_t ) FTL_OVERRIDE;
  void autoTangent( size_t ) FTL_OVERRIDE;
};

class RTValAnimXFCurveModel : public RTValAnimXFCurveConstModel
{
  Q_OBJECT

public:
  void setKey( size_t, Key ) FTL_OVERRIDE;
  void addKey() FTL_OVERRIDE;
  void deleteKey( size_t ) FTL_OVERRIDE;
  void setPreInfinityType( size_t ) FTL_OVERRIDE;
  void setPostInfinityType( size_t ) FTL_OVERRIDE;
  void autoTangent( size_t ) FTL_OVERRIDE;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEMODEL_H
