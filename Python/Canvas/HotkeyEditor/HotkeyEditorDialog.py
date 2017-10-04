#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyActions import *
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableWidget import HotkeyTableWidget
    
class HotkeyEditorDialog(QtGui.QDialog):

    def __init__(self, canvasWindow):
        """ Initializes the HotkeyEditorDialog.
            Arguments:
            - canvasWindow: A reference to the canvasWindow widget.
        """
        super(HotkeyEditorDialog, self).__init__(canvasWindow) 
        
        # qss
        self.setWindowTitle('Hotkey Editor')
        self.setObjectName('HotkeyEditorDialog')
        self.setStyleSheet(LoadFabricStyleSheet('FabricUI.qss'))

        # HotkeyTableWidget
        self.hotkeyTable = HotkeyTableWidget(self, canvasWindow)

        # Controls
        comboBoxLabel = QtGui.QLabel('Set')
        self.__itemComboBox = QtGui.QComboBox(self)
        self.__itemComboBox.addItem('All')
        self.__itemComboBox.addItem('Actions')
        self.__itemComboBox.addItem('Commands')
        self.__itemComboBox.currentIndexChanged.connect(self.__onFilterItems)

        checkBoxLabel = QtGui.QLabel('Editable')
        self.__editComboBox = QtGui.QComboBox(self)
        self.__editComboBox.addItem('All')
        self.__editComboBox.addItem('Yes')
        self.__editComboBox.addItem('No')
        self.__editComboBox.currentIndexChanged.connect(self.__onFilterItems)

        lineEditLabel = QtGui.QLabel('Search')
        toolTip = ( 'Type the name of the action/command you want to find. \n'
                    'Use # to find an action/command from its shortcut' )
        lineEditLabel.setToolTip(toolTip)
        self.__lineEdit = QtGui.QLineEdit(self)
        self.__lineEdit.textChanged.connect(self.__onFilterItems)

        ctrlLayout = QtGui.QHBoxLayout()
        ctrlLayout.addWidget(lineEditLabel)
        ctrlLayout.addWidget(self.__lineEdit)
        ctrlLayout.addWidget(comboBoxLabel)
        ctrlLayout.addWidget(self.__itemComboBox)
        ctrlLayout.addWidget(checkBoxLabel)
        ctrlLayout.addWidget(self.__editComboBox)
  
        # Toolbar
        toolBar = QtGui.QToolBar()
        toolBar.addAction(OpenFileAction(self))
        toolBar.addAction(SaveFileAction(self))
        toolBar.addAction(ResetAction(self))    
        toolBar.addSeparator()
        toolBar.addAction(UndoAction(self))
        toolBar.addAction(RedoAction(self))
        toolBar.addSeparator()
        toolBar.addAction(ExitAction(self))
    
        # All
        layout = QtGui.QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(toolBar)
        layout.addLayout(ctrlLayout)
        layout.addWidget(self.hotkeyTable)
        self.setLayout(layout)

        # !!!! To change
        self.setMinimumHeight(350)
        self.setMinimumWidth(550)
        self.adjustSize()
        
    def __onFilterItems(self):
        """ \internal. filter the items.
        """
        self.hotkeyTable.filterItems(
            self.__lineEdit.text(), 
            self.__editComboBox.currentIndex(),
            self.__itemComboBox.currentIndex())

    def showEvent(self, event):
        """ Impl. of QtGui.QDialog.
        """
        super(HotkeyEditorDialog, self).showEvent(event)
        try:
            GetCommandRegistry().synchronizeKL();
        except Exception as e:    
            print str(e)
 