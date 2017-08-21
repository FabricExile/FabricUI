"""
The LogWidget receives and saves logging output from the ScriptEditor.
"""

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import DFG, Actions

class AppendingTextWidget(QtGui.QTextEdit):
    def __init__(self):
        QtGui.QTextEdit.__init__(self)

        self.setReadOnly(True)
        self.prevCursorPos = 0
        
    def append(self, text, color):
        self.setTextColor(color)
        charFormat = self.currentCharFormat()
        textCursor = self.textCursor()
        textCursor.movePosition(QtGui.QTextCursor.End)
        self.prevCursorPos = textCursor.position()
        textCursor.insertText(text, charFormat)
        self.setTextCursor(textCursor)
        self.ensureCursorVisible()

    def replace(self, text, color):
        self.setTextColor(color)
        charFormat = self.currentCharFormat()
        textCursor = self.textCursor();
        textCursor.setPosition(self.prevCursorPos, QtGui.QTextCursor.KeepAnchor);
        self.moveCursor(QtGui.QTextCursor.End, QtGui.QTextCursor.KeepAnchor);
        textCursor.insertText(text, charFormat)
        self.setTextCursor(textCursor)
        self.ensureCursorVisible()

class LogWidget(AppendingTextWidget):
    def __init__(self):
        AppendingTextWidget.__init__(self)

        self.setFocusPolicy(QtCore.Qt.StrongFocus)

        self.setObjectName("ScriptEditorLogWidget")

        self.commandColor = QtGui.QColor(QtCore.Qt.white)
        self.commentColor = QtGui.QColor("#9AD6D6")
        self.exceptionColor = QtGui.QColor("#E14D59")

        self.copyAction = CopyLogAction(self)
        self.copyAvailable.connect(self.copyAction.setEnabled)

    def clearAction(self, desc):
        return ClearLogAction(self, desc)

    def appendCommand(self, text, replace = False):
        if replace is True:
            self.replace(text, self.commandColor)
        else:
            self.append(text, self.commandColor)

    def appendComment(self, text, replace = False):
        if replace is True:
            self.replace(text, self.commentColor)
        else:
            self.append(text, self.commentColor)

    def appendException(self, text, replace = False):
        if replace is True:
            self.replace(text, self.exceptionColor)
        else:
            self.append(text, self.exceptionColor)

    def contextMenuEvent(self, event):
        menu = QtGui.QMenu(self)
        menu.addAction(self.copyAction)
        menu.addSeparator()
        menu.addAction(self.clearAction("Clear"))
        menu.exec_(self.mapToGlobal(event.pos()))

class BaseLogWidgetAction(Actions.BaseAction):
 
    def __init__(self,
        logWidget, 
        name, 
        text, 
        shortcut = QtGui.QKeySequence(), 
        context = QtCore.Qt.WidgetShortcut):

        self.logWidget = logWidget

        super(BaseLogWidgetAction, self).__init__(
            logWidget, 
            name, 
            text, 
            shortcut, 
            context)
        self.logWidget.addAction(self)

class CopyLogAction(BaseLogWidgetAction):
 
    def __init__(self, logWidget):

        super(CopyLogAction, self).__init__(
            logWidget, 
            "LogWidget.CopyLogAction", 
            "Copy", 
            QtGui.QKeySequence.Copy)
        
        self.setEnabled(self.logWidget.textCursor().hasSelection())

    def onTriggered(self):
        self.logWidget.copy()

class ClearLogAction(BaseLogWidgetAction):
 
    def __init__(self, logWidget, desc):

        super(ClearLogAction, self).__init__(
            logWidget, 
            "LogWidget.clearAction", 
            desc)

    def onTriggered(self):
        self.logWidget.clear()
