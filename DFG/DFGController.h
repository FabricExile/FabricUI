//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGController__
#define __UI_DFG_DFGController__

#include <FabricUI/DFG/DFGBindingNotifier.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/ValueEditor_Legacy/ValueItem.h>
#include <SplitSearch/SplitSearch.hpp>
#include <vector>
#include <ASTWrapper/KLASTManager.h>
#include <QTimer>
#include <QAction>
 
using namespace FabricUI::ValueEditor_Legacy;

namespace FabricUI
{

  namespace DFG
  {

    class DFGUICmdHandler;
    class DFGNotificationRouter;
    class DFGWidget;

    class DFGController : public FabricUI::GraphView::Controller
    {
      Q_OBJECT

      friend class DFGWidget;

    public:

      typedef void(*LogFunc)(const char * message);

      DFGController(
        GraphView::Graph * graph,
        DFGWidget *dfgWidget,
        FabricCore::Client &client,
        FabricServices::ASTWrapper::KLASTManager * manager,
        DFGUICmdHandler *cmdHandler,
        bool overTakeBindingNotifications = true
        );
      ~DFGController();

      DFGWidget *getDFGWidget() const
        { return m_dfgWidget; }
      
      FabricCore::Client &getClient()
        { return m_client; }
      FabricCore::DFGHost &getHost()
        { return m_host; }
      FabricCore::DFGBinding &getBinding()
        { return m_binding; }
      QSharedPointer<DFGBindingNotifier> const &
      getBindingNotifier()
        { return m_bindingNotifier; }
      FTL::CStrRef getExecPath()
        { return m_execPath; }
      QString getExecPath_QS()
        { return QString::fromUtf8( m_execPath.data(), m_execPath.size() ); }
      FabricCore::DFGExec &getExec()
        { return m_exec; }
      FTL::CStrRef getExecBlockName()
        { return m_execBlockName; }

      DFGUICmdHandler *getCmdHandler() const
        { return m_cmdHandler; }

      void setHostBindingExec(
        FabricCore::DFGHost &host,
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec,
        FTL::StrRef execBlockName = FTL::StrRef()
        );
      void setBindingExec(
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec,
        FTL::StrRef execBlockName = FTL::StrRef()
        );
      void setExec(
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec,
        FTL::StrRef execBlockName = FTL::StrRef()
        );
      void refreshExec();

      void savePrefs();

      void focusNode( FTL::StrRef nodeName );

      DFGNotificationRouter * getRouter();
      void setRouter(DFGNotificationRouter * router);

      bool isViewingRootGraph() const
        { return m_execPath.empty(); }
      FabricServices::ASTWrapper::KLASTManager * astManager()
        { return m_manager; }

      // returns true if the user allows a preset edit
      bool validPresetSplit() const;

      static QPointF snapToGrid(QPointF &pos, float gridSnapSize)
      { return(QPointF(gridSnapSize * qRound(pos.rx() / gridSnapSize),
                       gridSnapSize * qRound(pos.ry() / gridSnapSize))); }

      // Parent virtual functions

      virtual std::string gvcDoCopy(
        );

      virtual void gvcDoPaste(
        bool mapPositionToMouseCursor = true
        );

      virtual bool gvcDoRemoveNodes(
        FTL::ArrayRef<GraphView::Node *> nodes
        );

      virtual bool gvcDoAddConnections(
        std::vector<GraphView::ConnectionTarget *> const &srcs,
        std::vector<GraphView::ConnectionTarget *> const &dsts
        );

      virtual bool gvcDoRemoveConnections(
        std::vector<GraphView::ConnectionTarget *> const &srcs,
        std::vector<GraphView::ConnectionTarget *> const &dsts
        );
      
      virtual bool gvcDoAddInstFromPreset(
        QString presetPath,
        QPointF pos
        );

      virtual void gvcDoAddPort(
        QString desiredPortName,
        GraphView::PortType portType,
        QString typeSpec = QString(),
        GraphView::ConnectionTarget *connectWith = 0,
        QString extDep = QString(),
        QString metaData = QString()
        );

      virtual void gvcDoRenameExecPort(
        QString oldName,
        QString desiredPortName,
        QString execPath
      );

      virtual void gvcDoRenameNode(
        GraphView::Node* node,
        QString newName
      );

      virtual void gvcDoSetNodeCommentExpanded(
        GraphView::Node *node,
        bool expanded
        );

      virtual void gvcDoMoveNode(
        GraphView::Node const *node,
        QPointF const &nodeOriginalPos,
        QSizeF const &nodeOriginalSize,
        QPointF delta,
        float gridSnapSize,
        std::vector<QPointF> const &nodeSnapPositions,
        qreal nodeSnapDistance,
        std::vector<qreal> const &portSnapPositionsSrcY,
        std::vector<qreal> const &portSnapPositionsDstY,
        qreal portSnapDistance,
        bool allowUndo
        );

      virtual void gvcDoMoveNodes(
        std::vector<GraphView::Node *> const &nodes,
        std::vector<QPointF> const &nodesOriginalPos,
        QPointF delta,
        float gridSnapSize,
        bool allowUndo
        );

      virtual void gvcDoResizeBackDropNode(
        GraphView::BackDropNode *backDropNode,
        QPointF newTopLeftPos,
        QSizeF newSize,
        float gridSnapSize,
        bool gridSnapTop,
        bool gridSnapBottom,
        bool gridSnapLeft,
        bool gridSnapRight,
        bool allowUndo
        );

      virtual void gvcDoMoveExecPort(
        QString srcName,
        QString dstName
        );

      virtual QString gvcGetCurrentExecPath();
      virtual bool gvcCurrentExecIsInstBlockExec();

      QMenu* gvcCreateNodeHeaderMenu( GraphView::Node *, GraphView::ConnectionTarget *, GraphView::PortType ) FTL_OVERRIDE;
      QMenu* gvcCreateInstBlockHeaderMenu( GraphView::InstBlock *, GraphView::ConnectionTarget *, GraphView::PortType ) FTL_OVERRIDE;
      std::string gvcEncodeMetadaToPersistValue() FTL_OVERRIDE;

      // Commands

      void cmdRemoveNodes(
        QStringList nodeNames
        );

      void cmdConnect(
        QStringList srcPaths, 
        QStringList dstPaths
        );

      void cmdDisconnect(
        QStringList srcPaths, 
        QStringList dstPaths
        );

      QString cmdAddInstWithEmptyGraph(
        QString title,
        QPointF pos
        );

      QString cmdAddInstFromJSON(
        QString nodeName,
        QString filePath,
        QPointF pos
        );

      QString cmdAddInstWithEmptyFunc(
        QString title,
        QString initialCode,
        QPointF pos
        );

      QString cmdAddInstFromPreset(
        QString presetPath,
        QPointF pos
        );

      QString cmdAddVar(
        QString desiredNodeName,
        QString dataType,
        QString extDep,
        QPointF pos
        );

      QString cmdAddGet(
        QString desiredNodeName,
        QString varPath,
        QPointF pos
        );

      QString cmdAddSet(
        QString desiredNodeName,
        QString varPath,
        QPointF pos
        );

      QString cmdAddPort(
        QString desiredPortName,
        FabricCore::DFGPortType dfgPortType,
        QString typeSpec,
        QString portToConnect,
        QString extDep,
        QString uiMetadata
        );
      
      QString cmdCreatePreset(
        QString nodeName,
        QString presetDirPath,
        QString presetName,
        bool updateOrigPreset
        );

      QString cmdEditPort(
        QString oldPortName,
        QString desiredNewPortName,
        FabricCore::DFGPortType portType,
        QString typeSpec,
        QString extDep,
        QString uiMetadata
        );

      void cmdRemovePort(
        QStringList portNames
        );

      void cmdMoveNodes(
        QStringList nodeNames,
        QList<QPointF> newTopLeftPoss
        );

      void cmdResizeBackDropNode(
        QString backDropNodeName,
        QPointF newTopLeftPos,
        QSizeF newSize
        );

      QString cmdImplodeNodes(
        QStringList nodeNames,
        QString desiredNodeName
        );

      QList<QString> cmdExplodeNode(
        QString nodeName
        );

      QString cmdAddBackDrop(
        QString title,
        QPointF pos
        );

      void cmdSetNodeComment(
        QString nodeName, 
        QString comment
        );

      void setNodeCommentExpanded(
        QString nodeName, 
        bool expanded
        );

      void cmdSetCode( QString code );

      QString cmdEditNode(
        QString oldName,
        QString desiredNewName,
        QString nodeMetadata,
        QString execMetadata
        );

      QString cmdRenameExecPort(
        QString oldName,
        QString desiredNewName,
        QString execPath
        );

      void cmdCut();

      void cmdPaste(bool mapPositionToMouseCursor = true);

      void cmdSetArgValue(
        QString argName,
        FabricCore::RTVal const &value
        );

      void cmdSetPortDefaultValue(
        QString portPath,
        FabricCore::RTVal const &value
        );

      void cmdSetRefVarPath(
        FabricCore::DFGBinding &binding,
        QString execPath,
        FabricCore::DFGExec &exec,
        QString refName,
        QString varPath
        );

      void cmdReorderPorts(
        FabricCore::DFGBinding &binding,
        QString execPath,
        FabricCore::DFGExec &exec,
        QString itemPath,
        QList<int> indices
        );

      void cmdSetExtDeps(
        QStringList nameAndVers
        );

      void cmdSplitFromPreset();

      QString cmdAddBlock(
        QString desiredName,
        QPointF pos
        );

      virtual QString reloadCode();

      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool relaxNodes(QStringList paths = QStringList());
      virtual bool setNodeColor(const char * nodeName, const char * key, QColor color);
      /// Sets the collapse state of a node and saves it in its preferences    
      virtual void setNodeCollapseState(int collapseState, GraphView::Node *node);
      /// Sets the collapse state of the selected nodes and saves it in their preferences    
      virtual void setSelectedNodesCollapseState(int collapseState);
      /// Returns the selected nodes name
      QStringList getSelectedNodesName();
      /// Returns the selected nodes path
      QStringList getSelectedNodesPath();
      /// Selects the nodes in the list 
      void selectNodes(QList<QString> nodeNames);

      virtual std::string copy();

      virtual bool reloadExtensionDependencies(char const * path);

      void log(const char * message) const;
      void logError(const char * message) const;

      virtual void setLogFunc(LogFunc func);

      void execute();

      virtual bool canConnectTo(
        char const *pathA,
        char const *pathB,
        std::string &failureReason
        ) const;

      FabricServices::SplitSearch::Matches
      getPresetPathsFromSearch( char const * search );

      virtual DFGNotificationRouter *createRouter();

      void emitVarsChanged()
      {
        m_varsChangedPending = true;
        startNotificationTimer();
      }

      void emitArgsChanged()
      {
        m_argsChangedPending = true;
        startNotificationTimer();
      }

      void emitArgValuesChanged()
      {
        m_argValuesChangedPending = true;
        startNotificationTimer();
      }

      void emitDefaultValuesChanged()
      {
        m_defaultValuesChangedPending = true;
        startNotificationTimer();
      }

      void emitTopoDirty()
      {
        m_topoDirtyPending = true;
        startNotificationTimer();
      }

      void emitDirty()
      {
        m_dirtyPending = true;
        startNotificationTimer();
      }

      void emitExecSplitChanged()
      {
        emit execSplitChanged();
      }

      void setBlockCompilations( bool blockCompilations );

      void updateNodeErrors();

      void processDelayedEvents();  // [FE-6568]

    signals:

      void hostChanged();
      void bindingChanged( FabricCore::DFGBinding const &newBinding );
      void execChanged();

      void varsChanged();
      // Emitted whenever variables might have changed (even if implicitly, because of 
      // a new graph, for example)
      void varsChangedImplicitly();
      void argsChanged();
      void argInserted( int index, const char* name, const char* type );
      void argTypeChanged( int index, const char* name, const char* type );
      void argRemoved( int index, const char* name );
      void argsReordered( const FTL::JSONArray* newOrder );

      void argValuesChanged();
      void defaultValuesChanged();
      void topoDirty();
      void dirty();
      void execSplitChanged();
      void bindingExecuted();

      void nodeEditRequested(FabricUI::GraphView::Node *);

    public slots:

      void onTopoDirty();
      void onFrameChanged( int frame );
      void onTimelineRangeChanged( int start, int end );
      void onTimelineTargetFramerateChanged( float frameRate );

      void onVariablesChanged();
      virtual void onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button);

    protected:

      void startNotificationTimer()
      {
        if ( !m_notificationTimer->isActive() )
          m_notificationTimer->start( 0 );
      }

      void appendPresetsAtPrefix(
        std::string &prefixedName,
        FTL::JSONStr &ds
        );

    protected slots:

      void onNotificationTimer();

    private:

      void updateErrors();
      void updatePresetPathDB();

      QTimer *m_notificationTimer;
      DFGWidget *m_dfgWidget;
      FabricCore::Client m_client;
      FabricCore::DFGHost m_host;
      FabricCore::DFGBinding m_binding;
      QSharedPointer<DFGBindingNotifier> m_bindingNotifier;
      QList< QSharedPointer<DFGExecNotifier> > m_ancestorExecNotifiers;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      std::string m_execBlockName;
      FabricServices::ASTWrapper::KLASTManager * m_manager;
      DFGUICmdHandler *m_cmdHandler;
      DFGNotificationRouter * m_router;
      LogFunc m_logFunc;
      bool const m_overTakeBindingNotifications;
      FabricServices::SplitSearch::Dict m_presetNameSpaceDict;
      FabricServices::SplitSearch::Dict m_presetPathDict;
      std::string m_tabSearchPrefsJSONFilename;
      std::vector<std::string> m_presetNameSpaceDictSTL;
      std::vector< std::pair<std::string, unsigned> > m_presetPathDictSTL;
      bool m_presetDictsUpToDate;

      uint32_t m_updateSignalBlockCount;
      bool m_varsChangedPending;
      bool m_argsChangedPending;
      bool m_argValuesChangedPending;
      bool m_defaultValuesChangedPending;
      bool m_topoDirtyPending;
      bool m_dirtyPending;

      // helper to compute the index of a native timeline port (-1 if none)
      int getTimelinePortIndex( const std::string& name );
      // helper to set the value of a timeline port (checks for -1)
      void setTimelinePortValue( int portIndex, float value );
      // computes the indices from the ports of the current graph
      void updateTimelinePortIndices();
      // reset indices to -1
      void resetTimelinePortIndices();
      // sends the timeline values to the current graph
      void setTimelineValuesToGraph();

      // Will be -1 if the port doesn't exist
      int m_timelinePortIndex
        , m_timelineStartPortIndex
        , m_timelineEndPortIndex
        , m_timelineFrameratePortIndex
      ;
      // Storing the values, in case the graph changes
      float m_timelineFrame
        , m_timelineStart
        , m_timelineEnd
        , m_timelineFramerate
      ;

      QTimer *m_executeTimer;

    private slots:

      void onBindingDirty();

      void onBindingArgInserted(
        unsigned index,
        FTL::CStrRef name,
        FTL::CStrRef typeName
        );
      void onBindingTopoDirty();

      void onBindingArgTypeChanged(
        unsigned index,
        FTL::CStrRef name,
        FTL::CStrRef newTypeName
        );

      void onBindingArgRemoved(
        unsigned index,
        FTL::CStrRef name
        );

      void onBindingArgsReordered(
        FTL::ArrayRef<unsigned> newOrder
        );

      void onBindingArgValueChanged(
        unsigned index,
        FTL::CStrRef name
        );

      void onBindingVarInserted(
        FTL::CStrRef varName,
        FTL::CStrRef varPath,
        FTL::CStrRef typeName,
        FTL::CStrRef extDep
        );

      void onBindingVarRemoved(
        FTL::CStrRef varName,
        FTL::CStrRef varPath
        );

      void onParentExecNodeRenamed(
        FTL::CStrRef oldNodeName,
        FTL::CStrRef newNodeName
        );
    };

    class ExposePortAction : public QAction
    {
      Q_OBJECT

    public:

      ExposePortAction(
        QObject *parent,
        DFGController *dfgController,
        GraphView::ConnectionTarget *other,
        GraphView::PortType connectionPortType
      );

    protected slots:

      void onTriggered();

    protected:

      virtual bool allowNonInPortType() const
      { return true; }

      virtual void invokeAddPort(
        QString desiredPortName,
        FabricCore::DFGPortType portType,
        QString typeSpec,
        QString extDep,
        QString metaData
      ) = 0;

      DFGController *m_dfgController;
      GraphView::ConnectionTarget *m_other;
      GraphView::PortType m_connectionPortType;

    };

  };

  inline FabricCore::DFGPortType PortTypeToDFGPortType( GraphView::PortType portType )
  {
    FabricCore::DFGPortType dfgPortType;
    switch ( portType )
    {
      case GraphView::PortType_Input:
        dfgPortType = FabricCore::DFGPortType_Out;
        break;
    
      case GraphView::PortType_Output:
        dfgPortType = FabricCore::DFGPortType_In;
        break;
    
      case GraphView::PortType_IO:
        dfgPortType = FabricCore::DFGPortType_IO;
        break;

      default:
        // [andrew 20150730] shouldn't be possible but needed to prevent
        // compiler warning
        assert( false );
        dfgPortType = FabricCore::DFGPortType_In;
        break;
    }
    return dfgPortType;
  }

};

#endif // __UI_DFG_DFGController__
