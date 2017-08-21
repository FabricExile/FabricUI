#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.Canvas.Utils import *

class HotkeyStyledItemDelegate(QtGui.QStyledItemDelegate):

    keyPressed = QtCore.Signal(QtGui.QKeySequence)

    def __init__(self, parent=None):
        super(HotkeyStyledItemDelegate, self).__init__(parent)
        self.parent = parent

    def createEditor(self, parent, option, index):
        self.editor = QtGui.QLineEdit(parent)
        self.editor.setFrame(False)
        self.editor.installEventFilter(self)
        return self.editor

    def setEditorData(self, editor, index):
        value = index.model().data(index, QtCore.Qt.EditRole)
        editor.setText(value)

    def setModelData(self, editor, model, index):
        value = editor.text()
        model.setData(index, value, QtCore.Qt.EditRole)

    def updateEditorGeometry(self, editor, option, index):
        editor.setGeometry(option.rect)

    def eventFilter(self, target, event):
        if target is self.editor:
            if event.type() == QtCore.QEvent.KeyPress:
                
                # Gets the sequence from the event.
                keySequence = GetQKeySequenceFromQKeyEvent(event)
                if keySequence is not None:
                    self.keyPressed.emit(keySequence)
                return True  

            if event.type() == QtCore.QEvent.MouseButtonPress:
                return True 

            if event.type() == QtCore.QEvent.MouseButtonDblClick:
                return True  

            if event.type() == QtCore.QEvent.MouseMove:
                return True

        return False    
