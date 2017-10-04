/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_GL_VIEWPORT__
#define __UI_GL_VIEWPORT__

#include <FabricCore.h>
#include "ViewportWidget.h"
#include "FabricUI/DFG/DFGConfig.h"
#include "FabricUI/DFG/Dialogs/DFGBaseDialog.h"

class QImage;

namespace FabricUI {
namespace Viewports {

class GLViewportCaptureSequenceDialog : public FabricUI::DFG::DFGBaseDialog
{
  Q_OBJECT

public:

  GLViewportCaptureSequenceDialog(QWidget *parent, QString title, const FabricUI::DFG::DFGConfig &dfgConfig = FabricUI::DFG::DFGConfig());

  ~GLViewportCaptureSequenceDialog();

  int captureResX();
  int captureResY();
  QString capturePath();
  QString captureFilename();
  QString captureExtension();
  int captureFramePadding();
  int captureFrameStart();
  int captureFrameEnd();

  void setCaptureResX(int resX);
  void setCaptureResY(int resY);
  void setCapturePath(QString path);
  void setCaptureFilename(QString filename);
  void setCaptureExtension(QString extension);
  void setCaptureFramePadding(int framePadding);
  void setCaptureFrameStart(int frameStart);
  void setCaptureFrameEnd(int frameEnd);

private:

  QLineEdit *m_lineEditCaptureResX;
  QLineEdit *m_lineEditCaptureResY;
  QLineEdit *m_lineEditCapturePath;
  QLineEdit *m_lineEditCaptureFilename;
  QLineEdit *m_lineEditCaptureExtension;
  QLineEdit *m_lineEditCaptureFramePadding;
  QLineEdit *m_lineEditCaptureFrameStart;
  QLineEdit *m_lineEditCaptureFrameEnd;
};

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
  
    /// Gets the camera.
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
    enum CaptureModes
    {
      CAPTURE_OFF,
      CAPTURE_ON,
      CAPTURE_ERROR,
    };

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
 
    CaptureModes m_captureMode;
    QString m_captureErrorDescription;

    FabricCore::RTVal m_camera;
    FabricCore::RTVal m_drawing;
    FabricCore::RTVal m_viewport;
    FabricCore::RTVal m_drawContext;
    FabricCore::RTVal m_cameraManipulator;
};

} // namespace Viewports
} // namespace FabricUI

#endif // __UI_GL_VIEWPORT__
