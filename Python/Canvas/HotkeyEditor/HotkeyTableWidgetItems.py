#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions as CppActions
 
class TableWidgetItemColor:
    Disabled = QtCore.Qt.gray
    IsAction = QtGui.QColor(223, 102, 0, 255)
    IsCppCommand = QtGui.QColor(0, 191, 191, 255)
    IsKLCommand = QtGui.QColor(0, 102, 223, 255)
    IsPythonCommand = QtGui.QColor(0, 168, 102, 255)
 
class BaseTableWidgetItem(QtGui.QTableWidgetItem):
    
    def __init__(self, actName, text, isEditable):
        super(BaseTableWidgetItem, self).__init__(text)
        self.widgetItemColor = TableWidgetItemColor
        self.actName = actName
        self.isEditable = isEditable
        if not isEditable:
            font = self.font()
            self.setFont(font)
            self.setForeground(self.widgetItemColor.Disabled)
 
class ActionTableWidgetItem(BaseTableWidgetItem):
    
    def __init__(self, actName, tooltip, isEditable, cmdImplType = None):
        super(ActionTableWidgetItem, self).__init__(actName, actName, isEditable)
        self.setToolTip(tooltip)
        self.setFlags(QtCore.Qt.NoItemFlags)
        
        if cmdImplType:
            if cmdImplType == "CPP":
                color = self.widgetItemColor.IsCppCommand

            elif cmdImplType == "KL":
                color = self.widgetItemColor.IsKLCommand

            elif cmdImplType == "PYTHON":
                color = self.widgetItemColor.IsPythonCommand

        else:
            color = self.widgetItemColor.IsAction

        self.__setIcon(color)

    def __setIcon(self, color):
        img = QtGui.QImage(20, 20, QtGui.QImage.Format_ARGB32)
        img.fill(QtGui.QColor(0, 0, 0, 0))

        for i in range(0, 20):
            for j in range(8, 12):
                img.setPixel(j, i, color.rgba())

        # https://forum.qt.io/topic/6971/solved-qicon-disabled-full-color/7
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap.fromImage(img), QtGui.QIcon.Disabled)
        self.setIcon(icon)

    def onActionChanged(self):
        actionRegistry = CppActions.ActionRegistry.GetActionRegistry()
        action = actionRegistry.getAction(self.actName)
        if action:
            tooltip = action.toolTip()
            if not tooltip:
                tooltip = action.text()
            self.setToolTip(tooltip)

class ShorcutTableWidgetItem(BaseTableWidgetItem):

    def __init__(self, actName, shorcut, isEditable, isGlobal):

        super(ShorcutTableWidgetItem, self).__init__(actName, shorcut, isEditable)
        if not isEditable:
            self.setFlags(QtCore.Qt.NoItemFlags)

        if isGlobal:
            font = self.font()
            font.setBold(True)
            self.setFont(font)
 