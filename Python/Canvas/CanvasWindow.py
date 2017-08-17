"""
This module adds the CanvasWindow which encompasses most of the UI
required to build a Python application. Users building their own
custom apps may want to subclass the CanvasWindow and simply override
relevant methods to change behavior as needed.
"""

import os
import sys

from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine import Core, FabricUI, Util
from FabricEngine.FabricUI import Application, DFG, KLASTManager, Viewports, TimeLine, Actions, OptionsEditor
from FabricEngine.Canvas.ScriptEditor import ScriptEditor
from FabricEngine.Canvas.UICmdHandler import UICmdHandler
from FabricEngine.Canvas.RTValEncoderDecoder import RTValEncoderDecoder
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
from FabricEngine.Canvas.Commands.CommandManager import *
from FabricEngine.Canvas.Application.FabricApplicationStates import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyEditorDialog import HotkeyEditorDialog
from FabricEngine.Canvas.Commands.CommandManagerCallback import CommandManagerCallback
from FabricEngine.Canvas.LoadFabricPixmap import LoadFabricPixmap

class CanvasWindowEventFilter(QtCore.QObject):

    def __init__(self, window):
        super(CanvasWindowEventFilter, self).__init__()
        self.window = window

class BaseCanvasWindowAction(Actions.BaseAction):

    def __init__(self,
        parent,
        canvasWindow, 
        name, 
        text, 
        shortcut = QtGui.QKeySequence(), 
        context = QtCore.Qt.ApplicationShortcut):

        self.canvasWindow = canvasWindow

        super(BaseCanvasWindowAction, self).__init__(
            parent, 
            name, 
            text, 
            shortcut, 
            context)

class NewGraphAction(BaseCanvasWindowAction):
 
    def __init__(self, parent, canvasWindow):
        super(NewGraphAction, self).__init__(
            parent,     
            canvasWindow, 
            "CanvasWindow.NewGraphAction", 
            "New Graph", 
            QtGui.QKeySequence.New)
        
    def onTriggered(self):
        self.canvasWindow.execNewGraph()

class LoadGraphAction(BaseCanvasWindowAction):
 
    def __init__(self, parent, canvasWindow):
        super(LoadGraphAction, self).__init__(
            parent,     
            canvasWindow, 
            "CanvasWindow.LoadGraphAction", 
            "Load Graph", 
            QtGui.QKeySequence.Open)
        
    def onTriggered(self):
        self.canvasWindow.onLoadGraph()

class ImportGraphAction(BaseCanvasWindowAction):
 
    def __init__(self, parent, canvasWindow):
        super(ImportGraphAction, self).__init__(
            parent,     
            canvasWindow, 
            "CanvasWindow.ImportGraphAction", 
            "Import Graph", 
            QtGui.QKeySequence('Ctrl+I'))
        
    def onTriggered(self):
        self.canvasWindow.onImportGraphAsNode()

class SaveGraphAction(BaseCanvasWindowAction):
 
    def __init__(self, parent, canvasWindow):
        super(SaveGraphAction, self).__init__(
            parent,     
            canvasWindow, 
            "CanvasWindow.SaveGraphAction", 
            "Save Graph", 
            QtGui.QKeySequence.Save)
        
    def onTriggered(self):
        self.canvasWindow.onSaveGraph()

class SaveGraphAsAction(BaseCanvasWindowAction):
 
    def __init__(self, parent, canvasWindow):
        super(SaveGraphAsAction, self).__init__(
            parent,     
            canvasWindow, 
            "CanvasWindow.SaveGraphAsAction", 
            "Save Graph As", 
            QtGui.QKeySequence.SaveAs)
        
    def onTriggered(self):
        self.canvasWindow.onSaveGraphAs()

class QuitApplicationAction(BaseCanvasWindowAction):
 
    def __init__(self, parent, canvasWindow):
        super(QuitApplicationAction, self).__init__(
            parent,     
            canvasWindow, 
            "CanvasWindow.QuitApplicationAction", 
            "Quit", 
            QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_Q))
    
    def onTriggered(self):
        self.canvasWindow.close()

class ToggleManipulationAction(BaseCanvasWindowAction):

    def __init__(self, canvasWindow, viewport):
        super(ToggleManipulationAction, self).__init__(
            viewport,     
            None, 
            "Viewport.ToggleManipulationAction", 
            "Toggle manipulation", 
            QtGui.QKeySequence(QtCore.Qt.Key_Q),
            QtCore.Qt.WidgetWithChildrenShortcut)
        
        viewport.addAction(self)
        self.viewport = viewport
        self.setCheckable(True)
        self.setChecked(self.viewport.isManipulationActive())
        self.toggled.connect( canvasWindow.valueEditor.toggleManipulation)
    
    def onTriggered(self):
        self.viewport.toggleManipulation()

    def triggerIfInactive(self):
        if not self.viewport.isManipulationActive():
            self.trigger()

class GridVisibilityAction(BaseCanvasWindowAction):

    def __init__(self, parent, viewport):
        super(GridVisibilityAction, self).__init__(
            parent,     
            None, 
            "Viewport.GridVisibilityAction", 
            "&Display Grid", 
            QtGui.QKeySequence(QtCore.Qt.Key_G),
            QtCore.Qt.WidgetWithChildrenShortcut)
        
        viewport.addAction(self)
        self.toggled.connect(viewport.setGridVisible)
        self.setCheckable(True)
        self.setChecked(viewport.isGridVisible())

class ResetCameraAction(BaseCanvasWindowAction):

    def __init__(self, parent, viewport):
        super(ResetCameraAction, self).__init__(
            parent,     
            None, 
            "Viewport.ResetCameraAction", 
            "&Reset Camera", 
            QtGui.QKeySequence(QtCore.Qt.Key_R),
            QtCore.Qt.WidgetWithChildrenShortcut)
        
        viewport.addAction(self)
        self.triggered.connect(viewport.resetCamera)
       

class ShowHotkeyEditorDialogAction(BaseCanvasWindowAction):

    def __init__(self, parent, canvasWindow):
        super(ShowHotkeyEditorDialogAction, self).__init__(
            parent,     
            canvasWindow, 
            "CanvasWindow.ShowHotkeyEditorDialogAction", 
            "Hotkey editor", 
            QtGui.QKeySequence(QtCore.Qt.Key_K))
        
        self.setToolTip(
            "Edit all the registered actions \n" +
            "to associate a shortcut."
            )

    def onTriggered(self):
        if not self.canvasWindow.hotkeyEditorDialog.isVisible():
            pos = QtGui.QCursor.pos()
            self.canvasWindow.hotkeyEditorDialog.move(pos.x(), pos.y())
            self.canvasWindow.hotkeyEditorDialog.exec_()

class CanvasWindow(QtGui.QMainWindow):
    """This window encompasses the entire Canvas application.

    Attributes:
        defaultFrameIn (int): Default in frame.
        defaultFrameOut (int): Default out frame.
        autosaveIntervalSecs (int): Interval at which to autosave the current graph.

    Arguments:
        settings (QtCore.QSettings): Settings object that is used to store and retrieve settings for the application.
        unguarded (bool): Whether to create the Fabric client in unguarded mode.
        noopt (bool): Whether to create the Fabric client in unoptimized mode.

    """

    isCanvas = True
    defaultFrameIn = 1
    defaultFrameOut = 250
    autosaveIntervalSecs = 30
    maxRecentFiles = 10

    def __init__(self, settings, unguarded, noopt):
        self.settings = settings
        self.isInitialized = False

        super(CanvasWindow, self).__init__()

        self.setStyleSheet(LoadFabricStyleSheet("FabricUI.qss"))

        self.autosaveTimer = QtCore.QTimer()
        self.autosaveTimer.timeout.connect(self.autosave)
        self.autosaveTimer.start(CanvasWindow.autosaveIntervalSecs * 1000)
        self.dockFeatures = QtGui.QDockWidget.DockWidgetMovable | QtGui.QDockWidget.DockWidgetFloatable | QtGui.QDockWidget.DockWidgetClosable

        self._init()
        self._initWindow()

        self.statusBar = None
        self.splashScreen = QtGui.QSplashScreen(LoadFabricPixmap("canvas-splash.png"))
        self.splashScreen.setFont(QtGui.QFont("Roboto", 8, QtGui.QFont.Normal))
        self.splashScreen.show()
        self.slowOpStack = []
        self._slowOpPush("Initializing Fabric Canvas")

        self._initKL(unguarded, noopt)
        self._initLog()
        self._initDFG()
        self._initCommand()
        self._initDFGWidget()
        self._initTools()
        self._initTreeView()
        self._initValueEditor()
        self._initGL()
        self._initTimeLine()
        self._initDocks()
        self._initMenus()

        if self.isCanvas:
            self.restoreGeometry(self.settings.value("mainWindow/geometry"))
            self.restoreState(self.settings.value("mainWindow/state"))
        self.onFrameChanged(self.timeLine.getTime())
        self.onGraphSet(self.dfgWidget.getUIGraph())
        self.valueEditor.initConnections()
        self.installEventFilter(CanvasWindowEventFilter(self))

        self._slowOpPop()
        self.splashScreen.finish(self)
        self.splashScreen = None
        self.statusBar = QtGui.QStatusBar()
        self.statusBar.setMinimumHeight(26)
        self.setStatusBar(self.statusBar)

        # Create Log button on the status bar to show log widget
        logDockWidgetButton = QtGui.QPushButton('', self)
        logDockWidgetButton.setObjectName('logWidget_button')
        self.statusBar.insertPermanentWidget(0, logDockWidgetButton)
        logDockWidgetButton.clicked.connect(self.logDockWidget.toggleViewAction().trigger)

    def _init(self):
        """Initializes the settings and config for the application.

        The autosave directory and file name are established here.
        """

        self.setAcceptDrops(True)

        DFG.DFGWidget.setSettings(self.settings)
        self.config = DFG.DFGConfig()

        # [andrew 20160414] despite similar names this is not the same as FABRIC_DIR
        userFabricDir = Core.CAPI.GetFabricDir()
        self.autosaveFilename = os.path.join(userFabricDir, 'autosave')
        if not os.path.exists(self.autosaveFilename):
            os.makedirs(self.autosaveFilename)

        autosaveBasename = 'autosave.' + str(os.getpid()) + '.canvas'
        self.autosaveFilename = os.path.join(self.autosaveFilename, autosaveBasename)
        print 'Will autosave to ' + self.autosaveFilename + ' every ' + str(
            CanvasWindow.autosaveIntervalSecs) + ' seconds'

    def _initWindow(self):
        """Initializes the window attributes, window widgets, actions, and the
        fps timer.
        """

        self.viewport = None
        self.dfgWidget = None
        self.currentGraph = None
        self.undoAction = None
        self.redoAction = None
        self.newGraphAction = None
        self.loadGraphAction = None
        self.importGraphAction = None
        self.saveGraphAction = None
        self.saveGraphAsAction = None
        self.recentFilesAction = []
        self.quitAction = None
        self.manipAction = None
        self.setGridVisibleAction = None
        self.resetCameraAction = None
        self.clearLogAction = None
        self.showHotkeyEditorDialogAction = None


        self.windowTitle = 'Fabric Engine - Canvas'
        self.lastFileName = ''
        self.onFileNameChanged('')

        self.fpsLabel = QtGui.QLabel()
        self.fpsLabel.setObjectName("FPSLabel")
        fpsLabelFM = QtGui.QFontMetrics(self.fpsLabel.font())
        self.fpsLabel.setFixedWidth(fpsLabelFM.width("9900 fps"))
        self.fpsLabel.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignVCenter)

        self.fpsTimer = QtCore.QTimer()
        self.fpsTimer.setInterval(1000)
        self.fpsTimer.timeout.connect(self.updateFPS)
        self.fpsTimer.start()

    def _reportCallback(self, source, level, line):
        """Callback method that fires when the client sends output.

        Report sources:
            Core.ReportSource.System
            Core.ReportSource.User

        Report levels:
            Core.ReportLevel.Error
            Core.ReportLevel.Warning
            Core.ReportLevel.Info
            Core.ReportLevel.Debug

        Arguments:
            source (int): Source of the report call.
            level (int): Level of the report.
            line (int): Text of the output.

        """

        if self.dfgWidget:
            self.dfgWidget.getDFGController().log(line)
        else:
            if source == Core.ReportSource.User or 'Ignoring' in line:
                sys.stdout.write(line + "\n")
            else:
                sys.stderr.write(line + "\n")

    def _updateSlowOpMsgs(self):
        if self.splashScreen:
            if len(self.slowOpStack):
                slowOpMsg = self.slowOpStack[-1]
            else:
                slowOpMsg = ""

            message = "{} {}\n{}\n{}".format(Core.CAPI.GetProductNameStr(),
                                             Core.CAPI.GetVersionWithBuildInfoStr(),
                                             Core.CAPI.GetCopyrightStr(),
                                             slowOpMsg)

            self.splashScreen.showMessage(message,
                QtCore.Qt.AlignLeft | QtCore.Qt.AlignBottom,
                QtGui.QColor(QtCore.Qt.white)
                )
            QtCore.QCoreApplication.processEvents()
        elif self.statusBar:
            if len(self.slowOpStack):
                self.statusBar.showMessage(self.slowOpStack[-1])
                self.statusBar.repaint()
            else:
                self.statusBar.clearMessage()
            self.statusBar.repaint()


    def _slowOpPush(self, desc):
        self.slowOpStack.append(desc + "...")
        self._updateSlowOpMsgs()


    def _slowOpPop(self):
        if len(self.slowOpStack): # should always be true
            self.slowOpStack.pop()
            self._updateSlowOpMsgs()


    def _statusCallback(self, target, data):
        """Status callback used for KL code to communicate status messages back
        to the client.

        Arguments:
            target (str): Target of the callback.
            data: Data to be sent to the target.

        """

        if target == "licensing":
            try:
                FabricUI.HandleLicenseData(self, self.client, data, True)
            except Exception as e:
                self.dfgWidget.getDFGController().logError(str(e))
                print e
        elif target == 'slowOp.push':
            self._slowOpPush(data)
        elif target == 'slowOp.pop':
            self._slowOpPop()


    def _initKL(self, unguarded, noopt):
        """Initializes the Fabric client.

        The core client is what drives the application and what the user
        interface interacts with to create data types and process the data. The
        client is required in all Fabric based applications.

        Arguments:
            unguarded (bool): Sets the client to run in guarded mode or not.
            noopt (bool): Set the client to skip KL code optimization

        """

        self.rtvalEncoderDecoder = RTValEncoderDecoder(None)
        clientOpts = {
          'guarded': not unguarded,
          'noOptimization': noopt,
          'interactive': True,
          'reportCallback': self._reportCallback,
          'rtValToJSONEncoder': self.rtvalEncoderDecoder.encode,
          'rtValFromJSONDecoder': self.rtvalEncoderDecoder.decode,
          }

        self.client = Core.createClient(clientOpts)
        self.client.setStatusCallback(self._statusCallback)
        self.client.loadExtension('Math')
        self.client.loadExtension('Parameters')
        self.client.loadExtension('Util')
        self.client.loadExtension('FabricInterfaces')
        self.client.loadExtension('Manipulation')
        self.rtvalEncoderDecoder.client = self.client
        CreateAppStates(self.client, self.settings)

    def _initDFG(self):
        """Initializes the Data Flow Graph.

        An evalContext is created to provide contextual information about the
        evaluation to operators and other objects and systems with Fabric
        Engine.

        A binding to a graph is initialized and is setup so the application can
        interact with it via the DFGWidget and through other scripted methods
        within the application.

        """

        self.evalContext = self.client.RT.types.EvalContext.create()
        self.evalContext = self.evalContext.getInstance('EvalContext')
        self.evalContext.host = 'Canvas'

        self.astManager = KLASTManager(self.client)
        self.host = self.client.getDFGHost()
        self.mainBinding = self.host.createBindingToNewGraph()
        self.mainBinding.setMetadata("host_app", "Canvas.py", False);
        self.lastSavedBindingVersion = self.mainBinding.getVersion()
        self.lastAutosaveBindingVersion = self.lastSavedBindingVersion

    def _initCommand(self):
        """Initializes the commands framework.
 
        The UICmdHandler handles the interaction between the UI and the client.

        """
        self.qUndoStack = QtGui.QUndoStack()
   
        self.qUndoView = QtGui.QUndoView(self.qUndoStack)
        self.qUndoView.setObjectName('DFGHistoryWidget')
        self.qUndoView.setEmptyLabel("New Graph")

        self.scriptEditor = ScriptEditor(self.client, self.mainBinding, self.qUndoStack, self.logWidget, self.settings, self, self.config)
        self.dfguiCommandHandler = UICmdHandler(self.client, self.scriptEditor)
        
        self.cmdManagerCallback = CommandManagerCallback(self.qUndoStack, self.scriptEditor)
        #self.hotkeyEditorDialog = HotkeyEditorDialog(self)


    def _initDFGWidget(self):
        """Initializes the Data Flow Graph.

        The DFGWidget is the UI that reflects the binding to the graph that is
        created and changed through the application.
        """

        graph = self.mainBinding.getExec()
        self.dfgWidget = DFG.DFGWidget(None, self.client, self.host,
                                       self.mainBinding, '', graph, self.astManager,
                                       self.dfguiCommandHandler, self.config)
        self.dfgWidget.fileMenuAboutToShow.connect(self.updateRecentFileActions)
        self.scriptEditor.setDFGControllerGlobal(self.dfgWidget.getDFGController())

        tabSearchWidget = self.dfgWidget.getTabSearchWidget()
        tabSearchWidget.enabled.connect(self.enableShortCutsAndUndoRedo)

        self.dfgWidget.onGraphSet.connect(self.onGraphSet)
        self.dfgWidget.additionalMenuActionsRequested.connect(self.onAdditionalMenuActionsRequested)
        self.dfgWidget.urlDropped.connect(self.onUrlDropped)

        controller = self.dfgWidget.getDFGController()
        controller.topoDirty.connect(GetCommandManager().synchronizeKL)
        FabricUI.DFG.DFGCommandRegistration.RegisterCommands(self.dfgWidget.getDFGController())
        
    def _initTools(self):
        """Initializes the Tools.
        """
        self.toolsDFGPVNotifierRegistry = FabricUI.DFG.DFGPVToolsNotifierRegistry()
        FabricUI.DFG.DFGToolsCommandRegistration.RegisterCommands(self.toolsDFGPVNotifierRegistry)

    def _initTreeView(self):
        """Initializes the preset TreeView.

        Also connects the DFG Controller's dirty signal to the onDirty method.
        """

        controller = self.dfgWidget.getDFGController()
        self.treeWidget = DFG.PresetTreeWidget(controller, self.config, True, False, False, False, False, True)
        self.dfgWidget.newPresetSaved.connect(self.treeWidget.refresh)
        self.dfgWidget.revealPresetInExplorer.connect(self.onRevealPresetInExplorer)
        # FE-8381 : Removed variables from the PresetTreeWidget
        #controller.varsChanged.connect(self.treeWidget.setModelDirty)
        controller.dirty.connect(self.onDirty)
        controller.topoDirty.connect(self.onTopoDirty)

    def _initGL(self):
        """Initializes the Open GL viewport widget."""

        glFormat = QtOpenGL.QGLFormat()
        glFormat.setDoubleBuffer(True)
        glFormat.setDepth(True)
        glFormat.setAlpha(True)
        glFormat.setSampleBuffers(True)
        glFormat.setSamples(4)

        self.viewport = Viewports.GLViewportWidget(self.config.defaultWindowColor, glFormat, self)
        self.setCentralWidget(self.viewport)
        self.viewport.portManipulationRequested.connect(self.onPortManipulationRequested)

        OptionsEditor.KLOptionsTargetEditor.create( "Rendering Options", "Rendering Options", "", self )

        # When a tool is activated/deactivated from the value-editor.
        self.valueEditor.refreshViewport.connect(self.viewport.redraw)
        self.toolsDFGPVNotifierRegistry.toolUpdated.connect(self.viewport.redraw)

    def _initValueEditor(self):
        """Initializes the value editor."""

        self.valueEditor = FabricUI.DFG.DFGVEEditorOwner(self.dfgWidget)
        self.dfgWidget.stylesReloaded.connect(self.valueEditor.reloadStyles)

    def _initLog(self):
        """Initializes the DFGLogWidget."""
        self.logWidget = DFG.DFGLogWidget(self.config)

    def _initTimeLine(self):
        """Initializes the TimeLineWidget.

        The timeline widget is setup with the class variables for the default in
        and out frames.

        The frameChanged signal is connected to the onFrameChanged method along
        with the Value Editor's onFrameChanged method too.
        """

        self.timeLine = TimeLine.TimeLineWidget()
        controller = self.dfgWidget.getDFGController()
        self.timeLine.frameChanged.connect( controller.onFrameChanged )
        self.timeLine.targetFrameRateChanged.connect( controller.onTimelineTargetFramerateChanged )
        controller.onTimelineTargetFramerateChanged( self.timeLine.framerate() )
        self.timeLine.rangeChanged.connect( controller.onTimelineRangeChanged )
        self.timeLine.setTimeRange(CanvasWindow.defaultFrameIn, CanvasWindow.defaultFrameOut)
        self.timeLine.updateTime(1)
        self.dfgWidget.stylesReloaded.connect(self.timeLine.reloadStyles)
        self.timeLine.frameChanged.connect(self.onFrameChanged)
        self.scriptEditor.setTimeLineGlobal(self.timeLine)

        self.timelineFrame = QtGui.QFrame()
        self.timelineFrame.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        self.timelineFrame.setObjectName("DFGTimelineFrame")
        self.timelineFrame.setLineWidth(0)
        self.timelineFrame.setContentsMargins(0, 0, 0, 0)
        self.timelineFrame.setFrameStyle(QtGui.QFrame.NoFrame|QtGui.QFrame.Plain)

        self.timelineFrameLayout = QtGui.QBoxLayout(QtGui.QBoxLayout.LeftToRight)
        self.timelineFrameLayout.setContentsMargins(0, 0, 0, 0)
        self.timelineFrameLayout.setSpacing(0)
        self.timelineFrameLayout.addWidget(self.timeLine)
        self.timelineFrameLayout.addSpacing(4)
        self.timelineFrameLayout.addWidget(self.fpsLabel)
        
        self.timelineFrame.setLayout(self.timelineFrameLayout)

    def _initDocks(self):
        """Initializes all of dock widgets for the application.

        The dock widgets host the main widgets for the application and are able
        to be toggled on and off via the menu items that are created. Some
        widgets that are hosted in dock widgets are instanced here and some have
        already been created such as the DFGWidget and TimeLineWidget.
        """

        # Undo Dock Widget
        self.undoDockWidget = QtGui.QDockWidget("History", self)
        self.undoDockWidget.setObjectName("History")
        self.undoDockWidget.setFeatures(self.dockFeatures)
        self.undoDockWidget.setWidget(self.qUndoView)
        self.undoDockWidget.hide()
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.undoDockWidget)

        # Log Dock Widget
        self.logDockWidget = QtGui.QDockWidget("Log Messages", self)
        self.logDockWidget.setObjectName("Log")
        self.logDockWidget.setFeatures(self.dockFeatures)
        self.logDockWidget.setWidget(self.logWidget)
        self.logDockWidget.hide()
        self.addDockWidget(QtCore.Qt.TopDockWidgetArea, self.logDockWidget, QtCore.Qt.Vertical)

        # Value Editor Dock Widget
        self.valueEditorDockWidget = QtGui.QDockWidget("Value Editor", self)
        self.valueEditorDockWidget.setObjectName("Values")
        self.valueEditorDockWidget.setFeatures(self.dockFeatures)
        self.valueEditorDockWidget.setWidget(self.valueEditor.getWidget())
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.valueEditorDockWidget)

        # DFG Dock Widget
        self.dfgDock = QtGui.QDockWidget('Canvas Graph', self)
        self.dfgDock.setObjectName('Canvas Graph')
        self.dfgDock.setFeatures(self.dockFeatures)
        self.dfgDock.setWidget(self.dfgWidget)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.dfgDock, QtCore.Qt.Vertical)

        # Explorer Dock Widget
        self.treeDock = QtGui.QDockWidget("Explorer", self)
        self.treeDock.setObjectName("Explorer")
        self.treeDock.setFeatures(self.dockFeatures)
        self.treeDock.setWidget(self.treeWidget)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.treeDock)

        # Timeline Dock Widget
        self.timeLineDock = QtGui.QDockWidget("Timeline", self)
        self.timeLineDock.setObjectName("TimeLine")
        self.timeLineDock.setFeatures(self.dockFeatures)
        # self.timeLineDock.setWidget(self.timeLine)
        self.timeLineDock.setWidget(self.timelineFrame)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.timeLineDock, QtCore.Qt.Vertical)

        # Script Editor Dock Widget
        self.scriptEditorDock = QtGui.QDockWidget('Script Editor', self)
        self.scriptEditorDock.setObjectName('Script Editor')
        self.scriptEditorDock.setFeatures(self.dockFeatures)
        self.scriptEditorDock.setWidget(self.scriptEditor)
        def scriptEditorTitleDataChanged(filename, isModified):
            windowTitle = "Script Editor"
            if filename:
                windowTitle += ' - '
                windowTitle += filename
            if isModified:
                windowTitle += ' (modified)'
            self.scriptEditorDock.setWindowTitle(windowTitle)
        self.scriptEditor.titleDataChanged.connect(scriptEditorTitleDataChanged)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.scriptEditorDock, QtCore.Qt.Vertical)

    def onTriggered(self):
        self.scriptEditor.newScript()

    def _initMenus(self):
        """Initializes all menus for the application."""

        # add the "File", "Edit" and "View" menus.
        self.dfgWidget.populateMenuBar(self.menuBar(), True, True, True, False, False)

        # add the "window" menu.
        # note: this menu is specific to the .exe and .py
        #       standalones which is why it is done here.
        windowMenu = self.menuBar().addMenu('&Window')

        # Toggle DFG Dock Widget Action
        toggleAction = self.dfgDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_1)
        Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.dfgDock.toggleViewAction", toggleAction)
        windowMenu.addAction(toggleAction)

        # Toggle Explorer Dock Widget Action
        toggleAction = self.treeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_2)
        Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.treeDock.toggleViewAction", toggleAction)
        windowMenu.addAction(toggleAction)

        # Toggle Value Editor Dock Widget Action
        toggleAction = self.valueEditorDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_3)
        Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.valueEditorDockWidget.toggleViewAction", toggleAction)
        windowMenu.addAction(toggleAction)

        # Toggle Timeline Dock Widget Action
        toggleAction = self.timeLineDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_4)
        Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.timeLineDock.toggleViewAction", toggleAction)
        windowMenu.addAction(toggleAction)

        windowMenu.addSeparator()

        # Toggle Undo Dock Widget Action
        toggleAction = self.undoDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_5)
        Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.undoDockWidget.toggleViewAction", toggleAction)
        windowMenu.addAction(toggleAction)

        # Toggle Log Dock Widget Action
        toggleAction = self.logDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_6)
        Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.logDockWidget.toggleViewAction", toggleAction)
        windowMenu.addAction(toggleAction)

        # Toggle Script Editor Dock Widget Action
        toggleAction = self.scriptEditorDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_7)
        Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.scriptEditorDock.toggleViewAction", toggleAction)
        windowMenu.addAction(toggleAction)

        try:
            dockWidget = FabricUI.Util.QtUtil.getDockWidget("Rendering Options")
            if dockWidget:
                dockWidget.hide()
                toggleAction = dockWidget.toggleViewAction()
                toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_8)
                Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.renderingOptionsDockWidget.toggleViewAction", toggleAction)
                windowMenu.addAction( toggleAction )
        except Exception as e:
            print str(e)

        self.dfgWidget.populateMenuBar(self.menuBar(), False, False, False, False, True)
    
    def onPortManipulationRequested(self, portName):
        """Method to trigger value changes that are requested by manipulators
        in the viewport.

        Arguments:
            portName (str): Name of the port that is being driven.

        """

        try:
            controller = self.dfgWidget.getDFGController()
            if len(portName) > 0:
                binding = controller.getBinding()
                dfgExec = binding.getExec()
                portResolvedType = dfgExec.getExecPortResolvedType(str(portName))
                value = self.viewport.getManipTool().getLastManipVal()
                if portResolvedType == 'Xfo':
                    pass
                elif portResolvedType == 'Mat44':
                    value = value.toMat44('Mat44')
                elif portResolvedType == 'Vec3':
                    value = value.tr
                elif portResolvedType == 'Quat':
                    value = value.ori
                else:
                    message = "Port '" + portName
                    message += "'to be driven has unsupported type '"
                    message += portResolvedType.data()
                    message += "'."
                    self.dfgWidget.getDFGController().logError(message)
                    return
                controller.cmdSetArgValue(portName, value)
            
            # Force the graph execution
            controller.processDelayedEvents() # [FE-6568]

        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    def onDirty(self):
        """Method called when the graph is dirtied."""
        self.dfgWidget.getDFGController().execute()
        self.valueEditor.onOutputsChanged()
        if self.viewport:
            self.viewport.redraw()

    def onTopoDirty(self):
        self.onDirty()

    def setCurrentFile(self, filePath):
        files = list(self.settings.value('mainWindow/recentFiles', []))
        if type(files) is not list:
          files = [files]           

        # Convert paths to abspath, to make sure that they collide
        filePath = os.path.abspath( filePath )
        files = [ os.path.abspath( file ) for file in files ]       

        # Try to remove the entry if it is already in the list
        try:
            files.remove(filePath)
        except ValueError:
            pass

        # Insert the entry first in the list
        files.insert(0, filePath)

        # Update the list and crop it to maxRecentFiles
        self.settings.setValue('mainWindow/recentFiles', files[:self.maxRecentFiles])

        self.updateRecentFileActions()

    def updateRecentFileActions(self):
        files = self.settings.value('mainWindow/recentFiles', [])
        if type(files) is not list:
          files = [files]                   

        # Only keep files that still exist
        files = [ f for f in files if os.path.exists( f ) ]               

        if len(self.recentFilesAction) >0:
            for i,filepath in enumerate(files):
                maxLen = 90
                displayedFilepath = filepath
                if len(displayedFilepath) > maxLen :
                    # crop the filepath in the middle if it is too long
                    displayedFilepath = displayedFilepath[:maxLen/2] + "..." + displayedFilepath[-maxLen/2:]
                text = str(i + 1) + " " + displayedFilepath
                self.recentFilesAction[i].setText(text)
                self.recentFilesAction[i].setData(filepath)
                self.recentFilesAction[i].setVisible(True)
            
    def loadGraph(self, filePath):
        """Method to load a graph from disk.

        Files are typically *.canvas files.

        Arguments:
            filePath (str): Path to the graph to load.

        """

        self.timeLine.pause()
        self._slowOpPush("Loading '%s'" % filePath)
        
        try:
            manipActive, binding = self.__clearApp()

            self.qUndoView.setEmptyLabel("Load Graph")

            jsonVal = open(filePath, 'rb').read()
            binding = self.host.createBindingFromJSON(jsonVal)
            binding.setMetadata("host_app", "Canvas.py", False);
            self.lastSavedBindingVersion = binding.getVersion()
            self.dfgWidget.replaceBinding(binding)
            self.scriptEditor.updateBinding(binding)

            dfgExec = binding.getExec()
            tl_start = dfgExec.getMetadata("timeline_start")
            tl_end = dfgExec.getMetadata("timeline_end")
            tl_loopMode = dfgExec.getMetadata("timeline_loopMode")
            tl_simulationMode = dfgExec.getMetadata("timeline_simMode")
            tl_current = dfgExec.getMetadata("timeline_current")
            tl_timerFps = dfgExec.getMetadata("timeline_timerFps")

            if len(tl_start) > 0 and len(tl_end) > 0:
                self.timeLine.setTimeRange(int(tl_start), int(tl_end))
            else:
                self.timeLine.setTimeRange(CanvasWindow.defaultFrameIn,
                                           CanvasWindow.defaultFrameOut)

            if len(tl_loopMode) > 0:
                self.timeLine.setLoopMode(int(tl_loopMode))
            else:
                self.timeLine.setLoopMode(1)

            if len(tl_simulationMode) > 0:
                self.timeLine.setSimulationMode(int(tl_simulationMode))
            else:
                self.timeLine.setSimulationMode(0)

            if len(tl_timerFps) > 0:
                self.timeLine.setTimerFromFps(float(tl_timerFps))

            camera_mat44 = dfgExec.getMetadata("camera_mat44")
            camera_focalDistance = dfgExec.getMetadata("camera_focalDistance")
            if len(camera_mat44) > 0 and len(camera_focalDistance) > 0:
                try:
                    mat44 = self.client.RT.constructRTValFromJSON('Mat44', camera_mat44)
                    focalDistance = self.client.RT.constructRTValFromJSON('Float32',
                        camera_focalDistance)

                    camera = self.viewport.getCamera()
                    camera.setFromMat44('', mat44)
                    camera.setFocalDistance("", focalDistance)
                except Exception as e:
                    sys.stderr.write("Exception: " + str(e) + "\n")

            self.dfgWidget.getDFGController().emitDirty()
            self.onFileNameChanged(filePath)

            QtCore.QCoreApplication.processEvents()

            # then set it to the current value if we still have it.
            # this will ensure that sim mode scenes will play correctly.
            if len(tl_current) > 0:
                self.timeLine.updateTime(int(tl_current), True)
            else:
                self.timeLine.updateTime(CanvasWindow.defaultFrameIn, True)

            self.setCurrentFile(filePath)

            if self.dfgWidget:
                self.dfgWidget.getDFGController().log("graph loaded \"" + str(filePath) + "\"")

            if manipActive is True:
                self.manipAction.onTriggered()

        except Exception as e:
            sys.stderr.write("Exception: " + str(e) + "\n")

        finally:
            self._slowOpPop()

        self.lastFileName = filePath

    def onSaveGraph(self):
        """Method called when the graph is saved."""

        self.saveGraph(False)

    def onSaveGraphAs(self):
        """Method called when the graph is to be saved under a different file
        name.
        """

        self.saveGraph(True)

    def closeEvent(self, event):
        """Standard close event method called when the Window is closed.

        Settings for the application are stored and the window widget is closed.
        The super class's closeEvent is then fired and the Fabric client is
        closed.

        Arguments:
            event (QtCore.QEvent): Event that has been triggered.

        """

        if not self.checkUnsavedChanges():
            event.ignore()
            return
        if not self.scriptEditor.checkUnsavedChanges():
            event.ignore()
            return

        if self.dfgWidget:
            dfgController = self.dfgWidget.getDFGController()
            if dfgController:
                dfgController.savePrefs()

        self.viewport.setManipulationActive(False)
        self.settings.setValue("mainWindow/geometry", self.saveGeometry())
        self.settings.setValue("mainWindow/state", self.saveState())

        QtGui.QMainWindow.closeEvent(self, event)

        self.astManager = None
        self.setCentralWidget(None)
        self.viewport = None
        self.client.close()

        if os.path.exists(self.autosaveFilename):
            os.remove(self.autosaveFilename)

    def checkUnsavedChanges(self):
        """Checks for any unsaved changes in the graph.

        If there are changes that haven't been saved the user is prompted with a
        dialog asking them if the graph should be saved.
        """

        binding = self.dfgWidget.getDFGController().getBinding()

        if binding.getVersion() != self.lastSavedBindingVersion:
            msgBox = QtGui.QMessageBox(QtGui.QMessageBox.NoIcon, "Fabric Engine", "", parent=self)
            msgBox.setText("Do you want to save your changes?")
            msgBox.setInformativeText(
                "Your changes will be lost if you don't save them.")
            msgBox.setStandardButtons(QtGui.QMessageBox.Save |
                                      QtGui.QMessageBox.Discard |
                                      QtGui.QMessageBox.Cancel)
            msgBox.setDefaultButton(QtGui.QMessageBox.Save)
            result = msgBox.exec_()
            if result == QtGui.QMessageBox.Discard:
                return True
            elif result == QtGui.QMessageBox.Cancel:
                return False
            else:
                return self.saveGraph(False)
        return True

    def onNodeEditRequested(self, node):
        """Method that is called when a request to edit the specified node has
        been emitted.
        """

        self.dfgWidget.onNodeEditRequested(node)

    def onFrameChanged(self, frame):
        """Method called when the user has changed frames.

        The frame change call pushes the new frame into the binding ports which
        map to the timeline.

        Arguments:
            frame (float): The new frame the user has changed to.

        """

        # [FE-6646] process all events before continuing.
        QtCore.QCoreApplication.processEvents()

        try:
            self.evalContext.time = frame
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    @staticmethod
    def formatFPS(fps):
        if fps >= 9950.0:
            caption = '-- fps'
        elif fps >= 995.0:
            caption = '{0:d}00 fps'.format(int(round(fps/100.0)))
        elif fps >= 99.5:
            caption = '{0:d}0 fps'.format(int(round(fps/10.0)))
        elif fps >= 9.95:
            caption = '{0:d} fps'.format(int(round(fps)))
        elif fps >= 0.995:
            caption = '{0:d}.{1:d} fps'.format(int(round(fps*10.0))/10, int(round(fps*10.0))%10)
        elif fps >= 0.095:
            caption = '0.{0:d} fps'.format(int(round(fps*100.0)))
        elif fps >= 0.0095:
            caption = '0.0{0:d} fps'.format(int(round(fps*100.0)))
        else:
            caption = '-- fps'
        return caption

    def updateFPS(self):
        """Method for updating the FPS label in the status bar."""

        if not self.viewport:
            return

        # for fps in [
        #     0.0094, 0.0095,
        #     0.094, 0.095,
        #     0.994, 0.995,
        #     9.94, 9.95,
        #     99.4, 99.5,
        #     994.0, 995.0,
        #     9940.0, 9950.0,
        #     ]:
        #     print "formatFPS(%f) = %s" % (fps, self.formatFPS(fps))
        self.fpsLabel.setText(self.formatFPS(self.viewport.fps()))

    def autosave(self):
        """Saves the scene when the auto-save timer triggers."""

        # [andrew 20150909] can happen if this triggers while the licensing
        # dialogs are up
        if not self.dfgWidget:
            return
        dfgController = self.dfgWidget.getDFGController();
        if not dfgController:
            return

        binding = dfgController.getBinding()
        if binding:
            bindingVersion = binding.getVersion()
            if bindingVersion != self.lastAutosaveBindingVersion:
                tmpAutosaveFilename = self.autosaveFilename
                tmpAutosaveFilename += ".tmp"

                if self.performSave(binding, tmpAutosaveFilename):
                    # [andrew 20160414] os.rename fails on Windows if file exists
                    if os.path.exists(self.autosaveFilename):
                        os.remove(self.autosaveFilename)
                    os.rename(tmpAutosaveFilename, self.autosaveFilename)
                    self.lastAutosaveBindingVersion = bindingVersion

        dfgController.savePrefs()

    def execNewGraph(self, skip_save=False):
        """Callback Executed when a key or menu command has requested a new graph.

        This simply executes the corresponding script command.

        Arguments:
            skip_save (bool): Whether to skip the check for unsaved changes.

        """

        self.scriptEditor.exec_("newGraph(skip_save=%s)" % str(skip_save))

    def __clearApp(self):
        """ Clear the app before loading a new graph.
        """
        manipActive = self.viewport.isManipulationActive()
        self.toolsDFGPVNotifierRegistry.unregisterAllPathValueTools()
        
        if manipActive is True:
            self.manipAction.onTriggered()
    
        binding = self.dfgWidget.getDFGController().getBinding()
        binding.deallocValues()

        self.host.flushUndoRedo()
        self.qUndoStack.clear()
        GetCommandManager().clear()
        self.logWidget.clear()
        self.viewport.clear()
        self.scriptEditor.clear()
        QtCore.QCoreApplication.processEvents()

        return manipActive, binding

    def onNewGraph(self, skip_save=False):
        """Callback Executed when a call to create a new graph has been made.

        This method deallocates the previous binding and creates a new one. It
        also resets the undo stack, clears inline drawing, and sets the
        timeline.

        Arguments:
            skip_save (bool): Whether to skip the check for unsaved changes.

        """

        self.timeLine.pause()

        if not skip_save:
            if not self.checkUnsavedChanges():
                return

        self.lastFileName = ""

        try:
            manipActive, binding = self.__clearApp()

            # Note: the previous binding is no longer functional
            #             create the new one before resetting the timeline options

            binding = self.host.createBindingToNewGraph()
            binding.setMetadata("host_app", "Canvas.py", False);
            self.lastSavedBindingVersion = binding.getVersion()

            self.dfgWidget.replaceBinding(binding)
            self.scriptEditor.updateBinding(binding)

            self.timeLine.setTimeRange(CanvasWindow.defaultFrameIn,
                                       CanvasWindow.defaultFrameOut)
            self.timeLine.setLoopMode(1)
            self.timeLine.setSimulationMode(0)
            self.timeLine.updateTime(CanvasWindow.defaultFrameIn, True)

            self.qUndoView.setEmptyLabel("New Graph")

            self.dfgWidget.getDFGController().emitDirty()
            self.onFileNameChanged('')

            if manipActive is True:
                self.manipAction.onTriggered()

        except Exception as e:
            print 'Exception: ' + str(e)
            raise e

    def onLoadGraph(self):
        """Callback for when users wish to load a graph from the UI.

        A file dialog is opened and users can select the file to load. The last
        directory the user saved or opened a graph from is used.
        """

        self.timeLine.pause()

        if not self.checkUnsavedChanges():
            return

        lastPresetFolder = str(self.settings.value(
            "mainWindow/lastPresetFolder"))
        filePath, _ = QtGui.QFileDialog.getOpenFileName(
            self, "Load graph", lastPresetFolder, "*.canvas")

        filePath = str(filePath)
        if len(filePath) > 0:
            folder = QtCore.QDir(filePath)
            folder.cdUp()
            self.settings.setValue("mainWindow/lastPresetFolder",
                                   str(folder.path()))
            self.loadGraph(filePath)

    def onImportGraphAsNode(self):
        """Callback for when users wish to import a graph as a node from the UI.

        A file dialog is opened and users can select the file to import. The last
        directory the user saved or opened a graph from is used.
        """

        lastPresetFolder = self.settings.value("mainWindow/lastPresetFolder")
        fileInfo = QtCore.QFileInfo(str(QtGui.QFileDialog.getOpenFileName(self, "Import graph as node", lastPresetFolder, "*.canvas")[0]))
        
        if fileInfo.exists() :
            point = self.dfgWidget.rect().center() 
            self.dfgWidget.createNewNodeFromJSON(fileInfo, point)
            
            fileInfo.dir().cdUp()
            self.settings.setValue( "mainWindow/lastPresetFolder", fileInfo.dir().path() )

    def performSave(self, binding, filePath):
        """Writes the current graph to disk.

        Arguments:
            binding: The graph binding to save to disk.
            filePath (str): The file path to which the binding is saved.

        """

        graph = binding.getExec()

        graph.setMetadata("timeline_start", str(self.timeLine.getRangeStart()),
                          False)
        graph.setMetadata("timeline_end", str(self.timeLine.getRangeEnd()),
                          False)
        graph.setMetadata("timeline_current", str(self.timeLine.getTime()),
                          False)
        graph.setMetadata("timeline_loopMode", str(self.timeLine.loopMode()),
                          False)
        graph.setMetadata("timeline_simMode", str(self.timeLine.simulationMode()), 
                          False)
        graph.setMetadata("timeline_timerFps", str(self.timeLine.getFps()), 
                          False)
        try:
            camera = self.viewport.getCamera()
            mat44 = camera.getMat44('Mat44')
            focalDistance = camera.getFocalDistance('Float32')

            graph.setMetadata("camera_mat44",
                              str(mat44.getJSONStr()), False)
            graph.setMetadata("camera_focalDistance",
                              str(focalDistance.getJSONStr()),
                              False)
        except Exception as e:
            print 'Exception: ' + str(e)
            raise e

        try:
            jsonVal = binding.exportJSON()
            jsonFile = open(filePath, "wb")
            if jsonFile:
                jsonFile.write(jsonVal)
                jsonFile.close()
        except Exception as e:
            print 'Exception: ' + str(e)
            return False

        return True

    def saveGraph(self, saveAs):
        """Wraps the performSave method.

        Opens a dialog for user to save the file if the lastFileName attribute
        is not set or if the saveas argument is True. Otherwise it overwrites
        the file which was opened.

        Arguments:
            saveAs (bool): Whether to save the graph to a different file path.

        """

        self.timeLine.pause()

        filePath = self.lastFileName
        if len(filePath) == 0 or saveAs:
            lastPresetFolder = str(self.settings.value(
                "mainWindow/lastPresetFolder"))
            if len(self.lastFileName) > 0:
                filePath = self.lastFileName
                if filePath.lower().endswith('.canvas'):
                    filePath = filePath[0:-7]
            else:
                filePath = lastPresetFolder

            filePath, _ = QtGui.QFileDialog.getSaveFileName(self, "Save graph",
                                                         filePath, "*.canvas")
            if len(filePath) == 0:
                return False
            if filePath.lower().endswith(".canvas.canvas"):
                filePath = filePath[0:-7]
            elif not filePath.lower().endswith(".canvas"):
                filePath += ".canvas"

        folder = QtCore.QDir(filePath)
        folder.cdUp()
        self.settings.setValue("mainWindow/lastPresetFolder", folder.path())

        binding = self.dfgWidget.getDFGController().getBinding()

        if not self.performSave(binding, filePath):
            if self.dfgWidget:
                self.dfgWidget.getDFGController().log("ERROR: failed to save graph, unable to open file.")
            return False

        if self.dfgWidget:
            self.dfgWidget.getDFGController().log("graph saved.")

        self.lastFileName = filePath

        self.onFileNameChanged(filePath)

        self.lastSavedBindingVersion = binding.getVersion()

        self.setCurrentFile(filePath)

        return True

    def onFileNameChanged(self, fileName):
        """Callback for when the file name has changed.

        This method updates the window title to reflect the new file path.
        """

        if not fileName:
            self.setWindowTitle(self.windowTitle)
        else:
            self.setWindowTitle(self.windowTitle + " - " + fileName)

    def enableShortCutsAndUndoRedo(self, enabled):
        """Enables or disables shortcuts.

        enabled (bool): Whether or not to enable the shortcuts.

        """
        if self.undoAction:
            self.undoAction.blockSignals(enabled)
        if self.redoAction:
            self.redoAction.blockSignals(enabled)
        if self.newGraphAction:
            self.newGraphAction.blockSignals(enabled)
        if self.loadGraphAction:
            self.loadGraphAction.blockSignals(enabled)
        if self.importGraphAction:
            self.importGraphAction.blockSignals(enabled)
        if self.saveGraphAction:
            self.saveGraphAction.blockSignals(enabled)
        if self.saveGraphAsAction:
            self.saveGraphAsAction.blockSignals(enabled)
        if self.quitAction:
            self.quitAction.blockSignals(enabled)
        if self.manipAction:
            self.manipAction.blockSignals(enabled)
        if self.setGridVisibleAction:
            self.setGridVisibleAction.blockSignals(enabled)
        if self.resetCameraAction:
            self.resetCameraAction.blockSignals(enabled)
        if self.clearLogAction:
            self.clearLogAction.blockSignals(enabled)
        if self.showHotkeyEditorDialogAction:
            self.showHotkeyEditorDialogAction.blockSignals(enabled)

    def openRecentFile(self):
        action = self.sender()
        if action:
            if not self.checkUnsavedChanges():
              return
            self.loadGraph(action.data())

    def onAdditionalMenuActionsRequested(self, name, menu, prefix):
        """Callback for when a request to add additional menu actions is called.

        Args:
            name (str): Name of the menu.
            menu (QMenu): Menu item to add to.
            prefix (str): Whether to add the prefixed menu items or not.

        """

        if name == 'File':
            if prefix:
                self.newGraphAction = NewGraphAction(menu, self)
                self.loadGraphAction = LoadGraphAction(menu, self)
                self.importGraphAction = ImportGraphAction(menu, self)
                self.saveGraphAction = SaveGraphAction(menu, self)
                self.saveGraphAsAction = SaveGraphAsAction(menu, self)

                for i in range(self.maxRecentFiles):
                    self.recentFilesAction.append(QtGui.QAction(menu))
                    self.recentFilesAction[-1].setVisible(False)
                    self.recentFilesAction[-1].triggered.connect(self.openRecentFile)

                menu.addAction(self.newGraphAction)
                menu.addAction(self.loadGraphAction)
                menu.addAction(self.importGraphAction)
                menu.addAction(self.saveGraphAction)
                menu.addAction(self.saveGraphAsAction)
                self.separator = menu.addSeparator()
                self.separator.setVisible(True)
                for i in range(self.maxRecentFiles):
                    menu.addAction(self.recentFilesAction[i])

                self.updateRecentFileActions()
  
            else:
                menu.addSeparator()
                self.quitAction = QuitApplicationAction(menu, self)
                menu.addAction(self.quitAction)

        elif name == 'Edit':
            if prefix:
                self.undoAction = self.qUndoStack.createUndoAction(self)
                self.undoAction.setShortcut(QtGui.QKeySequence.Undo)
                self.undoAction.setShortcutContext(QtCore.Qt.ApplicationShortcut)
                Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.undoAction", self.undoAction)
                menu.addAction(self.undoAction)
                
                self.redoAction = self.qUndoStack.createRedoAction(self)
                self.redoAction.setShortcut(QtGui.QKeySequence.Redo)
                self.redoAction.setShortcutContext(QtCore.Qt.ApplicationShortcut)
                Actions.ActionRegistry.GetActionRegistry().registerAction("CanvasWindow.redoAction", self.redoAction)
                menu.addAction(self.redoAction)
            else:
                if self.isCanvas:
                    menu.addSeparator()
                    self.manipAction = ToggleManipulationAction(self, self.viewport)
                    menu.addAction(self.manipAction)

                    self.toolsDFGPVNotifierRegistry.toolRegistered.connect(self.manipAction.triggerIfInactive)
                    self.deleteDFGPVToolsAction = FabricUI.DFG.DFGDeleteAllPVToolsAction(self, "CanvasWindow.deleteDFGPVToolsAction", "Delete Edition Tool")
                    menu.addAction(self.deleteDFGPVToolsAction)
                    # menu.addSeparator()

                    # editorMenu = menu.addMenu("Editors")
                    # self.showHotkeyEditorDialogAction = ShowHotkeyEditorDialogAction(editorMenu, self)
                    # editorMenu.addAction(self.showHotkeyEditorDialogAction)

        elif name == 'View':
            if prefix:

                if self.isCanvas:
                    self.setGridVisibleAction = GridVisibilityAction(self.viewport, self.viewport)
                    self.resetCameraAction = ResetCameraAction(self.viewport, self.viewport)

                self.clearLogAction = QtGui.QAction('&Clear Log Messages', None)
                self.clearLogAction.triggered.connect(self.logWidget.clear)

                if self.isCanvas:
                    menu.addAction(self.setGridVisibleAction)
                    menu.addSeparator()
                    menu.addAction(self.resetCameraAction)
                    menu.addSeparator()
                menu.addAction(self.clearLogAction)

    def onGraphSet(self, graph):
        """Callback when the graph is set.

        Args:
            graph: The graph that is being set as current.

        """

        if graph != self.currentGraph:
            graph = self.dfgWidget.getUIGraph()
            graph.nodeEditRequested.connect(self.onNodeEditRequested)
            graph.nodeInspectRequested.connect(self.onNodeInspectRequested)
            self.currentGraph = graph

    def dragEnterEvent(self, event):
        # we accept the proposed action only if we
        # are dealing with a single '.canvas' file.
        if event.mimeData().hasUrls():
            urls = event.mimeData().urls()
            if len(urls) == 1:
                url = urls[0];
                filename = FabricUI.Util.GetFilenameForFileURL(url)
                if filename.endswith(".canvas"):
                    event.acceptProposedAction()
                    return

        QtGui.QMainWindow.dragEnterEvent(self, event)

    def dropEvent(self, event):
        # The mimeData was already checked in the dragEnterEvent(), so we simply get the filepath and load the graph.
        # We also check if the Control key is down and, if it is, we load the scene without prompting.
        url = event.mimeData().urls()[0]
        event.acceptProposedAction()

        bypassUnsavedChanges = event.keyboardModifiers() & QtCore.Qt.ControlModifier
        self.onUrlDropped(url, bypassUnsavedChanges, False, event.pos() )

    def onUrlDropped(self, url, bypassUnsavedChanges, importAsNode, pos):
        """Callback when an item (.canvas file) is dropped on the graphview.

        Args:
            url: The path of the graph to load or import.
            bypassUnsavedChanges: It the graph is loaded (importAsNode == False), check if the graph needs to be saved
            importAsNode: If true, import the file as a node, load it as the current graph otherwise.
            pos: The drop position.
        """

        fileInfo = QtCore.QFileInfo(FabricUI.Util.GetFilenameForFileURL(url))
        if not fileInfo.exists():
            return

        if importAsNode:
            self.dfgWidget.createNewNodeFromJSON(fileInfo, pos)
        else:
            self.timeLine.pause()

            if not (bypassUnsavedChanges or self.checkUnsavedChanges()):
                return
        
            self.loadGraph(fileInfo.filePath())

    def onRevealPresetInExplorer(self, nodeName):
        """Callback for when 'Reveal in explorer' is invoked.
        """

        # [FE-8400] ensure the explorer is visible before revealing the preset.
        if not self.treeDock.isVisible() or self.treeDock.visibleRegion().isEmpty():
          self.treeDock.setVisible(True);
          self.treeDock.raise_();

        self.treeWidget.onExpandToAndSelectItem(nodeName)

    def onNodeInspectRequested(self, node):
        """Callback for when 'Inspect node' is invoked.
        """
        if node.isBackDropNode():
          return;

        # [FE-8411] ensure the value editor is visible.
        if not self.valueEditorDockWidget.isVisible() or self.valueEditorDockWidget.visibleRegion().isEmpty():
          self.valueEditorDockWidget.setVisible(True);
          self.valueEditorDockWidget.raise_();

        graph = self.dfgWidget.getUIGraph();
        graph.clearInspection();
        node.setInspected(True);
