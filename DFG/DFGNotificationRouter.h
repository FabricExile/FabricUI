// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGNotificationRouter__
#define __UI_DFG_DFGNotificationRouter__

#include <string>
#include <FabricCore.h>
#include <FTL/CStrRef.h>
#include <FTL/JSONValue.h>
#include <FabricUI/DFG/DFGConfig.h>
#include <FabricUI/GraphView/Node.h>

namespace FabricUI
{

  namespace DFG
  {
    class DFGController;
    class DFGWidget;

    class DFGNotificationRouter : public QObject
    {
      Q_OBJECT

      friend class DFGController;
      friend class DFGWidget;

    public:

      DFGNotificationRouter(
        DFGController *dfgController,
        const DFGConfig & config = DFGConfig()
        );
      virtual ~DFGNotificationRouter() {}

    public slots:

      void onExecChanged();

    protected:

      void onGraphSet();
      void onNotification(FTL::CStrRef json);
      void onNodeInserted(
        FTL::CStrRef nodeName,
        FTL::JSONObject const *jsonObject
        );
      void onNodeRemoved(
        FTL::CStrRef nodeName
        );
      void onNodePortInserted(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::JSONObject const *jsonObject
        );
      void onNodePortRemoved(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName
        );
      void onExecPortInserted(
        int portIndex,
        FTL::CStrRef portName,
        FTL::JSONObject const *jsonObject
        );
      void onExecPortRemoved(
        int portIndex,
        FTL::CStrRef portName
        );
      void onExecFixedPortInserted(
        int portIndex,
        FTL::CStrRef portName,
        FTL::JSONObject const *jsonObject
        );
      void onExecFixedPortRemoved(
        int portIndex,
        FTL::CStrRef portName
        );
      void onInstBlockInserted(
        FTL::CStrRef instName,
        int blockIndex,
        FTL::JSONObject const *blockDesc
        );
      void onInstBlockPortInserted(
        FTL::CStrRef instName,
        int blockIndex,
        int portIndex,
        FTL::JSONObject const *portDesc
        );
      void onExecBlockPortRenamed(
        int blockIndex,
        FTL::CStrRef blockName,
        int portIndex,
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName
        );
      void onInstBlockPortRemoved(
        FTL::CStrRef instName,
        int blockIndex,
        int portIndex
        );
      void onInstBlockRemoved(
        FTL::CStrRef instName,
        int blockIndex,
        FTL::CStrRef blockName
        );
      void onExecBlockInserted(
        unsigned blockIndex,
        FTL::JSONObject const *blockDesc
        );
      void onExecBlockPortInserted(
        int blockIndex,
        FTL::CStrRef blockName,
        int portIndex,
        FTL::JSONObject const *portDesc
        );
      void onExecBlockPortRemoved(
        int blockIndex,
        FTL::CStrRef blockName,
        int portIndex,
        FTL::CStrRef portName
        );
      void onInstBlockRenamed(
        unsigned instIndex,
        FTL::CStrRef instName,
        unsigned blockIndex,
        FTL::CStrRef oldBlockName,
        FTL::CStrRef newBlockName
        );
      void onExecBlockRemoved(
        unsigned blockIndex,
        FTL::CStrRef blockName
        );
      void onPortsConnected(
        FTL::CStrRef srcPath,
        FTL::CStrRef dstPath
        );
      void onPortsDisconnected(
        FTL::CStrRef srcPath,
        FTL::CStrRef dstPath
        );
      void onNodeMetadataChanged(
        FTL::CStrRef nodePath,
        FTL::CStrRef key,
        FTL::CStrRef value
        );
      void onExecBlockMetadataChanged(
        FTL::CStrRef name,
        FTL::CStrRef key,
        FTL::CStrRef value
        );
      void onNodeTitleChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef title
        );
      void onNodeRenamed(
        FTL::CStrRef oldNodeName,
        FTL::CStrRef newNodeName
        );
      void onExecPortRenamed(
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName,
        FTL::JSONObject const *execPortJSONObject
        );
      void onExecFixedPortRenamed(
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName,
        FTL::JSONObject const *execPortJSONObject
        );
      void onNodePortRenamed(
        FTL::CStrRef nodeName,
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName
        );
      void onExecMetadataChanged(
        FTL::CStrRef key,
        FTL::CStrRef value
        );
      void onExtDepAdded(
        FTL::CStrRef extension,
        FTL::CStrRef version
        );
      void onExtDepRemoved(
        FTL::CStrRef extension,
        FTL::CStrRef version
        );
      void onNodeCacheRuleChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef newCacheRule
        );
      void onExecCacheRuleChanged(
        FTL::CStrRef newCacheRule
        );
      void onExecPortResolvedTypeChanged(
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedType
        );
      void onExecFixedPortResolvedTypeChanged(
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedType
        );
      void onExecBlockPortResolvedTypeChanged(
        FTL::CStrRef blockName,
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedType
        );
      void onInstBlockPortResolvedTypeChanged(
        FTL::CStrRef instName,
        FTL::CStrRef blockName,
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedType
        );
      void onExecPortTypeSpecChanged(
        FTL::CStrRef portName,
        FTL::CStrRef typeSpec
        );
      void onExecBlockPortTypeSpecChanged(
        FTL::CStrRef blockName,
        FTL::CStrRef portName,
        FTL::CStrRef typeSpec
        );
      void onNodePortResolvedTypeChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedType
        );
      void onNodePortMetadataChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef key,
        FTL::CStrRef value
        );
      void onExecPortMetadataChanged(
        FTL::CStrRef portName,
        FTL::CStrRef key,
        FTL::CStrRef value
        );
      void onExecPortTypeChanged(
        FTL::CStrRef portName,
        FTL::CStrRef execPortType
        );
      void onNodePortTypeChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef nodePortType
        );
      void onExecBlockPortOutsidePortTypeChanged(
        FTL::CStrRef blockName,
        FTL::CStrRef portName,
        FTL::CStrRef outsidePortType
        );
      void onRefVarPathChanged(
        FTL::CStrRef refName,
        FTL::CStrRef newVarPath
        );
      void onFuncCodeChanged(
        FTL::CStrRef code
        );
      void onExecTitleChanged(
        FTL::CStrRef title
        );
      void onExecExtDepsChanged(
        FTL::CStrRef extDeps
        );
      void onExecPortDefaultValuesChanged(
        FTL::CStrRef portName
        );
      void onNodePortDefaultValuesChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName
        );
      void onExecBlockPortDefaultValuesChanged(
        FTL::CStrRef blockName,
        FTL::CStrRef portName
        );
      void onInstBlockPortDefaultValuesChanged(
        FTL::CStrRef instName,
        FTL::CStrRef blockName,
        FTL::CStrRef portName
        );
      void onRemovedFromOwner();
      void onExecPortsReordered(
        unsigned int indexCount,
        unsigned int * indices
        );
      void onExecFixedPortsReordered(
        unsigned int indexCount,
        unsigned int * indices
        );
      void onExecBlockPortsReordered(
        FTL::CStrRef blockName,
        unsigned int indexCount,
        unsigned int * indices
        );
      void onNodePortsReordered(
        FTL::CStrRef nodeName,
        unsigned int indexCount,
        unsigned int * indices
        );
      void onInstBlockPortRenamed(
        FTL::CStrRef instName,
        unsigned int blockIndex,
        unsigned int portIdex,
        FTL::CStrRef newPortName
        );
      void onInstBlockPortsReordered(
        FTL::CStrRef instName,
        unsigned int blockIndex,
        unsigned int indexCount,
        unsigned int * indices
        );
      void onExecDidAttachPreset(
        FTL::CStrRef presetFilePath
        );
      void onInstExecDidAttachPreset(
        FTL::CStrRef nodeName,
        FTL::CStrRef presetFilePath
        );
      void onExecWillDetachPreset(
        FTL::CStrRef presetFilePath
        );
      void onInstExecWillDetachPreset(
        FTL::CStrRef nodeName,
        FTL::CStrRef presetFilePath
        );
      void onExecEditWouldSplitFromPresetMayHaveChanged();
      void onInstExecEditWouldSplitFromPresetMayHaveChanged(
        FTL::CStrRef instName
        );
      void onInstBlockExecEditWouldSplitFromPresetMayHaveChanged(
        FTL::CStrRef instName,
        FTL::CStrRef blockName
        );

    private:

      void checkAndFixPanelPortOrder();
      void checkAndFixNodePortOrder(FabricCore::DFGExec &nodeExec, GraphView::Node *uiNode);

      void callback( FTL::CStrRef jsonStr );

      static void Callback(
        void *thisVoidPtr,
        char const *jsonCStr,
        unsigned int jsonSize
        )
      {
        static_cast<DFGNotificationRouter *>( thisVoidPtr )->callback(
          FTL::CStrRef( jsonCStr, jsonSize )
          );
      }

      DFGController *m_dfgController;
      FabricCore::DFGView m_coreDFGView;
      DFGConfig m_config;
      bool m_performChecks;
    };

  };

};

#endif // __UI_DFG_DFGNotificationRouter__
