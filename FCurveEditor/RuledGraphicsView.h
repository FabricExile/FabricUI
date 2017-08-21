//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H
#define FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H

#include <QFrame>
#include <FTL/Config.h>

class QTimer;
class QGraphicsView;

namespace FabricUI
{
namespace FCurveEditor
{

class RuledGraphicsView : public QFrame
{
  Q_OBJECT

  Q_PROPERTY( int rulersSize READ rulersSize WRITE setRulersSize )
  int m_rulersSize;

  // Do the Y coordinates go from top-to-bottom (default) or from bottom-to-top ?
  Q_PROPERTY( bool topToBottomY READ topToBottomY WRITE setTopToBottomY )

  // Color of the grid in the background (alpha also works)
  Q_PROPERTY( QColor gridColor READ gridColor WRITE setGridColor )
  QColor m_gridColor;

  // the average space (in px) between 2 of the grid's thickest lines
  Q_PROPERTY( int gridMaxSpacing READ gridMaxSpacing WRITE setGridMaxSpacing )
  int m_gridMaxSpacing;

  // same as gridMaxSpacing, but for the thinest lines (must be smaller than gridMaxSpacing)
  Q_PROPERTY( int gridMinSpacing READ gridMinSpacing WRITE setGridMinSpacing )
  int m_gridMinSpacing;

  // thickness of the thickest grid lines (in pixels)
  Q_PROPERTY( qreal gridThickness READ gridThickness WRITE setGridThickness )
  qreal m_gridThickness;

  // if true, the thickness will be squarred and thus decrease faster with scales
  Q_PROPERTY( bool gridThicknessSquarred READ gridThicknessSquarred WRITE setGridThicknessSquarred )
  bool m_gridThicknessSquarred;

  // the grid lines will be logarithmic multiples of that scale
  Q_PROPERTY( qreal logScale READ logScale WRITE setLogScale )
  qreal m_logScale;

  bool m_rectangleSelectionEnabled;

public:
  RuledGraphicsView();
  QGraphicsView* view();

  // TODO : custom scale (see FCurveEditor::Ruler's properties)
  // TODO : fix the "smoothZoom" on Linux

  void fitInView( const QRectF, qreal margin = 0.15 );
  inline int rulersSize() const { return m_rulersSize; }
  void setRulersSize( const int );
  bool topToBottomY() const;
  void setTopToBottomY( bool );
  inline void enableRectangleSelection( bool e ) { m_rectangleSelectionEnabled = e; }

  inline QColor gridColor() const { return m_gridColor; }
  inline void setGridColor( const QColor& c ) { m_gridColor = c; this->update(); }

  inline int gridMaxSpacing() const { return m_gridMaxSpacing; }
  inline void setGridMaxSpacing( int s ) { m_gridMaxSpacing = std::max<int>( 1, s ); this->update(); }
  inline int gridMinSpacing() const { return m_gridMinSpacing; }
  inline void setGridMinSpacing( int s ) { m_gridMinSpacing = std::max<int>( 1, s ); this->update(); }
  inline qreal gridThickness() const { return m_gridThickness; }
  inline void setGridThickness( qreal t ) { m_gridThickness = t; this->update(); }
  inline bool gridThicknessSquarred() const { return m_gridThicknessSquarred; }
  inline void setGridThicknessSquarred( bool s ) { m_gridThicknessSquarred = s; this->update(); }
  inline qreal logScale() const { return m_logScale; }
  inline void setLogScale( qreal s ) { if( s > 0 ) { m_logScale = s; this->update(); } }

signals:
  void rectangleSelectReleased( const QRectF&, Qt::KeyboardModifiers ) const;

protected:
  void wheelEvent( QWheelEvent * ) FTL_OVERRIDE;
  void resizeEvent( QResizeEvent * ) FTL_OVERRIDE;

private slots:
  void tick();

private:

  void wheelEvent(
    int xDelta,
    int yDelta,
    // center (in scene-space) of the scaling
    // it will be a fixed-point in the view coordinates
    QPointF scalingCenter
  );
  void centeredScale( qreal x, qreal y );

  class GraphicsView;
  GraphicsView* m_view;
  class Ruler;
  void updateRulersRange();
  Ruler* m_hRuler;
  Ruler* m_vRuler;

  qreal m_scrollSpeed;
  bool m_zoomOnCursor;

  // Smooth zoom (animated)
  bool m_smoothZoom;
  QPointF m_scalingCenter;
  QPointF m_targetScale;
  QTimer* m_timer;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H
