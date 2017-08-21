//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_RULER_H
#define FABRICUI_FCURVEEDITOR_RULER_H

#include <QFrame>
#include <FTL/Config.h>

namespace FabricUI
{
namespace FCurveEditor
{

class Ruler : public QFrame
{
  Q_OBJECT

  typedef QFrame Parent;

  // maximum space (in pixels) between 2 major graduations
  Q_PROPERTY( int majorGradsPixelSpacing READ majorGradsPixelsSpacing WRITE setMajorGradsPixelSpacing )

  // number of minor graduations between two major graduations
  Q_PROPERTY( qreal majToMinGradsRatio READ majToMinGradsRatio WRITE setMajToMinGradsRatio )

  // the major graduations displayed will be logarithmic multiples of that scale
  Q_PROPERTY( qreal logScale READ logScale WRITE setLogScale )

  Q_PROPERTY( QColor penColor READ penColor WRITE setPenColor )
  Q_PROPERTY( QColor penTextColor READ penTextColor WRITE setPenTextColor )
  Q_PROPERTY( qreal majorPenWidth READ majorPenWidth WRITE setMajorPenWidth )
  Q_PROPERTY( qreal minorPenWidth READ minorPenWidth WRITE setMinorPenWidth )

  // ratio between the size of the graduation, and the breadth (smaller side) of the ruler (in [0;1])
  Q_PROPERTY( qreal majGradToBreadthRatio READ majGradToBreadthRatio WRITE setMajGradToBreadthRatio )
  Q_PROPERTY( qreal minGradToBreadthRatio READ minGradToBreadthRatio WRITE setMinGradToBreadthRatio )
  Q_PROPERTY( qreal textBreadthPos READ textBreadthPos WRITE setTextBreadthPos )

public:

  enum Orientation
  {
    Top,
    Bottom,
    Left,
    Right
  };

private:

  qreal m_start, m_end;
  Orientation m_orientation;
  int m_majorGradsPixelSpacing;
  qreal m_majToMinGradsRatio;
  qreal m_logScale;
  QColor m_penColor;
  QColor m_penTextColor;
  qreal m_majorPenWidth;
  qreal m_minorPenWidth;
  qreal m_majGradToBreadthRatio;
  qreal m_minGradToBreadthRatio;
  qreal m_textBreadthPos;

public:

  // TODO : properties
  // - scale : base2, base10, "mixed-grading", etc...
  // - labels for large/small numbers : [ "0.5", "0.50123" ] or [ "1000000", "2000000" ] 

  Ruler( Orientation );
  void setRange( qreal start, qreal end );

  inline int majorGradsPixelsSpacing() const { return m_majorGradsPixelSpacing; }
  inline void setMajorGradsPixelSpacing( int s ) { m_majorGradsPixelSpacing = s; this->update(); }
  inline qreal majToMinGradsRatio() const { return m_majToMinGradsRatio; }
  inline void setMajToMinGradsRatio( qreal r ) { m_majToMinGradsRatio = r; this->update(); }
  inline qreal logScale() const { return m_logScale; }
  inline void setLogScale( qreal s ) { if( s > 0 ) { m_logScale = s; this->update(); } }

  inline QColor penColor() const { return m_penColor; }
  inline void setPenColor( QColor c ) { m_penColor = c; this->update(); }
  inline QColor penTextColor() const { return m_penTextColor; }
  inline void setPenTextColor( QColor c ) { m_penTextColor = c; this->update(); }
  inline qreal majorPenWidth() const { return m_majorPenWidth; }
  inline void setMajorPenWidth( qreal w ) { m_majorPenWidth = w; this->update(); }
  inline qreal minorPenWidth() const { return m_minorPenWidth; }
  inline void setMinorPenWidth( qreal w ) { m_minorPenWidth = w; this->update(); }

  inline qreal majGradToBreadthRatio() const { return m_majGradToBreadthRatio; }
  inline void setMajGradToBreadthRatio( qreal r )
    { m_majGradToBreadthRatio = std::max<qreal>( 0, std::min<qreal>( 1, r ) ); this->update(); }
  inline qreal minGradToBreadthRatio() const { return m_minGradToBreadthRatio; }
  inline void setMinGradToBreadthRatio( qreal r )
  { m_minGradToBreadthRatio = std::max<qreal>( 0, std::min<qreal>( 1, r ) ); this->update(); }
  inline qreal textBreadthPos() const { return m_textBreadthPos; }
  inline void setTextBreadthPos( qreal p )
    { m_textBreadthPos = std::max<qreal>( 0, std::min<qreal>( 1, p ) ); this->update(); }

protected:
  void paintEvent( QPaintEvent * ) FTL_OVERRIDE;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RULER_H
