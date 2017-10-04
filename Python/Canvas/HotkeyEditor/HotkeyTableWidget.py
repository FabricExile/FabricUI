#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#
    
import re
from PySide import QtCore, QtGui
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.FabricUI import Actions as CppActions, Commands as CppCommands
from FabricEngine.Canvas.HotkeyEditor.HotkeyActions import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyCommands import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableModel import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableWidgetItems import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyStyledItemDelegate import *

class HotkeyTableWidget(QtGui.QTableWidget):

    """ HotkeyTableWidget is used to associate a shortcut to an action/command.
        When an action or command is registered, a signal is sent to the 
        table so it creates a new item. Items can be filtered according 
        to the actions name or shortcut. 

        To associate a new shortcut to an action, the user needs first to 
        select which action item to update and then press a key. When done, 
        the user can accept or reject the changes to actually update the 
        actions' shortcuts.
         
        A list of pair {actName, shortcut} can be saved in as json.
        The file can be opened and sets the shortucts of the actions it 
        lists. When an action is registered, we check if it's litsed in
        the file to override its shortcut.
    """

    editingItem = QtCore.Signal(bool)
 
    def __init__(self, parent, canvasWindow):
        """ Initializes the HotkeyTableWidget.
            
            Arguments:
            - parent: A reference to wiget.
            - canvasWindow: A reference the canvasWindow.
        """
        super(HotkeyTableWidget, self).__init__(parent)
        self.canEditItem = False # To edit item on double click only

        self.qUndoStack = QtGui.QUndoStack()

        # Used to attached the Command actions
        self.canvasWindow = canvasWindow
        self.model = HotkeyTableModel(self)
        self.model.updateShortcutItem.connect(self.__onUpdateShortcutItem)

        # Notify when an action is registered-
        # unregistered from CppActions.ActionRegistry.
        actRegistry = CppActions.ActionRegistry.GetActionRegistry()
        actRegistry.actionRegistered.connect(self.__onActionRegistered)
        actRegistry.actionUnregistered.connect(self.__onActionUnregistered)

        # Notify when an command is registered.
        GetCommandRegistry().registrationDone.connect(self.__onCommandRegistered)
        
        # Construct the item-delegate
        itemDelegate = HotkeyStyledItemDelegate(self)
        itemDelegate.keyPressed.connect(self.__onSetItemKeySequence)
        self.setItemDelegate(itemDelegate)

        # Two coloums: [actName, shortcut]
        self.setColumnCount(2)
        self.setHorizontalHeaderItem(0, QtGui.QTableWidgetItem('Name'))
        self.setHorizontalHeaderItem(1, QtGui.QTableWidgetItem('Shortcut'))
 
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragEnabled(False)
        self.setSortingEnabled(True)
        self.verticalHeader().setVisible(False)
        self.horizontalHeader().setStretchLastSection(True)

        # qss
        self.setObjectName('HotkeyTableWidget')

        for cmdName in GetCommandRegistry().getCommandNames():
            cmdType, implType = GetCommandRegistry().getCommandSpecs(cmdName)
            self.__onCommandRegistered(cmdName, cmdType, implType)

    def onEmitEditingItem(self, editing):
        self.editingItem.emit(editing)

    def __onUpdateShortcutItem(self, actName, shortcut):
        item = self.__getShorcutItem(actName)
        if item :
            item.setText(shortcut)
 
    def __getCurrentShortcutItem(self):
        """ \internal.
            Gets the current shortcut item
        """
        if self.currentColumn() == self.columnCount()-1:
            return self.item(self.currentRow(), self.currentColumn())

    def __getShorcutItem(self, actName):
        """ \internal.
            Gets the shortcut item from the action name.
        """
        items = self.findItems(actName, QtCore.Qt.MatchExactly)
        if items:
            return self.item(items[0].row(), 1)

    def __getActionItem(self, actName):
        """ \internal.
            Gets the action item from the action name.
        """
        items = self.findItems(actName, QtCore.Qt.MatchExactly)
        if items:
            return self.item(items[0].row(), 0)
 
    def mouseDoubleClickEvent(self, event):
        """ Implementation of QtGui.QTableWidget.
        """
        super(HotkeyTableWidget, self).mouseDoubleClickEvent(event)
        item = self.__getCurrentShortcutItem()
        if item:
            self.onEmitEditingItem(True)
        self.canEditItem = True

    def mousePressEvent(self, event):
        """ Implementation of QtGui.QTableWidget.
        """
        # Reset the selection.
        self.setCurrentItem(None, QtGui.QItemSelectionModel.Clear)
        super(HotkeyTableWidget, self).mousePressEvent(event)
        if not self.__getCurrentShortcutItem():
            self.onEmitEditingItem(False)
        self.canEditItem = False

    def keyboardSearch(self, search):
        """ Implementation of QtGui.QAbstractItemView.
            Do nothing.
        """
        pass

    def keyPressEvent(self, event):
        """ Implementation of QtGui.QAbstractItemView.
            Edit item on double click only.
        """
        item = self.__getCurrentShortcutItem()

        if item and self.canEditItem is False:
            return

        super(HotkeyTableWidget, self).keyPressEvent(event)

    def __createNewRow(self, actName, action):
        """ \internal.
            Create a new row: [actName, shortcut] items.
        """
        rowCount = self.rowCount() 
        self.insertRow(rowCount)
        
        # Check if the action is editable
        isEditable = True
        if issubclass(type(action), CppActions.BaseAction):
            isEditable = action.isEditable()

        # Check if the action is associated to a command.
        isCommand = None
        if issubclass(type(action), CommandAction):
            isCommand = action.implType
            
        # 1. Action item
        item = ActionTableWidgetItem(actName, action.toolTip(), isEditable, isCommand) 
        self.setItem(rowCount, 0, item)

        # 2. Shortcut item
        keySequence = action.shortcut()
        shortcut = keySequence.toString(QtGui.QKeySequence.NativeText)
        actRegistry = CppActions.ActionRegistry.GetActionRegistry()
        isActGlobal = actRegistry.isActionContextGlobal(actName) 
        item = ShorcutTableWidgetItem(shortcut, isEditable, isActGlobal)
        
        self.setItem(rowCount, 1, item)
        self.resizeColumnsToContents()

        self.model.initItemKeySequence(actName, keySequence) 

    def __onCommandRegistered(self, cmdName, cmdType, implType):
        """ \internal.
            Called when an command has been registered in CommandRegistry.
            Create an action associated to the command. 
        """
        try:
            actRegistry = CppActions.ActionRegistry.GetActionRegistry()

            if actRegistry.getAction(cmdName) is None:
                # Must construct the command to get the tooltip
                cmd = GetCommandRegistry().createCommand(cmdName)

                tooltip = cmdType+ "[" + implType + "]\n\n"
                tooltip += str(cmd.getHelp())
                isScriptable = CppCommands.CommandHelpers.isScriptableCommand(cmd)

                # Add the action to the canvasWindow so it's available.
                # Actions of hidden widgets are not triggered.
                action = CommandAction(
                    self, 
                    cmdName, 
                    implType,
                    QtGui.QKeySequence(), 
                    tooltip, 
                    isScriptable)

                self.canvasWindow.addAction(action)
        except Exception as e:    
            print str(e)
            
    def __onActionRegistered(self, actName, action):
        """ \internal.
            Called when an action has been registered in CppActions.ActionRegistry.
            Create an item associated to the action. 
        """
        actRegistry = CppActions.ActionRegistry.GetActionRegistry()

        # To determine if the item should be created, it should be sufficient to only 
        # check if the number of actions registered under `actName` is equal to one 
        # However, because of C++/Python thread issues, it may happens that this condition 
        # is not enough, so we need to check on the UI side too. 
        if actRegistry.getRegistrationCount(actName) == 1 and self.__getActionItem(actName) is None:    
            self.__createNewRow(actName, action)
        
        # To update the item tool tip.
        actionTableWidgetItem = self.__getActionItem(actName)
        if actionTableWidgetItem:
            action.changed.connect(actionTableWidgetItem.onActionChanged)

    def __onActionUnregistered(self, actName):
        """ \internal.
            Called when an action has been  
            unregistered from CppActions.ActionRegistry.
        """
        actRegistry = CppActions.ActionRegistry.GetActionRegistry()
        # Check there is no more action registered under `actName`.
        if actRegistry.getRegistrationCount(actName) == 0:
            # if so, remove the item.
            item = self.__getActionItem(actName)
            if item:
                self.removeRow(item.row())

    def __onSetItemKeySequence(self, keySeq):
        """ \internal.
            Sets the keySeq of the current 
            item when the user press a key.
        """
        item = self.__getCurrentShortcutItem()
        actName = self.item(item.row(), 0).text()
        curKeySeq = QtGui.QKeySequence(item.text())

        if item and keySeq != curKeySeq:
            cmd = SetKeySequenceCommand(self.model, actName, curKeySeq, keySeq)
            if cmd.succefullyDone is True:
                self.qUndoStack.push(cmd)
                self.onEmitEditingItem(True)
 
    def filterItems(self, query, edit = 0, show = 0):
        """ \internal.
            Filters the items according the actions' names or shorcuts.
            To filter by shortcut, use '#' before the query.  
        """
        actRegistry = CppActions.ActionRegistry.GetActionRegistry()

        searchByShortcut = False
        if len(query):
            searchByShortcut = query[0] == '#'

        # Remove white-space and non alpha-num
        pattern = query.lower().replace(' ', '')
        regex = re.compile('[^0-9a-zA-Z]')
        pattern = regex.sub('', pattern)

        # Converts 'abc' to 'a.*b.*c'
        pattern = '.*'.join(pattern)   
        regex = re.compile(pattern) 

        for i in range(0, self.rowCount()):

            # Checks the action's name/shortcut matches.
            actName = self.item(i, 0).text()
            shortCut = self.item(i, 1).text()

            isEditable = True
            action = actRegistry.getAction(actName)
            if issubclass(type(action), CppActions.BaseAction):
                isEditable = action.isEditable()
 
            if  (   (searchByShortcut and regex.search(shortCut.lower()) ) or 
                    (not searchByShortcut and regex.search(actName.lower()) ) ):

                isCommand = GetCommandRegistry().isCommandRegistered(actName)
                
                showEditable = True
                if edit == 1 and isEditable == False:
                    showEditable = False
                elif edit == 2 and isEditable == True:
                    showEditable = False

                hide = not showEditable

                # Show actions + cmds
                if show == 0:
                    self.setRowHidden(i, hide)
                
                # Show actions  
                elif show == 1:
                    if isCommand:
                        self.setRowHidden(i, True)
                    else:
                        self.setRowHidden(i, hide)

                # Show cmd  
                elif show == 2:
                    if isCommand:
                        self.setRowHidden(i, hide)
                    else:
                        self.setRowHidden(i, True)

            # If nothing found, hide the row.    
            else:
                self.setRowHidden(i, True)

