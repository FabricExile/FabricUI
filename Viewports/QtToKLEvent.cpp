/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */
 
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "QtToKLEvent.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace Application;

FabricCore::RTVal QtToKLMousePosition(
  QPoint pos, 
  RTVal viewport, 
  bool swapAxis) 
{
  RTVal klpos;

  try
  {
    Context context = viewport.getContext();
    RTVal klViewportDim = viewport.callMethod("Vec2", "getDimensions", 0, 0);
    klpos = RTVal::Construct(context, "Vec2", 0, 0);
    klpos.setMember("x", RTVal::ConstructFloat32(context, pos.x()));
    // We must inverse the y coordinate to match Qt/RTR viewport system of coordonates
    if(swapAxis)
      klpos.setMember("y", RTVal::ConstructFloat32(context, klViewportDim.maybeGetMember("y").getFloat32() - pos.y()));
    else
      klpos.setMember("y", RTVal::ConstructFloat32(context, pos.y()));
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "QtToKLEvent::QtToKLMousePosition",
      "",
      e.getDesc_cstr());
  }

  return klpos;
}

RTVal QtToKLEvent(
  QEvent *event, 
  RTVal viewport, 
  char const *hostName, 
  bool swapAxis) 
{
  RTVal klevent;
  
  try
  {
    Context context = viewport.getContext();

    if(event->type() == QEvent::Enter)
      klevent = RTVal::Create(context, "MouseEvent", 0, 0);
    
    else if(event->type() == QEvent::Leave)
      klevent = RTVal::Create(context, "MouseEvent", 0, 0);

    else if ( event->type() == QEvent::KeyPress || 
              event->type() == QEvent::ShortcutOverride || 
              event->type() == QEvent::KeyRelease) 
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      klevent = RTVal::Create(context, "KeyEvent", 0, 0);
      klevent.setMember("key", RTVal::ConstructUInt32(context, keyEvent->key()));
      klevent.setMember("count", RTVal::ConstructUInt32(context, keyEvent->count()));
      klevent.setMember("isAutoRepeat", RTVal::ConstructBoolean(context, keyEvent->isAutoRepeat()));
    } 

    else if(event->type() == QEvent::MouseMove || 
            event->type() == QEvent::MouseButtonDblClick || 
            event->type() == QEvent::MouseButtonPress || 
            event->type() == QEvent::MouseButtonRelease) 
    {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
      klevent = RTVal::Create(context, "MouseEvent", 0, 0);
      RTVal klpos = QtToKLMousePosition(mouseEvent->pos(), viewport, swapAxis);
      klevent.setMember("button", RTVal::ConstructUInt32(context, mouseEvent->button()));
      klevent.setMember("buttons", RTVal::ConstructUInt32(context, mouseEvent->buttons()));
      klevent.setMember("pos", klpos);
    } 

    else if (event->type() == QEvent::Wheel) 
    {
      QWheelEvent *mouseWheelEvent = static_cast<QWheelEvent *>(event);
      klevent = RTVal::Create(context, "MouseWheelEvent", 0, 0);
      RTVal klpos = QtToKLMousePosition(mouseWheelEvent->pos(), viewport, swapAxis);
      klevent.setMember("buttons", RTVal::ConstructUInt32(context, mouseWheelEvent->buttons()));
      klevent.setMember("delta", RTVal::ConstructSInt32(context, mouseWheelEvent->delta()));
      klevent.setMember("pos", klpos);
    }

    if(klevent.isValid())
    {
      int eventType = event->type() == QEvent::ShortcutOverride 
        ? int(QEvent::KeyPress)
        : int(event->type());
    
      // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent.setMember("eventType", RTVal::ConstructUInt32(context, eventType));

      QInputEvent *inputEvent = static_cast<QInputEvent *>(event);
      klevent.setMember("modifiers", RTVal::ConstructUInt32(context, inputEvent->modifiers()));

      //////////////////////////
      // Setup the viewport
      klevent.setMember("viewport", viewport);

      //////////////////////////
      // Setup the Host
      // We cannot set an interface value via RTVals.
      RTVal host = RTVal::Create(context, "Host", 0, 0);
      if ( hostName )
        host.setMember("hostName", RTVal::ConstructString(context, hostName ));
      else
        host.setMember("hostName", RTVal::ConstructString(context, "Canvas"));
      klevent.setMember("host", host);
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "QtToKLEvent::QtToKLEvent",
      "",
      e.getDesc_cstr());
  }

  return klevent;
}
