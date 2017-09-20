/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include <QMouseEvent>
#include <QApplication>
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include "QtToKLEvent.h"
#include "GLViewportWidget.h"
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Commands;
using namespace Viewports;
using namespace FabricCore;
using namespace Application;

GLViewportWidget::GLViewportWidget(
  QColor bgColor, 
  QGLFormat format, 
  QWidget *parent)
  : ViewportWidget(format, parent)
  , m_bgColor(bgColor)
{	
  setAutoBufferSwap(false);

  m_gridVisible = true; // default value
  
  QSettings* settings = FabricApplicationStates::GetAppStates()->getSettings();
  if(settings && settings->contains("glviewport/gridVisible"))
    m_gridVisible = settings->value("glviewport/gridVisible").toBool();

  FABRIC_CATCH_BEGIN();

  Client client = FabricApplicationStates::GetAppStates()->getClient();
  client.loadExtension("Manipulation", "", false);
  client.loadExtension("InlineDrawing", "", false);
  
  FABRIC_CATCH_END("GLViewportWidget::GLViewportWidget");

  m_resizedOnce = false;
  initializeID(false /*shouldUpdateGL*/);
}

GLViewportWidget::~GLViewportWidget()
{
}

RTVal GLViewportWidget::getCamera() 
{ 
  return m_camera; 
}

bool GLViewportWidget::onEvent(
  QEvent *event)
{
  bool isAccepted = false;
  
  FABRIC_CATCH_BEGIN();

  if(!m_viewport.isValid())
    return false;

  // Now we translate the Qt events to FabricEngine events..
  RTVal klevent = QtToKLEvent(
    event, 
    m_viewport, 
    "Canvas");

  if(!klevent.isValid())
    return false;

  bool alt = QApplication::keyboardModifiers().testFlag(Qt::AltModifier);
  
  if(!alt && m_manipTool->isActive())
  {
    bool redrawRequested;
    QString manipulatedPortName;

    isAccepted = m_manipTool->onEvent(
      klevent,
      redrawRequested,
      manipulatedPortName);

    if(!event->isAccepted() && isAccepted)
      event->setAccepted(isAccepted);

    // In certain cases, the kl event is not accepted but should be.
    // We check if KL commands where added.
    if(isAccepted || event->type() == QEvent::MouseButtonRelease)
    {
      KLCommandManager *manager = qobject_cast<KLCommandManager*>(
        CommandManager::getCommandManager());

      // Check the command execution and print the exception, 
      // we don't want to crash the app if the command fails.
      try
      {
        manager->synchronizeKL();
      }

      catch(FabricException &e)
      {
        FabricException::Throw(
          "GLViewportWidget::onEvent",
          "",
          e.what(),
          FabricException::LOG);
        
        return isAccepted;
      }
    }    

    ///!!!Force the graph to update correclty
    if(isAccepted)
      emit portManipulationRequested(manipulatedPortName);    

    if(redrawRequested)
      updateGL();
  }
  
  else if(alt)
    isAccepted = manipulateCamera(klevent);
 
  FABRIC_CATCH_END("GLViewportWidget::onEvent");

  return isAccepted;
}

void GLViewportWidget::setBackgroundColor(
  QColor color)
{
  m_bgColor = color;

  if(m_viewport.isValid())
  {
    FABRIC_CATCH_BEGIN();

    Context context = FabricApplicationStates::GetAppStates()->getContext();
    RTVal bgColorVal = m_viewport.maybeGetMember("bgColor");
    bgColorVal.setMember("r", RTVal::ConstructFloat32(context, float(m_bgColor.red()) / 255.0f));
    bgColorVal.setMember("g", RTVal::ConstructFloat32(context, float(m_bgColor.green()) / 255.0f));
    bgColorVal.setMember("b", RTVal::ConstructFloat32(context, float(m_bgColor.blue()) / 255.0f));
    m_viewport.setMember("bgColor", bgColorVal);
    
    FABRIC_CATCH_END("GLViewportWidget::setBackgroundColor");
  }
}

void GLViewportWidget::clear()
{    
  if(m_viewport.isValid() && m_drawContext.isValid())
  {
    initializeID();
    initializeGL();
    resizeGL(size().width(), size().height());
  }
}

void GLViewportWidget::initializeGL()
{
  FABRIC_CATCH_BEGIN();

  m_viewport.callMethod("", "setup", 1, &m_drawContext);
  m_drawing = m_drawing.callMethod("OGLInlineDrawing", "getInstance", 0, 0);
  setGridVisible(m_gridVisible, false);
  emit initComplete();
 
  FABRIC_CATCH_END("GLViewportWidget::initializeGL");
}

void GLViewportWidget::resizeGL(
  int width, 
  int height)
{
  FABRIC_CATCH_BEGIN();

  Context context = FabricApplicationStates::GetAppStates()->getContext();

  RTVal args[3] = {
    m_drawContext,
    RTVal::ConstructUInt32(context, (unsigned int)width),
    RTVal::ConstructUInt32(context, (unsigned int)height)
  };

  m_viewport.callMethod("", "resize", 3, args);
  m_resizedOnce = true;

  FABRIC_CATCH_END("GLViewportWidget::resizeGL");
}

void GLViewportWidget::paintGL()
{
  if(!m_resizedOnce)
  {
    QSize scale = size();
    resizeGL(scale.width(), scale.height());
  }

  computeFPS();

  FABRIC_CATCH_BEGIN();

  RTVal args[2] = {
    RTVal::ConstructString(FabricApplicationStates::GetAppStates()->getContext(), "default"),
    m_drawContext
  };
  m_drawing.callMethod("", "drawViewport", 2, args);

  FABRIC_CATCH_END("GLViewportWidget::resizeGL");

  swapBuffers();
  emit redrawn();
}

void GLViewportWidget::initializeID(
  bool shouldUpdateGL)
{
  FABRIC_CATCH_BEGIN();

  Context context = FabricApplicationStates::GetAppStates()->getContext();

  // Call onNewScene (Singletons interface)
  RTVal singletonHandle = RTVal::Create( context, "SingletonHandle", 0, 0 );
  singletonHandle.callMethod( "", "onNewScene", 0, NULL );

  m_drawing = RTVal::Create(context, "OGLInlineDrawing", 0, 0);
  if(!m_drawing.isValid())
  {
    printf("[GLWidget] Error: Cannot construct OGLInlineDrawing RTVal (extension loaded?)\n");
    return;
  }
  m_drawing = m_drawing.callMethod("OGLInlineDrawing", "getInstance", 0, 0);

  m_viewport = RTVal::Create(context, "OGLStandaloneViewport", 0, 0);
  if(!m_viewport.isValid())
  {
    printf("[GLWidget] Error: Cannot construct OGLStandaloneViewport RTVal (extension loaded?)\n");
    return;
  }
  else
  {
    RTVal args[2] = {
      RTVal::ConstructString(context, "default"),
      m_viewport
    };

    m_drawing.callMethod("", "registerViewport", 2, args);

    // [pzion 20150909] No viewport overlay, at least for now
    // Client client = FabricApplicationStates::GetAppStates()->getClient();
    // bool hasCommercialLicense = client.hasCommercialLicense();
    // if(!hasCommercialLicense)
    //   m_viewport.callMethod("", "setupLicensingOverlay", 0, 0);
  }

  // Proto tool setup
  RTVal::Create(context, "Tool::InlineDrawingRender::RenderSetup", 0, 0);

  m_camera = m_viewport.maybeGetMember("camera");
  m_cameraManipulator = RTVal::Create(context, "CameraManipulator", 1, &m_camera);

  m_viewport.setMember("windowId", RTVal::ConstructUInt64(context, (uint64_t)this->winId()));

  setBackgroundColor(m_bgColor);

  m_drawContext = RTVal::Create(context, "DrawContext", 0, 0);
  if(!m_drawContext.isValid())
  {
    printf("[GLWidget] Error: Cannot construct DrawContext RTVal (extension loaded?)\n");
    return;
  }
  m_drawContext = m_drawContext.callMethod("DrawContext", "getInstance", 0, 0);
  m_drawContext.callMethod("", "resetAllViewportParams", 0, 0);
 
  FABRIC_CATCH_END("GLViewportWidget::initializeID");

  setGridVisible(m_gridVisible, shouldUpdateGL);
  emit initComplete();
}

bool GLViewportWidget::manipulateCamera(
  RTVal klevent,
  bool shouldUpdateGL)
{
  bool result = false;

  FABRIC_CATCH_BEGIN();

  // And then pass the event to the camera manipulator for handling.
  m_cameraManipulator.callMethod("", "onEvent", 1, &klevent);
  result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
  
  FABRIC_CATCH_END("GLViewportWidget::manipulateCamera");

  if(shouldUpdateGL)
    update();

  return result;
}

bool GLViewportWidget::isGridVisible()
{
  return m_gridVisible;
}

void GLViewportWidget::setGridVisible(
  bool gridVisible, 
  bool updateView)
{
  m_gridVisible = gridVisible;

  QSettings* settings = FabricApplicationStates::GetAppStates()->getSettings();
  if(settings)
    settings->setValue("glviewport/gridVisible", m_gridVisible);

  if(!m_viewport.isValid())
    return;

  FABRIC_CATCH_BEGIN();

  Context context = FabricApplicationStates::GetAppStates()->getContext();
  RTVal gridVisibleVal = RTVal::ConstructBoolean(context, m_gridVisible);
  m_viewport.callMethod("", "setGridVisible", 1, &gridVisibleVal);

  FABRIC_CATCH_END("GLViewportWidget::setGridVisible");

  if(updateView)
    update();
}

void GLViewportWidget::resetCamera()
{
  if(!m_viewport.isValid())
    return;

  FABRIC_CATCH_BEGIN();

  Context context = FabricApplicationStates::GetAppStates()->getContext();
  
  RTVal position[3];
  position[0] = RTVal::ConstructFloat32(context, 30.0f);
  position[1] = RTVal::ConstructFloat32(context, 20.0f);
  position[2] = RTVal::ConstructFloat32(context, 40.0f);
  
  RTVal target[3];
  target[0] = RTVal::ConstructFloat32(context, 0.0f);
  target[1] = RTVal::ConstructFloat32(context, 0.0f);
  target[2] = RTVal::ConstructFloat32(context, 0.0f);
  
  RTVal args[2] = {
    RTVal::Construct(context, "Vec3", 3, position),
    RTVal::Construct(context, "Vec3", 3, target)
  };

  m_camera.callMethod("", "setFromPositionAndTarget", 2, args);
  m_cameraManipulator = RTVal::Create(context, "CameraManipulator", 1, &m_camera);
  
  FABRIC_CATCH_END("GLViewportWidget::resetCamera");

  update();
}

void GLViewportWidget::startViewportCapture()
{
  if (!m_viewport.isValid())
    return;

  FABRIC_CATCH_BEGIN();

  Context context = FabricApplicationStates::GetAppStates()->getContext();

  RTVal rtvalCapturePath         = m_viewport.maybeGetMember("capturePath");
  RTVal rtvalCaptureFilename     = m_viewport.maybeGetMember("captureFilename");
  RTVal rtvalCaptureExtension    = m_viewport.maybeGetMember("captureExtension");
  RTVal rtvalCaptureFramePadding = m_viewport.maybeGetMember("captureFramePadding");

  QString  capturePath         = rtvalCapturePath        .getStringCString();
  QString  captureFilename     = rtvalCaptureFilename    .getStringCString();
  QString  captureExtension    = rtvalCaptureExtension   .getStringCString();
  uint32_t captureFramePadding = rtvalCaptureFramePadding.getUInt32();

  if (!captureExtension.startsWith("."))
    captureExtension.push_front(".");

  char paddedFrame[64];

  int frame = 17;
  sprintf(paddedFrame, "%0*d", captureFramePadding, frame);

  QString filepath = capturePath + "/" + captureFilename + paddedFrame + captureExtension;

  printf("filepath \"%s\"\n", filepath.toUtf8().data());

  FABRIC_CATCH_END("GLViewportWidget::startViewportCapture");
}

void GLViewportWidget::saveViewportAs()
{
  if (!m_viewport.isValid())
  {
    printf("error: viewport not valid\n");
    return;
  }

  FABRIC_CATCH_BEGIN();

  QImage image = grabFrameBuffer(false /* withAlpha */);
  if (image.isNull())
  {
    printf("error: grabFrameBuffer() failed\n");
    return;
  }

  QFileInfo fileInfo(QFileDialog::getSaveFileName(this, "Save Capture As", "capture.png"));
  if (!fileInfo.exists())
    return;

  QString filepath = fileInfo.absoluteFilePath();
  if (!image.save(filepath))
  {
    printf("error: failed to save image as \"%s\"\n", filepath.toUtf8().data());
    return;
  }

  FABRIC_CATCH_END("GLViewportWidget::saveViewportAs");
}
