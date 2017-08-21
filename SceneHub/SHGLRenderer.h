/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_GLRENDERER_H__
#define __UI_SCENEHUB_GLRENDERER_H__

#include "SHStates.h"
#include <FabricCore.h>
#include <QList>
#include <QString>
#include <QStringList>
#include <QDrag>
#include <QVector3D>
#include <QMouseEvent>
#include <QWidget>
 
namespace FabricUI {
namespace SceneHub {


class SHGLRenderer : public QObject {

  /**
    SHGLRenderer is a helper class wrapping :kl-ref:`SHGLRenderer`.
    It allows easy and safe access in C++ to the KL methods.

    When possible, the logic is to write most of the code in :kl-ref:`SHGLRenderer`,
    to provide app-independent example code and behavior.
  */
  
  Q_OBJECT
  
  public:
    /// Constructor.
    SHGLRenderer();

    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    SHGLRenderer(FabricCore::Client client);

    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    /// \param shRenderer A reference to the SHGLRenderer.
    SHGLRenderer(FabricCore::Client client, FabricCore::RTVal shRenderer);

    /// Constructor.
    virtual ~SHGLRenderer();

    /// Updates the renderer, if set externally.
    void update();

    /// Sets the client.
    /// \param client A reference to the FabricCore::Client.
    void setClient(FabricCore::Client client);

    /// Gets the client.
    FabricCore::Client getClient();

    /// \internal
    void setSHGLRenderer(FabricCore::RTVal shRenderer);

    /// \internal
    FabricCore::RTVal getSHGLRenderer();
        
    /// Gets the drawing statistics.
    /// \param viewportID The viewport ID.
    /// \param obj the total number of renderer objects
    /// \param point the total number of renderer points
    /// \param line the total number of renderer lines
    /// \param triangle the total number of renderer triangless
    QList<unsigned int> getDrawStats(unsigned int viewportID);
    
    /// Fast getter to the viewport at this viewportID.
    /// \param viewportID The viewport ID.
    FabricCore::RTVal getViewport(unsigned int viewportID);
    
    /// Gets/creates a viewport.
    /// \param viewportID The viewport ID.
    FabricCore::RTVal getOrAddViewport(unsigned int viewportID);
    
    /// Gets/creates a setreo-viewport.
    /// \param viewportID The viewport ID.
    FabricCore::RTVal getOrAddStereoViewport(unsigned int viewportID);
    
    /// Removes the viewport.
    /// \param viewportID The viewport ID. 
    void removeViewport(unsigned int viewportID);
    
    /// Sets the viewport camera as orthographic.
    /// \param viewportID The viewport ID.
    void setOrthographicViewport(unsigned int viewportID, bool orthographic);
    
    /// Gets the viewport camera.
    /// \param viewportID The viewport ID.
    FabricCore::RTVal getCamera(unsigned int viewportID);
    
    /// Returns a ray from a Viewport and a position in camera space.
    /// \param viewportID The ID of the viewport.
    /// \param pos The mouse's position in the viewport
    FabricCore::RTVal castRay(unsigned int viewportID, QPoint pos);

    /// This helper method will raycast in the scene. 
    /// If no result is found, it will intersect with the Y = 0 plane. 
    /// If this is outside the near/far camera range, it will return the origin.
    /// \param viewportID The ID of the viewport.
    /// \param pos The mouse's position in the viewport
    QList<float> get3DScenePosFrom2DScreenPos(unsigned int viewportID, QPoint pos);

    /// This helper method will raycast in the scene and
    /// return the closest hit SGObject (which might be invalid).
    /// \param viewportID The ID of the viewport.
    /// \param pos The mouse's position in the viewport
    FabricCore::RTVal getSGObjectFrom2DScreenPos(unsigned int viewportID, QPoint pos);

    /// Activates the playback.
    /// \param playback It true, playback.
    void setPlayback(bool playback);
    
    /// Checks if the playback is active.
    bool isPlayback();
    
    /// Renders within this viewport.
    /// \param viewportID The viewport ID.
    /// \param width the viewport width.
    /// \param height the viewport height.
    /// \param samples Anti-aliasing number of samples.
    void render(unsigned int viewportID, unsigned int width, unsigned int height, unsigned int samples);
    
    /// Renders within this viewport.
    /// \param viewportID The viewport ID.
    /// \param width the viewport width.
    /// \param height the viewport height.
    /// \param samples Anti-aliasing number of samples.
    /// \param drawPhase Drawing phases (pre-post draw).
    void render(unsigned int viewportID, unsigned int width, unsigned int height, unsigned int samples, unsigned int drawPhase);
        
    /// Propagates the events.
    /// \param viewportID The viewport ID.
    /// \param event The event.
    /// \param dragging If true when dragging an asset or texture in the scene.
    bool onEvent(unsigned int viewportID, QEvent *event, bool dragging);

    /// Returns a reference to the LegacyRootDispatcher.
    FabricCore::RTVal getRootDispatcher();

    /// Gets the names and the keys of the registered tools.
    FabricCore::RTVal getDescription();
    
    /// Returns the selection set if any.
    FabricCore::RTVal getSelectionSet();
    
    /// Gets the category of the selection.
    /// Used to know what type of element is selected.
    /// For showing the right contextual menu.
    QString getSelectionCategory();
    
    void emitShowContextualMenu(unsigned int viewportID, QPoint pos, QWidget *parent);

    /// If true, selection will be enabled for scene graph shared objects.
    /// Otherwise, only the last instance level will be selectable.
    void enableSharedObjectSelection( bool enable );


  signals:
    void manipsAcceptedEvent(FabricCore::RTVal event, bool redrawAllViewports);

    void itemDoubleClicked();
    
    /// Emitted to show the contextual menu.
    void showContextualMenu(
      QPoint point, 
      FabricCore::RTVal sgObject,
      QWidget *parent,
      bool fromViewport);


  protected:
    /// \internal
    FabricCore::Client m_client;    
    /// \internal
    FabricCore::RTVal m_shGLRendererVal;
  
};

} // namespace SceneHub
} // namespace FabricUI

#endif // __UI_SCENEHUB_GLRENDERER_H__
