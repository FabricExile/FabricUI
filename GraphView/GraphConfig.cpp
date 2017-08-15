// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "GraphConfig.h"

#include <FTL/Config.h>
#include <FabricUI/Util/Config.h>

using namespace FabricUI::GraphView;
using namespace FTL;

namespace FabricUI {
namespace Util {

// TODO : move it to Config.cpp if it's useful somewhere else
#include <QPen>

template<>
JSONValue* ConfigSection::createValue( const QPen v ) const
{
  JSONObject* obj = new JSONObject();
  obj->insert( "color", ConfigSection::createValue( v.color() ) );
  obj->insert( "width", ConfigSection::createValue( v.widthF() ) );
  obj->insert( "style", ConfigSection::createValue<int>( v.style() ) );
  return obj;
}

template<>
QPen ConfigSection::getValue( const JSONValue* entry ) const
{
  QPen v;
  const JSONObject* obj = entry->cast<JSONObject>();
  if ( obj->has( "color" ) ) v.setColor( getValue<QColor>( obj->get( "color" ) ) );
  if ( obj->has( "width" ) ) v.setWidthF( getValue<double>( obj->get( "width" ) ) );
  if ( obj->has( "style" ) ) v.setStyle( Qt::PenStyle( getValue<int>( obj->get( "style" ) ) ) );
  return v;
}

#include <QPointF>

template<>
JSONValue* ConfigSection::createValue( const QPointF v ) const
{
  JSONObject* obj = new JSONObject();
  obj->insert( "x", createValue( v.x() ) );
  obj->insert( "y", createValue( v.y() ) );
  return obj;
}

template<>
QPointF ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  return QPointF(
    obj->getFloat64( "x" ),
    obj->getFloat64( "y" )
  );
}

}} // namespace FabricUI::Util

GraphConfig::GraphConfig()
{
#if defined(FTL_OS_DARWIN)
  // [pzion 20160218] We can't use OpenGL drawing on OS X because you
  // can't pop up the tab-search window overtop.  We can fix this if we
  // convert the tab-search window into a QGraphicsItem, which should be 
  // quite trivial to do.
  useOpenGL = false;
#else
  useOpenGL = true;
#endif

  Util::Config rootConfig;
  Util::ConfigSection& cfg = rootConfig.getOrCreateSection( "GraphView" );

#define GET_PARAMETER( parameter, defaultValue ) \
  parameter = cfg.getOrCreateValue( #parameter, defaultValue )

#define GET_FONT( font ) \
  GET_PARAMETER( font, font )

#define GET_PEN( pen ) \
  GET_PARAMETER( pen, pen )
  
  pathSep = ".";

  GET_PARAMETER( disconnectInputsAutomatically, true );
  GET_PARAMETER( middleClickDeletesConnections, false );

  GET_PARAMETER( mainPanelBackgroundColor, QColor(58, 58, 58, 255) );

  GET_PARAMETER( mainPanelHotkeyZoomBackgroundColor, QColor(45, 45, 45, 255) );
  GET_PARAMETER( mainPanelHotkeyZoomBorderColor, QColor(255, 255, 255, 160) );

  GET_PARAMETER( mainPanelDrawGrid, true );
  GET_PARAMETER( mainPanelGridSpan, 30 );
  GET_PARAMETER( mainPanelGridSnap, true);
  GET_PARAMETER( mainPanelGridSnapSize, 10 );
  GET_PARAMETER( mainPanelNodeSnap, true );
  GET_PARAMETER( mainPanelNodeSnapDistance, 20 );
  GET_PARAMETER( mainPanelPortSnap, true );
  GET_PARAMETER( mainPanelPortSnapDistance, 20 );
  GET_PARAMETER( mainPanelGridPen, QPen(QColor(44, 44, 44, 255), 1.0) );

  GET_PARAMETER( mainPanelBackGroundPanFixed, true );
  
  GET_PARAMETER( nodeMinWidth, 100.0f );
  GET_PARAMETER( nodeMinHeight, 28.0f );
  GET_PARAMETER( nodeDefaultColor, QColor(121, 134, 143) );
  GET_PARAMETER( nodeDefaultLabelColor, QColor(80, 98, 110) );

  nodeFont = QFont("Roboto", 11, QFont::Bold);
  nodeFont.setHintingPreference( QFont::PreferFullHinting );
  nodeFont.setBold(true);
  GET_FONT( nodeFont );

  instBlockHeaderFont = QFont("Roboto", 10, QFont::Bold);
  instBlockHeaderFont.setHintingPreference( QFont::PreferFullHinting );
  instBlockHeaderFont.setBold(true);
  GET_FONT( instBlockHeaderFont );

  GET_PARAMETER( nodeFontColor, QColor(20, 20, 20, 255) );
  GET_PARAMETER( nodeFontHighlightColor, QColor(195, 195, 195, 255) );
  GET_PARAMETER( nodeDefaultPen, QPen(nodeFontColor, 1.0) );
  GET_PARAMETER( nodeDefaultPenUsesNodeColor, false );
  GET_PARAMETER( nodeInspectedOutlineColor, QColor(33, 191, 198, 175) );
  GET_PARAMETER( nodeInspectedSelectedOutlineColor, QColor(33, 191, 198, 255) );

  nodeSelectedPen = QPen(Qt::SolidLine);
  nodeSelectedPen.setColor(nodeFontHighlightColor);
  nodeSelectedPen.setWidth(1);
  GET_PEN( nodeSelectedPen );

  nodeInspectedPen = QPen();
  nodeInspectedPen.setColor(nodeInspectedOutlineColor);
  nodeInspectedPen.setWidth(2);
  GET_PEN(nodeInspectedPen);

  nodeInspectedSelectedPen = QPen();
  nodeInspectedSelectedPen.setColor(nodeInspectedSelectedOutlineColor);
  nodeInspectedSelectedPen.setWidth(2);
  GET_PEN(nodeInspectedSelectedPen);

  nodeErrorPen = QPen(Qt::SolidLine);
  nodeErrorPen.setColor(QColor(255, 0, 0, 255));
  nodeErrorPen.setWidth(1);
  GET_PEN( nodeErrorPen );

  GET_PARAMETER( nodeContentMargins, 2.0f );
  GET_PARAMETER( nodeWidthReduction, 15.0f );
  GET_PARAMETER( nodeHeaderContentMargins, 4.0f );
  GET_PARAMETER( nodeHeaderSpacing, 5.0f );
  GET_PARAMETER( nodeHeaderAlwaysShowPins, true );
  GET_PARAMETER( nodeCornerRadius, 5.0f );
  GET_PARAMETER( nodeSpaceAbovePorts, 4.0f );
  GET_PARAMETER( nodeSpaceBelowPorts, 4.0f );
  GET_PARAMETER( nodePinSpacing, 7.0f );
  GET_PARAMETER( nodePinStretch, 16.0f );
  GET_PARAMETER( nodeShadowEnabled, true );
  GET_PARAMETER( nodeShadowColor, QColor(0, 0, 0, 75) );
  GET_PARAMETER( nodeShadowOffset, QPointF(2.5, 2.5) );
  GET_PARAMETER( nodeShadowBlurRadius, 10.0 );

  GET_PARAMETER( nodeHeaderButtonSeparator, 2.0f );
  GET_PARAMETER( nodeHeaderButtonIconDir, QString("${FABRIC_DIR}/Resources/Icons/") );

  pinFont = QFont("Roboto", 10, QFont::Normal);
  pinFont.setHintingPreference( QFont::PreferFullHinting );
  GET_FONT( pinFont );

  GET_PARAMETER( pinFontColor, nodeFontColor );
  GET_PARAMETER( pinFontHighlightColor, nodeFontHighlightColor );
  GET_PARAMETER( pinDefaultPen, nodeDefaultPen );
  GET_PARAMETER( pinUsesColorForPen, false );
  GET_PARAMETER( pinHoverPen, QPen(nodeFontHighlightColor, 1.5) );
  GET_PARAMETER( pinRadius, 5.5f );
  GET_PARAMETER( pinLabelSpacing, 2.0f );
  GET_PARAMETER( pinDisplayAsFullCircle, false );
  GET_PARAMETER( pinClickableDistance, 30.0f );

  GET_PARAMETER( dimConnectionLines, true );
  GET_PARAMETER( connectionUsePinColor, true );
  GET_PARAMETER( connectionShowTooltip, true );
  GET_PARAMETER( highlightConnectionTargets, true );
  GET_PARAMETER( connectionNodePortColorRatio, 0.75 );
  GET_PARAMETER( connectionColor, QColor(130, 130, 130) );
  GET_PARAMETER( connectionDefaultPen, QPen(connectionColor, 1.5) );
  GET_PARAMETER( connectionExposePen, QPen(connectionColor, 1.5) );
  GET_PARAMETER( connectionExposeRadius, 50.0 );
  GET_PARAMETER( connectionHoverPen, QPen(QColor(170, 170, 170), 2.0, Qt::SolidLine) );
  GET_PARAMETER( connectionClickableDistance, 4.0f );
  GET_PARAMETER( connectionDrawAsCurves, true );
  GET_PARAMETER( portsCentered, true );

  GET_PARAMETER( sidePanelBackgroundColor, nodeDefaultColor );
  GET_PARAMETER( sidePanelCollapsedWidth, 24.0f );
  GET_PARAMETER( sidePanelPen, QPen(Qt::NoPen) );
  GET_PARAMETER( sidePanelContentMargins, 4.0f );
  GET_PARAMETER( sidePanelSpacing, 10.0f );

  sidePanelFont = QFont("Roboto", 10, QFont::Normal);
  sidePanelFont.setHintingPreference( QFont::PreferFullHinting );
  GET_FONT( sidePanelFont );

  GET_PARAMETER( sidePanelFontColor, pinFontColor );
  GET_PARAMETER( sidePanelFontHighlightColor, QColor(255, 255, 255, 255) );
  GET_PARAMETER( sidePanelPortLabelSpacing, 4.0f );
  GET_PARAMETER( sidePanelWidthReduction, 10.5f );

  GET_PARAMETER( headerMargins, 4 );
  GET_PARAMETER( headerBackgroundColor, nodeDefaultLabelColor );
  GET_PARAMETER( headerPen, nodeDefaultPen );
  GET_PARAMETER( headerFont, nodeFont );
  GET_PARAMETER( headerFontColor, nodeFontColor );

  GET_PARAMETER( selectionRectColor, QColor(100, 100, 100, 50) );
  GET_PARAMETER( selectionRectPen, QPen(QColor(25, 25, 25), 1.0, Qt::DashLine) );

  GET_PARAMETER( mouseGrabberRadius, pinRadius * 1.75 );
  GET_PARAMETER( mouseWheelZoomRate, 0.0005f );

  GET_PARAMETER( backDropNodeAlpha, 0.45f );
  GET_PARAMETER( nodeBubbleMinWidth, 30.0 );
  GET_PARAMETER( nodeBubbleMinHeight, 13.0 );
  GET_PARAMETER( nodeBubbleColor, QColor(255, 247, 155) );

  nodeBubbleFont = nodeFont;
  nodeBubbleFont.setBold(false);
  nodeBubbleFont.setPointSizeF(7.0);
  GET_FONT( nodeBubbleFont );

  GET_PARAMETER( infoOverlayMinWidth, 100.0 );
  GET_PARAMETER( infoOverlayMinHeight, 40.0 );
  GET_PARAMETER( infoOverlayColor, QColor(178, 224, 253) );

  infoOverlayFont = nodeFont;
  infoOverlayFont.setBold(false);
  infoOverlayFont.setPointSizeF(7.0);
  GET_FONT( infoOverlayFont );

  GET_PARAMETER( instBlockMinWidth, 80.0f );
  GET_PARAMETER( instBlockMinHeight, 24.0f );

  GET_PARAMETER( blockNodeDefaultColor, QColor(193, 189, 138) );
  GET_PARAMETER( blockLabelDefaultColor, QColor(158, 153, 98) );
}
