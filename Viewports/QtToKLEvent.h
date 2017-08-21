/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_QT_TO_KL_EVENT__
#define __UI_QT_TO_KL_EVENT__

#include <QEvent>
#include <QPoint>
#include <FabricCore.h>

FabricCore::RTVal QtToKLMousePosition(
  QPoint pos, 
  FabricCore::RTVal viewport, 
  bool swapAxis = false
  );

FabricCore::RTVal QtToKLEvent(
  QEvent *event, 
  FabricCore::RTVal viewport, 
  char const *hostName, 
  bool swapAxis = false
  );

#endif // __UI_QT_TO_KL_EVENT__
