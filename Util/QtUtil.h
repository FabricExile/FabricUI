//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_QT_UTIL__
#define __UI_QT_UTIL__

#include <QString>
#include <QGLWidget>
#include <QMainWindow>
#include <QDockWidget>

namespace FabricUI {
namespace Util {

class QtUtil 
{
  /**
  */
  public:
    static QMainWindow* getMainWindow();

    static QGLWidget* getQGLWidget(
      QString widgetName = ""
      );

    static QDockWidget* getDockWidget(
      QString dockName = ""
      );

    template <class T> static T* getQWidget(
      QString widgetName = "")
    {
      return getMainWindow()->findChild<T*>(
        widgetName
        );
    }
};

} // namespace FabricUI
} // namespace Util

#endif // __UI_QT_UTIL__
