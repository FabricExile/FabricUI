import os
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI, Util
from FabricEngine.FabricUI import *
from FabricEngine.FabricUI import SceneHub
from FabricEngine.Util import *
from FabricEngine.SceneHub.SHLightsMenu import SHLightsMenu
from FabricEngine.SceneHub.SHAssetsMenu import SHAssetsMenu

class SHContextualMenu(SceneHub.SHBaseContextualMenu):

    """SHContextualMenu

    SHContextualMenu specializes SHBaseContextualMenu.

    Arguments:
        client (FabricEngine.Core.Client): A reference to the FabricCore.Client.
        shGLScene (SceneHub.SHGLScene): A reference to a SHGLScene.
        shStates (SceneHub.SHStates): A reference to the SHStates.
        targetSGObject (Fabr.RTVal): A reference to the sgObject to edit, pass as a RTVal.
        shTreeView (QWidget): A reference to the SceneHub.SHTreeView, can be None.
        shTreeView (QWidget): A reference to parent, can be None.

    """
   
    def __init__(
        self, 
        shStates, 
        targetSGObject, 
        shTreeView = None,
        shGLRenderer = None, 
        parent = None):

        super(SHContextualMenu, self).__init__(shStates, targetSGObject, shTreeView, parent)
        self.shGLRenderer = shGLRenderer
        self.constructMenu()

    def constructMenu(self):
        """Implementation of BaseMenu.
        """
        # Construct the base contextual menu.
        super(SHContextualMenu, self).constructMenu()

        if str(self.m_targetSGObject.type("String").getSimpleType()) == "SGObject":
            self.addSeparator()
            exportAlembicAction = QtGui.QAction("Export to Alembic...", self)
            self.addAction(exportAlembicAction)
            exportAlembicAction.triggered.connect(self.exportToAlembic)

        elif str(self.m_targetSGObject.type("String").getSimpleType()) == "None":
            self.addMenu(SHAssetsMenu(self.m_shStates.getActiveScene()))
            self.addMenu(SHLightsMenu(self.m_shStates.getActiveScene()))
    
        if self.shGLRenderer is not None:
            self.addMenu(SceneHub.SHToolsMenu(self.shGLRenderer))
        
    def exportToAlembic(self):      
        """Exports the current scene to alembic.
        """
        
        fileName, _ = QtGui.QFileDialog.getSaveFileName(self, "Export to Alembic", "", "Files (*.abc)")
        if not fileName: 
            return
            
        baseName, extension = os.path.splitext(fileName)
 
        if extension != ".abc":
            fileName = baseName + ".abc"

        pathList = []
        pathList.append(fileName)
        pathList = Util.StringUtil.ProcessPathQStringForOsX(pathList)
 
        if str(self.m_targetSGObject.type("String").getSimpleType()) == "SGObject":
            self.m_shStates.getActiveScene().exportToAlembic(self.m_targetSGObject, pathList[0])

        elif str(self.m_targetSGObject.type("String").getSimpleType()) == "None":
            self.m_shStates.getActiveScene().exportToAlembic(pathList[0])
