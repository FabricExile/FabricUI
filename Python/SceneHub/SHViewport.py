import math
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine import Core
from FabricEngine.FabricUI import *  
 
class SHViewport(Viewports.ViewportWidget):

    """SHViewport

    SHViewport specializedViewports.ViewportWidget for RTR2 render.
    When the dock is closed, an event is emitted so the viewport the dock owns
    can be properly deleted.
    
    Arguments:
        index (int): The viewport index in the SHViewportsManager viewports list.
        viewportIndex (int): The internal viewport index (used in KL).
        parent (QMainWindow): A reference to the QDock parent.
    """

    sceneChanged = QtCore.Signal()
    deleteViewport = QtCore.Signal(int)
    redrawOnAlwaysRefresh = QtCore.Signal()

    def __init__(self, renderer, shStates, index, orthographic, context, mainwindow, sharedWidget):
        # Need to hold the context
        self.orthographic = orthographic
        self.shStates = shStates
        self.qglContext = context
        self.samples = self.qglContext.format().samples()
        self.viewportIndex = index
        self.viewport = None
        self.alwaysRefresh = False
        self.shGLRenderer = renderer
        self.shWindow = mainwindow   
        self.client = mainwindow.client
        self.updateRequested = False
        super(SHViewport, self).__init__(renderer.getClient(), QtGui.QColor(), self.qglContext, self.shWindow, sharedWidget, self.shWindow.settings)

        # Force to track mouse movment when not clicking
        self.setMouseTracking(True)
        self.setAcceptDrops(True)
        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.viewport = self.shGLRenderer.getOrAddViewport(self.viewportIndex)
        self.setOrthographic( orthographic )
 
    def detachFromRTRViewport(self):
        """ Sets the viewport camera.
        """

        if(self.viewportIndex > -1): 
            self.shGLRenderer.removeViewport(self.viewportIndex)
        self.viewportIndex = -1

    def setOrthographic(self, orthographic): 
        """ Sets the viewport camera.
        """

        self.orthographic = orthographic
        self.shGLRenderer.setOrthographicViewport(self.viewportIndex, self.orthographic)
      

    def update(self):
        self.updateRequested = True
        super(SHViewport, self).update()

    def paintGL(self):
        """ Override QtGui.QGLWidget paintGL, call the RTR2 render.
        """
        self.computeFPS()
        self.shGLRenderer.render(self.viewportIndex, self.width, self.height, self.samples)
        self.updateRequested = False
        if(self.alwaysRefresh): 
            self.redrawOnAlwaysRefresh.emit()
      
    def resizeGL(self, width, height): 
        """ Override QtGui.QGLWidget resizeGL.
        """

        self.width = width
        self.height = height
      
    def toggleAlwaysRefresh(self):
        """ Force the QGLWidget to draw on update.
        """

        if self.alwaysRefresh:
            self.alwaysRefresh = False
        else: 
            self.alwaysRefresh = True

    def isOrthographic(self):
        """ Checks if the viewport camera is orthographic.
        """

        return self.orthographic

    def getViewportIndex(self):
        """ Gets the viewport index.
        """

        return self.viewportIndex
     
    def alwaysRefreshes(self):
        """ Checks if the QGLWidget is forced to refreshs.
        """

        return self.alwaysRefresh

    def getCamera(self): 
        """ Gets the viewport RTRCamera.
        """ 

        return self.shGLRenderer.getCamera(self.viewportIndex)

    def enterEvent(self, event):
        """ Override QtGui.QWidget enterEvent.

        When the mouse enters the viewports, catchs all the keybord event
        so the tools can be activated without having to click in the view.

        """ 

        self.grabKeyboard()

    def leaveEvent(self, event): 
        """ Override QtGui.QWidget leaveEvent.
        """ 

        self.releaseKeyboard()

    def keyPressEvent(self, event):
        """ Override QtGui.QWidget keyPressEvent.
        """

        self.__onEvent(event)

    def keyReleaseEvent(self, event):
        """ Override QtGui.QWidget keyReleaseEvent.
        """

        self.__onEvent(event)

    def wheelEvent(self, event):
        """ Override QtGui.QWidget wheelEvent.
        """

        self.__onEvent(event)

    def mouseMoveEvent(self, event):
        """ Override QtGui.QWidget mouseMoveEvent.
        """

        self.__onEvent(event)

    def mouseReleaseEvent(self, event):
        """ Override QtGui.QWidget mouseReleaseEvent.
        The synchronizeCommands signal is emitted to synchronize
        the KL and Qt command stacks, cf SceneHub.SHCmdHandler. 
SHContextualMenu
        """
        
        self.__onEvent(event)
        
    def mousePressEvent(self, event):
        """ Override QtGui.QWidget mousePressEvent.
        Display the contextual menu if the event has not been accepted before. 

        """        
        if not self.__onEvent(event) and event.button() == QtCore.Qt.RightButton:
            self.shGLRenderer.emitShowContextualMenu(
                self.viewportIndex,
                event.pos(),
                self);

        
    def mouseDoubleClickEvent(self, event):
        """ Override QtGui.QWidget mouseDoubleClickEvent.

        """

        self.__onEvent(event)
 
    def __onEvent(self, event):
        """ Redirects all the event to the KL EventDispatcher.
        """

        return self.shGLRenderer.onEvent(self.viewportIndex, event, False)
 
    def dragEnterEvent(self, event):
        """ Drag event, to add assets or textures.
        """

        if event.mimeData().hasUrls() and (event.possibleActions() & QtCore.Qt.CopyAction):
            event.setDropAction(QtCore.Qt.CopyAction)
            event.accept()

    def dragMoveEvent(self, event): 
        """ DragMove event, to add assets or textures.
        """

        if(event.mimeData().hasUrls() and (event.possibleActions() & QtCore.Qt.CopyAction)):
            # Convert to a mouseMove event
            mouseEvent = QtGui.QMouseEvent(QtCore.QEvent.MouseMove, event.pos(), QtCore.Qt.LeftButton, QtCore.Qt.LeftButton, QtCore.Qt.NoModifier)
            self.shGLRenderer.onEvent(self.viewportIndex, mouseEvent, True)

    def dropEvent(self, event):
        """ Drop event, to add assets or textures.
        """

        shGLScene = self.shStates.getActiveScene()
        if shGLScene.hasSG(): 
            myData = event.mimeData()
            if myData is None: return
            if event.mimeData().hasUrls() == False: return

            pathList = []
            for url in event.mimeData().urls():
                pathList.append(url.toLocalFile())
            if len(pathList) == 0: return
           
            pos = self.shGLRenderer.get3DScenePosFrom2DScreenPos(self.viewportIndex, event.pos())
            pathList = Util.StringUtil.ProcessPathQStringForOsX(pathList)
            shGLScene.addExternalFileList(
                pathList, 
                event.keyboardModifiers() & QtCore.Qt.ControlModifier, 
                pos[0],
                pos[1],
                pos[2])

            event.acceptProposedAction()
            self.sceneChanged.emit()
