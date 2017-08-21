#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions as CppActions
  
class HotkeyTableModel(QtCore.QObject):

    """ HotkeyTableModel. 
    """

    updateShortcutItem = QtCore.Signal(str, str)

    def __init__(self, parentWidget):
        super(HotkeyTableModel, self).__init__()
        self.parentWidget = parentWidget

    def __setItemKeySequenceAndShortcut(self, actName, keySeq = QtGui.QKeySequence(), shortcut = ''):
        """ \internal.
        """
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
            the method returns. if 'force' is true, force to setting.

            If a keySeq is invalid or corresponds the `Del` or 
            `Backspace` key, the item keySeq is invalidate.
        """
        if  (  keySeq != QtGui.QKeySequence() and 
                keySeq != QtGui.QKeySequence('Del') and 
                keySeq != QtGui.QKeySequence('Backspace') ):

            shortcut = keySeq.toString(QtGui.QKeySequence.NativeText)
            actName_ = self.__shortcutIsUsedBy(actName, keySeq)
                
            if actName_ and not force:
                message = QtGui.QMessageBox()
                message.warning(self.parentWidget, 'Hotkey editor', 
                    'shorcut ' + str(shortcut) + ' already used by ' + str(actName_))
                return False

            elif not actName_ or force:
                self.__setItemKeySequenceAndShortcut(actName, keySeq, shortcut)
        
        else:
            self.__setItemKeySequenceAndShortcut(actName)

        return True
