// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_STYLE_FABRICSTYLE__
#define __UI_STYLE_FABRICSTYLE__

#include <QWidget>
#include <QProxyStyle>
#include <QApplication>
#include <QStyleFactory>

namespace FabricUI {
namespace Style {

class FabricStyle : public QProxyStyle
{
  Q_OBJECT

  public:
    FabricStyle() : QProxyStyle(QStyleFactory::create("windows")) {}

    virtual void polish(
      QPalette &palette
      );
};

class FabricStyleUtil
{
  public:
    static void applyFabricStyle(
      QApplication *app
      );

    static void applyFabricStyle(
      QWidget *widget
      );
};

} // namespace Style
} // namespace FabricUI

#endif // __UI_STYLE_FABRICSTYLE__
