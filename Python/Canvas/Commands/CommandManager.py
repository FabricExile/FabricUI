#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.Canvas.Utils import *
from FabricEngine.FabricUI import Commands as CppCommands
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.Commands.CommandHelpers import CommandHelpers

class KLCommandManager(CppCommands.KLCommandManager):
    
    """ KLCommandManager specializes KLCommandManager_Python (C++). See Commands/KLCommandManager.h(cpp),
        Commands/KLCommandManager.h(cpp) and Commands/KLCommandManager_Python.h(cpp). The manager is 
        shared between the C++ and Python, so commands defined in Python can be created from C++ code, 
        and vice versa.

        The CommandManager is a singleton and should not be created directly.
        - Create the singleton: cmdManager = CommandManager(fabricClient)

        - Get the singleton: cmdManager = GetCommandManager()

        Available methods:
        - Create a command (KL/C++/Python): cmd = cmdManager.createCommand(cmdName, args, doIt)
                
        - Execute a command: cmdManager.doCmd(cmd)

        - Process undo: cmdManager.undoCmd()

        - Process redo: cmdManager.redoCmd()

        - Clear the stacks: cmdManagerclear()

        - Number of commands (undo+redo): cmdManager.count()

        - Get current stack index: cmdManager.getStackIndex()
        
        - Get the stack content as string: cmdManager.getContent()

        - Synchronize with the KL manager: cmdManager.synchronizeKL()


        When a command is registered, a function creating the command with named arguments is dynimi-
        cally defined in Python. Each function in added to the python 'Commands' module. For example, 
        the scriptable command FooCmd with arguments {arg_1:"foo_1", arg_2:"foo_2"} can be created  
        and executed via:
        - from FabricEngine.Canvas.Commands.CommandManager import *
        - import Commands
        - Commands.FooCmd(arg_1 = "foo_1", arg_2 = "foo_2)

        This is equivalent to (see CommandManager.py):
        - args = {arg_1:"foo_1", arg_2:"foo_2"}
        - GetCommandManager().createCommand("FooCmd", args, True)
    """
    
    def __init__(self):
        """ Initializes the KLCommandManager.
        """
        super(KLCommandManager, self).__init__()
        # There is no "new" in python, we need to own the commands created
        # in Python. They are referenced in the C++ KLCommandManager stacks. 
        self.__flatCommandsStack = []
        
    def createCommand(self, cmdName, args={}, doCmd=True, canMergeID=-1):
        """ Creates and executes a command (if doCmd == true).
            If executed, the command is added to the manager stack.
            Raises an exception if an error occurs.
        """
        try:
            cmd = GetCommandRegistry().createCommand(cmdName)
            if len(args) > 0:
 
                isRTValCommand, args = CommandHelpers.CastAndCheckCmdArgs(cmd, args)
            
                if isRTValCommand:
                   super(KLCommandManager, self).checkRTValCommandArgs(cmd, args)
                else:
                    super(KLCommandManager, self).checkCommandArgs(cmd, args)
 
            if doCmd:
                super(KLCommandManager, self).doCommand(cmd, canMergeID)

            return cmd
        except Exception as e:   
            raise Exception(e)

    def clear(self):
        """ Clears all the commands.
        """
        super(KLCommandManager, self).clear()
        self.__flatCommandsStack = []
 
    def clearRedoStack(self):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        count = self.totalUndoCount()
        super(KLCommandManager, self).clearRedoStack()
        self.__flatCommandsStack = self.__flatCommandsStack[:count]

    def commandPushed(self, cmd, isLowCmd):
        """ \internal, impl. of Commands.KLCommandManager_Python. 
            Add the commands to the python stack.
        """
        self.__flatCommandsStack.append(cmd)
  
    def _onCommandRegistered(self, cmdName, cmdType, implType):
        """ \internal, callback when a command is registered.
            Run-time creation of the function to create the command.
            The function is added to the "Commands" module. 
        """
        exec('\
def ' + cmdName + '(**kwargs):\n\
    try:\n\
        return GetCommandManager().createCommand("' + cmdName + '", kwargs )\n\
    except Exception as e:\n\
        raise Exception(e)\n\
setattr(GetOrCreateModule("Commands") , "' + cmdName + '", '+ cmdName + ')')


# \internal, Create the "Commands" module right now so it can  
# be imported with the usual syntax later : 'import Commands'
GetOrCreateModule('Commands')

# \internal, store the manager singleton
global s_klCmdManagerSingleton
s_klCmdManagerSingleton = None
 
def GetCommandManager():
    """ Creates the CommandManager singleton.
    """
    global s_klCmdManagerSingleton
    if s_klCmdManagerSingleton is None:
        # Be sure the command registry is created.
        s_klCmdManagerSingleton = KLCommandManager()
        # Sets the C++ singleton
        CppCommands.CommandManager.setCommandManagerSingleton(s_klCmdManagerSingleton)
        # Connect our-self.
        GetCommandRegistry().registrationDone.connect(s_klCmdManagerSingleton._onCommandRegistered)
    
        for cmdName in GetCommandRegistry().getCommandNames():
            cmdType, implType = GetCommandRegistry().getCommandSpecs(cmdName)
            s_klCmdManagerSingleton._onCommandRegistered(cmdName, cmdType, implType)

    return s_klCmdManagerSingleton
