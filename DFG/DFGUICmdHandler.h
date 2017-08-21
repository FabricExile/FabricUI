//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGUICmdHandler__
#define __UI_DFG_DFGUICmdHandler__

#include <FabricCore.h>

#include <FabricUI/DFG/DFGUINamespace.h>

#include <QList>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QStringList>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmdHandler
{
public:

  virtual ~DFGUICmdHandler() {}

  // json encoding / decoding

  static QString encodeRTValToJSON(
    FabricCore::Context const& context,
    FabricCore::RTVal const& rtVal
    );

  static void decodeRTValFromJSON(
    FabricCore::Context const& context,
    FabricCore::RTVal & rtVal,
    QString json
    );

  static QString NewPresetPathname(
    FabricCore::DFGHost &host,
    QString presetDirPath,
    QString presetName
    );

  virtual void dfgDoRemoveNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames
    ) = 0;

  virtual void dfgDoConnect(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList srcPortPaths, 
    QStringList dstPortPaths
    ) = 0;

  virtual void dfgDoDisconnect(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList srcPortPaths, 
    QStringList dstPortPaths
    ) = 0;

  virtual QString dfgDoAddGraph(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QPointF pos
    ) = 0;

  virtual QString dfgDoImportNodeFromJSON(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString filePath,
    QPointF pos
    ) = 0;

  virtual QString dfgDoAddFunc(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QString initialCode,
    QPointF pos
    ) = 0;

  virtual QString dfgDoInstPreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString presetPath,
    QPointF pos
    ) = 0;

  virtual QString dfgDoAddVar(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString dataType,
    QString extDep,
    QPointF pos
    ) = 0;

  virtual QString dfgDoAddGet(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString varPath,
    QPointF pos
    ) = 0;

  virtual QString dfgDoAddSet(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString varPath,
    QPointF pos
    ) = 0;

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
    ) = 0;

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
    ) = 0;

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
    ) = 0;

  virtual QString dfgDoCreatePreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString presetDirPath,
    QString presetName,
    bool updateOrigPreset
    ) = 0;

  virtual QString dfgDoEditPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldPortPath,
    QString desiredNewPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString extDep,
    QString uiMetadata
    ) = 0;

  virtual void dfgDoRemovePort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList portPaths
    ) = 0;

  virtual void dfgDoMoveNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames,
    QList<QPointF> newTopLeftPoss
    ) = 0;

  virtual void dfgDoResizeBackDrop(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString backDropNodeName,
    QPointF newTopLeftPos,
    QSizeF newSize
    ) = 0;

  virtual QString dfgDoImplodeNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames,
    QString desiredNodeName
    ) = 0;

  virtual QStringList dfgDoExplodeNode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName
    ) = 0;

  virtual QString dfgDoAddBackDrop(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QPointF pos
    ) = 0;

  virtual void dfgDoSetNodeComment(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString comment
    ) = 0;

  virtual void dfgDoSetCode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString code
    ) = 0;

  virtual QString dfgDoEditNode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldNodeName,
    QString desiredNewNodeName,
    QString nodeMetadata,
    QString execMetadata
    ) = 0;

  virtual QString dfgDoRenamePort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString portPath,
    QString desiredNewPortName
    ) = 0;

  virtual QStringList dfgDoPaste(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString json,
    QPointF cursorPos
    ) = 0;

  virtual void dfgDoSetArgValue(
    FabricCore::DFGBinding const &binding,
    QString argName,
    FabricCore::RTVal const &value
    ) = 0;

  virtual void dfgDoSetPortDefaultValue(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString portPath,
    FabricCore::RTVal const &value
    ) = 0;

  virtual void dfgDoSetRefVarPath(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString refName,
    QString varPath
    ) = 0;

  virtual void dfgDoReorderPorts(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString itemPath,
    QList<int> indices
    ) = 0;

  virtual void dfgDoSetExtDeps(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList extDeps
    ) = 0;

  virtual void dfgDoSplitFromPreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec
    ) = 0;

  virtual void dfgDoDismissLoadDiags(
    FabricCore::DFGBinding const &binding,
    QList<int> diagIndices
    ) = 0;

  virtual QString dfgDoAddBlock(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredName,
    QPointF pos
    ) = 0;

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
    ) = 0;

  virtual QString dfgDoAddNLSPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredPortName,
    QString typeSpec,
    QString portToConnect,
    QString extDep,
    QString metaData
    ) = 0;

  virtual void dfgDoReorderNLSPorts(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString itemPath,
    QList<int> indices
    ) = 0;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmdHandler__
