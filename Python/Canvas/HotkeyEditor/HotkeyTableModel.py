#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions as CppActions
from FabricEngine.Canvas.Application.FabricApplicationStates import *

class HotkeyTableModel(QtCore.QObject):

    """ HotkeyTableModel. 
    """

    updateShortcutItem = QtCore.Signal(str, str)

    def __init__(self, parentWidget):
        super(HotkeyTableModel, self).__init__()
        self.parentWidget = parentWidget

    def initItemKeySequence(self, actName, keySeq):
        settings = GetAppStates().getSettings()

        # Fetching the value from the QSettings
        if settings and settings.contains(actName):
            shortcut = settings.value(actName)
            newKeySeq = QtGui.QKeySequence(shortcut)

            if shortcut and newKeySeq != keySeq:
                keySeq = newKeySeq
    
        self.__setItemKeySequenceAndShortcut(actName, keySeq)

    def resetItemKeySequence(self):
        registry = CppActions.ActionRegistry.GetActionRegistry()
        for actName in registry.getActionNameList():
            self.__setItemKeySequenceAndShortcut(actName, registry.getDefaultShortcut(actName))

    def __setItemKeySequenceAndShortcut(self, actName, keySeq = QtGui.QKeySequence()):
        """ \internal.
        """
        settings = GetAppStates().getSettings()
        shortcut = keySeq.toString(QtGui.QKeySequence.NativeText)

        if settings:
            settings.setValue(actName, shortcut)

        CppActions.ActionRegistry.GetActionRegistry().setShortcut(actName, keySeq)
        self.updateShortcutItem.emit(actName, shortcut)

    def __shortcutIsUsedBy(self, actName, shortcut):
        """ \internal.
            Check if the action named `actName` can use the shorcut.
            If true, returns None. Otherwise, returns the name of the 
            action using the short cut alredy.
        """
        actRegistry = CppActions.ActionRegistry.GetActionRegistry()
        otherActList = actRegistry.isShortcutUsed(shortcut)
        globalContext = actRegistry.isActionContextGlobal(actName) 

        action = actRegistry.getAction(actName)
        if action is None:
            return

        if action in otherActList:
            otherActList.remove(action)

        for otherAction in otherActList:
            otherActName = actRegistry.getActionName(otherAction)
            otherGlobalContext = actRegistry.isActionContextGlobal(otherActName) 
            
            # If both actions are local (to their widget),
            # check if they have the same parent.
            shareParent = False
            if not otherGlobalContext and not globalContext:
                shareParent = otherAction.parentWidget() == action.parentWidget()

            if otherGlobalContext or globalContext or shareParent:
                return otherActName
 
    def setItemKeySequence(self, actName, keySeq, force = True):
        """ \internal.
            Sets the keySeq to the item. If another items/actions 
            used the keySeq already, a warning is displayed and 
            the method returns. if 'force' is true, force the setting.

            If a keySeq is invalid or corresponds the `Del` or 
            `Backspace` key, the item keySeq is invalidate.
        """
        if  (   keySeq != QtGui.QKeySequence() and 
                keySeq != QtGui.QKeySequence('Del') and 
                keySeq != QtGui.QKeySequence('Backspace') ):

            actName_ = self.__shortcutIsUsedBy(actName, keySeq)
                
            if actName_ and not force:
                shortcut = keySeq.toString(QtGui.QKeySequence.NativeText)
                message = QtGui.QMessageBox()
                message.warning(self.parentWidget, 'Hotkey editor', 
                    'shorcut ' + str(shortcut) + ' already used by ' + str(actName_))
                return False

            elif not actName_ or force:
                self.__setItemKeySequenceAndShortcut(actName, keySeq)
        
        else:
            self.__setItemKeySequenceAndShortcut(actName)

        return True
