// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "FabricStyle.h"

#include <QStyleHintReturn>
#include <QStyleOptionComplex>
#include <QStyleOptionButton>
#include <QStyleOption>

#include <FabricUI/Util/Config.h>

using namespace FabricUI::Style;

namespace FabricUI {
namespace Util {

template<>
QBrush FabricUI::Util::ConfigSection::getOrCreateValue( const FTL::StrRef key, QBrush value )
{ return QBrush( getOrCreateValue( key, value.color() ) ); }

}} // namespace FabricUI::Util

void FabricStyle::polish(QPalette &palette)
{
  QColor baseColor = QColor(60, 60, 60);
  QColor highlightColor = QColor(240, 240, 240);
  float brightnessSpread = 4.5f;
  float spread = brightnessSpread;

  if(baseColor.toHsv().valueF() > 0.5)
    spread = 100.0f / brightnessSpread;
  else
    spread = 100.0f * brightnessSpread;
  
  QColor highlightedTextColor;
  if(highlightColor.toHsv().valueF() > 0.6)
    highlightedTextColor= baseColor.darker(int(spread*2));
  else
    highlightedTextColor= baseColor.lighter(int(spread*2));

  FabricUI::Util::Config rootConfig;
  FabricUI::Util::ConfigSection& cfg = rootConfig.getOrCreateSection( "FabricStyle" );

#define PALETTE_SET_BRUSH( parameter, defaultValue ) \
  palette.setBrush( parameter, cfg.getOrCreateValue( #parameter, defaultValue ) )
  
  PALETTE_SET_BRUSH(QPalette::Background, QBrush(baseColor));
  PALETTE_SET_BRUSH(QPalette::Window, QBrush(baseColor));
  PALETTE_SET_BRUSH(QPalette::Foreground, baseColor.lighter(int(spread)));
  PALETTE_SET_BRUSH(QPalette::WindowText, baseColor.lighter(int(spread)));
  PALETTE_SET_BRUSH(QPalette::Base, baseColor);
  PALETTE_SET_BRUSH(QPalette::AlternateBase, baseColor.darker(int(spread)));
  PALETTE_SET_BRUSH(QPalette::ToolTipBase, baseColor);
  PALETTE_SET_BRUSH(QPalette::ToolTipText, baseColor.lighter(int(spread)));
  PALETTE_SET_BRUSH(QPalette::Text, baseColor.lighter(int(spread*1.2)));
  PALETTE_SET_BRUSH(QPalette::Button, baseColor);
  PALETTE_SET_BRUSH(QPalette::ButtonText, baseColor.lighter(int(spread)));
  PALETTE_SET_BRUSH(QPalette::BrightText, QColor(240, 240, 240));
  PALETTE_SET_BRUSH(QPalette::Light, baseColor.lighter(int(spread/2)));
  PALETTE_SET_BRUSH(QPalette::Midlight, baseColor.lighter(int(spread/4)));
  PALETTE_SET_BRUSH(QPalette::Dark, baseColor.darker(int(spread/4)));
  PALETTE_SET_BRUSH(QPalette::Mid, baseColor);
  PALETTE_SET_BRUSH(QPalette::Shadow, baseColor.darker(int(spread/2)));
  PALETTE_SET_BRUSH(QPalette::Highlight, highlightColor);
  PALETTE_SET_BRUSH(QPalette::HighlightedText, highlightedTextColor);
}

void FabricStyleUtil::applyFabricStyle(
  QApplication *app)
{
  app->setStyle(new FabricStyle());
}

void FabricStyleUtil::applyFabricStyle(
  QWidget *widget)
{
  widget->setStyle(new FabricStyle());
}
