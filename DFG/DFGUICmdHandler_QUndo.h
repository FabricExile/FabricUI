// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmdHandler_QUndo__
#define __UI_DFG_DFGUICmdHandler_QUndo__

#include <QUndoStack>
#include <FabricUI/DFG/DFGUICmdHandler.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmdHandler_QUndo : public DFGUICmdHandler
{
public:

  DFGUICmdHandler_QUndo() {}
  
  DFGUICmdHandler_QUndo(
    QUndoStack *qUndoStack
    )
    : m_qUndoStack( qUndoStack )
    {}

  virtual ~DFGUICmdHandler_QUndo() {}

  virtual void dfgDoRemoveNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames
    );

  virtual void dfgDoConnect(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList srcPaths, 
    QStringList dstPaths
    );

  virtual void dfgDoDisconnect(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList srcPaths, 
    QStringList dstPaths
    );

  virtual QString dfgDoAddGraph(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QPointF pos
    );

  virtual QString dfgDoImportNodeFromJSON(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString filePath,
    QPointF pos
    );

  virtual QString dfgDoAddFunc(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QString initialCode,
    QPointF pos
    );

  virtual QString dfgDoInstPreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString presetPath,
    QPointF pos
    );

  virtual QString dfgDoAddVar(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString dataType,
    QString extDep,
    QPointF pos
    );

  virtual QString dfgDoAddGet(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString varPath,
    QPointF pos
    );

  virtual QString dfgDoAddSet(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString varPath,
    QPointF pos
    );

  virtual QString dfgDoAddPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString portToConnect,
    QString extDep,
    QString metaData
    );

  virtual QString dfgDoAddInstPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString instName,
    QString desiredPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString pathToConnect,
    FabricCore::DFGPortType connectType,
    QString extDep,
    QString metaData
    );

  virtual QString dfgDoAddInstBlockPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString instName,
    QString blockName,
    QString desiredPortName,
    QString typeSpec,
    QString pathToConnect,
    QString extDep,
    QString metaData
    );

  virtual QString dfgDoCreatePreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString presetDirPath,
    QString presetName,
    bool updateOrigPreset
    );

  virtual QString dfgDoEditPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldPortName,
    QString desiredNewPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString extDep,
    QString uiMetadata
    );

  virtual void dfgDoRemovePort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList portNames
    );

  virtual void dfgDoResizeBackDrop(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString backDropNodeName,
    QPointF newTopLeftPos,
    QSizeF newSize
    );
  
  virtual void dfgDoMoveNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames,
    QList<QPointF> newTopLeftPoss
    );

  virtual QString dfgDoImplodeNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames,
    QString desiredNodeName
    );

  virtual QStringList dfgDoExplodeNode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName
    );

  virtual QString dfgDoAddBackDrop(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QPointF pos
    );

  virtual void dfgDoSetNodeComment(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString comment
    );

  virtual void dfgDoSetCode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString code
    );

  virtual QString dfgDoEditNode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldNodeName,
    QString desiredNewNodeName,
    QString nodeMetadata,
    QString execMetadata
    );

  virtual QString dfgDoRenamePort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString portPath,
    QString desiredNewPortName
    );

  virtual QStringList dfgDoPaste(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString json,
    QPointF cursorPos
    );

  virtual void dfgDoSetArgValue(
    FabricCore::DFGBinding const &binding,
    QString argName,
    FabricCore::RTVal const &value
    );

  virtual void dfgDoSetPortDefaultValue(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString portOrPinPath,
    FabricCore::RTVal const &value
    );

  virtual void dfgDoSetRefVarPath(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString refName,
    QString varPath
    );

  virtual void dfgDoReorderPorts(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString itemPath,
    QList<int> indices
    );

  virtual void dfgDoSetExtDeps(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList extDeps
    );

  virtual void dfgDoSplitFromPreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec
    );
  
  virtual void dfgDoDismissLoadDiags(
    FabricCore::DFGBinding const &binding,
    QList<int> diagIndices
    );
  
  virtual QString dfgDoAddBlock(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredName,
    QPointF pos
    );

  virtual QString dfgDoAddBlockPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString blockName,
    QString desiredPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString pathToConnect,
    FabricCore::DFGPortType connectType,
    QString extDep,
    QString metaData
    );

  virtual QString dfgDoAddNLSPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredPortName,
    QString typeSpec,
    QString portToConnect,
    QString extDep,
    QString metaData
    );

  virtual void dfgDoReorderNLSPorts(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString itemPath,
    QList<int> indices
    );

protected:

  class WrappedCommand;

protected:

  QUndoStack *m_qUndoStack;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmdHandler_QUndo__
