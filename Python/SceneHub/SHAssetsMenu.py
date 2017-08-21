
import os
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.FabricUI import SceneHub

class SHAssetsMenu(SceneHub.SHBaseSceneMenu):

    """SHAssetsMenu

    SHAssetsMenu specializes SHBaseSceneMenu for importing/exporting assets in a SceneHub.SHGLScene.
    It supports : importing Alembic/Fbx archives within scene.
                  exporting the scene to an Alembic archive.

    Arguments:
        shGLScene (SceneHub.SHGLScene): A reference to a SHGLScene.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, shGLScene, parent = None):
        super(SHAssetsMenu, self).__init__(shGLScene, "Assets", parent)
        self.constructMenu()
     
    def constructMenu(self):
        """Implementation of BaseMenu.
        """

        addAction = self.addAction("Add Asset")
        addExpandAction = self.addAction("Add Asset & load recursively")
        self.addSeparator()
        exportAlembicAction = self.addAction("Export to Alembic")

        addAction.triggered.connect(self.addArchive)
        addExpandAction.triggered.connect(self.addArchive)
        exportAlembicAction.triggered.connect(self.exportToAlembic)

    def addArchive(self):   
        """Import an archive (alembic or fbx) in the current scene.
        """

        dialog = QtGui.QFileDialog(self)
        dialog.setFileMode(QtGui.QFileDialog.ExistingFiles)
        dialog.setNameFilter("Files (*.abc *.fbx)")
        if dialog.exec_():
            pathList = Util.StringUtil.ProcessPathQStringForOsX(dialog.selectedFiles())
            self.m_shGLScene.addExternalFileList(pathList, self.sender().text() != "Add Asset")
      
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
        self.m_shGLScene.exportToAlembic(pathList[0])
      