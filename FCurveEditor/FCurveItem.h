//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_FCURVEITEM_H
#define FABRICUI_FCURVEEDITOR_FCURVEITEM_H

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <QGraphicsWidget>
#include <FTL/Config.h>
#include <set>

namespace FabricUI
{
namespace FCurveEditor
{

class FCurveEditorScene;

/*
  An FCurveItem is the QGraphics representatation of
  an FCurve (its curve, keys and tangents)
*/
class FCurveItem : public QGraphicsWidget
{
  Q_OBJECT

public:
  enum KeyProp { POSITION, TAN_IN, TAN_OUT, NOTHING };

private:
  FCurveEditorScene* m_scene;
  AbstractFCurveModel* m_curve;
  class FCurveShape;
  FCurveShape* m_curveShape;
  class KeyWidget;
  std::vector<KeyWidget*> m_keys;
  std::set<size_t> m_selectedKeys;
  KeyProp m_editedKeyProp;

  void addKey( size_t );
  void removeKeyFromSelection( size_t );
  void moveSelectedKeys( QPointF delta );
  void editKey( size_t, KeyProp p = POSITION );

public:
  FCurveItem( FCurveEditorScene* );
  inline AbstractFCurveModel* curve() { return m_curve; }
  inline AbstractFCurveModel const* curve() const { return m_curve; }
  void setCurve( AbstractFCurveModel* );
  void clearKeySelection();
  void addKeyToSelection( size_t );
  void rectangleSelect( const QRectF&, Qt::KeyboardModifiers );
  void deleteSelectedKeys();
  void selectAllKeys();
  inline KeyProp editedKeyProp() const { return m_editedKeyProp; }
  inline const std::set<size_t>& selectedKeys() const { return m_selectedKeys; }
  QRectF keysBoundingRect() const;
  void paint( QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) FTL_OVERRIDE;
  QRectF selectedKeysBoundingRect() const;

signals:
  void interactionBegin();
  void interactionEnd();
  void selectionChanged();
  void editedKeyValueChanged() const;
  void editedKeyPropChanged() const;

private slots:
  void onKeyAdded();
  void onKeyDeleted( size_t );
  void onKeyMoved( size_t );
  void onInfinityTypesChanged();
  inline void onDirty() { this->update(); }
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEITEM_H
