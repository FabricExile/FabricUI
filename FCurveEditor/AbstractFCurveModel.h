//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_ABSTRACTFCURVEMODEL_H
#define FABRICUI_FCURVEEDITOR_ABSTRACTFCURVEMODEL_H

#include <QObject>
#include <QPoint>

namespace FabricUI
{
namespace FCurveEditor
{

struct Key
{
  QPointF pos;
  QPointF tanIn, tanOut;
  size_t tanInType, tanOutType;
};

class AbstractFCurveModel : public QObject
{
  Q_OBJECT

public:

  virtual size_t getKeyCount() const = 0;
  virtual Key getKey( size_t ) const = 0;
  virtual Key getOrderedKey( size_t ) const = 0;
  virtual size_t getIndexAfterTime( qreal ) const = 0;
  virtual void setKey( size_t, Key ) {}
  virtual void moveKeys( const size_t* indices, const size_t nbIndices, QPointF delta );
  virtual void addKey() = 0;
  virtual void addKey( Key k ) { this->addKey(); this->setKey( getKeyCount() - 1, k ); }
  virtual void addKey( Key k, bool autoTangent ) { this->addKey( k ); this->autoTangent( getKeyCount() - 1 ); }
  // Deleting an key will shift the indices of all the keys after it
  virtual void deleteKey( size_t ) = 0;
  // the indices will be sorted in increasing order
  virtual void deleteKeys( const size_t* indices, const size_t nbIndices );

  virtual size_t tangentTypeCount() const = 0;
  virtual QString tangentTypeName( size_t i ) const { return QString::number( i ); }

  virtual size_t infinityTypeCount() const = 0;
  virtual QString infinityTypeName( size_t i ) const { return QString::number( i ); }
  virtual size_t getPreInfinityType() const = 0;
  virtual size_t getPostInfinityType() const = 0;
  virtual void setPreInfinityType( size_t ) = 0;
  virtual void setPostInfinityType( size_t ) = 0;

  virtual void autoTangent( size_t ) = 0;

  virtual qreal evaluate( qreal v ) const = 0;
  // TODO : evaluate arrays, to reduce the overhead

  virtual void init() {}
  // update() will pull changes from the underlying model, and emit change signals
  virtual void update() {}

signals:
  void keyAdded() const;
  void keyDeleted( size_t ) const;
  void keyMoved( size_t ) const;
  void infinityTypesChanged() const;

  // when emitted, UI items should schedule an update
  void dirty() const;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_ABSTRACTFCURVEMODEL_H
