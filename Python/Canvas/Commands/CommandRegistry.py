#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands as CppCommands

class KLCommandRegistry(CppCommands.KLCommandRegistry):

    """ KLCommandRegistry registers commands used in the Canvas application. It specializes the C++ 
        KLCommandRegistry_Python (./Commands/KLCommandRegistry_Python.h(cpp)) so it can create commands 
        registered in KL, C++ or Python. However, it can register Python command only. The registry is 
        shared between the C++ and Python, so commands defined in Python can be created from C++ code, 
        and vice versa.

        It is a singleton and should not be created directly.
        - Get the registry: cmdRegistry = GetCommandRegistry()

        Available methods:
        - Register a command (Python): cmdRegistry.registerCmd(cmdName, cmdType, userData)

        - Get a command specs [type, implementationType] (Python/C++/KL): cmdRegistry.getCmdSpecs(cmdName)

        - Check a command is registered (Python/C++/KL): cmdRegistry.isCmdRegistered(cmdName)

        - Create a command (Python/C++/KL): cmd = cmdRegistry.createCommand(cmdName)
        
        - Get the stack content as string: cmdRegistry.getContent()

        - Synchronize with the KL registry: cmdRegistry.synchronizeKL()

        In the RegisterCommand method, the userData argument is used to pass optional custom data to the 
        command (C++ void *). The data is referenced by the registry, and given to the command throught 
        the BaseCommand::registrationCallback callback.
    """

    def __init__(self): 
        """ Initializes the CommandRegistry.
        """
        super(KLCommandRegistry, self).__init__()
        # Command implementation types.
        self.COMMAND_PYTHON = 'PYTHON'

        # Dictionaty of Command Object types (to construct them)
        # {cmdName, pythonCmdType}
        self.__cmdObjectTypeDict = {}
        # Dictionaty of user data
        self.__cmdUserDataDict = {}

    def registerCommand(self, cmdName, cmdType, userData = None):
        """ Registers a Python command 'cmdType' under the name "cmdName".
            Raises an exception if:
            - The command name is empty, 
            - cmdType is not command (inherite from Commands.BaseCommand)
            - The command has already been registered under another type
        """
        if not cmdName:
            raise Exception('CommandRegistry.registerCommand, error: cmdName is empty') 

        if not issubclass(cmdType, CppCommands.BaseCommand):
            raise Exception(
                "CommandRegistry.registerCommand, error: command '" + 
                str(cmdName) + "': type '" + 
                str(cmdType) + "' is not a command")

        if super(KLCommandRegistry, self).isCommandRegistered(cmdName) is False:
            self.__cmdUserDataDict[cmdName] = userData
            self.__cmdObjectTypeDict[cmdName] = cmdType
            self.commandRegistered(cmdName, str(cmdType), self.COMMAND_PYTHON)

        elif cmdType != self.__cmdObjectTypeDict[cmdName]:
            raise Exception(
                "CommandRegistry.registerCommand, error: command '" + str(cmdName) + 
                "': type '" + str(cmdType) + 
                "' overriding previous type '" + str(self.__cmdObjectTypeDict[cmdName]) + "'")
    
    def createCommand(self, cmdName):
        if super(KLCommandRegistry, self).isCommandRegistered(cmdName) is True:
            type_, impType = super(KLCommandRegistry, self).getCommandSpecs(cmdName)
            
            if impType == self.COMMAND_PYTHON:
                try:
                    # Create the command
                    cmd = self.__cmdObjectTypeDict[cmdName]() 
                    # Callback -> set name and user-data
                    cmd.registrationCallback(cmdName, self.__cmdUserDataDict[cmdName])
                except Exception as e:    
                    raise Exception(e)
                return cmd

            # Otherwise (C++/KL), create it in C++.
            else:
                return super(KLCommandRegistry, self).createCommand(cmdName)
        else:
            raise Exception("CommandRegistry.createCommand, error: " +
                "cannot create command '" + cmdName + "', it is not registered") 

# \internal, store the registry singleton 
global s_klCmdRegistrySingleton
s_klCmdRegistrySingleton = None

def GetCommandRegistry():
    """ Creates the KLCommandRegistry singleton.
    """
    global s_klCmdRegistrySingleton
    if s_klCmdRegistrySingleton is None:
        s_klCmdRegistrySingleton = KLCommandRegistry()
         # Sets the C++ singleton
        CppCommands.CommandRegistry.setCommandRegistrySingleton(s_klCmdRegistrySingleton)
      
    return s_klCmdRegistrySingleton
