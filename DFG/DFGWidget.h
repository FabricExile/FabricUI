// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGWidget__
#define __UI_DFG_DFGWidget__

#include <QSettings>
#include <QWidget>
#include <QCursor>
#include <QMenuBar>
#include <QFileInfo>
#include <QProxyStyle>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDesktopServices>
#include <Commands/CommandStack.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/DFG/DFGConfig.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecHeaderWidget.h>
#include <FabricUI/DFG/DFGGraphViewWidget.h>
#include <FabricUI/DFG/DFGKLEditorWidget.h>
#include <FabricUI/DFG/DFGNotificationRouter.h>
#include <FabricUI/DFG/DFGTabSearchWidget.h>
#include <FabricUI/DFG/TabSearch/DFGPresetSearchWidget.h>
#include <FabricUI/DFG/Dialogs/DFGBaseDialog.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/Actions/BaseAction.h>
#include <FabricUI/Util/LoadPixmap.h>

#include <FTL/OwnedPtr.h>
#include <FTL/JSONEnc.h>
#include <FTL/FS.h>

namespace FabricUI {
namespace DFG {
  
    class DFGErrorsWidget;
    class DFGExecBlockEditorWidget;
    class DFGExecHeaderWidget;
    class DFGUICmdHandler;

    class DFGWidgetProxyStyle
      : public QProxyStyle
    {
    public:

      DFGWidgetProxyStyle(
        QStyle* style = NULL
        );

      virtual void drawControl(
        ControlElement element,
        const QStyleOption * option,
        QPainter * painter,
        const QWidget * widget
        ) const /*override*/;
    };

    class DFGWidget : public QWidget
    {
      Q_OBJECT

      typedef QWidget Parent;

    public:

      DFGWidget(
        QWidget * parent, 
        FabricCore::Client &client,
        FabricCore::DFGHost &host,
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec,
        FabricServices::ASTWrapper::KLASTManager * manager,
        DFGUICmdHandler *cmdHandler,
        const DFGConfig & dfgConfig,
        bool overTakeBindingNotifications = true
        );
      virtual ~DFGWidget();

      DFGConfig & getConfig() { return m_dfgConfig; }

      DFGController *getDFGController()
        { return m_uiController.get(); }

      GraphView::Graph * getUIGraph();
      DFGKLEditorWidget * getKLEditor();
      DFGController * getUIController();
      const DFGController * getUIController() const;
      DFGAbstractTabSearchWidget * getTabSearchWidget();
      DFGGraphViewWidget * getGraphViewWidget();
      const DFGGraphViewWidget * getGraphViewWidget() const;
      DFGExecHeaderWidget * getHeaderWidget();
      DFGErrorsWidget *getErrorsWidget() const
        { return m_errorsWidget; }

      bool isEditable() const { return m_isEditable; }
      static QSettings * getSettings();
      static void setSettings(QSettings * settings);

      std::string getBindingHostApp() const;

      bool isBindingHostAppStandalone() const { return (isBindingHostAppCanvasPy() || isBindingHostAppCanvasExe()); }
      bool isBindingHostAppCanvasPy()   const { return (getBindingHostApp() == "Canvas.py"); }
      bool isBindingHostAppCanvasExe()  const { return (getBindingHostApp() == "Canvas.exe"); }
      bool isBindingHostAppMaya()       const { return (getBindingHostApp() == "Maya"); }
      bool isBindingHostAppModo()       const { return (getBindingHostApp() == "Modo"); }
      bool isBindingHostApp3dsMax()     const { return (getBindingHostApp() == "3dsMax"); }

      bool isQuickZoomActive() const { return (m_uiGraphZoomBeforeQuickZoom != 0); }
      
      void onExecPathOrTitleChanged();
      void refreshExtDeps( FTL::CStrRef extDeps );

      void populateMenuBar(QMenuBar *menuBar, bool addFileMenu, bool addEditMenu, bool addViewMenu, bool addDCCMenu, bool addHelpMenu);

      bool maybeEditNode( FabricUI::GraphView::Node *node );
      bool maybeEditInstBlock( FabricUI::GraphView::InstBlock *instBlock );

      void reloadStyles();

      void tabSearch();
      bool isUsingLegacyTabSearch() const;
      void emitNodeInspectRequested(FabricUI::GraphView::Node *);

      void createPort( FabricUI::GraphView::PortType portType );
      void deletePort( FabricUI::GraphView::Port *port );
      void deletePorts( bool deleteIn, bool deleteOut, bool deleteIO );
      void editPort( FTL::CStrRef execPortName, bool duplicatePort );

      void movePortsToEnd( bool moveInputs );
      void implodeSelectedNodes( bool displayDialog );
      void openPresetDoc( const char *nodeName );
      void splitFromPreset( const char *nodeName );
      void createPreset( const char *nodeName );
      void updateOrigPreset( const char *nodeName );
      void exportGraph( const char *nodeName );
      void explodeNode( const char *nodeName, bool clearCurrentSelection = true, bool selectNewNodes = true );

      void createNewGraphNode( QPoint const &globalPos );
      void createNewNodeFromJSON( QPoint const &globalPos );
      void createNewNodeFromJSON( QFileInfo const &fileInfo, QPointF const &pos );
      void createNewFunctionNode( QPoint const &globalPos );
      void createNewBackdropNode( QPoint const &globalPos);
      void createNewBlockNode( QPoint const &globalPos );
      void createNewCacheNode( QPoint const &globalPos );
      void createNewVariableNode( QPoint const &globalPos );
      void createNewVariableGetNode( QPoint const &globalPos );
      void createNewVariableSetNode( QPoint const &globalPos );

      void replaceBinding( FabricCore::DFGBinding &binding );
      bool priorExecStackIsEmpty() const
        { return m_priorExecStack.empty(); }

      virtual void keyPressEvent(QKeyEvent * event) /*override*/;
      virtual void keyReleaseEvent(QKeyEvent * event) /*override*/;

    signals:

      void additionalMenuActionsRequested(QString, QMenu*, bool);
      void fileMenuAboutToShow(); // used to filter the recent files
      void execChanged();
      void newPresetSaved(QString presetFilePath);
      void onGraphSet(FabricUI::GraphView::Graph* graph);
      void portEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog);
      void portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData);
      void nodeInspectRequested(FabricUI::GraphView::Node *);
      void urlDropped( QUrl url, bool ctrlPressed, bool altPressed, QPointF pos );
      void stylesReloaded();
      void revealPresetInExplorer(QString);

    public slots:

      void onExecChanged();
      void onExecSplitChanged();
      void onGoUpPressed();
      void onNodeEditRequested(FabricUI::GraphView::Node *);
      void onBubbleEditRequested(FabricUI::GraphView::Node * node);
      void onToggleBlockCompilations();
      void onToggleDimConnections();
      void onToggleConnectionShowTooltip();
      void onToggleHighlightConnectionTargets();
      void onToggleConnectionDrawAsCurves();
      void onTogglePortsCentered();
      void onToggleDrawGrid();
      void onToggleSnapToGrid();
      void onToggleSnapToNode();
      void onToggleSnapToPort();
      void onEditSelectedNode();
      void onEditSelectedNodeProperties();
      void onRevealPresetInExplorer(const char* nodeName);
      void onPresetAddedFromTabSearch( QString preset );
      void onBackdropAddedFromTabSearch();
      void onNewBlockAddedFromTabSearch();
      void onVariableCreationRequestedFromTabSearch();
      void onVariableSetterAddedFromTabSearch( const std::string name );
      void onVariableGetterAddedFromTabSearch( const std::string name );
      void onFocusGivenFromTabSearch();
      void onToggleLegacyTabSearch( bool toggled );
      void onReloadStyles();

    private slots:

      void onExecSelected(
        FTL::CStrRef execPath,
        int line,
        int column
        );

      void onNodeSelected(
        FTL::CStrRef execPath,
        FTL::CStrRef nodeName,
        int line,
        int column
        );
      void tabSearchVariablesSetDirty();
      void tabSearchVariablesUpdate();
      void tabSearchBlockToggleChanged();

    private:

      static QMenu* graphContextMenuCallback(FabricUI::GraphView::Graph* graph, void* userData);
      static QMenu* nodeContextMenuCallback(FabricUI::GraphView::Node* node, void* userData);
      static QMenu* portContextMenuCallback(FabricUI::GraphView::Port* port, void* userData);
      static QMenu* pinContextMenuCallback(FabricUI::GraphView::Pin* pin, void* userData);
      static QMenu* fixedPortContextMenuCallback(FabricUI::GraphView::FixedPort* fixedPort, void* userData);
      static QMenu* connectionContextMenuCallback(FabricUI::GraphView::Connection* connection, void* userData);
      static QMenu* sidePanelContextMenuCallback(FabricUI::GraphView::SidePanel* panel, void* userData);

      bool maybePushExec(
        FTL::StrRef nodeName,
        FabricCore::DFGExec &exec,
        FTL::StrRef execBlockName = FTL::StrRef()
        );
      bool maybePopExec( std::string &nodeName );

      bool checkForUnsaved();
      QPointF getTabSearchScenePos() const;

      DFGGraphViewWidget * m_uiGraphViewWidget;
      DFGExecHeaderWidget * m_uiHeader;
      DFGErrorsWidget *m_errorsWidget;
      GraphView::Graph * m_uiGraph;
      FTL::OwnedPtr<DFGController> m_uiController;
      DFGNotificationRouter * m_router;
      DFGKLEditorWidget * m_klEditor;
      DFGExecBlockEditorWidget *m_execBlockEditorWidget;
      QPoint m_tabSearchPos;
      DFGTabSearchWidget * m_legacyTabSearchWidget;
      DFGPresetSearchWidget * m_tabSearchWidget;
      bool m_tabSearchVariablesDirty;
      FabricServices::ASTWrapper::KLASTManager * m_manager;
      DFGConfig m_dfgConfig;

      typedef std::pair<FabricCore::DFGExec, std::string> PriorExecStackEntry;
      std::vector<PriorExecStackEntry> m_priorExecStack;

      bool m_isEditable;

      float m_uiGraphZoomBeforeQuickZoom;

      static QSettings * g_settings;
    };

    class BaseDFGWidgetAction : public Actions::BaseAction
    {
      Q_OBJECT

    public:

      BaseDFGWidgetAction(
        DFGWidget *dfgWidget,
        QObject *parent
      ) : Actions::BaseAction( parent )
        , m_dfgWidget( dfgWidget )
      {
      }

      BaseDFGWidgetAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        const QString &name, 
        const QString &text = "", 
        QKeySequence shortcut = QKeySequence(),
        Qt::ShortcutContext context = Qt::WidgetWithChildrenShortcut,
        bool enable = true)
        : Actions::BaseAction( 
          parent
          , name 
          , text 
          , shortcut 
          , context
          , enable)
        , m_dfgWidget( dfgWidget )
      {
      }

      virtual ~BaseDFGWidgetAction()
      {
      }

    protected:

      DFGWidget *m_dfgWidget;
    };

    class TabSearchAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      TabSearchAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::TabSearchAction" 
          , "Tab Search" 
          , Qt::Key_Tab 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~TabSearchAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->tabSearch();
      }

    };

    class GoUpAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      GoUpAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::goUpAction" 
          , "Back" 
          , Qt::Key_U 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~GoUpAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->onGoUpPressed();
      }
    };

    class InspectNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      InspectNodeAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::InspectNodeAction" 
          , "Inspect" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->emitNodeInspectRequested(m_node);
      }

    private:
      GraphView::Node *m_node;
    };

    class CreatePortAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      CreatePortAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::PortType portType,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::CreatePortAction" 
          , "Create Port" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_portType( portType )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createPort( m_portType );
      }

    private:

      FabricUI::GraphView::PortType m_portType;
    };

    class DeletePortAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      DeletePortAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::Port *port,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::DeletePortAction" 
          , "Delete" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_port( port )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->deletePort( m_port );
      }

    private:

      FabricUI::GraphView::Port *m_port;
    };

    class DeleteAllPortsAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:
      DeleteAllPortsAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool deleteIn,
        bool deleteOut,
        bool deleteIO,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::DeleteAllPortsAction" 
          , "Delete all Ports" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_deleteIn ( deleteIn )
        , m_deleteOut( deleteOut )
        , m_deleteIO ( deleteIO )
      {
        if      (  deleteIn &&  deleteOut &&  deleteIO )
          setText( "Delete all Ports" );
        else if ( !deleteIn &&  deleteOut &&  deleteIO )
          setText( "Delete all Output and IO Ports" );
        else if (  deleteIn && !deleteOut &&  deleteIO )
          setText( "Delete all Input and IO Ports" );
        else if ( !deleteIn && !deleteOut &&  deleteIO )
          setText( "Delete all IO Ports" );
        else if (  deleteIn &&  deleteOut && !deleteIO )
          setText( "Delete all Input and Output Ports" );
        else if ( !deleteIn &&  deleteOut && !deleteIO )
          setText( "Delete all Output Ports" );
        else if (  deleteIn && !deleteOut && !deleteIO )
          setText( "Delete all Input Ports" );
        else
          setText( "Delete nothing" );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->deletePorts( m_deleteIn, m_deleteOut, m_deleteIO );
      }

    private:

      bool m_deleteIn;
      bool m_deleteOut;
      bool m_deleteIO;
    };

    class DuplicatePortAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      DuplicatePortAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::Port *port,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::DuplicatePortAction" 
          , "Duplicate" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_port( port )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->editPort( m_port->name(), true /* duplicatePort */ );
      }

    private:

      FabricUI::GraphView::Port *m_port;
    };

    class EditPortAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      EditPortAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::Port *port,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::EditPortAction" 
          , "Edit" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_port( port )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->editPort( m_port->name(), false /* duplicatePort */ );
      }

    private:

      FabricUI::GraphView::Port *m_port;
    };

    class CreateTimelinePortAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      CreateTimelinePortAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        int createWhat,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::CreateTimelinePortAction" 
          , "Create Timeline Port" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
        m_portname = "noname";
        switch (createWhat)
        {
          case 0:   m_portname = "timeline";          break;
          case 1:   m_portname = "timelineStart";     break;
          case 2:   m_portname = "timelineEnd";       break;
          case 3:   m_portname = "timelineFramerate"; break;
          default:                                    break;
        };
        QString capitalizedPortName = m_portname;
        capitalizedPortName[0] = capitalizedPortName[0].toUpper();
        setText( "Create " + capitalizedPortName + " Port" );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->cmdAddPort(
          m_portname.toUtf8().data(),
          FabricCore::DFGPortType_In,
          "Scalar",
          QString(), // portToConnect
          QString(), // extDep
          QString()  // uiMetadata
          );
      }

    private:

      QString m_portname;
    };

    class AbstractAction : public QAction
    {
      Q_OBJECT

    public:
      AbstractAction( QObject* parent ) : QAction( parent )
      {
        connect( this, SIGNAL(triggered()),
                 this, SLOT(onTriggered()) );
      }

    protected slots:
      virtual void onTriggered() = 0;
    };

    class CreateAllTimelinePortsAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      CreateAllTimelinePortsAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool createOnlyMissingPorts = true,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::CreateAllTimelinePortsAction" 
          , "Create all timeline Ports" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_createOnlyMissingPorts( createOnlyMissingPorts )
      {
      }

    private slots:

      void onTriggered()
      {
        for (int i=0;i<4;i++)
        {
          QString portname = "";
          switch (i)
          {
            case 0:   portname = "timeline";          break;
            case 1:   portname = "timelineStart";     break;
            case 2:   portname = "timelineEnd";       break;
            case 3:   portname = "timelineFramerate"; break;
            default:                                  break;
          };

          if (   m_createOnlyMissingPorts
              && m_dfgWidget->getUIGraph()->ports(portname.toUtf8().data()).size() > 0 )
            continue;

          m_dfgWidget->getUIController()->cmdAddPort(
            portname.toUtf8().data(),
            FabricCore::DFGPortType_In,
            "Scalar",
            QString(), // portToConnect
            QString(), // extDep
            QString()  // uiMetadata
            );
        }
      }

    private:

      bool m_createOnlyMissingPorts;
    };

    class NewVariableNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      NewVariableNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::NewVariableNodeAction" 
          , "New Variable" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_pos( pos )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewVariableNode( m_pos );
      }

    private:

      QPoint m_pos;
    };

    class NewVariableGetNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      NewVariableGetNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::NewVariableGetNodeAction" 
          , "Get Variable" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_pos( pos )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewVariableGetNode( m_pos );
      }

    private:

      QPoint m_pos;
    };

    class NewVariableSetNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      NewVariableSetNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::NewVariableSetNodeAction" 
          , "Set Variable" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_pos( pos )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewVariableSetNode( m_pos );
      }

    private:

      QPoint m_pos;
    };

    class MoveInputPortsToEndAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      MoveInputPortsToEndAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::MoveInputPortsToEndAction" 
          , "Move Input Ports to End" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->movePortsToEnd( true /* moveInputs */ );
      }
    };

    class MoveOutputPortsToEndAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      MoveOutputPortsToEndAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::MoveOutputPortsToEndAction" 
          , "Move Output Ports to End" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->movePortsToEnd( false /* moveInputs */ );
      }
    };

    class ImplodeSelectedNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ImplodeSelectedNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ImplodeSelectedNodesAction" 
          , "Implode Selected Nodes" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

    private slots:

      void onTriggered()
      {
        Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
        bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);
        m_dfgWidget->implodeSelectedNodes(isCTRL);
      }
    };

    class ExplodeSelectedNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ExplodeSelectedNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ExplodeSelectedNodesAction" 
          , "Explode Selected Nodes" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

    private slots:

      void onTriggered()
      {
        DFGController *controller = m_dfgWidget->getUIController();
        if (controller)
        {
          GraphView::Graph *graph = controller->graph();

          // create an array of nodes that are selected and 'explodable'.
          std::vector<std::string> nodes;
          {
            FabricCore::DFGExec exec = controller->getExec();
            std::vector<GraphView::Node *> selectedNodes = graph->selectedNodes();
            for (size_t i=0;i<selectedNodes.size();i++)
            {
              GraphView::Node *node = selectedNodes[i];
              std::string nodeName = node->name();

              if (   node->isBackDropNode()
                  || node->isBlockNode() )
                continue;

              FabricCore::DFGNodeType dfgNodeType = exec.getNodeType( selectedNodes[i]->name().c_str() );
              if (   dfgNodeType != FabricCore::DFGNodeType_Inst
                  && dfgNodeType != FabricCore::DFGNodeType_User )
                continue;

              if (exec.getSubExec(nodeName.c_str()).getType() != FabricCore::DFGExecType_Graph)
                continue;

              nodes.push_back(nodeName);
            }
          }

          // explode the nodes.
          if (nodes.size() > 0)
          {
            graph->clearSelection();
            for (size_t i=0;i<nodes.size();i++)
            {
              GraphView::Node *node = graph->node(nodes[i]);
              if (node)
                m_dfgWidget->explodeNode(node->name().c_str(), false /* clearCurrentSelection */, true /* selectNewNodes */);
            }
          }
        }
      }
    };

    class OpenPresetDocAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      OpenPresetDocAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::OpenPresetDocAction" 
          , "Documentation" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->openPresetDoc(m_node->name().c_str());
      }

    private:

      GraphView::Node *m_node;
    };

    class RevealPresetInExplorerAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      RevealPresetInExplorerAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::RevealPresetInExplorerAction" 
          , "Reveal in Explorer" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->onRevealPresetInExplorer(m_node->name().c_str());
      }

    private:

      GraphView::Node *m_node;
    };

    class EditSelectedNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      EditSelectedNodeAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::EditSelectedNodeAction" 
          , "Edit" 
          , Qt::Key_I 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~EditSelectedNodeAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->onEditSelectedNode();
      }
    };

    class EditSelectedNodePropertiesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      EditSelectedNodePropertiesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::EditSelectedNodePropertiesAction" 
          , "Properties" 
          , Qt::Key_F2 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~EditSelectedNodePropertiesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->onEditSelectedNodeProperties();
      }

    };

    class SplitFromPresetAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      SplitFromPresetAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::SplitFromPresetAction" 
          , "Split from Preset" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        DFGController *UIController = m_dfgWidget->getDFGController();        
        FabricCore::DFGExec exec  = UIController->getExec().getSubExec(m_node->name().c_str());
        UIController->getCmdHandler()->dfgDoSplitFromPreset(UIController->getBinding(), m_node->name_QS(), exec);
      }

    private:

      GraphView::Node *m_node;
    };

    class CreatePresetAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      CreatePresetAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::CreatePresetAction" 
          , "Create New Preset" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createPreset(m_node->name().c_str());
      }

    private:

      GraphView::Node *m_node;
    };

    class UpdatePresetAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      UpdatePresetAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::UpdatePresetAction" 
          , "Update Original Preset" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {;
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->updateOrigPreset(m_node->name().c_str());
      }

    private:

      GraphView::Node *m_node;
    };

    class ExportGraphAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ExportGraphAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ExportGraphAction" 
          , "Export Graph" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->exportGraph(m_node->name().c_str());
      }

    private:

      GraphView::Node *m_node;
    };

    class ExplodeNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ExplodeNodeAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ExplodeNodeAction" 
          , "Explode Node" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->explodeNode(m_node->name().c_str());
      }

    private:

      GraphView::Node *m_node;
    };

    class SelectAllNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      SelectAllNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::SelectAllNodesAction" 
          , "Select all" 
          , QKeySequence::SelectAll
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~SelectAllNodesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIGraph()->selectAllNodes();
      }
    };

    class DeselectAllNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      DeselectAllNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::DeselectAllNodesAction" 
          , "Deselect all" 
          , QKeySequence(Qt::CTRL + Qt::Key_D)
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~DeselectAllNodesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIGraph()->clearSelection();
      }
    };

    class SidePanelScrollUpAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      SidePanelScrollUpAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::SidePanel *sidePanel,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::SidePanelScrollUpAction" 
          , "Scroll Up" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_sidePanel( sidePanel )
      {
      }

    private slots:

      void onTriggered()
      {
        m_sidePanel->scroll(m_sidePanel->size().height());
      }

    private:

      FabricUI::GraphView::SidePanel *m_sidePanel;
    };

    class SidePanelScrollDownAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      SidePanelScrollDownAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::SidePanel *sidePanel,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::SidePanelScrollDownAction" 
          , "Scroll Down" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_sidePanel( sidePanel )
      {
      }

    private slots:

      void onTriggered()
      {
        m_sidePanel->scroll(-m_sidePanel->size().height());
      }

    private:

      FabricUI::GraphView::SidePanel *m_sidePanel;
    };

    class AutoConnectionsAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      AutoConnectionsAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::AutoConnectionsAction" 
          , "Auto Connect Selected Nodes" 
          , Qt::Key_C 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~AutoConnectionsAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIGraph()->autoConnections();
      }

    };

    class RemoveConnectionsAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      RemoveConnectionsAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::RemoveConnectionsAction" 
          , "Remove Connections to Selected Node(s)" 
          , Qt::Key_D 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~RemoveConnectionsAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIGraph()->removeConnections();
      }

    };

    class ExposeAllUnconnectedInputPortsAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ExposeAllUnconnectedInputPortsAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true)
        : BaseDFGWidgetAction(
          dfgWidget
          , parent
          , "DFGWidget::exposeAllPorts( /*inputs*/ )"
          , "Expose all Unconnected Input Ports"
          , QKeySequence()
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~ExposeAllUnconnectedInputPortsAction()
      {
      }

      private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIGraph()->exposeAllPorts(true /* exposeUnconnectedInputs */, false /* exposeUnconnectedOutputs */);
      }
    };

    class ExposeAllUnconnectedOutputPortsAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ExposeAllUnconnectedOutputPortsAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true)
        : BaseDFGWidgetAction(
          dfgWidget
          , parent
          , "DFGWidget::exposeAllPorts( /*outputs*/ )"
          , "Expose all Unconnected Output Ports"
          , QKeySequence()
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~ExposeAllUnconnectedOutputPortsAction()
      {
      }

      private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIGraph()->exposeAllPorts(false /* exposeUnconnectedInputs */, true /* exposeUnconnectedOutputs */);
      }
    };

    class CopyNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      CopyNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::copyNodesAction" 
          , "Copy" 
          , QKeySequence::Copy
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~CopyNodesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->copy();
      }

    };

    class CutNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      CutNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::cutNodesAction" 
          , "Cut" 
          , QKeySequence::Cut
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~CutNodesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->cmdCut();
      }

    };

    class PasteNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      PasteNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::PasteNodesAction" 
          , "Paste" 
          , QKeySequence::Paste
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~PasteNodesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->cmdPaste();
      }

    };

    class CollapseLevel1Action : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      CollapseLevel1Action(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::CollapseLevel1Action" 
          , "Collapse Level 1" 
          , Qt::Key_1
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~CollapseLevel1Action()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->setSelectedNodesCollapseState(2);
      }

    };

    class CollapseLevel2Action : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      CollapseLevel2Action(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::CollapseLevel2Action" 
          , "Collapse Level 2" 
          , Qt::Key_2
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~CollapseLevel2Action()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->setSelectedNodesCollapseState(1);
      }

    };

    class CollapseLevel3Action : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      CollapseLevel3Action(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::CollapseLevel3Action" 
          , "Collapse Level 3" 
          , Qt::Key_3
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~CollapseLevel3Action()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->setSelectedNodesCollapseState(0);
      }

    };

    class ResetZoomAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ResetZoomAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ResetZoomAction" 
          , "Reset Zoom" 
          , QKeySequence(Qt::CTRL + Qt::Key_0)
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~ResetZoomAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->zoomCanvas(1.0);
      }

    };

    class FrameSelectedNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      FrameSelectedNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::FrameSelectedNodesAction" 
          , "Frame selected" 
          , Qt::Key_F 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~FrameSelectedNodesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->frameSelectedNodes();
      }
 
    };

    class FrameAllNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      FrameAllNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::FrameAllNodesAction" 
          , "Frame all" 
          , Qt::Key_A 
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~FrameAllNodesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->frameAllNodes();
      }

    };

    class RelaxNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      RelaxNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::RelaxNodesAction" 
          , "Relax Nodes" 
          , QKeySequence(Qt::CTRL + Qt::Key_R)
          , Qt::WidgetWithChildrenShortcut
          , enable)
      {
      }

      virtual ~RelaxNodesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->getUIController()->relaxNodes();
      }

    };

    class ReloadExtensionsAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ReloadExtensionsAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ReloadExtensionsAction" 
          , "Reload Extension(s)" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->reloadExtensionDependencies(m_node->name().c_str());
      }

    private:

      GraphView::Node *m_node;
    };

    class SetNodeCommentAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      SetNodeCommentAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool useSetText = true,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::SetNodeCommentAction" 
          , "Set Comment" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
        setText( useSetText ? "Set Comment" : "Edit Comment" );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->onBubbleEditRequested(m_node);
      }

    private:

      GraphView::Node *m_node;
    };

    class RemoveNodeCommentAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      RemoveNodeCommentAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::RemoveNodeCommentAction" 
          , "Remove Comment" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        QString nodeName = QString::fromUtf8( m_node->name().c_str() );
        m_dfgWidget->getUIController()->setNodeCommentExpanded( nodeName, false );
        m_dfgWidget->getUIController()->cmdSetNodeComment( nodeName, QString() );
      }

    private:

      GraphView::Node *m_node;
    };

    class DeleteNodesAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      DeleteNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( dfgWidget, parent )
      {
        QList<QKeySequence> shortcuts;
        shortcuts += Qt::Key_Delete;
        shortcuts += Qt::Key_Backspace;

        this->init(
          "DFGWidget::DeleteNodesAction"
          , "Delete"
          , shortcuts
          , Qt::WidgetWithChildrenShortcut
          , enable
        );
      }

      virtual ~DeleteNodesAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        if (m_dfgWidget->isEditable())
        {
          std::vector<GraphView::Node *> nodes = m_dfgWidget->getUIGraph()->selectedNodes();
          m_dfgWidget->getUIController()->gvcDoRemoveNodes(nodes);
        }
      }

    };

    class NewBlockNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      NewBlockNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::NewBlockNodeAction" 
          , "New Block" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_pos( pos )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewBlockNode( m_pos );
      }

    private:

      QPoint m_pos;
    };

    class NewCacheNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      NewCacheNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::NewCacheNodeAction" 
          , "New Cache Node" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_pos( pos )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewCacheNode( m_pos );
      }

    private:

      QPoint m_pos;
    };

    class NewGraphNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      NewGraphNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::NewGraphNodeAction" 
          , "New Empty Graph" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_pos( pos )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewGraphNode( m_pos );
      }

    private:

      QPoint m_pos;
    };
 
    class NewFunctionNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      NewFunctionNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::NewFunctionNodeAction" 
          , "New Empty Function" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_pos( pos )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewFunctionNode( m_pos );
      }

    private:

      QPoint m_pos;
    };

    class NewBackdropNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      NewBackdropNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::NewBackdropNodeAction" 
          , "New Backdrop" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_pos( pos )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewBackdropNode( m_pos);
      }

    private:

      QPoint m_pos;
    };

    class EditNodeAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      EditNodeAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::EditNodeAction" 
          , "Edit Node" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_node( node )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->maybeEditNode( m_node );
      }

    private:

      GraphView::Node *m_node;
    };

    class EditInstBlockAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      EditInstBlockAction(
        DFGWidget *dfgWidget,
        GraphView::InstBlock *instBlock,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::EditInstBlockAction" 
          , QString("Edit Block '" + instBlock->name_QS() + "'") 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_instBlock( instBlock )
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->maybeEditInstBlock( m_instBlock );
      }

    private:

      GraphView::InstBlock *m_instBlock;
    };

    class ConnectionSelectSourceAndTargetAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ConnectionSelectSourceAndTargetAction(
        DFGWidget *dfgWidget,
        GraphView::Connection *connection,
        QObject *parent,
        bool clearCurrentSelection,
        bool selectSource,
        bool selectTarget,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ConnectionSelectSourceAndTargetAction" 
          , "Select Source" 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_connection( connection )
        , m_clearCurrentSelection( clearCurrentSelection )
        , m_selectSource( selectSource )
        , m_selectTarget( selectTarget )
      {
        QString text = "null";
        if (selectSource || selectTarget)
        {
          text = "Select ";
          if      (!selectTarget)   text += "Source";
          else if (!selectSource)   text += "Target";
          else                      text += "Source and Target";
        }
        setText( text );
        setEnabled( enable && text != "null" );
      }

    private slots:

      void onTriggered()
      {
        if (m_clearCurrentSelection)
          m_dfgWidget->getUIGraph()->clearSelection();

        if (m_selectSource)
        {
          if (m_connection->src()->targetType() == GraphView::TargetType_Pin)
          {
            GraphView::Pin  *pin  = (GraphView::Pin *)m_connection->src();
            GraphView::Node *node = pin->node();
            node->setSelected(true);
          }
          else if (m_connection->src()->targetType() == GraphView::TargetType_InstBlockPort)
          {
            GraphView::InstBlockPort *instBlockPort = (GraphView::InstBlockPort *)m_connection->src();
            GraphView::Node *node = instBlockPort->instBlock()->node();
            node->setSelected(true);
          }
        }

        if (m_selectTarget)
        {
          if (m_connection->dst()->targetType() == GraphView::TargetType_Pin)
          {
            GraphView::Pin  *pin  = (GraphView::Pin *)m_connection->dst();
            GraphView::Node *node = pin->node();
            node->setSelected(true);
          }
          else if (m_connection->dst()->targetType() == GraphView::TargetType_InstBlockPort)
          {
            GraphView::InstBlockPort *instBlockPort = (GraphView::InstBlockPort *)m_connection->dst();
            GraphView::Node *node = instBlockPort->instBlock()->node();
            node->setSelected(true);
          }
        }
      }

    private:

      GraphView::Connection *m_connection;
      bool m_clearCurrentSelection;
      bool m_selectSource;
      bool m_selectTarget;
    };

    class ConnectionRemoveAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ConnectionRemoveAction(
        DFGWidget *dfgWidget,
        GraphView::Connection *connection,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ConnectionRemoveAction" 
          , "Remove Connection" 
          , QKeySequence(Qt::Key_D)
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_connection( connection )
      {
      }

      virtual ~ConnectionRemoveAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        std::vector<GraphView::ConnectionTarget *> srcs;
        std::vector<GraphView::ConnectionTarget *> dsts;
        srcs.push_back( m_connection->src() );
        dsts.push_back( m_connection->dst() );
        m_dfgWidget->getUIController()->gvcDoRemoveConnections(srcs, dsts);
      }

    private:

      GraphView::Connection *m_connection;
    };

    class ConnectionInsertPresetAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ConnectionInsertPresetAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        GraphView::Connection *connection,
        QString presetPath,
        QString presetPortIn,
        QString presetPortOut,
        QPoint  presetGlobalPos,
        QKeySequence shortcut = QKeySequence(),
        QString presetPortSetFromSrcName = "",
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ConnectionInsertPresetAction" 
          , "" 
          , shortcut
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_connection( connection )
        , m_presetPath( presetPath )
        , m_presetPortIn( presetPortIn )
        , m_presetPortOut( presetPortOut )
        , m_presetGlobalPos( presetGlobalPos )
        , m_presetPortSetFromSrcName( presetPortSetFromSrcName )
      {
        QString presetName = getPresetNameFromPath(m_presetPath);
        setText( "Insert '" + presetName + "' Preset");
      }

      QString getPresetNameFromPath(QString presetPath)
      {
        QStringList path = presetPath.split(".");
        return (path.size() > 0 ? path[path.size() - 1] : "");
      }

      virtual ~ConnectionInsertPresetAction()
      {
      }

    private slots:

      virtual void onTriggered()
      {
        if (m_dfgWidget->isEditable())
        {
          // if m_connection is NULL then look for
          // a connection that the mouse is hovering.
          if (!m_connection)
          {
            std::vector<GraphView::Connection *> connections = m_dfgWidget->getUIGraph()->connections();
            for(int i=0;i<(int)connections.size();i++)
              if (connections[i]->isHovered())
              {
                m_presetGlobalPos = QCursor::pos();
                m_connection = connections[i];
                break;
              }
          }
          if (m_connection)
          {
            QString nodeName = m_dfgWidget->getUIController()->cmdAddInstFromPreset(m_presetPath, m_dfgWidget->getGraphViewWidget()->mapToGraph(m_presetGlobalPos));
            GraphView::Node *node = m_dfgWidget->getUIGraph()->node(nodeName);
            if (node)
            {
              // center the node.
              QList<QPointF> centeredPos;
              centeredPos.push_back(node->topLeftGraphPos());
              centeredPos[0].rx() -= 0.5 * node->minimumWidth();
              centeredPos[0].ry() -= 0.5 * node->minimumHeight();
              QStringList nodeName;
              nodeName.push_back(node->name_QS());
              m_dfgWidget->getUIController()->cmdMoveNodes(nodeName, centeredPos);

              // create the connections.
              GraphView::Pin *pinIn  = node->pin(m_presetPortIn .toUtf8().data());
              GraphView::Pin *pinOut = node->pin(m_presetPortOut.toUtf8().data());
              if (pinIn && pinOut)
              {
                if (!m_presetPortSetFromSrcName.isEmpty())
                {
                  // set the port m_presetPortSetFromSrcName equal the connection's source name.
                  GraphView::Pin *pin = node->pin(m_presetPortSetFromSrcName.toUtf8().data());
                  if (pin && pin->dataType() == "String")
                  {
                    QString value = m_connection->src()->path_QS();
                    FabricCore::RTVal rtval = FabricCore::RTVal::ConstructString(m_dfgWidget->getUIController()->getClient(), value.toUtf8().data());
                    QString portPath = QString(node->name().c_str()) + "." + m_presetPortSetFromSrcName;
                    m_dfgWidget->getUIController()->cmdSetPortDefaultValue(portPath, rtval);
                  }
                }
                std::vector<GraphView::ConnectionTarget *> srcs;
                std::vector<GraphView::ConnectionTarget *> dsts;
                srcs.push_back( m_connection->src() );
                dsts.push_back( pinIn );
                srcs.push_back( pinOut );
                dsts.push_back( m_connection->dst() );
                m_dfgWidget->getUIController()->gvcDoAddConnections(srcs, dsts);

                // remove the original connection if it still exists.
                // note: this can happen with "Execute" ports that
                //       support more than one source. 
                std::vector<GraphView::Connection *> connections = m_dfgWidget->getUIGraph()->connections();
                for(int i=0;i<(int)connections.size();i++)
                  if (   connections[i]->src() == m_connection->src()
                      && connections[i]->dst() == m_connection->dst() )
                  {
                    m_dfgWidget->getUIController()->cmdDisconnect( QStringList( m_connection->src()->path_QS() ),
                                                                   QStringList( m_connection->dst()->path_QS() ) );
                    break;
                  }
              }
            }
            m_connection = NULL;
          }
        }
      }

    private:

      GraphView::Connection *m_connection;
      QString m_presetPath;
      QString m_presetPortIn;
      QString m_presetPortOut;
      QPoint  m_presetGlobalPos;
      QString m_presetPortSetFromSrcName;
    };

    class ReloadStyleAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      ReloadStyleAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction( 
          dfgWidget
          , parent
          , "DFGWidget::ReloadStyleAction" 
          , "Reload QSS Styles" 
          , QKeySequence( "Ctrl+Shift+R" )
          , Qt::WindowShortcut
          , enable)
      {
      }

      virtual ~ReloadStyleAction()
      {
      }
      
    private slots:

      virtual void onTriggered()
      {
        m_dfgWidget->onReloadStyles();
      }

    };

    class AboutFabricAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:

      AboutFabricAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true )
        : BaseDFGWidgetAction(
          dfgWidget, 
          parent, 
          "DFGWidget::AboutFabricAction",
          "&About Fabric",
          QKeySequence(),
          Qt::WidgetWithChildrenShortcut,
          enable)
      {
      }

      void invokeOnTriggered()
      {
        onTriggered();
      }

    private slots:

      void onTriggered()
      {
        QMessageBox msgBox(QMessageBox::NoIcon, "About Fabric",
          "",
          QMessageBox::NoButton,
          m_dfgWidget);

        msgBox.addButton("Ok", QMessageBox::AcceptRole);

        char *fabricDir = getenv( "FABRIC_DIR" );
        if ( fabricDir )
        {
          std::string logoPath = FTL::PathJoin( fabricDir, "Resources" );
          FTL::PathAppendEntry( logoPath, "fe_logo.png" );
          QPixmap pixmap(logoPath.c_str());
          msgBox.setIconPixmap(pixmap.scaled(pixmap.size() / 2));
        }

        QString text = "";
        text += "<br/>";
        text += "Fabric Engine version " + QString(FabricCore::GetVersionWithBuildInfoStr());
        text += "<br/>";
        text += "<br/>Copyright (c) 2010-2017 Fabric Software Inc.";
        text += "<br/>All rights reserved.";
        text += "<br/>";
        text += "<br/><a href='http://fabricengine.com/eula/'><font color=#2ab7e5>End User License Agreement (EULA)</font></a>";
        text += "<br/>";
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(text);

        msgBox.exec();
      }

    };

    class OpenUrlAction : public Actions::BaseAction
    {
      Q_OBJECT

    public:

      OpenUrlAction(
        QObject *parent,
        QString menuItemName,
        QString url,
        bool enable = true )
        : Actions::BaseAction( 
          parent
          , "DFGWidget::OpenUrlAction" 
          , menuItemName 
          , QKeySequence() 
          , Qt::WidgetWithChildrenShortcut
          , enable)
        , m_url( url )
      {
      }

    private slots:

      void onTriggered()
      {
        QDesktopServices::openUrl(m_url);
      }

    private:

      QUrl m_url;
    };

    class BlockCompilationsAction : public BaseDFGWidgetAction
    {
      Q_OBJECT

    public:
      BlockCompilationsAction(
        DFGWidget *dfgWidget,
        QObject *parent,
        bool enable = true)
        : BaseDFGWidgetAction(
          dfgWidget, 
          parent, 
          "DFGWidget::BlockCompilationsAction",
          "Disable Graph Compilations",
          QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_Return),
          Qt::WidgetWithChildrenShortcut,
          enable)
      {
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->onToggleBlockCompilations();
      }
    };

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_DFGWidget__
