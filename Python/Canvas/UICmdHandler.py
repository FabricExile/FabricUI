"""
The UICmdHandler is responsible for executing DFG commands sent
from other parts of the UI (for example from the Canvas DFGWidget).

Each Canvas integration has its own UICmdHandler (for example Maya)
and this is the implementation for Python.
The UICmdHandler turns each command into a pure Python command and
then sends it to the ScriptEditor for execution. This allows logging
all commands run by the Canvas standalone as simple Python commands
which can later be used to recreate behaviors that were originally
generated using the UI.
"""

from FabricEngine.FabricUI import DFG
from FabricEngine.Canvas.RTValEncoderDecoder import RTValEncoderDecoder

class UICmdHandler(DFG.DFGUICmdHandler_Python):

    def __init__(self, client, scriptEditor):
        super(UICmdHandler, self).__init__()
        self.client = client
        self.scriptEditor = scriptEditor
        self.rtvalEncoderDecoder = RTValEncoderDecoder(self.client)

    @staticmethod
    def encodeStringChars(string):
        result = ""
        for ch in string:
            if ch == "\"":
                result += "\\\""
            elif ch == "\r":
                result += "\\r"
            elif ch == "\n":
                result += "\\n"
            elif ch == "\t":
                result += "\\t"
            elif ch == "\\":
                result += "\\\\"
            else:
                result += ch
        return result
    
    @staticmethod
    def encodeString(string):
        return "\"" + UICmdHandler.encodeStringChars(string) + "\""

    @staticmethod
    def encodeStrings(strings):
        result = "\""
        for i in range(0, len(strings)):
            if i > 0:
                result += "|"
            result += UICmdHandler.encodeStringChars(strings[i])
        result += "\""
        return result

    @staticmethod
    def encodeBool(x):
        return str(x)

    @staticmethod
    def encodeInt(x):
        return str(x)

    @staticmethod
    def encodeInts(xs):
        result = "\""
        for i in range(0, len(xs)):
            if i > 0:
                result += "|"
            result += UICmdHandler.encodeInt(xs[i])
        result += "\""
        return result

    @staticmethod
    def encodeFloat(x):
        return str(x)

    @staticmethod
    def encodePosXs(poss):
        result = "\""
        for i in range(0, len(poss)):
            if i > 0:
                result += "|"
            result += UICmdHandler.encodeFloat(poss[i].x())
        result += "\""
        return result

    @staticmethod
    def encodePosYs(poss):
        result = "\""
        for i in range(0, len(poss)):
            if i > 0:
                result += "|"
            result += UICmdHandler.encodeFloat(poss[i].y())
        result += "\""
        return result

    def encodePortType(self, portType):
        if portType == self.client.DFG.PortTypes.IO:
            return "\"io\""
        elif portType == self.client.DFG.PortTypes.Out:
            return "\"out\""
        else:
            return "\"in\""

    def evalCmdWithArgs(self, cmd, encodedArgs):
        code = "binding."
        code += cmd
        code += "("
        code += ", ".join(encodedArgs)
        code += ")"
        return self.scriptEditor.eval(code)

    def dfgDoInstPreset(
        self,
        binding,
        execPath,
        exec_,
        presetPath,
        pos
        ):
        return self.evalCmdWithArgs(
            "instPreset",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(presetPath),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddGraph(
        self,
        binding,
        execPath,
        exec_,
        title,
        pos
        ):
        return self.evalCmdWithArgs(
            "addGraph",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(title),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoImportNodeFromJSON(
        self,
        binding,
        execPath,
        exec_,
        nodeName,
        graphPath,
        pos
        ):
        return self.evalCmdWithArgs(
            "importNodeFromJSON",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(nodeName),
                UICmdHandler.encodeString(graphPath),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddFunc(
        self,
        binding,
        execPath,
        exec_,
        title,
        initialCode,
        pos
        ):
        return self.evalCmdWithArgs(
            "addFunc",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(title),
                UICmdHandler.encodeString(initialCode),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddBackDrop(
        self,
        binding,
        execPath,
        exec_,
        title,
        pos
        ):
        return self.evalCmdWithArgs(
            "addBackDrop",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(title),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddVar(
        self,
        binding,
        execPath,
        exec_,
        desiredNodeName,
        dataType,
        extDep,
        pos
        ):
        return self.evalCmdWithArgs(
            "addVar",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredNodeName),
                UICmdHandler.encodeString(dataType),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddGet(
        self,
        binding,
        execPath,
        exec_,
        desiredNodeName,
        varPath,
        pos
        ):
        return self.evalCmdWithArgs(
            "addGet",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredNodeName),
                UICmdHandler.encodeString(varPath),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddSet(
        self,
        binding,
        execPath,
        exec_,
        desiredNodeName,
        varPath,
        pos
        ):
        return self.evalCmdWithArgs(
            "addSet",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredNodeName),
                UICmdHandler.encodeString(varPath),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoMoveNodes(
        self,
        binding,
        execPath,
        exec_,
        nodeNames,
        newTopLeftPoss
        ):
        self.evalCmdWithArgs(
            "moveNodes",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(nodeNames),
                UICmdHandler.encodePosXs(newTopLeftPoss),
                UICmdHandler.encodePosYs(newTopLeftPoss),
                ]
            )

    def dfgDoResizeBackDrop(
        self,
        binding,
        execPath,
        exec_,
        backDropNodeName,
        newTopLeftPos,
        newSize,
        ):
        self.evalCmdWithArgs(
            "resizeBackDrop",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(backDropNodeName),
                UICmdHandler.encodeFloat(newTopLeftPos.x()),
                UICmdHandler.encodeFloat(newTopLeftPos.y()),
                UICmdHandler.encodeFloat(newSize.width()),
                UICmdHandler.encodeFloat(newSize.height()),
                ]
            )

    def dfgDoRemoveNodes(
        self,
        binding,
        execPath,
        exec_,
        nodeNames,
        ):
        self.evalCmdWithArgs(
            "removeNodes",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(nodeNames),
                ]
            )
 
    def dfgDoImplodeNodes(
        self,
        binding,
        execPath,
        exec_,
        nodeNames,
        desiredNodeName
        ):
        return self.evalCmdWithArgs(
            "implodeNodes",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(nodeNames),
                UICmdHandler.encodeString(desiredNodeName),
                ]
            )
 
    def dfgDoExplodeNode(
        self,
        binding,
        execPath,
        exec_,
        nodeName
        ):
        return self.evalCmdWithArgs(
            "explodeNode",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(nodeName),
                ]
            )
 
    def dfgDoConnect(
        self,
        binding,
        execPath,
        exec_,
        srcPorts,
        dstPorts,
        ):
        return self.evalCmdWithArgs(
            "connect",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(srcPorts),
                UICmdHandler.encodeStrings(dstPorts),
                ]
            )
 
    def dfgDoDisconnect(
        self,
        binding,
        execPath,
        exec_,
        srcPorts,
        dstPorts,
        ):
        return self.evalCmdWithArgs(
            "disconnect",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(srcPorts),
                UICmdHandler.encodeStrings(dstPorts),
                ]
            )

    def dfgDoAddPort(
        self,
        binding,
        execPath,
        exec_,
        desiredPortName,
        portType,
        typeSpec,
        portToConnect,
        extDep,
        metaData,
        ):
        return self.evalCmdWithArgs(
            "addPort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredPortName),
                self.encodePortType(portType),
                UICmdHandler.encodeString(typeSpec),
                UICmdHandler.encodeString(portToConnect),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeString(metaData),
                ]
            )

    def dfgDoAddInstPort(
        self,
        binding,
        execPath,
        exec_,
        instName,
        desiredPortName,
        portType,
        typeSpec,
        pathToConnect,
        connectType,
        extDep,
        metaData,
        ):
        return self.evalCmdWithArgs(
            "addInstPort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(instName),
                UICmdHandler.encodeString(desiredPortName),
                self.encodePortType(portType),
                UICmdHandler.encodeString(typeSpec),
                UICmdHandler.encodeString(pathToConnect),
                self.encodePortType(connectType),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeString(metaData),
                ]
            )

    def dfgDoAddInstBlockPort(
        self,
        binding,
        execPath,
        exec_,
        instName,
        blockName,
        desiredPortName,
        typeSpec,
        pathToConnect,
        extDep,
        metaData,
        ):
        return self.evalCmdWithArgs(
            "addInstBlockPort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(instName),
                UICmdHandler.encodeString(blockName),
                UICmdHandler.encodeString(desiredPortName),
                UICmdHandler.encodeString(typeSpec),
                UICmdHandler.encodeString(pathToConnect),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeString(metaData),
                ]
            )

    def dfgDoEditPort(
        self,
        binding,
        execPath,
        exec_,
        oldPortName,
        desiredNewPortName,
        portType,
        typeSpec,
        extDep,
        metaData,
        ):
        return self.evalCmdWithArgs(
            "editPort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(oldPortName),
                UICmdHandler.encodeString(desiredNewPortName),
                self.encodePortType(portType),
                UICmdHandler.encodeString(typeSpec),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeString(metaData),
                ]
            )

    def dfgDoRemovePort(
        self,
        binding,
        execPath,
        exec_,
        portNames,
        ):
        return self.evalCmdWithArgs(
            "removePort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(portNames),
                ]
            )

    def dfgDoCreatePreset(
        self,
        binding,
        execPath,
        exec_,
        nodeName,
        presetDirPath,
        presetName,
        updateOrigPreset,
        ):
        return self.evalCmdWithArgs(
            "createPreset",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(nodeName),
                UICmdHandler.encodeString(presetDirPath),
                UICmdHandler.encodeString(presetName),
                UICmdHandler.encodeBool(updateOrigPreset),
                ]
            )

    def dfgDoSetNodeComment(
        self,
        binding,
        execPath,
        exec_,
        nodeName,
        comment,
        ):
        return self.evalCmdWithArgs(
            "setNodeComment",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(nodeName),
                UICmdHandler.encodeString(comment),
                ]
            )

    def dfgDoSetCode(
        self,
        binding,
        execPath,
        exec_,
        code,
        ):
        return self.evalCmdWithArgs(
            "setCode",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(code),
                ]
            )

    def dfgDoEditNode(
        self,
        binding,
        execPath,
        exec_,
        oldNodeName,
        desiredNewNodeName,
        nodeMetadata,
        execMetadata,
        ):
        return self.evalCmdWithArgs(
            "editNode",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(oldNodeName),
                UICmdHandler.encodeString(desiredNewNodeName),
                UICmdHandler.encodeString(nodeMetadata),
                UICmdHandler.encodeString(execMetadata),
                ]
            )

    def dfgDoRenamePort(
        self,
        binding,
        execPath,
        exec_,
        portPath,
        desiredNewPortName,
        ):
        return self.evalCmdWithArgs(
            "renamePort",
            [
                UICmdHandler.encodeString(exec_.getExecPath()),
                UICmdHandler.encodeString(portPath),
                UICmdHandler.encodeString(desiredNewPortName),
                ]
            )

    def dfgDoPaste(
        self,
        binding,
        execPath,
        exec_,
        json,
        cursorPos,
        ):
        return self.evalCmdWithArgs(
            "paste",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(json),
                UICmdHandler.encodeFloat(cursorPos.x()),
                UICmdHandler.encodeFloat(cursorPos.y()),
                ]
            )

    def dfgDoSetArgValue(
        self,
        binding,
        argName,
        value,
        ):
        return self.evalCmdWithArgs(
            "setArgValue",
            [
                UICmdHandler.encodeString(argName),
                UICmdHandler.encodeString(value.getTypeNameStr()),
                UICmdHandler.encodeString(self.rtvalEncoderDecoder.getAsString(value)),
                ]
            )

    def dfgDoSetPortDefaultValue(
        self,
        binding,
        execPath,
        exec_,
        portPath,
        value,
        ):
        return self.evalCmdWithArgs(
            "setPortDefaultValue",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(portPath),
                UICmdHandler.encodeString(value.getTypeNameStr()),
                UICmdHandler.encodeString(self.rtvalEncoderDecoder.getAsString(value)),
                ]
            )

    def dfgDoSetRefVarPath(
        self,
        binding,
        execPath,
        exec_,
        refName,
        varPath,
        ):
        return self.evalCmdWithArgs(
            "setRefVarPath",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(refName),
                UICmdHandler.encodeString(varPath),
                ]
            )

    def dfgDoReorderPorts(
        self,
        binding,
        execPath,
        exec_,
        itemPath,
        indices,
        ):
        return self.evalCmdWithArgs(
            "reorderPorts",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(itemPath),
                UICmdHandler.encodeInts(indices),
                ]
            )

    def dfgDoDismissLoadDiags(
        self,
        binding,
        indices,
        ):
        return self.evalCmdWithArgs(
            "dismissLoadDiags",
            [
                UICmdHandler.encodeInts(indices),
                ]
            )

    def dfgDoSetExtDeps(
        self,
        binding,
        execPath,
        exec_,
        extDeps,
        ):
        return self.evalCmdWithArgs(
            "setExtDeps",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(extDeps),
                ]
            )

    def dfgDoSplitFromPreset(
        self,
        binding,
        execPath,
        exec_,
        ):
        return self.evalCmdWithArgs(
            "splitFromPreset",
            [
                UICmdHandler.encodeString(exec_.getExecPath()),
                UICmdHandler.encodeString(execPath),
                ]
            )


    def dfgDoAddBlock(
        self,
        binding,
        execPath,
        exec_,
        desiredName,
        pos
        ):
        return self.evalCmdWithArgs(
            "addBlock",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredName),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddBlockPort(
        self,
        binding,
        execPath,
        exec_,
        blockName,
        desiredPortName,
        portType,
        typeSpec,
        pathToConnect,
        connectType,
        extDep,
        metaData,
        ):
        return self.evalCmdWithArgs(
            "addBlockPort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(blockName),
                UICmdHandler.encodeString(desiredPortName),
                self.encodePortType(portType),
                UICmdHandler.encodeString(typeSpec),
                UICmdHandler.encodeString(pathToConnect),
                self.encodePortType(connectType),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeString(metaData),
                ]
            )

    def dfgDoAddNLSPort(
        self,
        binding,
        execPath,
        exec_,
        desiredPortName,
        typeSpec,
        portToConnect,
        extDep,
        metaData,
        ):
        return self.evalCmdWithArgs(
            "addLocal",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredPortName),
                UICmdHandler.encodeString(typeSpec),
                UICmdHandler.encodeString(portToConnect),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeString(metaData),
                ]
            )

    def dfgDoReorderNLSPorts(
        self,
        binding,
        execPath,
        exec_,
        itemPath,
        indices,
        ):
        return self.evalCmdWithArgs(
            "reorderLocals",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(itemPath),
                UICmdHandler.encodeInts(indices),
                ]
            )
