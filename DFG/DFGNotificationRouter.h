// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGNotificationRouter__
#define __UI_DFG_DFGNotificationRouter__

#include <FTL/JSONValue.h>
#include <FTL/StrRef.h>
#include <GraphView/Controller.h>
#include <GraphView/Port.h>
#include "NotificationRouter.h"
#include "DFGConfig.h"

namespace FabricUI
{

  namespace DFG
  {
    // forward decl
    class DFGController;

    class DFGNotificationRouter : public NotificationRouter
    {

      friend class DFGController;
      friend class DFGWidget;

    public:

      DFGNotificationRouter(
        FabricCore::DFGBinding coreDFGBinding,
        FabricCore::DFGExec coreDFGGraph,
        const DFGConfig & config = DFGConfig()
        );

      GraphView::Port * getLastPortInserted();

      static float getFloatFromVariant(const FabricCore::Variant * variant);


    protected:

      virtual void onGraphSet();
      virtual void onNotification(FTL::CStrRef json);
      virtual void onNodeInserted(
        FTL::CStrRef nodeName,
        FTL::JSONObject const *jsonObject
        );
      virtual void onNodeRemoved(
        FTL::CStrRef nodeName
        );
      virtual void onNodePortInserted(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::JSONObject const *jsonObject
        );
      virtual void onNodePortRemoved(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName
        );
      virtual void onExecPortInserted(
        FTL::CStrRef portName,
        FTL::JSONObject const *jsonObject
        );
      virtual void onExecPortRemoved(
        FTL::CStrRef portName
        );
      virtual void onPortsConnected(
        FTL::CStrRef srcPath,
        FTL::CStrRef dstPath
        );
      virtual void onPortsDisconnected(
        FTL::CStrRef srcPath,
        FTL::CStrRef dstPath
        );
      virtual void onNodeMetadataChanged(FTL::CStrRef nodePath, FTL::CStrRef key, FTL::CStrRef metadata);
      virtual void onNodeTitleChanged(FTL::CStrRef nodePath, FTL::CStrRef title);
      virtual void onExecPortRenamed(
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName,
        FTL::JSONObject const *execPortJSONObject
        );
      virtual void onNodePortRenamed(FabricCore::DFGExec parent, FTL::CStrRef oldPath, FTL::CStrRef newPath);
      virtual void onExecMetadataChanged(FabricCore::DFGExec exec, FTL::CStrRef key, FTL::CStrRef metadata);
      virtual void onExtDepAdded(FabricCore::DFGExec exec, FTL::CStrRef extension, FTL::CStrRef version);
      virtual void onExtDepRemoved(FabricCore::DFGExec exec, FTL::CStrRef extension, FTL::CStrRef version);
      virtual void onNodeCacheRuleChanged(FabricCore::DFGExec parent, FTL::CStrRef path, FTL::CStrRef rule);
      virtual void onExecCacheRuleChanged(FabricCore::DFGExec exec, FTL::CStrRef rule);
      virtual void onExecPortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::CStrRef portPath, FTL::CStrRef resolvedType);
      virtual void onExecPortTypeSpecChanged(FabricCore::DFGExec exec,  FTL::CStrRef portPath, FTL::CStrRef typeSpec);
      virtual void onNodePortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::CStrRef nodePortPath, FTL::CStrRef resolvedType);
      virtual void onExecPortMetadataChanged(FTL::CStrRef portPath, FTL::CStrRef key, FTL::CStrRef metadata);
      virtual void onNodePortMetadataChanged(FTL::CStrRef nodePortPath, FTL::CStrRef key, FTL::CStrRef metadata);
      virtual void onExecPortTypeChanged(FTL::CStrRef portPath, FTL::CStrRef portType);
      virtual void onNodePortTypeChanged(FTL::CStrRef nodePortPath, FTL::CStrRef portType);

    private:

      DFGConfig m_config;
      GraphView::Port * m_lastPortInserted;
      bool m_performChecks;
    };

  };

};

#endif // __UI_DFG_DFGNotificationRouter__
