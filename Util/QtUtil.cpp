//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <QWidget>
#include "QtUtil.h"
#include <QApplication>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;

QMainWindow* QtUtil::getMainWindow() 
{
  foreach(QWidget *wiget, QApplication::topLevelWidgets())
  { 
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(wiget);
    if(mainWindow != 0)
      return mainWindow;
  }
  return 0;
}

QGLWidget* QtUtil::getQGLWidget(
	QString widgetName) 
{
  return getQWidget<QGLWidget>(widgetName);
}

QDockWidget* QtUtil::getDockWidget(
	QString dockName) 
{
  return getQWidget<QDockWidget>(dockName);
}
