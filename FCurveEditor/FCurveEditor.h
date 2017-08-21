//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H
#define FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H

#include <QFrame>
#include <QAction>
#include <FTL/Config.h>

class QGraphicsScene;

namespace FabricUI
{
namespace FCurveEditor
{
class RuledGraphicsView;
class AbstractFCurveModel;
class FCurveEditorScene;

class AbstractAction : public QAction
{
  Q_OBJECT

public:
  AbstractAction( QObject* );

protected slots:
  virtual void onTriggered() = 0;
};

/*
  An FCurveEditor is a widget to display and edit FCurves (through
  the AbstractFCurveModel interface). Internally, it uses QGraphics.
*/
class FCurveEditor : public QFrame
{
  Q_OBJECT

  typedef QFrame Parent;

  // relative position of the value editor (negative values will be from the right/bottom)
  Q_PROPERTY( QPoint vePos READ vePos WRITE setVEPos )
  QPoint m_vePos;
  Q_PROPERTY( bool toolBarEnabled READ toolBarEnabled WRITE setToolBarEnabled )
  bool m_toolbarEnabled;

  RuledGraphicsView* m_rview;
  bool m_owningScene;
  FCurveEditorScene* m_scene;
  class KeyValueEditor;
  KeyValueEditor* m_keyValueEditor;
  class ToolBar;
  ToolBar* m_toolBar;
  void veEditFinished( bool isXNotY );
  void updateVEPos();
  void deleteOwnedScene();
  void linkToScene();
  class SetInfinityTypeAction;

  QAction* m_clearAction;
  QAction* m_keysSelectAllAction;
  QAction* m_keysDeselectAllAction;
  QAction* m_keysFrameAllAction;
  QAction* m_keysFrameSelectedAction;
  QAction* m_keysDeleteAction;
  QAction* m_autoTangentsAction;
  QAction* m_tangentsZeroSlopeAction;

  QAction* m_presetRampIn;
  QAction* m_presetRampOut;
  QAction* m_presetSmoothStep;

public:
  FCurveEditor();
  ~FCurveEditor();
  void setModel( AbstractFCurveModel* );
  AbstractFCurveModel* model();
  void deriveFrom( FCurveEditor* );
  void frameAllKeys();
  void frameSelectedKeys();

  inline QPoint vePos() const { return m_vePos; }
  inline void setVEPos( const QPoint& p ) { m_vePos = p; this->updateVEPos(); }
  inline bool toolBarEnabled() const { return m_toolbarEnabled; }
  void setToolBarEnabled( bool );

protected:
  void resizeEvent( QResizeEvent * ) FTL_OVERRIDE;
  void keyPressEvent( QKeyEvent * ) FTL_OVERRIDE;
  void keyReleaseEvent( QKeyEvent * ) FTL_OVERRIDE;

private slots:

  void onRectangleSelectReleased( const QRectF&, Qt::KeyboardModifiers );
  void onSelectionChanged();
  void onEditedKeysChanged();
  void onModeChanged();
  void onSnapToCurveChanged();
  void setSnapToCurveFromButton();
  void onInfinityTypesChanged();
  void veTanTypeEditFinished();
  inline void veXEditFinished() { this->veEditFinished( true ); }
  inline void veYEditFinished() { this->veEditFinished( false ); }

  void showContextMenu(const QPoint& pos);

  // ToolBar
  void setModeSelect();
  void setModeAdd();
  void setModeRemove();

  // QActions
  void onClearAllKeys();
  void onFrameAllKeys();
  void onFrameSelectedKeys();
  void onDeleteSelectedKeys();
  void onSelectAllKeys();
  void onDeselectAllKeys();
  void onAutoTangents();
  void onTangentsZeroSlope();
  void onPresetRampIn();
  void onPresetRampOut();
  void onPresetSmoothStep();

signals:
  void interactionBegin();
  void interactionEnd();
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H
