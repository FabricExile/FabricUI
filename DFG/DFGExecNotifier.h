//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_DFG_DFGEXECNOTIFIER_H
#define FABRICUI_DFG_DFGEXECNOTIFIER_H

#include <string>
#include <iostream>
#include <FabricCore.h>
#include <FabricUI/DFG/DFGNotifier.h>
#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>
#include <QSharedPointer>
#if defined(FTL_OS_WINDOWS)
# include <unordered_map>
#elif defined(FTL_OS_DARWIN)
# include <unordered_map>
#else
# include <tr1/unordered_map>
#endif

namespace FTL {
class JSONObject;
}

namespace FabricUI {
namespace DFG {

class DFGExecNotifier : public DFGNotifier
{
  Q_OBJECT

public:

  static QSharedPointer<DFGExecNotifier>
  Create( FabricCore::DFGExec exec )
  {
    return QSharedPointer<DFGExecNotifier>(
      new DFGExecNotifier( exec )
      );
  }

  virtual ~DFGExecNotifier();

signals:

  // The executable itself
  
  void metadataChanged(
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void titleChanged(
    FTL::CStrRef title
    );

  void editWouldSplitFromPresetMayHaveChanged();

  void funcCodeChanged(
    FTL::CStrRef code
    );

  void didAttachPreset(
    FTL::CStrRef presetFilePath
    );

  void willDetachPreset(
    FTL::CStrRef presetFilePath
    );

  void extDepsChanged(
    FTL::CStrRef extDeps
    );

  void removedFromOwner();

  // The executable's ports

  void execPortInserted(
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::JSONObject const *portDesc
    );

  void execPortRenamed(
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void execPortRemoved(
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void execPortMetadataChanged(
    FTL::CStrRef portName,
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void execPortTypeChanged(
    unsigned portIndex,
    FTL::CStrRef portName,
    FabricCore::DFGPortType newPortType
    );

  void execPortTypeSpecChanged(
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::CStrRef newTypeSpec
    );

  void execPortResolvedTypeChanged(
    FTL::CStrRef portName,
    FTL::CStrRef newResolvedTypeName
    );

  void execPortsReordered(
    FTL::ArrayRef<unsigned> newOrder
    );

  // the executable's fixedPorts

  void execFixedPortInserted(
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::JSONObject const *portDesc
    );

  void execFixedPortRenamed(
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void execFixedPortResolvedTypeChanged(
    FTL::CStrRef portName,
    FTL::CStrRef newResolvedTypeName
    );

  void execFixedPortRemoved(
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void execPortDefaultValuesChanged(
    FTL::CStrRef portName
    );

  void execFixedPortsReordered(
    FTL::ArrayRef<unsigned> newOrder
    );

  // The executable's nlsPorts

  void nlsPortInserted(
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::JSONObject const *portDesc
    );

  void nlsPortRenamed(
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void nlsPortRemoved(
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void nlsPortMetadataChanged(
    FTL::CStrRef portName,
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void nlsPortTypeSpecChanged(
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::CStrRef newTypeSpec
    );

  void nlsPortResolvedTypeChanged(
    FTL::CStrRef portName,
    FTL::CStrRef newResolvedTypeName
    );

  void nlsPortsReordered(
    FTL::ArrayRef<unsigned> newOrder
    );

  // The executable's blocks

  void execBlockInserted(
    unsigned blockIndex,
    FTL::CStrRef blockName
    );

  void execBlockPortInserted(
    FTL::CStrRef blockName,
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::JSONObject const *portDesc
    );

  void execBlockPortRenamed(
    FTL::CStrRef blockName,
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void execBlockPortRemoved(
    FTL::CStrRef blockName,
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void execBlockPortsReordered(
    FTL::CStrRef blockName,
    FTL::ArrayRef<unsigned> newOrder
    );

  void execBlockRemoved(
    unsigned blockIndex,
    FTL::CStrRef blockName
    );

  void execBlockMetadataChanged(
    FTL::CStrRef blockName,
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void execBlockPortOutsidePortTypeChanged(
    FTL::CStrRef nodeName,
    unsigned portIndex,
    FTL::CStrRef portName,
    FabricCore::DFGPortType newOutsidePortType
    );

  void execBlockPortResolvedTypeChanged(
    FTL::CStrRef blockName,
    FTL::CStrRef portName,
    FTL::CStrRef newResolvedTypeName
    );

  void execBlockPortTypeSpecChanged(
    FTL::CStrRef blockName,
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::CStrRef newResolvedTypeName
    );

  void execBlockPortDefaultValuesChanged(
    FTL::CStrRef blockName,
    FTL::CStrRef portName
    );

  void instBlockPortDefaultValuesChanged(
    FTL::CStrRef instName,
    FTL::CStrRef blockName,
    FTL::CStrRef portName
    );

  // The executable's nodes

  void nodeInserted(
    FTL::CStrRef nodeName
    );

  void nodeRenamed(
    FTL::CStrRef oldNodeName,
    FTL::CStrRef newNodeName
    );

  void nodeRemoved(
    FTL::CStrRef nodeName
    );

  void nodeMetadataChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void instExecTitleChanged(
    FTL::CStrRef instName,
    FTL::CStrRef execTitle
    );

  void instExecEditWouldSplitFromPresetMayHaveChanged(
    FTL::CStrRef instName
    );

  void instExecDidAttachPreset(
    FTL::CStrRef instName,
    FTL::CStrRef presetFilePath
    );

  void refVarPathChanged(
    FTL::CStrRef refName,
    FTL::CStrRef newVarPath
    );

  void instBlockInserted(
    FTL::CStrRef instName,
    FTL::CStrRef blockName
    );

  void instBlockPortInserted(
    FTL::CStrRef instName,
    FTL::CStrRef blockName,
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void instBlockPortResolvedTypeChanged(
    FTL::CStrRef instName,
    FTL::CStrRef blockName,
    FTL::CStrRef portName,
    FTL::CStrRef newResolvedTypeName
    );

  void instBlockPortRenamed(
    FTL::CStrRef instName,
    FTL::CStrRef blockName,
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void instBlockPortRemoved(
    FTL::CStrRef instName,
    FTL::CStrRef blockName,
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void instBlockRemoved(
    FTL::CStrRef instName,
    FTL::CStrRef blockName
    );

  void instBlockExecEditWouldSplitFromPresetMayHaveChanged(
    FTL::CStrRef instName,
    FTL::CStrRef blockName
    );

  void instBlockPortsReordered(
    FTL::CStrRef instName,
    FTL::CStrRef blockName,
    FTL::ArrayRef<unsigned> newOrder
    );

  void execBlockRenamed(
    unsigned blockIndex,
    FTL::CStrRef oldBlockName,
    FTL::CStrRef newBlockName
    );

  void instBlockRenamed(
    FTL::CStrRef instName,
    FTL::CStrRef oldBlockName,
    FTL::CStrRef newBlockName
    );

  // The executable's nodes' ports

  void nodePortInserted(
    FTL::CStrRef nodeName,
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void nodePortRenamed(
    FTL::CStrRef nodeName,
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void nodePortRemoved(
    FTL::CStrRef nodeName,
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void nodePortMetadataChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef portName,
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void nodePortTypeChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef portName,
    FTL::CStrRef newNodePortType
    );

  void nodePortResolvedTypeChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef portName,
    FTL::CStrRef newResolvedTypeName
    );

  void nodePortDefaultValuesChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef portName
    );

  void nodePortsReordered(
    FTL::CStrRef nodeName,
    FTL::ArrayRef<unsigned> newOrder
    );

  // Conections

  void portsConnected(
    FTL::CStrRef srcPortPath,
    FTL::CStrRef dstPortPath
    );

  void portsDisconnected(
    FTL::CStrRef srcPortPath,
    FTL::CStrRef dstPortPath
    );

private:

  DFGExecNotifier( FabricCore::DFGExec exec )
    : m_view( exec.createView( &Callback, this ) ) 
    {
      FabricCore::String execPath = exec.getExecPath();
    }

  virtual void handle( FTL::CStrRef jsonStr ) /*override*/;

  typedef void (DFGExecNotifier::*Handler)(
    FTL::JSONObject const *jsonObject
    );

  void handler_execBlockInserted( FTL::JSONObject const *jsonObject );
  void handler_execBlockMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_execBlockPortDefaultValuesChanged( FTL::JSONObject const *jsonObject );
  void handler_execBlockPortInserted( FTL::JSONObject const *jsonObject );
  void handler_execBlockPortOutsidePortTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_execBlockPortRemoved( FTL::JSONObject const *jsonObject );
  void handler_execBlockPortRenamed( FTL::JSONObject const *jsonObject );
  void handler_execBlockPortResolvedTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_execBlockPortsReordered( FTL::JSONObject const *jsonObject );
  void handler_execBlockPortTypeSpecChanged( FTL::JSONObject const *jsonObject );
  void handler_execBlockRemoved( FTL::JSONObject const *jsonObject );
  void handler_execBlockRenamed( FTL::JSONObject const *jsonObject );
  void handler_execDidAttachPreset( FTL::JSONObject const *jsonObject );
  void handler_execEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject );
  void handler_execFixedPortInserted( FTL::JSONObject const *jsonObject );
  void handler_execFixedPortRemoved( FTL::JSONObject const *jsonObject );
  void handler_execFixedPortRenamed( FTL::JSONObject const *jsonObject );
  void handler_execFixedPortResolvedTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_execFixedPortsReordered( FTL::JSONObject const *jsonObject );
  void handler_execMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortDefaultValuesChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortInserted( FTL::JSONObject const *jsonObject );
  void handler_execPortMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortRemoved( FTL::JSONObject const *jsonObject );
  void handler_execPortRenamed( FTL::JSONObject const *jsonObject );
  void handler_execPortResolvedTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortsReordered( FTL::JSONObject const *jsonObject );
  void handler_execPortTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortTypeSpecChanged( FTL::JSONObject const *jsonObject );
  void handler_execTitleChanged( FTL::JSONObject const *jsonObject );
  void handler_execWillDetachPreset( FTL::JSONObject const *jsonObject );
  void handler_extDepsChanged( FTL::JSONObject const *jsonObject );
  void handler_extDepAdded( FTL::JSONObject const *jsonObject );
  void handler_funcCodeChanged( FTL::JSONObject const *jsonObject );
  void handler_instBlockExecEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject );
  void handler_instBlockInserted( FTL::JSONObject const *jsonObject );
  void handler_instBlockPortDefaultValuesChanged( FTL::JSONObject const *jsonObject );
  void handler_instBlockPortInserted( FTL::JSONObject const *jsonObject );
  void handler_instBlockPortRemoved( FTL::JSONObject const *jsonObject );
  void handler_instBlockPortRenamed( FTL::JSONObject const *jsonObject );
  void handler_instBlockPortResolvedTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_instBlockPortsReordered( FTL::JSONObject const *jsonObject );
  void handler_instBlockRemoved( FTL::JSONObject const *jsonObject );
  void handler_instBlockRenamed( FTL::JSONObject const *jsonObject );
  void handler_instExecDidAttachPreset( FTL::JSONObject const *jsonObject );
  void handler_instExecEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject );
  void handler_instExecTitleChanged( FTL::JSONObject const *jsonObject );
  void handler_nodeInserted( FTL::JSONObject const *jsonObject );
  void handler_nodeMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_nodePortDefaultValuesChanged( FTL::JSONObject const *jsonObject );
  void handler_nodePortInserted( FTL::JSONObject const *jsonObject );
  void handler_nodePortMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_nodePortRemoved( FTL::JSONObject const *jsonObject );
  void handler_nodePortRenamed( FTL::JSONObject const *jsonObject );
  void handler_nodePortResolvedTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_nodePortsReordered( FTL::JSONObject const *jsonObject );
  void handler_nodePortTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_nodeRemoved( FTL::JSONObject const *jsonObject );
  void handler_nodeRenamed( FTL::JSONObject const *jsonObject );
  void handler_portsConnected( FTL::JSONObject const *jsonObject );
  void handler_portsDisconnected( FTL::JSONObject const *jsonObject );
  void handler_refVarPathChanged( FTL::JSONObject const *jsonObject );
  void handler_removedFromOwner( FTL::JSONObject const *jsonObject );
  void handler_nlsPortInserted( FTL::JSONObject const *jsonObject );
  void handler_nlsPortMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_nlsPortRemoved( FTL::JSONObject const *jsonObject );
  void handler_nlsPortRenamed( FTL::JSONObject const *jsonObject );
  void handler_nlsPortResolvedTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_nlsPortTypeSpecChanged( FTL::JSONObject const *jsonObject );
  void handler_nlsPortsReordered( FTL::JSONObject const *jsonObject );

  FabricCore::DFGView m_view;

#if defined(FTL_OS_WINDOWS)
  typedef std::unordered_map<
#elif defined(FTL_OS_DARWIN)
  typedef std::unordered_map<
#else
  typedef std::tr1::unordered_map<
#endif
    FTL::StrRef,
    Handler,
    FTL::StrRef::Hash,
    FTL::StrRef::Equals
    > HandlerMap;

  static HandlerMap const &GetHandlerMap();
};

} // namespace DFG
} // namespace FabricUI

#endif // FABRICUI_DFG_DFGEXECNOTIFIER_H
