#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import DFG, Application as CppApplication

class FabricLog(CppApplication.FabricLog):

    """ FabricLog specialize CppApplication.FabricLog in python
    """
    def __init__(self): 
        super(FabricLog, self).__init__()
 
    def log(self, message):
        """ Impl. of CppApplication.FabricLog.
        """
        super(FabricLog, self).log(message)
        DFG.DFGLogWidget.log(str(message))

# \internal, store the app-states singleton 
global s_logSingleton
s_logSingleton = None

def GetFabricLog():
    """ Creates the FabricLog singleton.
    """
    global s_logSingleton
    if s_logSingleton is None:
        s_logSingleton = FabricLog()
    return s_logSingleton
 