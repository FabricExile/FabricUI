/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */
 
#include <QAction>
#include <QKeyEvent>
#include <QKeySequence>
#include "ViewportWidget.h"
#include "ViewportEventFilter.h"

using namespace FabricUI;
using namespace Viewports;

ViewportEventFilter::ViewportEventFilter(
  ViewportWidget *viewport)
 : QObject()
 , m_viewport(viewport) 
{
}

bool ViewportEventFilter::eventFilter(
  QObject *object, 
  QEvent *event)
{
  // QEvent::ShortcutOverride is always called first.
  // Use it instead of QEvent::KeyPress so we catch
  // all the key-strockes.
  if(event->type() != QEvent::KeyPress)  
  {
  	if(event->type() == QEvent::ShortcutOverride)
  	{
   		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
 
   		QAction *action;
   		foreach(action, m_viewport->actions())
   		{
	   		if(QKeySequence(keyEvent->text()) == action->shortcut())
	   			m_viewport->setFocus();	   		
   		}
  	}

    m_viewport->onEvent(event);
  }
  return false;
}
