#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import json, os
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions as CppActions
from FabricEngine.Canvas.Application.FabricApplicationStates import *
 
class BaseHotkeyCommand(QtGui.QUndoCommand):
    class State:
        REDO_IT = 0
        UNDO_IT = 1

    def __init__(self):
        super(BaseHotkeyCommand, self).__init__()
        self.state = self.State.UNDO_IT

class SetKeySequenceCommand(BaseHotkeyCommand):
    def __init__(self, model, actName, prevKeySeq, keySeq):
        super(SetKeySequenceCommand, self).__init__()
        self.prevKeySeq = prevKeySeq
        self.keySeq = keySeq
        self.actName = actName
        self.model = model;

        self.state = self.State.REDO_IT
        self.redo()
 
    def redo(self):
        if self.state == self.State.REDO_IT:
            self.model.setItemKeySequence(self.actName, self.keySeq, False)
            self.state = self.State.UNDO_IT

    def undo(self):
        if self.state == self.State.UNDO_IT:
            self.model.setItemKeySequence(self.actName, self.prevKeySeq)
            self.state = self.State.REDO_IT

class OpenFileCommand(BaseHotkeyCommand):
    
    def __init__(self, hotkeyEditor):
        super(OpenFileCommand, self).__init__()
        self.jsonData = None
        self.preShortcutList = {}
        self.prevWindowTitle = None
        self.hotkeyEditor = hotkeyEditor
         
    def doIt(self):
        lastDir = str(GetAppStates().getSettings().value("hotkeyEditor/lastFolder"))
        fname, _ = QtGui.QFileDialog.getOpenFileName(None, "Open Hotkey file", lastDir, "*.json")
 
        # Override the shortcuts from the json matches.
        if len(fname) > 0:
            with open(fname) as infile:  
                self.jsonData = json.load(infile)
                
            head, tail = os.path.split(fname)
            self.windowtTitle = 'Hotkey Editor ' + tail
            self.prevWindowTitle = self.hotkeyEditor.windowTitle()

            actRegistry = CppActions.ActionRegistry.GetActionRegistry()
            for actName in actRegistry.getActionNameList():
                self.preShortcutList[actName] = actRegistry.getShortcuts(actName);

            self.state = self.State.REDO_IT
            self.redo()
            return True

        return False

    def __setKeySequenceList(self, items, windowTitle):
        for actName, shortcutList in items.iteritems():
            keySeq = QtGui.QKeySequence()
            if shortcutList:
                keySeq = QtGui.QKeySequence(shortcutList[0])
            self.hotkeyEditor.hotkeyTable.model.setItemKeySequence(actName, keySeq)
            self.hotkeyEditor.setWindowTitle(windowTitle)

    def redo(self):
        if self.state == self.State.REDO_IT:
            self.__setKeySequenceList(self.jsonData, self.windowtTitle)
            self.state = self.State.UNDO_IT

    def undo(self):
        if self.state == self.State.UNDO_IT:
            self.__setKeySequenceList(self.preShortcutList, self.prevWindowTitle)
            self.state = self.State.REDO_IT

class SaveFileCommand(BaseHotkeyCommand):
    def __init__(self):
        super(SaveFileCommand, self).__init__()
 
        ext = ".json"
        fname = str(GetAppStates().getSettings().value("hotkeyEditor/lastFolder"))
        fname, _ = QtGui.QFileDialog.getSaveFileName(None, "Save Hotkey file", fname, str("*" + ext))
            
        if not fname:
            return False

        # Pyside QFileDialog bug on linux, extension is not added by default.
        if not fname.endswith(ext):
            fname += ext

        GetAppStates().getSettings().setValue("hotkeyEditor/lastFolder", os.path.dirname(fname))
        actRegistry = CppActions.ActionRegistry.GetActionRegistry()

        jsonData = {}
        for actName in actRegistry.getActionNameList():
            shortcutList = []
            for keySeq in actRegistry.getShortcuts(actName):
                shortcutList.append(keySeq.toString(QtGui.QKeySequence.NativeText))
            jsonData[actName] = shortcutList
         
        with open(fname, 'w') as outfile:  
            json.dump(jsonData, outfile, ensure_ascii=False, indent=4)
 