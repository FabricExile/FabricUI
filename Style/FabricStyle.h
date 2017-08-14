// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_Style_FabricStyle__
#define __UI_Style_FabricStyle__

#include <QWidget>
#include <QProxyStyle>
#include <QApplication>
#include <QStyleFactory>

namespace FabricUI
{

  namespace Style
  {

    class FabricStyle : public QProxyStyle
    {
        Q_OBJECT

    public:
        FabricStyle() : QProxyStyle(QStyleFactory::create("windows")) {}

        virtual void polish(QPalette &palette);
    };

class FabricStyleUtil
{
  public:
    static void applyStyle(
      QApplication *app
      );

    static void applyStyle(
      QWidget *widget
      );
};

  };

};

#endif // __UI_Style_FabricStyle__
