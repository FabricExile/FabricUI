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
  // FE-8859
  // In Qt, a QAction is performed only if its owner widget has the focus. 
  // The usual way to set the focus to a widget is to click on it, or press tab.
  // Manipulation are toggled using a QAction owned by the viewport. We don't 
  // want to have to click on the viewport so it has the focus. Instead, we detect 
  // in the event filter if the key pressed correpond to the shortcut that toogles 
  // the manipulation and set the focus accordingly.

  // When a key is pressed, QEvent::ShortcutOverride and QEvent::KeyPress are both fired,
  // QEvent::ShortcutOverride is always called first, use it to catch the key pressed event.
  if(event->type() != QEvent::KeyPress)
	{
    if(event->type() == QEvent::ShortcutOverride)
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

      // Check if the key pressed toggles the manipulation
      QAction *action;
      foreach(action, m_viewport->actions())
      {
        // If so, sets the focus
        if(QKeySequence(keyEvent->text()) == action->shortcut())
          m_viewport->setFocus();       
      }
    }
    
    m_viewport->onEvent(event);
  }
  return false;
}
