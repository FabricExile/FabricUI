// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphConfig__
#define __UI_GraphView_GraphConfig__

#include <QColor>
#include <QPen>
#include <QFont>

namespace FabricUI
{

  namespace GraphView
  {

    struct GraphConfig
    {
      bool useOpenGL;
      
      char const *pathSep;
      bool disconnectInputsAutomatically;
      bool middleClickDeletesConnections;

      int headerMargins;
      QColor headerBackgroundColor;
      QPen headerPen;
      QFont headerFont;
      QColor headerFontColor;

      QColor mainPanelBackgroundColor;
      QColor mainPanelHotkeyZoomBackgroundColor;
      QColor mainPanelHotkeyZoomBorderColor;
      bool mainPanelDrawGrid;
      int mainPanelGridSpan;
      bool mainPanelGridSnap;
      int mainPanelGridSnapSize;
      bool mainPanelNodeSnap;
      int mainPanelNodeSnapDistance;
      bool mainPanelPortSnap;
      int mainPanelPortSnapDistance;
      QPen mainPanelGridPen;
      bool mainPanelBackGroundPanFixed;

      QColor sidePanelBackgroundColor;
      float sidePanelCollapsedWidth;
      QPen sidePanelPen;
      float sidePanelContentMargins;
      float sidePanelSpacing;
      QFont sidePanelFont;
      QColor sidePanelFontColor;
      QColor sidePanelFontHighlightColor;
      float sidePanelPortLabelSpacing;
      float sidePanelWidthReduction;
      
      float nodeMinWidth;
      float nodeMinHeight;
      QColor nodeDefaultColor;
      QColor nodeDefaultLabelColor;
      QColor nodeInspectedOutlineColor;
      QColor nodeInspectedSelectedOutlineColor;
      QPen nodeDefaultPen;
      bool nodeDefaultPenUsesNodeColor;
      QPen nodeSelectedPen;
      QPen nodeInspectedPen;
      QPen nodeInspectedSelectedPen;
      QPen nodeErrorPen;
      QFont nodeFont;
      QColor nodeFontColor;
      QColor nodeFontHighlightColor;
      QFont instBlockHeaderFont;
      float nodeContentMargins;
      float nodeWidthReduction;
      float nodeHeaderContentMargins;
      float nodeHeaderSpacing;
      bool nodeHeaderAlwaysShowPins;
      float nodeCornerRadius;
      float nodeSpaceAbovePorts;
      float nodeSpaceBelowPorts;
      float nodePinSpacing;
      float nodePinStretch;
      bool nodeShadowEnabled;
      QColor nodeShadowColor;
      QPointF nodeShadowOffset;
      float nodeShadowBlurRadius;

      float nodeHeaderButtonSeparator;
      QString nodeHeaderButtonIconDir;

      QPen pinDefaultPen;
      bool pinUsesColorForPen;
      QPen pinHoverPen;
      QFont pinFont;
      QColor pinFontColor;
      QColor pinFontHighlightColor;
      float pinRadius;
      float pinLabelSpacing;
      bool pinDisplayAsFullCircle;
      float pinClickableDistance;
      
      float instBlockMinWidth;
      float instBlockMinHeight;

      bool dimConnectionLines;
      QPen connectionDefaultPen;
      QPen connectionExposePen;
      float connectionExposeRadius;
      QPen connectionHoverPen;
      QColor connectionColor;
      bool connectionUsePinColor;
      bool connectionShowTooltip;
      bool highlightConnectionTargets;
      float connectionNodePortColorRatio;
      float connectionClickableDistance;
      bool connectionDrawAsCurves;
      bool portsCentered;

      QColor selectionRectColor;
      QPen selectionRectPen;
      float mouseGrabberRadius;
      float mouseWheelZoomRate;

      float backDropNodeAlpha;
      float nodeBubbleMinWidth;
      float nodeBubbleMinHeight;
      QColor nodeBubbleColor;
      QFont nodeBubbleFont;

      float infoOverlayMinWidth;
      float infoOverlayMinHeight;
      QColor infoOverlayColor;
      QFont infoOverlayFont;

      QColor blockNodeDefaultColor;
      QColor blockLabelDefaultColor;

      GraphConfig();
    };

  };

};

#endif // __UI_GraphView_GraphConfig__
