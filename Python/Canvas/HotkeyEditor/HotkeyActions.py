#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions as CppActions
from FabricEngine.Canvas.Commands.CommandManager import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyCommands import *

class CommandAction(CppActions.BaseAction):
    
    def __init__(self, parent, cmdName, implType, shortcut, tooltip, isScriptable):
        super(CommandAction, self).__init__(parent)
        self.implType = implType
        self.cmdName = cmdName
        self.init(cmdName,tooltip, shortcut, 
            QtCore.Qt.ApplicationShortcut,
            True, not isScriptable)  
 
    def onTriggered(self):
        try:
            GetCommandManager().createCommand(self.cmdName)
            GetCommandManager().synchronizeKL()
        except Exception as e:    
            print str(e)

class BaseHotkeyEditorAction(CppActions.BaseAction):
 
    def __init__(self, hotkeyEditor, name, text, shortcut):

        super(BaseHotkeyEditorAction, self).__init__(hotkeyEditor)
        self.hotkeyEditor = hotkeyEditor
        self.hotkeyEditor.addAction(self)
        self.hotkeyEditor.hotkeyTable.editingItem.connect(self.onEditingItem)

        self.init(name, text, shortcut, 
            QtCore.Qt.WidgetWithChildrenShortcut,
            True, False)  

    def onEditingItem(self, editing):
        self.setEnabled(not editing)

class OpenFileAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(OpenFileAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.OpenFileAction", 
            "Open", 
            QtGui.QKeySequence("Ctrl+O"))

        self.setToolTip('Open file')
        
    def onTriggered(self):
        cmd = OpenFileCommand(self.hotkeyEditor)
        if cmd.doIt():
            self.hotkeyEditor.hotkeyTable.qUndoStack.push(cmd)
            self.hotkeyEditor.hotkeyTable.onEmitEditingItem(False)
            
class SaveFileAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(SaveFileAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.SaveFileAction", 
            "Save", 
            QtGui.QKeySequence("Ctrl+S"))

        self.setToolTip('Save file')
        
    def onTriggered(self):
        SaveFileCommand()
        self.hotkeyEditor.hotkeyTable.onEmitEditingItem(False)

class ExitAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(ExitAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.ExitAction", 
            "Ok", 
            QtGui.QKeySequence("Return"))

        self.setToolTip('Accept the changes and close the dialog')
        
    def onTriggered(self):
        self.hotkeyEditor.accept()
 
class UndoAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(UndoAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.UndoAction", 
            "Undo", 
            QtGui.QKeySequence("Ctrl+Z"))

        self.setToolTip('Undo the changes.')
        self.setEnabled(False)
    
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.qUndoStack.undo()
        self.hotkeyEditor.hotkeyTable.onEmitEditingItem(False)

    def onEditingItem(self, editing):
        index = self.hotkeyEditor.hotkeyTable.qUndoStack.index()
        count = self.hotkeyEditor.hotkeyTable.qUndoStack.count()
        if count > 0 and index > 0:
            self.setEnabled(not editing)
        else:
            self.setEnabled(False)

class RedoAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(RedoAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.RedoAction", 
            "Redo", 
            QtGui.QKeySequence("Ctrl+Shift+Z"))

        self.setEnabled(False)
        self.setToolTip('Redo the changes.')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.qUndoStack.redo()
        self.hotkeyEditor.hotkeyTable.onEmitEditingItem(False)

    def onEditingItem(self, editing):
        index = self.hotkeyEditor.hotkeyTable.qUndoStack.index()
        count = self.hotkeyEditor.hotkeyTable.qUndoStack.count()
        if count > 0 and index < count:
            self.setEnabled(not editing)
        else:
            self.setEnabled(False)