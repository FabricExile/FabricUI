#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Application as CppApplication

class FabricApplicationStates(CppApplication.FabricApplicationStates):

    """ FabricApplicationStates specializes 
        CppApplication.FabricApplicationStates in python
    """
    def __init__(self, client, settings): 
        super(FabricApplicationStates, self).__init__(client, settings)
        self.__client = client

    def getClient(self):
        """ Impl. of CppApplication.FabricApplicationStates.
        """
        return self.__client

    def getContext(self):
        """ Impl. of CppApplication.FabricApplicationStates.
        """
        return self.__client.getContext()
   
# \internal, store the app-states singleton 
global s_appStatesSingleton
s_appStatesSingleton = None

def GetAppStates():
    """ Gets the FabricApplicationStates singleton.
        Raises an exception if the app-states is not created. 
    """
    if s_appStatesSingleton is None:
        raise Exception('FabricApplicationStates.GetAppStates, the app-states is null.\n\
            To create it : CreateAppStates(FabricCore.Client).')
    else:
        return s_appStatesSingleton

def CreateAppStates(client, settings):
    """ Creates the FabricApplicationStates singleton.
    """
    global s_appStatesSingleton
    # Overwrite the previous one if any; the unit tests reuse the same process with different CanvasWindow / clients
    s_appStatesSingleton = FabricApplicationStates(client, settings)
    return s_appStatesSingleton
