/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include "SHGLRenderer.h"
#include <FabricUI/Viewports/QtToKLEvent.h>
 
using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;


SHGLRenderer::SHGLRenderer() {
}

SHGLRenderer::SHGLRenderer(Client client) 
  : m_client(client) {
  try 
  {
    RTVal dummyGLRendererVal = RTVal::Construct(m_client, "SHGLRenderer", 0, 0);
    m_shGLRendererVal = dummyGLRendererVal.callMethod("SHRenderer", "create", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::SHGLRenderer: exception: %s\n", e.getDesc_cstr());
  }
}

SHGLRenderer::SHGLRenderer(Client client, RTVal shRenderer) 
  : m_client(client)
  , m_shGLRendererVal(shRenderer) {
}

SHGLRenderer::~SHGLRenderer() {
}

Client SHGLRenderer::getClient() { 
  return m_client; 
}

void SHGLRenderer::setClient(Client client) { 
  m_client = client; 
}

RTVal SHGLRenderer::getSHGLRenderer() { 
  return m_shGLRendererVal; 
}

void SHGLRenderer::setSHGLRenderer(RTVal shGLRendererVal) { 
  m_shGLRendererVal = shGLRendererVal; 
}

void SHGLRenderer::update() {
  try 
  {
    RTVal dummyGLRendererVal = RTVal::Construct(m_client, "SHGLRenderer", 0, 0);
    m_shGLRendererVal = dummyGLRendererVal.callMethod("SHRenderer", "getOrCreate", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::update: exception: %s\n", e.getDesc_cstr());
  }
}

QList<unsigned int> SHGLRenderer::getDrawStats(unsigned int viewportID) {
  QList<unsigned int> stats;
  try 
  {
    RTVal args[5] = {
      RTVal::ConstructUInt32(m_client, viewportID), 
      RTVal::ConstructUInt32(m_client, 0),
      RTVal::ConstructUInt32(m_client, 0),
      RTVal::ConstructUInt32(m_client, 0),
      RTVal::ConstructUInt32(m_client, 0)
    };
    m_shGLRendererVal.callMethod("Boolean", "getDrawStats", 5, &args[0]);
    stats.append(args[1].getUInt32());
    stats.append(args[2].getUInt32());
    stats.append(args[3].getUInt32());
    stats.append(args[4].getUInt32());
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getDrawStats: exception: %s\n", e.getDesc_cstr());
  }
  return stats;
}

RTVal SHGLRenderer::getViewport(unsigned int viewportID) {
  RTVal viewportVal;
  try 
  {
    RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
    viewportVal = m_shGLRendererVal.callMethod("BaseRTRViewport", "getViewport", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getViewport: exception: %s\n", e.getDesc_cstr());
  }
  return viewportVal;
}

RTVal SHGLRenderer::getOrAddViewport(unsigned int viewportID) {
  RTVal viewportVal;
  try 
  {
    RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
    viewportVal = m_shGLRendererVal.callMethod("BaseRTRViewport", "getOrAddViewport", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddViewport: exception: %s\n", e.getDesc_cstr());
  }
  return viewportVal;
}

RTVal SHGLRenderer::getOrAddStereoViewport(unsigned int viewportID) {
  RTVal viewportVal;
  try 
  {
    RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
    viewportVal = m_shGLRendererVal.callMethod("BaseRTRViewport", "getOrAddStereoViewport", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddStereoViewport: exception: %s\n", e.getDesc_cstr());
  }
  return viewportVal;
}

void SHGLRenderer::removeViewport(unsigned int viewportID) {
  try 
  {
    RTVal glRenderer = m_shGLRendererVal;
    if(glRenderer.isValid() && !glRenderer.isNullObject()) {
      RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
      m_shGLRendererVal.callMethod("BaseRTRViewport", "removeViewport", 1, &arg);
    }
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::removeViewport: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLRenderer::setOrthographicViewport(unsigned int viewportID, bool orthographic) {
  try 
  {
    RTVal args[2] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      RTVal::ConstructBoolean(m_client, orthographic)
    };
    m_shGLRendererVal.callMethod("", "setOrthographicViewport", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::setOrthographicViewport: exception: %s\n", e.getDesc_cstr());
  }
}

RTVal SHGLRenderer::getCamera(unsigned int viewportID) {
  RTVal cameraVal;
  try 
  {
    RTVal viewportVal = getOrAddViewport(viewportID);
    cameraVal = viewportVal.callMethod("Camera", "getCamera", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddStereoViewport: exception: %s\n", e.getDesc_cstr());
  }
  return cameraVal;
}

void SHGLRenderer::setPlayback(bool playback) {
  try 
  {
    RTVal arg = RTVal::ConstructBoolean(m_client, playback);
    m_shGLRendererVal.callMethod("", "setPlayback", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::setPlayback: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLRenderer::isPlayback() {
  try 
  {
    return m_shGLRendererVal.callMethod("Boolean", "isPlayback", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::isPlayback: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

RTVal SHGLRenderer::castRay(unsigned int viewportID, QPoint pos) {
  RTVal rayVal;
  try 
  {
    RTVal posVal = RTVal::Construct(m_client, "Vec2", 0, 0);
    posVal.setMember("x", RTVal::ConstructFloat32(m_client, pos.x()));
    posVal.setMember("y", RTVal::ConstructFloat32(m_client, pos.y()));
    RTVal args[2] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      posVal
    };
    rayVal = m_shGLRendererVal.callMethod("Ray", "castRay", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::castRay: exception: %s\n", e.getDesc_cstr());
  }
  return rayVal;
}

QList<float> SHGLRenderer::get3DScenePosFrom2DScreenPos(unsigned int viewportID, QPoint pos) {
  QList<float> list;
  try 
  {
    RTVal posVal = QtToKLMousePosition(pos, getOrAddViewport(viewportID), true);
    RTVal args[2] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      posVal
    };
    RTVal pos3DVal = m_shGLRendererVal.callMethod("Vec3", "get3DScenePosFrom2DScreenPos", 2, &args[0]);
    list.append(pos3DVal.maybeGetMember("x").getFloat32());
    list.append(pos3DVal.maybeGetMember("y").getFloat32());
    list.append(pos3DVal.maybeGetMember("z").getFloat32());
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::get3DScenePosFrom2DScreenPos: exception: %s\n", e.getDesc_cstr());
  }
  return list;
}

RTVal SHGLRenderer::getSGObjectFrom2DScreenPos(unsigned int viewportID, QPoint pos) {
  RTVal result;
  try {
    RTVal posVal = QtToKLMousePosition(pos, getOrAddViewport(viewportID), true);
    RTVal validVal = RTVal::ConstructBoolean(m_client, false);
    RTVal args[3] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      posVal,
      validVal
    };
    result = m_shGLRendererVal.callMethod("SGObject", "getSGObjectFrom2DScreenPos", 3, args);
    if(!validVal.getBoolean())
      result = RTVal();//set as empty
  }
  catch(Exception e) {
    printf("SHGLRenderer::getSGObjectFrom2DScreenPos: exception: %s\n", e.getDesc_cstr());
  }
  return result;
}

void SHGLRenderer::render(unsigned int viewportID, unsigned int width, unsigned int height, unsigned int samples) {
  try 
  {
    RTVal args[4] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      RTVal::ConstructUInt32(m_client, width),
      RTVal::ConstructUInt32(m_client, height),
      RTVal::ConstructUInt32(m_client, samples)
    };
    m_shGLRendererVal.callMethod("", "render", 4, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::render: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLRenderer::render(unsigned int viewportID, unsigned int width, unsigned int height, unsigned int samples, unsigned int drawPhase) {
  try 
  {
    RTVal args[5] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      RTVal::ConstructUInt32(m_client, width),
      RTVal::ConstructUInt32(m_client, height),
      RTVal::ConstructUInt32(m_client, samples),
      RTVal::ConstructUInt32(m_client, drawPhase)
    };
    m_shGLRendererVal.callMethod("", "render", 5, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::render: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLRenderer::onEvent(unsigned int viewportID, QEvent *event, bool dragging) {
  try 
  {    
    if(event->type() == QEvent::MouseButtonDblClick)
    {
      emit itemDoubleClicked();
      event->setAccepted(true);
      return true;
    }

    RTVal klEvent = QtToKLEvent(
      event, 
      getOrAddViewport(viewportID),
      "Canvas",
      true);

    if(!klEvent.isValid())
      return false;

    RTVal args[2] = {
      klEvent,
      RTVal::ConstructBoolean(m_client, dragging)
    };   
     
    RTVal rootDispatcher = getRootDispatcher();
    rootDispatcher.callMethod("", "onEvent", 2, args);
    bool result = args[0].callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
    
    if(result)
    {
      event->setAccepted(result);
      bool redrawAllViewports = args[0].callMethod("Boolean", "redrawAllViewports", 0, 0).getBoolean();
      emit manipsAcceptedEvent(args[0], redrawAllViewports);
    }
    
    return result;
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::onEvent: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

void SHGLRenderer::emitShowContextualMenu(unsigned int viewportID, QPoint pos, QWidget *parent) {
  RTVal sgObject = getSGObjectFrom2DScreenPos(viewportID, pos);
  emit showContextualMenu(
      parent->mapToGlobal(pos),
      sgObject,
      parent,
      true);
}

RTVal SHGLRenderer::getRootDispatcher() {
  RTVal rootDispatcher;
  try 
  {
    rootDispatcher = m_shGLRendererVal.callMethod("LegacyRootDispatcher", "getRootDispatcher", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getRootDispatcher: exception: %s\n", e.getDesc_cstr());
  }
  return rootDispatcher;
}

RTVal SHGLRenderer::getDescription() {
  RTVal list;
  try 
  {
    RTVal rootDispatcher = getRootDispatcher();
    list = rootDispatcher.callMethod("LegacyDispatcherDescription", "getDescription", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getDescription: exception: %s\n", e.getDesc_cstr());
  }
  return list;
}

RTVal SHGLRenderer::getSelectionSet() {
  RTVal selectionSetVal;
  try 
  {
    selectionSetVal = m_shGLRendererVal.callMethod("SWElementHandleSet", "getSelectionSet", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getSelectionSet: exception: %s\n", e.getDesc_cstr());
  }
  return selectionSetVal;
}

QString SHGLRenderer::getSelectionCategory() {
  try 
  {
    return QString(m_shGLRendererVal.callMethod("String", "getSelectionCategory", 0, 0).getStringCString());
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getSelectionCategory: exception: %s\n", e.getDesc_cstr());
  }
  return QString();
}

void SHGLRenderer::enableSharedObjectSelection( bool enable ) {
  try {
    RTVal arg = RTVal::ConstructBoolean( m_client, enable );
    m_shGLRendererVal.callMethod( "", "enableSharedObjectSelection", 1, &arg );
  }
  catch( Exception e ) {
    printf( "SHGLRenderer::enableSharedObjectSelection: exception: %s\n", e.getDesc_cstr() );
  }
}
