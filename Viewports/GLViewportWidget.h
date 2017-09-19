/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_GL_VIEWPORT__
#define __UI_GL_VIEWPORT__

#include <FabricCore.h>
#include "ViewportWidget.h"

class QImage;

namespace FabricUI {
namespace Viewports {

class GLViewportWidget : public ViewportWidget
{
	Q_OBJECT

  friend class MainWindow;
 
  public:
  	GLViewportWidget(
      QColor bgColor, 
      QGLFormat format, 
      QWidget *parent = 0
      );

  	virtual ~GLViewportWidget();
  
    /// Gets tge camera.
    FabricCore::RTVal getCamera();

    /// Checks if the grid is visible.
    bool isGridVisible();

  public slots:
    /// Sets the grid visibility.
    void setGridVisible( 
      bool gridVisible, 
      bool update = true 
      );
    
    /// Implementation of ViewportWidget.
    virtual void clear();

    /// Implementation of ViewportWidget
    virtual bool onEvent(
      QEvent *event
      );

    /// Resets the camera to its 
    /// default parameters.
    void resetCamera();

    /// Starts the viewport capture.
    /// (sequence).
    void startViewportCapture();

    /// Saves the current viewport.
    /// (single frame).
    void saveViewportAs();

protected:
    /// Implementation of QGLWidget
    virtual void initializeGL();
    
    /// Implementation of QGLWidget
    virtual void resizeGL(
      int width, 
      int height
      );

    /// Implementation of QGLWidget
    virtual void paintGL();

  private:
    /// Sets the background color.
    void setBackgroundColor(
      QColor color
      );
      
    /// Initializes InlineDrawing.
    void initializeID( 
      bool shouldUpdateGL = true 
      );
    
    /// Manipulates the camera.
    bool manipulateCamera(
      FabricCore::RTVal klevent,
      bool shouldUpdateGL = true
      );

    QColor m_bgColor;
    bool m_resizedOnce;
    bool m_gridVisible;
 
    FabricCore::RTVal m_camera;
    FabricCore::RTVal m_drawing;
    FabricCore::RTVal m_viewport;
    FabricCore::RTVal m_drawContext;
    FabricCore::RTVal m_cameraManipulator;
};

} // namespace Viewports
} // namespace FabricUI

#endif // __UI_GL_VIEWPORT__
