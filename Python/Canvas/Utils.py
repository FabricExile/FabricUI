#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import re, sys, imp
from PySide import QtCore, QtGui

def CreateSearchRegex(query):
    """ Creates a regex to search for 
        a string within the query.
    """
    
    # Remove white-space and non alpha-num
    pattern = query.lower().replace(' ', '')
    regex = re.compile('[^0-9a-zA-Z]')
    pattern = regex.sub('', pattern)

    # Converts 'abc' to 'a.*b.*c'
    pattern = '.*'.join(pattern)   
    regex = re.compile(pattern) 

    return regex

def GetOrCreateModule(name):
    """ Get or creates dynamically a module named "name".
    	- module = GetOrCreateModule(moduleName)

    	Once the module has been created, it can be imported the usual way:
    	- import module
    """
    
    exist = True

    # Check if the module exists already.
    try:
        module = sys.modules[name]
    except Exception, e:
        exist = False

    if exist is False:
        module = imp.new_module(name)
        sys.modules[name] = module

    return sys.modules[name]

def GetQKeySequenceFromQKeyEvent(keyEvent):
    """ Gets the QKeySequence of the key 
        strockes of the key event.
    """
    
    if keyEvent.type() is not QtCore.QEvent.KeyPress:
        return None

    if keyEvent.isAutoRepeat():
        return None

    key = keyEvent.key() 

    if key == QtCore.Qt.Key_unknown:
        return None

    if (key == QtCore.Qt.Key_Control or
        key == QtCore.Qt.Key_Shift or
        key == QtCore.Qt.Key_Alt or
        key == QtCore.Qt.Key_Meta):
        return None
 
    modifiers = keyEvent.modifiers() 
    if modifiers & QtCore.Qt.ShiftModifier:
        key = key + QtCore.Qt.SHIFT 

    if modifiers & QtCore.Qt.ControlModifier:
        key = key + QtCore.Qt.CTRL 

    if modifiers & QtCore.Qt.AltModifier:
        key = key + QtCore.Qt.ALT 

    if modifiers & QtCore.Qt.MetaModifier:
        key = key + QtCore.Qt.META 

    return QtGui.QKeySequence(key)
