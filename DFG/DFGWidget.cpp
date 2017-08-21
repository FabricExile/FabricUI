// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 
#include <assert.h>
#include <FabricCore.h>
#include <FabricUI/DFG/DFGErrorsWidget.h>
#include <FabricUI/DFG/DFGExecBlockEditorWidget.h>
#include <FabricUI/DFG/DFGGraphViewWidget.h>
#include <FabricUI/DFG/DFGMetaDataHelpers.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/Dialogs/DFGBlockPropertiesDialog.h>
#include <FabricUI/DFG/Dialogs/DFGEditPortDialog.h>
#include <FabricUI/DFG/Dialogs/DFGGetStringDialog.h>
#include <FabricUI/DFG/Dialogs/DFGGetTextDialog.h>
#include <FabricUI/DFG/Dialogs/DFGNewVariableDialog.h>
#include <FabricUI/DFG/Dialogs/DFGNodePropertiesDialog.h>
#include <FabricUI/DFG/Dialogs/DFGPickVariableDialog.h>
#include <FabricUI/DFG/Dialogs/DFGSavePresetDialog.h>
#include <FabricUI/DFG/DFGBindingUtils.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/FixedPort.h>
#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/MainPanel.h>
#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/Util/FabricResourcePath.h>
#include <FabricUI/Util/LoadFabricStyleSheet.h>
#include <FabricUI/Util/UIRange.h>
#include <FabricUI/Util/DocUrl.h>
#include <FTL/FS.h>
#include <FTL/Path.h>
#include <Persistence/RTValToJSONEncoder.hpp>
#include <QCoreApplication>
#include <QDebug>
#include <QUrl>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <FabricUI/DFG/Tools/DFGPVToolActions.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

QSettings * DFGWidget::g_settings = NULL;

DFGWidget::DFGWidget(
  QWidget * parent, 
  FabricCore::Client &client,
  FabricCore::DFGHost &host,
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec,
  FabricServices::ASTWrapper::KLASTManager * manager,
  DFGUICmdHandler *cmdHandler,
  const DFGConfig & dfgConfig,
  bool overTakeBindingNotifications
  )
  : QWidget( parent )
  , m_errorsWidget( 0 )
  , m_uiGraph( 0 )
  , m_router( 0 )
  , m_tabSearchVariablesDirty( true )
  , m_manager( manager )
  , m_dfgConfig( dfgConfig )
  , m_isEditable( false )
  , m_uiGraphZoomBeforeQuickZoom( 0 )
{
  std::string fontsDir = FabricResourcePath( FTL_STR("Fonts") );
  std::vector<std::string> familyNames;
  FTL::FSDirAppendEntries( fontsDir.c_str(), familyNames );
  for ( std::vector<std::string>::const_iterator it = familyNames.begin();
    it != familyNames.end(); ++it )
  {
    FTL::StrRef familyName = *it;

    std::string familyDir = fontsDir;
    FTL::PathAppendEntry( familyDir, familyName );

    if ( FTL::FSIsDir( familyDir ) )
    {
      std::vector<std::string> fontFilenames;
      FTL::FSDirAppendEntries( familyDir.c_str(), fontFilenames );
      for ( std::vector<std::string>::const_iterator it = fontFilenames.begin();
        it != fontFilenames.end(); ++it )
      {
        FTL::StrRef fontFilename = *it;

        std::string fontPathname = familyDir;
        FTL::PathAppendEntry( fontPathname, fontFilename );

        if ( FTL::FSIsFile( fontPathname )
          && FTL::StrRef( fontPathname ).endswith( FTL_STR(".ttf") ) )
        {
          QString fontPathnameQS = StrRefFilenameToQString( fontPathname );
          // qDebug() << fontPathnameQS;
          if ( QFontDatabase::addApplicationFont( fontPathnameQS ) == -1 )
            qDebug() << "WARNING: failed to add font: " << fontPathnameQS;
        }
      }
    }
  }

  setStyle( new DFGWidgetProxyStyle( style() ) );
  
  reloadStyles();

  m_uiController = new DFGController(
    0,
    this,
    client,
    m_manager,
    cmdHandler,
    overTakeBindingNotifications
    );

  QObject::connect(
    m_uiController.get(), SIGNAL( execChanged() ),
    this, SLOT( onExecChanged() )
    );
  QObject::connect(
    m_uiController.get(), SIGNAL( execSplitChanged() ),
    this, SLOT( onExecSplitChanged() )
    );

  m_uiHeader =
    new DFGExecHeaderWidget(
      this,
      m_uiController.get(),
      dfgConfig.graphConfig
      );
  QObject::connect(
    this, SIGNAL( execChanged() ),
    m_uiHeader, SLOT( refresh() )
    );

  m_uiGraphViewWidget = new DFGGraphViewWidget(this, dfgConfig.graphConfig, NULL);
  QObject::connect(
    m_uiGraphViewWidget, SIGNAL(urlDropped(QUrl, bool, bool, QPointF)),
    this, SIGNAL(urlDropped(QUrl, bool, bool, QPointF))
    );

  m_uiGraphViewWidget->addAction(new TabSearchAction                 (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new GoUpAction                      (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new SelectAllNodesAction            (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new DeselectAllNodesAction          (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new AutoConnectionsAction           (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new RemoveConnectionsAction         (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new CutNodesAction                  (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new CopyNodesAction                 (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new PasteNodesAction                (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new CollapseLevel1Action            (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new CollapseLevel2Action            (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new CollapseLevel3Action            (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new ResetZoomAction                 (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new FrameSelectedNodesAction        (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new FrameAllNodesAction             (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new RelaxNodesAction                (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new DeleteNodesAction               (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new EditSelectedNodeAction          (this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new EditSelectedNodePropertiesAction(this, m_uiGraphViewWidget));
  m_uiGraphViewWidget->addAction(new ConnectionInsertPresetAction    (this, m_uiGraphViewWidget,
                                                                      NULL,
                                                                      "Fabric.Compounds.Debug.Log.LabeledReport",
                                                                      "value",
                                                                      "value",
                                                                      QCursor::pos(),
                                                                      QKeySequence(),
                                                                      "label"));

  m_klEditor =
    new DFGKLEditorWidget(
      this,
      m_uiHeader,
      m_uiController.get(),
      m_manager,
      m_dfgConfig
      );

  m_execBlockEditorWidget =
    new DFGExecBlockEditorWidget(
      this,
      m_uiHeader
      );

  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing( 0 );
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_uiHeader);
  layout->addWidget(m_uiGraphViewWidget);
  layout->addWidget(m_klEditor);
  layout->addWidget(m_execBlockEditorWidget);

  QWidget *widget = new QWidget;
  widget->setSizePolicy(
    QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding )
    );
  widget->setLayout( layout );

  m_errorsWidget = new DFGErrorsWidget( m_uiController.get() ); 
  connect(
    m_errorsWidget, SIGNAL(execSelected(FTL::CStrRef, int, int)),
    this, SLOT(onExecSelected(FTL::CStrRef, int, int))
    );
  connect(
    m_errorsWidget, SIGNAL(nodeSelected(FTL::CStrRef, FTL::CStrRef, int, int)),
    this, SLOT(onNodeSelected(FTL::CStrRef, FTL::CStrRef, int, int))
    );

  QSplitter *splitter = new QSplitter;
  splitter->setOrientation( Qt::Vertical );
  splitter->setSizePolicy(
    QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding )
    );
  splitter->setContentsMargins(0, 0, 0, 0);
  splitter->setChildrenCollapsible(false);
  splitter->addWidget( widget );
  splitter->setStretchFactor( 0, 4 );
  splitter->addWidget( m_errorsWidget );
  splitter->setStretchFactor( 1, 1 );

  layout = new QVBoxLayout;
  layout->setSpacing( 0 );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( splitter );
  setLayout( layout );

  m_legacyTabSearchWidget = new DFGTabSearchWidget( this, m_dfgConfig );
  m_legacyTabSearchWidget->hide();
  m_tabSearchWidget = new DFGPresetSearchWidget( &getDFGController()->getHost() );
  m_tabSearchWidget->setParent( this );
  m_tabSearchWidget->hide();
  QObject::connect(
    m_tabSearchWidget, SIGNAL( selectedPreset( QString ) ),
    this, SLOT( onPresetAddedFromTabSearch( QString ) )
  );
  QObject::connect(
    m_tabSearchWidget, SIGNAL( selectedBackdrop() ),
    this, SLOT( onBackdropAddedFromTabSearch() )
  );
  QObject::connect(
    m_tabSearchWidget, SIGNAL( selectedNewBlock() ),
    this, SLOT( onNewBlockAddedFromTabSearch() )
  );
  QObject::connect(
    m_tabSearchWidget, SIGNAL( selectedCreateNewVariable() ),
    this, SLOT( onVariableCreationRequestedFromTabSearch() )
  );
  QObject::connect(
    m_tabSearchWidget, SIGNAL( selectedGetVariable( const std::string ) ),
    this, SLOT( onVariableGetterAddedFromTabSearch( const std::string ) )
  );
  QObject::connect(
    m_tabSearchWidget, SIGNAL( giveFocusToParent() ),
    this, SLOT( onFocusGivenFromTabSearch() )
  );
  QObject::connect(
    m_tabSearchWidget, SIGNAL( selectedSetVariable( const std::string ) ),
    this, SLOT( onVariableSetterAddedFromTabSearch( const std::string ) )
  );
  QObject::connect(
    getUIController(), SIGNAL( varsChangedImplicitly() ),
    this, SLOT( tabSearchVariablesSetDirty() )
  );

  QObject::connect(
    m_uiHeader, SIGNAL(goUpPressed()),
    this, SLOT(onGoUpPressed())
    );

  QObject::connect(
    m_uiController.get(), SIGNAL(nodeEditRequested(FabricUI::GraphView::Node *)), 
    this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node *))
    );

  m_uiController->setHostBindingExec( host, binding, execPath, exec );

  addAction( new ReloadStyleAction(this, this) );
}

DFGWidget::~DFGWidget()
{
  m_errorsWidget->focusNone();

  if ( m_uiController )
    m_uiController->setRouter( 0 );

  if ( m_router )
    delete m_router;
}

GraphView::Graph * DFGWidget::getUIGraph()
{
  return m_uiGraph;
}

DFGKLEditorWidget * DFGWidget::getKLEditor()
{
  return m_klEditor;
}

DFGController * DFGWidget::getUIController()
{
  return m_uiController.get();
}

const DFGController * DFGWidget::getUIController() const
{
  return m_uiController.get();
}

std::string DFGWidget::getBindingHostApp() const
{
  std::string host_app = "";
  DFGController *controller = (DFGController *)getUIController();
  if (controller)
  {
    const char *host_app_ptr = controller->getBinding().getMetadata("host_app");
    if (host_app_ptr)
      host_app = host_app_ptr;
  }
  return host_app;
}

const char* legacyTabSearchKey = "useLegacyTabSearch";

bool DFGWidget::isUsingLegacyTabSearch() const
{
  return getSettings()->value( legacyTabSearchKey, false ).toBool();
}

void DFGWidget::onToggleLegacyTabSearch( bool toggled )
{
  getSettings()->setValue( legacyTabSearchKey, toggled );
}

DFGAbstractTabSearchWidget * DFGWidget::getTabSearchWidget()
{
  if( this->isUsingLegacyTabSearch() )
    return m_legacyTabSearchWidget;
  return m_tabSearchWidget;
}

DFGGraphViewWidget * DFGWidget::getGraphViewWidget() { return m_uiGraphViewWidget; }
const DFGGraphViewWidget * DFGWidget::getGraphViewWidget() const { return m_uiGraphViewWidget; }

DFGExecHeaderWidget * DFGWidget::getHeaderWidget()
{
  return m_uiHeader;
}

static void CountNodeTypes(
  FabricCore::DFGExec exec,
  FTL::ArrayRef<GraphView::Node *> nodes,
  unsigned &varNodeCount,
  unsigned &getNodeCount,
  unsigned &setNodeCount,
  unsigned &instNodeCount,
  unsigned &userNodeCount,
  unsigned &backdropNodeCount,
  unsigned &blockNodeCount
  )
{
  varNodeCount = 0;
  getNodeCount = 0;
  setNodeCount = 0;
  instNodeCount = 0;
  userNodeCount = 0;
  backdropNodeCount = 0;
  blockNodeCount = 0;
  for(unsigned int i=0;i<nodes.size();i++)
  {
    if (nodes[i]->isBackDropNode())
    {
      backdropNodeCount++;
      continue;
    }

    char const * nodeName = nodes[i]->name().c_str();
    if ( exec.isExecBlock( nodeName ) )
      ++blockNodeCount;
    else
    {
      FabricCore::DFGNodeType dfgNodeType = exec.getNodeType( nodeName );
      
      if ( dfgNodeType == FabricCore::DFGNodeType_Var)
        varNodeCount++;
      else if ( dfgNodeType == FabricCore::DFGNodeType_Get)
        getNodeCount++;
      else if ( dfgNodeType == FabricCore::DFGNodeType_Set)
        setNodeCount++;
      else if ( dfgNodeType == FabricCore::DFGNodeType_Inst)
        instNodeCount++;
      else
        userNodeCount++;
    }
  }
}

QMenu* DFGWidget::graphContextMenuCallback(FabricUI::GraphView::Graph* graph, void* userData)
{
  DFGWidget     *graphWidget = (DFGWidget*)userData;
  DFGController *controller  = static_cast<DFGController *>(graph->controller());
  if (!controller)
    return NULL;

  if (graphWidget->isQuickZoomActive())  // [FE-7950]
    return NULL;

  std::vector<GraphView::Node *> nodes = graph->selectedNodes();
  
  unsigned varNodeCount;
  unsigned getNodeCount;
  unsigned setNodeCount;
  unsigned instNodeCount;
  unsigned userNodeCount;
  unsigned backdropNodeCount;
  unsigned blockNodeCount;
  CountNodeTypes(
    controller->getExec(),
    nodes,
    varNodeCount,
    getNodeCount,
    setNodeCount,
    instNodeCount,
    userNodeCount,
    backdropNodeCount,
    blockNodeCount
    );

  QMenu *result = new QMenu( graph->scene()->views()[0] );
  
  result->addAction(new GoUpAction(graphWidget, result, !controller->isViewingRootGraph()) );

  result->addSeparator();

  result->addAction(new NewGraphNodeAction   (graphWidget, QCursor::pos(), result, graphWidget->isEditable()));
  result->addAction(new NewFunctionNodeAction(graphWidget, QCursor::pos(), result, graphWidget->isEditable()));
  result->addAction(new NewBackdropNodeAction(graphWidget, QCursor::pos(), result, graphWidget->isEditable()));

  result->addSeparator();

  result->addAction(new ImplodeSelectedNodesAction(graphWidget, result, graphWidget->isEditable() && blockNodeCount == 0 && nodes.size() > 0));
  result->addAction(new ExplodeSelectedNodesAction(graphWidget, result, graphWidget->isEditable() && instNodeCount + userNodeCount > 0));

  result->addSeparator();

  result->addAction(new NewVariableNodeAction   (graphWidget, QCursor::pos(), result, graphWidget->isEditable()));
  result->addAction(new NewVariableGetNodeAction(graphWidget, QCursor::pos(), result, graphWidget->isEditable()));
  result->addAction(new NewVariableSetNodeAction(graphWidget, QCursor::pos(), result, graphWidget->isEditable()));
  result->addAction(new NewCacheNodeAction      (graphWidget, QCursor::pos(), result, graphWidget->isEditable()));

  result->addSeparator();

  result->addAction(new NewBlockNodeAction(graphWidget, QCursor::pos(), result, graphWidget->isEditable() && controller->getExec().allowsBlocks()));

  result->addSeparator();

  result->addAction(new CopyNodesAction       (graphWidget, result, nodes.size() > 0));
  result->addAction(new CutNodesAction        (graphWidget, result, graphWidget->isEditable() && nodes.size() > 0));
  result->addAction(new PasteNodesAction      (graphWidget, result, graphWidget->isEditable() && !QApplication::clipboard()->text().isEmpty()));
  result->addAction(new SelectAllNodesAction  (graphWidget, result, graph->nodes().size() > 0));
  result->addAction(new DeselectAllNodesAction(graphWidget, result, nodes.size() > 0));

  result->addSeparator();

  QMenu *exposePortsMenu = result->addMenu(tr("Expose Ports"));
  exposePortsMenu->setEnabled(graphWidget->isEditable() && nodes.size() - backdropNodeCount > 0);
  exposePortsMenu->addAction(new ExposeAllUnconnectedInputPortsAction(graphWidget, exposePortsMenu));
  exposePortsMenu->addAction(new ExposeAllUnconnectedOutputPortsAction(graphWidget, exposePortsMenu));
  result->addAction(new AutoConnectionsAction  (graphWidget, result, graphWidget->isEditable() && nodes.size() - backdropNodeCount > 1));
  result->addAction(new RemoveConnectionsAction(graphWidget, result, graphWidget->isEditable() && nodes.size() - backdropNodeCount > 0));

  result->addSeparator();

  result->addAction(new ResetZoomAction(graphWidget, result));

  result->setFocus( Qt::OtherFocusReason );

  return result;
}

QMenu *DFGWidget::nodeContextMenuCallback(
  FabricUI::GraphView::Node *uiNode,
  void *dfgWidgetVoidPtr
  )
{
  try
  {
    DFGWidget           *dfgWidget = static_cast<DFGWidget *>(dfgWidgetVoidPtr);
    FabricCore::DFGExec &exec      = dfgWidget->m_uiController->getExec();
    GraphView::Graph    *graph     = dfgWidget->m_uiGraph;
    if (graph->controller() == NULL)
      return NULL;

    if (dfgWidget->isQuickZoomActive())  // [FE-7950]
      return NULL;

    std::vector<GraphView::Node *> nodes = dfgWidget->getUIController()->graph()->selectedNodes();
    unsigned varNodeCount;
    unsigned getNodeCount;
    unsigned setNodeCount;
    unsigned instNodeCount;
    unsigned userNodeCount;
    unsigned backdropNodeCount;
    unsigned blockNodeCount;
    CountNodeTypes(
      exec,
      nodes,
      varNodeCount,
      getNodeCount,
      setNodeCount,
      instNodeCount,
      userNodeCount,
      backdropNodeCount,
      blockNodeCount
      );
    bool onlyInstNodes        = (instNodeCount == nodes.size());
    bool onlyInstOrBlockNodes = (nodes.size() == (instNodeCount + blockNodeCount));
    bool someVarNodes         = (varNodeCount > 0);
    bool someGetNodes         = (getNodeCount > 0);
    bool someSetNodes         = (setNodeCount > 0);

    char const *nodeName = uiNode->name().c_str();

    FabricCore::DFGExec instExec;
    bool instExecCanCreatePreset = false;
    bool instExecCanUpdatePreset = false;
    if ( uiNode->isInstNode() )
    {
      instExec = exec.getSubExec( nodeName );
      if ( !instExec.isPreset() )
      {
        instExecCanCreatePreset = true;
        
        FabricCore::String origPresetGUID = instExec.getOrigPresetGUID();
        if ( origPresetGUID.getSize() > 0 )
        {
          FabricCore::DFGHost host = exec.getHost();
          FabricCore::String presetPath =
            host.getPresetPathForPresetGUID( origPresetGUID.getCStr() );

          instExecCanUpdatePreset = presetPath.getSize() > 0;
          if(instExecCanUpdatePreset)
          {
            // if we are supposed to hide the fabric dir
            bool hideFabricDir = true;
            const char * hideFabricDirStr = getenv("FABRIC_CANVAS_UPDATE_BUILTIN_PRESETS");
            if(hideFabricDirStr != NULL)
              hideFabricDir = (FTL::StrRef(hideFabricDirStr) == "0");

            // let's disable the update preset action if the preset is below the fabric_dir
            if(hideFabricDir)
            {
              FTL::CStrRef presetPathRef = presetPath.getCStr();
              if(presetPathRef.startswith("Fabric."))
                instExecCanUpdatePreset = false;
            }
          }
        }
      }
    }

    bool uiNodeIsInstOrBlockNode = (uiNode->isInstNode() || uiNode->isBlockNode());
    bool uiNodeHasDocUrl         = false;
    if (  !exec.isExecBlock(nodeName)
        && exec.getNodeType(nodeName) == FabricCore::DFGNodeType_Inst)
    {
      QString uiDocUrl = exec.getNodeMetadata(nodeName, "uiDocUrl");
      if (uiDocUrl.isEmpty())
        uiDocUrl = exec.getSubExec(nodeName).getMetadata("uiDocUrl");
      uiNodeHasDocUrl = !uiDocUrl.isEmpty();
    }

    QMenu *result = new QMenu(uiNode->scene()->views()[0]);

    result->addAction(new OpenPresetDocAction(dfgWidget, uiNode, result, uiNodeHasDocUrl));

    result->addSeparator();

    result->addAction(new InspectNodeAction(dfgWidget, uiNode, result, uiNodeIsInstOrBlockNode));
    if ( onlyInstOrBlockNodes )
    {
      result->addSeparator();

      GraphView::Node::EditingTargets editingTargets;
      uiNode->collectEditingTargets( editingTargets );
      int lastPriority = -1;
      bool inTopPriority = true;
      for ( size_t i = 0; i < editingTargets.size(); ++i )
      {
        int priority = editingTargets[i].second;
        bool first = lastPriority == -1;
        if ( !first && priority > lastPriority )
        {
          inTopPriority = false;
          result->addSeparator();
        }

        QAction *editAction = NULL;
        if ( editingTargets[i].first->type() == GraphView::QGraphicsItemType_Node )
          editAction = new EditNodeAction(
            dfgWidget,
            static_cast<GraphView::Node *>( editingTargets[i].first ),
            result
            );
        else if ( editingTargets[i].first->type() == GraphView::QGraphicsItemType_InstBlock )
          editAction = new EditInstBlockAction(
            dfgWidget,
            static_cast<GraphView::InstBlock *>( editingTargets[i].first ),
            result
            );
        else assert( false );
        if ( inTopPriority )
        {
          editAction->setShortcut( Qt::Key_I );
          editAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
        }
        result->addAction( editAction );

        lastPriority = priority;
      }
    }
    
    result->addSeparator();

    result->addAction(new EditSelectedNodePropertiesAction(dfgWidget, result, nodes.size() == 1 && !someVarNodes && !someGetNodes && !someSetNodes));

    result->addSeparator();

    result->addAction(new DeleteNodesAction(dfgWidget, result, dfgWidget->isEditable()));

    result->addSeparator();

    result->addAction(new CopyNodesAction     (dfgWidget, result, !someVarNodes));
    result->addAction(new CutNodesAction      (dfgWidget, result, dfgWidget->isEditable() && !someVarNodes));
    result->addAction(new PasteNodesAction    (dfgWidget, result, dfgWidget->isEditable() && !QApplication::clipboard()->text().isEmpty()));
    result->addAction(new SelectAllNodesAction(dfgWidget, result));

    result->addSeparator();

    QMenu *exposePortsMenu = result->addMenu(tr("Expose Ports"));
    exposePortsMenu->setEnabled(dfgWidget->isEditable() && nodes.size() - backdropNodeCount > 0);
    exposePortsMenu->addAction(new ExposeAllUnconnectedInputPortsAction(dfgWidget, exposePortsMenu));
    exposePortsMenu->addAction(new ExposeAllUnconnectedOutputPortsAction(dfgWidget, exposePortsMenu));
    result->addAction(new AutoConnectionsAction  (dfgWidget, result, dfgWidget->isEditable() && nodes.size() - backdropNodeCount > 1));
    result->addAction(new RemoveConnectionsAction(dfgWidget, result, dfgWidget->isEditable() && nodes.size() != backdropNodeCount));
    result->addAction(new SplitFromPresetAction  (dfgWidget, uiNode, result, onlyInstNodes && instNodeCount == 1 && exec.getSubExec(uiNode->name().c_str()).editWouldSplitFromPreset()));

    result->addSeparator();

    result->addAction(new UpdatePresetAction          (dfgWidget, uiNode, result, onlyInstNodes && instNodeCount == 1 && dfgWidget->isEditable() && instExecCanUpdatePreset));
    result->addAction(new CreatePresetAction          (dfgWidget, uiNode, result, onlyInstNodes && instNodeCount == 1 && dfgWidget->isEditable() && instExecCanCreatePreset));
    result->addAction(new RevealPresetInExplorerAction(dfgWidget, uiNode, result, nodes.size() == 1 && instExec.isValid() && instExec.isPreset()));
    result->addAction(new ExportGraphAction           (dfgWidget, uiNode, result, onlyInstNodes && instNodeCount == 1));
    result->addSeparator();

    result->addAction(new ImplodeSelectedNodesAction(dfgWidget, result, dfgWidget->isEditable() && blockNodeCount == 0 && nodes.size() > 0));
    result->addAction(new ExplodeSelectedNodesAction(dfgWidget, result, dfgWidget->isEditable() && instNodeCount + userNodeCount > 0));

    result->addSeparator();

    result->addAction(new ReloadExtensionsAction(dfgWidget, uiNode, result, onlyInstNodes && exec.getSubExec(nodeName).getExtDepCount() > 0));

    result->addSeparator();

    result->addAction(new SetNodeCommentAction   (dfgWidget, uiNode, result, uiNode->comment().isEmpty(), dfgWidget->isEditable()));
    result->addAction(new RemoveNodeCommentAction(dfgWidget, uiNode, result, dfgWidget->isEditable() && !uiNode->comment().isEmpty()));

    result->setFocus( Qt::OtherFocusReason );

    return result;
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
  return NULL;
}

QMenu *DFGWidget::portContextMenuCallback(
  FabricUI::GraphView::Port* port,
  void* userData
  )
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if (!graph->controller())
    return NULL;

  if (graphWidget->isQuickZoomActive())  // [FE-7950]
    return NULL;

  FabricCore::DFGExec &exec = graphWidget->getDFGController()->getExec();

  bool editable = (graphWidget->isEditable() && graphWidget->getDFGController()->validPresetSplit());

  int numPortsIn  = 0;
  int numPortsOut = 0;
  int numPortsIO  = 0;
  for (unsigned int i=0;i<exec.getExecPortCount();i++)
  {
    FabricCore::DFGPortType type = exec.getExecPortType(i);
    if      (type == FabricCore::DFGPortType_In)    numPortsIn++;
    else if (type == FabricCore::DFGPortType_Out)   numPortsOut++;
    else if (type == FabricCore::DFGPortType_IO)    numPortsIO++;
  }

  QMenu *result = new QMenu( port->scene()->views()[0] );

  result->addAction( new EditPortAction     ( graphWidget, port, result, editable && port->allowEdits() ) );
  result->addAction( new DeletePortAction   ( graphWidget, port, result, editable && port->allowEdits() ) );
  result->addAction( new DuplicatePortAction( graphWidget, port, result, editable && port->allowEdits() ) );

  result->addSeparator();

  result->addAction( new MoveInputPortsToEndAction ( graphWidget, result, editable && exec.getExecPortCount() > 1 && numPortsIn  > 0 ) );
  result->addAction( new MoveOutputPortsToEndAction( graphWidget, result, editable && exec.getExecPortCount() > 1 && numPortsOut > 0 ) );

  // FE-8736 : if the current executable is the root
  // The path has the form '.node.port' or , remove the first '.'
  QString path = port->path().data();
  if(path.mid(0, 1) == ".")
    path = path.mid(1);

  FabricCore::DFGBinding binding = graphWidget->getUIController()->getBinding();
  path = QString::number(binding.getBindingID()) + "." + path;
 
  if(DFGPVToolMenu::canCreate(path))
  {
    result->addSeparator();
    QAction *action;
    foreach(action, DFGPVToolMenu::createActions( port->scene()->views()[0], path ))
      result->addAction(action);
  }

  result->setFocus( Qt::OtherFocusReason );

  return result;
}

QMenu *DFGWidget::pinContextMenuCallback(
  FabricUI::GraphView::Pin* pin,
  void* userData
  )
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
 
  // FE-8736 : if the current executable is the root
  // The path has the form '.node.port' or , remove the first '.'
  QString path = pin->path().data();
  if(path.mid(0, 1) == ".")
      path = path.mid(1);

  FabricCore::DFGBinding binding = graphWidget->getUIController()->getBinding();
  path = QString::number(binding.getBindingID()) + "." + path;
 
  QMenu *result = 0;
  if(DFGPVToolMenu::canCreate(path))
  {
    result = DFGPVToolMenu::createMenu( pin->scene()->views()[0], path );
    result->setFocus( Qt::OtherFocusReason );
  }

  return result;
}

QMenu *DFGWidget::fixedPortContextMenuCallback(
  FabricUI::GraphView::FixedPort *fixedPort,
  void *userData
  )
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;

  if (graphWidget->isQuickZoomActive())  // [FE-7950]
    return NULL;

  bool editable = (graphWidget->isEditable() && graphWidget->getDFGController()->validPresetSplit());

  QMenu *menu = new QMenu( fixedPort->scene()->views()[0] );

  QAction *dummyAction = new QAction( "Port is Locked", menu );
  dummyAction->setEnabled( editable && false );
  menu->addAction( dummyAction );

  menu->setFocus( Qt::OtherFocusReason );

  return menu;
}

QMenu *DFGWidget::connectionContextMenuCallback(
  FabricUI::GraphView::Connection *connection,
  void *userData
  )
{
  DFGWidget * dfgWidget = (DFGWidget*)userData;

  if (dfgWidget->isQuickZoomActive())  // [FE-7950]
    return NULL;  // [FE-7950]

  QMenu *result = new QMenu(connection->scene()->views()[0]);

  result->addAction(new ConnectionRemoveAction(dfgWidget, connection, result, dfgWidget->isEditable()));

  result->addSeparator();

  QMenu *selectMenu = result->addMenu(tr("Select"));
  selectMenu->addAction(new ConnectionSelectSourceAndTargetAction(dfgWidget, connection, selectMenu, true, true,  false));
  selectMenu->addAction(new ConnectionSelectSourceAndTargetAction(dfgWidget, connection, selectMenu, true, false, true));
  selectMenu->addAction(new ConnectionSelectSourceAndTargetAction(dfgWidget, connection, selectMenu, true, true,  true));

  result->addSeparator();

  QMenu *insertPresetMenu = result->addMenu(tr("Insert Preset"));
  insertPresetMenu->addAction(new ConnectionInsertPresetAction(dfgWidget, insertPresetMenu, connection, 
                                                               "Fabric.Compounds.Debug.Log.Report",
                                                               "value",
                                                               "value",
                                                               QCursor::pos(),
                                                               QKeySequence(),
                                                               "",
                                                               dfgWidget->isEditable()));
  insertPresetMenu->addAction(new ConnectionInsertPresetAction(dfgWidget, result, connection,
                                                               "Fabric.Compounds.Debug.Log.LabeledReport",
                                                               "value",
                                                               "value",
                                                               QCursor::pos(),
                                                               QKeySequence(),
                                                               "label",
                                                               dfgWidget->isEditable()));

  insertPresetMenu->addSeparator();

  insertPresetMenu->addAction(new ConnectionInsertPresetAction(dfgWidget, insertPresetMenu, connection,
                                                               "Fabric.Core.Data.Cache",
                                                               "value",
                                                               "value",
                                                               QCursor::pos(),
                                                               QKeySequence(),
                                                               "",
                                                               dfgWidget->isEditable()));

  insertPresetMenu->addSeparator();

  insertPresetMenu->addAction(new ConnectionInsertPresetAction(dfgWidget, insertPresetMenu, connection,
                                                               "Fabric.Compounds.Data.PassIn",
                                                               "value",
                                                               "value",
                                                               QCursor::pos(),
                                                               QKeySequence(),
                                                               "",
                                                               dfgWidget->isEditable()));
  insertPresetMenu->addAction(new ConnectionInsertPresetAction(dfgWidget, insertPresetMenu, connection,
                                                               "Fabric.Compounds.Data.PassIO",
                                                               "value",
                                                               "value",
                                                               QCursor::pos(),
                                                               QKeySequence(),
                                                               "",
                                                               dfgWidget->isEditable()));

  result->setFocus( Qt::OtherFocusReason );

  return result;
}

class CreatePredefinedPortAction : public AbstractAction
{
  DFGWidget* m_dfgWidget;
  DFGConfig::PredefinedPort m_preset;

public:
  CreatePredefinedPortAction(
    DFGWidget* dfgWidget,
    QObject* parent,
    const DFGConfig::PredefinedPort& preset
  ) : AbstractAction( parent )
    , m_dfgWidget( dfgWidget )
    , m_preset( preset )
  {
    this->setText( "Create " + m_preset.name + " Port" );
  }

protected:
  void onTriggered() FTL_OVERRIDE
  {
    m_dfgWidget->getUIController()->cmdAddPort(
      m_preset.portName,
      FabricCore::DFGPortType_In,
      m_preset.typeSpec,
      QString(),
      m_preset.extDep,
      m_preset.metaData
    );
  }
};

QMenu *DFGWidget::sidePanelContextMenuCallback(
  FabricUI::GraphView::SidePanel* panel,
  void* userData
  )
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if (graph->controller() == NULL)
    return NULL;

  if (graphWidget->isQuickZoomActive())  // [FE-7950]
    return NULL;

  FabricCore::DFGExec &exec = graphWidget->getDFGController()->getExec();

  bool editable = (graphWidget->isEditable() && graphWidget->getDFGController()->validPresetSplit());
  FabricUI::GraphView::PortType portType = panel->portType();
  
  int numPortsIn  = 0;
  int numPortsOut = 0;
  int numPortsIO  = 0;
  for (unsigned int i=0;i<exec.getExecPortCount();i++)
  {
    FabricCore::DFGPortType type = exec.getExecPortType(i);
    if      (type == FabricCore::DFGPortType_In)    numPortsIn++;
    else if (type == FabricCore::DFGPortType_Out)   numPortsOut++;
    else if (type == FabricCore::DFGPortType_IO)    numPortsIO++;
  }
  QMenu *result = new QMenu( panel->scene()->views()[0] );

  result->addAction( new CreatePortAction( graphWidget, portType, result, editable && !(portType != FabricUI::GraphView::PortType_Output && exec.isInstBlockExec()) ) );

  result->addSeparator();

  // [FE-8248] we only show the 'Timeline' menu for certain host applications (e.g. Canvas standalone).
  if ( graphWidget->isBindingHostAppStandalone() )
  {
    QMenu *timelinePortsMenu = result->addMenu(tr("Timeline Ports"));
    timelinePortsMenu->setDisabled( portType != FabricUI::GraphView::PortType_Output );
    {
      QString portname[4] = {"timeline", "timelineStart", "timelineEnd", "timelineFramerate"};
      bool canAddTimelinePort[4];
      bool canAddAllTimelinePorts = false;
      for (int i=0;i<4;i++)
      {
        canAddTimelinePort[i] = (   editable
                                  && portType == FabricUI::GraphView::PortType_Output
                                  && exec.getExecPath().getLength() == 0
                                  && graph->ports(portname[i].toUtf8().data()).size() == 0 );
        canAddAllTimelinePorts |= canAddTimelinePort[i];
        timelinePortsMenu->addAction( new CreateTimelinePortAction( graphWidget, timelinePortsMenu, i, canAddTimelinePort[i] ) );
      }
      timelinePortsMenu->addSeparator();
      timelinePortsMenu->addAction( new CreateAllTimelinePortsAction( graphWidget, timelinePortsMenu, true /* createOnlyMissingPorts */, canAddAllTimelinePorts ) );
    }
  }

  // Predefined Ports
  {
    QMenu* predefinedPortsMenu = result->addMenu( "Predefined Ports" );
    const std::vector<DFGConfig::PredefinedPort>& predefinedPorts = graphWidget->getConfig().predefinedPorts;
    predefinedPortsMenu->setDisabled( portType != FabricUI::GraphView::PortType_Output || predefinedPorts.empty() );
    if( predefinedPorts.empty() )
      predefinedPortsMenu->setToolTip( "No Predefined Ports" );

    for( std::vector<DFGConfig::PredefinedPort>::const_iterator it = predefinedPorts.begin(); it != predefinedPorts.end(); it++ )
      predefinedPortsMenu->addAction( new CreatePredefinedPortAction( graphWidget, predefinedPortsMenu, *it ) );

    result->addSeparator();
  }

  bool canDeleteAllPorts = (    editable
                            &&  exec.getExecPortCount() > 1
                            && (   (numPortsIn  > 0 && portType == FabricUI::GraphView::PortType_Output)
                                || (numPortsOut > 0 && portType == FabricUI::GraphView::PortType_Input) ) );
  result->addAction( new DeleteAllPortsAction( graphWidget, result, portType == FabricUI::GraphView::PortType_Output, portType == FabricUI::GraphView::PortType_Input, false, canDeleteAllPorts ) );

  result->addSeparator();

  bool canScroll = (    exec.getExecPortCount() > 1
                    && (   (numPortsIn  > 0 && portType == FabricUI::GraphView::PortType_Output)
                        || (numPortsOut > 0 && portType == FabricUI::GraphView::PortType_Input) ) );
  result->addAction( new SidePanelScrollUpAction  ( graphWidget, panel, result, canScroll ) );
  result->addAction( new SidePanelScrollDownAction( graphWidget, panel, result, canScroll ) );

  result->setFocus( Qt::OtherFocusReason );

  return result;
}

void DFGWidget::onGoUpPressed()
{
  std::string nodeName;
  if ( maybePopExec( nodeName ) )
  {
    getUIGraph()->clearSelection();
    if ( !nodeName.empty() )
      if ( GraphView::Node *uiNode = getUIGraph()->node( nodeName ) )
        uiNode->setSelected( true );
  }
}

void DFGWidget::tabSearch()
{
  if (m_isEditable)
  {
    if (getUIController()->validPresetSplit() && this->getGraphViewWidget()->hasFocus())
    {
      QPoint pos = getGraphViewWidget()->lastEventPos();
      m_tabSearchPos = pos;
      pos = getGraphViewWidget()->mapToGlobal(pos);
      tabSearchVariablesUpdate();
      getTabSearchWidget()->showForSearch(pos);
    }
  }
}

inline void MaybeSelectNode( GraphView::Graph* graph, const QString& node )
{
  if( !node.isEmpty() )
  {
    graph->clearSelection();
    if( GraphView::Node *uiNode = graph->node( node ) )
      uiNode->setSelected( true );
  }
}

QPointF DFGWidget::getTabSearchScenePos() const
{
  return this->getGraphViewWidget()->graph()->itemGroup()->mapFromScene( m_tabSearchPos );
}

void DFGWidget::onPresetAddedFromTabSearch( QString preset )
{
  QString addedPreset = this->getUIController()->cmdAddInstFromPreset(
    preset,
    getTabSearchScenePos()
  );
  MaybeSelectNode( this->getGraphViewWidget()->graph(), addedPreset );
}

void DFGWidget::onBackdropAddedFromTabSearch()
{
  QString addedBackdrop = this->getUIController()->cmdAddBackDrop(
    "backdrop",
    getTabSearchScenePos()
  );
  if (GraphView::Node *uiNode = m_uiGraph->node(addedBackdrop))
  {
    QRectF rect = getUIGraph()->selectedNodesRect();
    if (!rect.isEmpty())
    {
      rect.adjust(-16, -40, 16, 13);
      m_uiController->cmdResizeBackDropNode(uiNode->name_QS(), rect.topLeft(), rect.size());
    }
  }
  MaybeSelectNode(this->getGraphViewWidget()->graph(), addedBackdrop);
}

void DFGWidget::onNewBlockAddedFromTabSearch()
{
  QString addedBlock = this->getUIController()->cmdAddBlock(
    "block",
    getTabSearchScenePos()
  );
  MaybeSelectNode(this->getGraphViewWidget()->graph(), addedBlock);
}

void DFGWidget::onVariableCreationRequestedFromTabSearch()
{
  DFGNewVariableDialog dialog(
    this,
    this->getDFGController()->getClient(),
    this->getDFGController()->getBinding(),
    this->getDFGController()->getExecPath()
  );

  if( dialog.exec() != QDialog::Accepted )
    return;


  QString name = dialog.name();
  QString dataType = dialog.dataType();
  QString extension = dialog.extension();

  if( name.isEmpty() )
  {
    this->getDFGController()->log( "Warning: no variable created (empty name)." );
    return;
  }
  if( dataType.isEmpty() )
  {
    this->getDFGController()->log( "Warning: no variable created (empty type)." );
    return;
  }

  QString node = this->getDFGController()->cmdAddVar(
    name.toUtf8().constData(),
    dataType.toUtf8().constData(),
    extension.toUtf8().constData(),
    getTabSearchScenePos()
  );
  MaybeSelectNode( this->getGraphViewWidget()->graph(), node );
}

void DFGWidget::onVariableSetterAddedFromTabSearch( const std::string name )
{
  QString node = this->getUIController()->cmdAddSet( "set", QString::fromStdString( name ), getTabSearchScenePos() );
  MaybeSelectNode( this->getGraphViewWidget()->graph(), node );
}

void DFGWidget::onVariableGetterAddedFromTabSearch( const std::string name )
{
  QString node = this->getUIController()->cmdAddGet( "get", QString::fromStdString( name ), getTabSearchScenePos() );
  MaybeSelectNode( this->getGraphViewWidget()->graph(), node );
}

void DFGWidget::onFocusGivenFromTabSearch()
{
  this->getGraphViewWidget()->setFocus( Qt::OtherFocusReason );
}

void DFGWidget::tabSearchVariablesSetDirty()
{
  m_tabSearchVariablesDirty = true;
  if( m_tabSearchWidget->isVisible() )
    tabSearchVariablesUpdate();
}

void DFGWidget::tabSearchVariablesUpdate()
{
  if( !m_tabSearchVariablesDirty )
    return;

  m_tabSearchWidget->unregisterVariables();

  FabricCore::DFGBinding& binding = this->getUIController()->getBinding();
  QStringList variableNames = DFGBindingUtils::getVariableWordsFromBinding(
    binding,
    this->getUIController()->getExecPath()
  );
  for( QStringList::const_iterator it = variableNames.begin(); it != variableNames.end(); it++ )
  {
    try
    {
      const std::string varName = it->toUtf8().constData();
      const std::string varType =
        binding.getExec().getVarValue( varName.data() ).getTypeNameCStr();
      m_tabSearchWidget->registerVariable( varName, varType );
    }
    catch( const FabricCore::Exception& e )
    {
      std::cerr << e.getDesc_cstr() << std::endl;
    }
  }

  m_tabSearchVariablesDirty = false;

  m_tabSearchWidget->updateResults();
}

void DFGWidget::tabSearchBlockToggleChanged()
{
  FabricCore::DFGExec &exec = this->getUIController()->getExec();
  m_tabSearchWidget->toggleNewBlocks(
    exec.isValid()
    && this->isEditable()
    && exec.allowsBlocks()
  );
}

void DFGWidget::emitNodeInspectRequested(FabricUI::GraphView::Node *node)
{
  emit nodeInspectRequested(node);
  node->graph()->clearInspection();
  node->setInspected(true);
}

void DFGWidget::createNewBlockNode( QPoint const &globalPos )
{
  QString text = "block";

  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  bool isCTRL  = keyMod.testFlag( Qt::ControlModifier );
  if (isCTRL)
  {
    DFGGetStringDialog dialog(this, "New Block", text, m_dfgConfig, true); 
    if(dialog.exec() != QDialog::Accepted)
      return;

    text = dialog.text();
    if(text.length() == 0)
    { m_uiController->log("Warning: block not created (empty name).");
      return; }
  }

  QString blockName =
    m_uiController->cmdAddBlock(
      text,
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( blockName ) )
    uiNode->setSelected( true );
}

void DFGWidget::createNewCacheNode( QPoint const &globalPos )
{
  QString nodeName =
    m_uiController->cmdAddInstFromPreset(
      "Fabric.Core.Data.Cache",
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( nodeName ) )
    uiNode->setSelected( true );
}

void DFGWidget::createNewGraphNode( QPoint const &globalPos )
{
  QString text = "graph";
  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);
  if (isCTRL)
  {
    DFGGetStringDialog dialog(this, "New Empty Graph", text, m_dfgConfig, true);
    if(dialog.exec() != QDialog::Accepted)
      return;

    text = dialog.text();
    if(text.length() == 0)
    { m_uiController->log("Warning: graph not created (empty name).");
      return; }
  }

  QString nodeName =
    m_uiController->cmdAddInstWithEmptyGraph(
      text.toUtf8().constData(),
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( nodeName ) )
    uiNode->setSelected( true );
}

void DFGWidget::createNewNodeFromJSON( QPoint const &globalPos )
{
  QString lastPresetFolder = getSettings()->value("mainWindow/lastPresetFolder").toString();
  QFileInfo fileInfo(QFileDialog::getOpenFileName(this, "Import Graph", lastPresetFolder, "*.canvas"));
  if ( fileInfo.exists() )
  {
    fileInfo.dir().cdUp();
    getSettings()->setValue( "mainWindow/lastPresetFolder", fileInfo.dir().path() );
    createNewNodeFromJSON(fileInfo, m_uiGraphViewWidget->mapToGraph( globalPos ) );
  }
}

void DFGWidget::createNewNodeFromJSON( QFileInfo const &fileInfo, QPointF const &pos )
{
  QString nodeName = m_uiController->cmdAddInstFromJSON(
    fileInfo.baseName(), 
    fileInfo.filePath(), 
    pos 
    );

  QStringList selectedNodes;
  selectedNodes.append( nodeName );
  m_uiController->selectNodes( selectedNodes );
}

void DFGWidget::createNewFunctionNode( QPoint const &globalPos )
{
  QString text = "func";

  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);
  if (isCTRL)
  {
    DFGGetStringDialog dialog(this, "New Empty Function", text, m_dfgConfig, true);
    if(dialog.exec() != QDialog::Accepted)
      return;

    text = dialog.text();
    if(text.length() == 0)
    { m_uiController->log("Warning: function node not created (empty name).");
      return; }
  }

  static const FTL::CStrRef initialCode = FTL_STR("\
dfgEntry {\n\
  // result = a + b;\n\
}\n");
  QString nodeName =
    m_uiController->cmdAddInstWithEmptyFunc(
      text.toUtf8().constData(),
      QString::fromUtf8( initialCode.c_str() ),
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( nodeName ) )
    uiNode->setSelected( true );
}

void DFGWidget::createNewBackdropNode( QPoint const &globalPos)
{
  QString text = "backdrop";

  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);
  if (isCTRL)
  {
    DFGGetStringDialog dialog(this, "New Backdrop", text, m_dfgConfig, false);
    if(dialog.exec() != QDialog::Accepted)
      return;

    text = dialog.text();
    if(text.length() == 0)
    { m_uiController->log("Warning: no backdrop created (empty name).");
      return; }
  }

  QString nodeName =
    m_uiController->cmdAddBackDrop(
      text.toUtf8().constData(),
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  GraphView::Node *uiNode = m_uiGraph->node(nodeName);
  if (uiNode)
  {
    QRectF rect = getUIGraph()->selectedNodesRect();
    if (!rect.isEmpty())
    {
      rect.adjust(-16, -40, 16, 13);
      m_uiController->cmdResizeBackDropNode(uiNode->name_QS(), rect.topLeft(), rect.size());
    }
    m_uiGraph->clearSelection();
    uiNode->setSelected(true);
  }
}

void DFGWidget::createNewVariableNode( QPoint const &globalPos )
{
  DFGController *controller = getUIController();
  FabricCore::Client client = controller->getClient();
  FabricCore::DFGBinding &binding = controller->getBinding();
  FTL::CStrRef execPath = controller->getExecPath();

  DFGNewVariableDialog dialog( this, client, binding, execPath, true);
  if(dialog.exec() != QDialog::Accepted)
    return;

  QString name = dialog.name();
  QString dataType = dialog.dataType();
  QString extension = dialog.extension();

  if (name.isEmpty())
  { controller->log("Warning: no variable created (empty name).");
    return; }
  if (dataType.isEmpty())
  { controller->log("Warning: no variable created (empty type).");
    return; }

  QString nodeName =
    m_uiController->cmdAddVar(
      name,
      dataType,
      extension,
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( nodeName ) )
    uiNode->setSelected( true );
}

void DFGWidget::createNewVariableGetNode( QPoint const &globalPos )
{
  DFGController *controller = getUIController();
  FabricCore::Client client = controller->getClient();
  FabricCore::DFGBinding &binding = controller->getBinding();
  FTL::CStrRef execPath = controller->getExecPath();

  DFGPickVariableDialog dialog(this, client, binding, execPath, true);
  if(dialog.exec() != QDialog::Accepted)
    return;

  QString name = dialog.name();
  if(name.length() == 0)
    return;

  QString nodeName =
    m_uiController->cmdAddGet(
      "get",
      name.toUtf8().constData(),
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( nodeName ) )
    uiNode->setSelected( true );
}

void DFGWidget::createNewVariableSetNode( QPoint const &globalPos )
{
  DFGController *controller = getUIController();
  FabricCore::Client client = controller->getClient();
  FabricCore::DFGBinding &binding = controller->getBinding();
  FTL::CStrRef execPath = controller->getExecPath();

  DFGPickVariableDialog dialog(this, client, binding, execPath, true);
  if(dialog.exec() != QDialog::Accepted)
    return;

  QString name = dialog.name();
  if(name.length() == 0)
    return;

  QString nodeName =
    m_uiController->cmdAddSet(
      "set",
      name.toUtf8().constData(),
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( nodeName ) )
    uiNode->setSelected( true );
}

void DFGWidget::createPort( FabricUI::GraphView::PortType portType )
{
  FabricCore::Client &client = m_uiController->getClient();
  FabricCore::DFGExec &exec = m_uiController->getExec();
  bool showPortType = !exec.isInstBlockExec();

  DFGEditPortDialog dialog(
    this,
    client,
    showPortType,
    true, // canEditPortType
    m_dfgConfig,
    true // setAlphaNum
    );

  if ( showPortType )
  {
    if ( portType == FabricUI::GraphView::PortType_Output )
      dialog.setPortType("In");
    else
      dialog.setPortType("Out");
  }

  emit portEditDialogCreated(&dialog);

  if(dialog.exec() != QDialog::Accepted)
    return;

  std::string metaData;
  {
    FTL::JSONEnc<> metaDataEnc( metaData );
    FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );
    if(dialog.hidden())
      DFGAddMetaDataPair( metaDataObjectEnc, "uiHidden", "true" );
    if(dialog.opaque())
      DFGAddMetaDataPair( metaDataObjectEnc, "uiOpaque", "true" );

    if(dialog.persistValue())
      DFGAddMetaDataPair( metaDataObjectEnc, DFG_METADATA_UIPERSISTVALUE, "true" );

    if(dialog.hasSoftRange())
    {
      QString range = "(" + QString::number(dialog.softRangeMin()) + ", " + QString::number(dialog.softRangeMax()) + ")";
      DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", range.toUtf8().constData() );
    } else
      DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", "" );

    if(dialog.hasHardRange())
    {
      QString range = "(" + QString::number(dialog.hardRangeMin()) + ", " + QString::number(dialog.hardRangeMax()) + ")";
      DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", range.toUtf8().constData() );
    } else
      DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", "" );

    if(dialog.hasCombo())
    {
      QStringList combo = dialog.comboValues();
      QString flat = "(";
      for(int i=0;i<combo.length();i++)
      {
        if(i > 0)
          flat += ", ";
        flat += "\"" + combo[i] + "\"";
      }
      flat += ")";
      DFGAddMetaDataPair( metaDataObjectEnc, "uiCombo", flat.toUtf8().constData() );
    }

    emit portEditDialogInvoked(&dialog, &metaDataObjectEnc);
  }

  QString title = dialog.title();

  QString dataType = dialog.dataType();
  QString extDep = dialog.extension();

  if ( !title.isEmpty() )
  {
    FabricCore::DFGPortType portType = FabricCore::DFGPortType_In;
    if ( showPortType )
    {
      if ( dialog.portType() == "Out" )
        portType = FabricCore::DFGPortType_Out;
      else if ( dialog.portType() == "IO" )
        portType = FabricCore::DFGPortType_IO;
    }

    if ( metaData == "{}" )
      metaData = "";

    m_uiController->cmdAddPort(
      title,
      portType,
      dataType,
      QString(), // portToConnect
      extDep,
      QString::fromUtf8( metaData.c_str() )
      );
  }
}

void DFGWidget::deletePort( FabricUI::GraphView::Port *port )
{
  m_uiController->cmdRemovePort( QStringList( port->nameQString() ) );
}

void DFGWidget::deletePorts( bool deleteIn, bool deleteOut, bool deleteIO )
{
  FabricCore::DFGExec &exec = m_uiController->getExec();

  QStringList portNames;
  for (unsigned int i=0;i<exec.getExecPortCount();i++)
  {
    FabricCore::DFGPortType type = exec.getExecPortType(i);
    if (   (deleteIn  && type == FabricCore::DFGPortType_In)
        || (deleteOut && type == FabricCore::DFGPortType_Out)
        || (deleteIO  && type == FabricCore::DFGPortType_IO) )
      portNames.push_back( exec.getExecPortName(i) );
  }

  m_uiController->cmdRemovePort( portNames );
}

void DFGWidget::editPort( FTL::CStrRef execPortName, bool duplicatePort)
{
  try
  {
    QString execPortName_QS =
      QString::fromUtf8( execPortName.data(), execPortName.size() );

    FabricCore::Client &client = m_uiController->getClient();
    FabricCore::DFGExec &exec = m_uiController->getExec();

    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
    bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);

    DFGEditPortDialog dialog(
      this,
      client,
      false,
      true, //canEditPortType
      m_dfgConfig,
      true // setAlphaNum
      );

    dialog.setTitle( execPortName_QS );
    dialog.setDataType(exec.getExecPortTypeSpec(execPortName.c_str()));

    FTL::StrRef uiHidden = exec.getExecPortMetadata(execPortName.c_str(), "uiHidden");
    if( uiHidden == "true" )
      dialog.setHidden();

    FTL::StrRef uiOpaque = exec.getExecPortMetadata(execPortName.c_str(), "uiOpaque");
    if( uiOpaque == "true" )
      dialog.setOpaque();

    FTL::StrRef uiPersistValue = exec.getExecPortMetadata(execPortName.c_str(), DFG_METADATA_UIPERSISTVALUE);
    dialog.setPersistValue( uiPersistValue == "true" );

    bool expandMetadataSection = false; // [FE-6068]

    FTL::StrRef uiRange = exec.getExecPortMetadata(execPortName.c_str(), "uiRange");
    double softMinimum = 0.0;
    double softMaximum = 0.0;
    if(FabricUI::DecodeUIRange(uiRange, softMinimum, softMaximum))
    {
      dialog.setHasSoftRange(true);
      dialog.setSoftRangeMin(softMinimum);
      dialog.setSoftRangeMax(softMaximum);
      expandMetadataSection = true;
    }

    FTL::StrRef uiHardRange = exec.getExecPortMetadata(execPortName.c_str(), "uiHardRange");
    double hardMinimum = 0.0;
    double hardMaximum = 0.0;
    if(FabricUI::DecodeUIRange(uiHardRange, hardMinimum, hardMaximum))
    {
      dialog.setHasHardRange(true);
      dialog.setHardRangeMin(hardMinimum);
      dialog.setHardRangeMax(hardMaximum);
      expandMetadataSection = true;
    }

    FTL::StrRef uiCombo = exec.getExecPortMetadata(execPortName.c_str(), "uiCombo");
    std::string uiComboStr;
    if(uiCombo.size() > 0)
      uiComboStr = uiCombo.data();
    if(uiComboStr.size() > 0)
    {
      if(uiComboStr[0] == '(')
        uiComboStr = uiComboStr.substr(1);
      if(uiComboStr[uiComboStr.size()-1] == ')')
        uiComboStr = uiComboStr.substr(0, uiComboStr.size()-1);
      QStringList parts = QString(uiComboStr.c_str()).split(',');
      dialog.setHasCombo(true);
      dialog.setComboValues(parts);
      expandMetadataSection = true;
    }

    FTL::StrRef uiFileTypeFilter = exec.getExecPortMetadata(execPortName.c_str(), "uiFileTypeFilter");
    std::string uiFileTypeFilterStr;
    if(uiFileTypeFilter.size() > 0)
      uiFileTypeFilterStr = uiFileTypeFilter.data();
    if(uiFileTypeFilterStr.size() > 0)
    {
      dialog.setHasFileTypeFilter(true);
      dialog.setFileTypeFilter(uiFileTypeFilterStr.c_str());
      expandMetadataSection = true;
    }

    FTL::StrRef uiIsOpenFile = exec.getExecPortMetadata(execPortName.c_str(), "uiIsOpenFile");
    std::string uiIsOpenFileStr;
    if(uiIsOpenFile.size() > 0)
      uiIsOpenFileStr = uiIsOpenFile.data();
    if(uiIsOpenFileStr.size() > 0)
    {
      if(uiIsOpenFile == "true")
        dialog.setIsOpenFile(true);
      else
        dialog.setIsOpenFile(false);
    }

    dialog.setSectionCollapsed("Metadata", !expandMetadataSection);

    if (!duplicatePort || isCTRL)
    {
      emit portEditDialogCreated(&dialog);

      if(dialog.exec() != QDialog::Accepted)
        return;
    }

    emit portEditDialogInvoked(&dialog, NULL);

    QString newPortName = dialog.title();
    QString typeSpec = dialog.dataType();
    QString extDep = dialog.extension();

    std::string uiMetadata;
    {
      FTL::JSONEnc<> metaDataEnc( uiMetadata );
      FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );

      DFGAddMetaDataPair( metaDataObjectEnc, "uiHidden", dialog.hidden() ? "true" : "" );//"" will remove the metadata
      DFGAddMetaDataPair( metaDataObjectEnc, "uiOpaque", dialog.opaque() ? "true" : "" );//"" will remove the metadata
      DFGAddMetaDataPair( metaDataObjectEnc, DFG_METADATA_UIPERSISTVALUE, dialog.persistValue() ? "true" : "" );//"" will remove the metadata

      if(dialog.hasSoftRange())
      {
        QString range = "(" + QString::number(dialog.softRangeMin()) + ", " + QString::number(dialog.softRangeMax()) + ")";
        DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", range.toUtf8().constData() );
      } else
        DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", "" );//"" will remove the metadata

      if(dialog.hasHardRange())
      {
        QString range = "(" + QString::number(dialog.hardRangeMin()) + ", " + QString::number(dialog.hardRangeMax()) + ")";
        DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", range.toUtf8().constData() );
      } else
        DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", "" );//"" will remove the metadata

      if(dialog.hasCombo())
      {
        QStringList combo = dialog.comboValues();
        QString flat = "(";
        for(int i=0;i<combo.length();i++)
        {
          if(i > 0)
            flat += ", ";
          flat += "\"" + combo[i] + "\"";
        }
        flat += ")";
        DFGAddMetaDataPair( metaDataObjectEnc, "uiCombo", flat.toUtf8().constData() );
      } else
        DFGAddMetaDataPair( metaDataObjectEnc, "uiCombo", "" );//"" will remove the metadata

      if(dialog.hasFileTypeFilter())
      {
        QString fileTypeFilter = dialog.fileTypeFilter();
        DFGAddMetaDataPair( metaDataObjectEnc, "uiFileTypeFilter", fileTypeFilter.toUtf8().constData() );
        DFGAddMetaDataPair( metaDataObjectEnc, "uiIsOpenFile", dialog.isOpenFile() ? "true" : "false");
      } else {
        DFGAddMetaDataPair( metaDataObjectEnc, "uiFileTypeFilter", "" );//"" will remove the metadata
        DFGAddMetaDataPair( metaDataObjectEnc, "uiIsOpenFile", "" );//"" will remove the metadata
      }

      // FE-8542 : Don't add the meta-data if the port is duplicated.
      emit portEditDialogInvoked(&dialog, !duplicatePort ? &metaDataObjectEnc : NULL);
    }

    if ( FTL::StrRef( uiMetadata ) == FTL_STR("{}") )
      uiMetadata.clear();

    if (!duplicatePort)
    {
      m_uiController->cmdEditPort(
        execPortName_QS,
        newPortName,
        exec.getExecPortType( execPortName.c_str() ),
        typeSpec,
        extDep,
        QString::fromUtf8( uiMetadata.c_str() )
        );
    }
    else
    {
      m_uiController->cmdAddPort(
        newPortName,
        exec.getExecPortType( execPortName.c_str() ),
        typeSpec,
        QString(), // portToConnect
        extDep,
        QString::fromUtf8( uiMetadata.c_str() )
        );
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
}

void DFGWidget::movePortsToEnd( bool moveInputs )
{
  try
  {
    FabricCore::DFGBinding &binding = m_uiController->getBinding();
    QString execPath = m_uiController->getExecPath_QS();
    FabricCore::DFGExec &exec = m_uiController->getExec();

    // create an index list with the inputs first
    QList<int> inputsFirst;
    QList<int> outputsFirst;
    for(unsigned i=0;i<exec.getExecPortCount();i++)
    {
      if(exec.getExecPortType(i) == FEC_DFGPortType_IO)
      {
        inputsFirst.append(i);
        outputsFirst.append(i);
      }
    }
    for(unsigned i=0;i<exec.getExecPortCount();i++)
    {
      if(exec.getExecPortType(i) == FEC_DFGPortType_In)
        inputsFirst.append(i);
      else if(exec.getExecPortType(i) == FEC_DFGPortType_Out)
        outputsFirst.append(i);
    }
    for(unsigned i=0;i<exec.getExecPortCount();i++)
    {
      if(exec.getExecPortType(i) == FEC_DFGPortType_In)
        outputsFirst.append(i);
      else if(exec.getExecPortType(i) == FEC_DFGPortType_Out)
        inputsFirst.append(i);
    }

    QList<int> indices;

    if (moveInputs)
      indices = outputsFirst;
    else
      indices = inputsFirst;

    m_uiController->cmdReorderPorts(
      binding,
      execPath,
      exec,
      QString(), // itemPath
      indices
      );
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
}

void DFGWidget::implodeSelectedNodes( bool displayDialog )
{
  QString text = "imploded_nodes";

  if (displayDialog)
  {
    DFGGetStringDialog dialog(this, "Implode Nodes", text, m_dfgConfig, true);
    if(dialog.exec() != QDialog::Accepted)
      return;

    text = dialog.text();
    if(text.length() == 0)
      return;
  }

  const std::vector<GraphView::Node*> & nodes =
    m_uiController->graph()->selectedNodes();

  QStringList nodeNames;
  nodeNames.reserve( nodes.size() );
  for ( size_t i = 0; i < nodes.size(); ++i )
  {
    FTL::CStrRef nodeName = nodes[i]->name();
    nodeNames.push_back( QString::fromUtf8( nodeName.data(), nodeName.size() ) );
  }

  QString newNodeName =
    m_uiController->cmdImplodeNodes(
      nodeNames,
      text.toUtf8().constData()
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( newNodeName ) )
    uiNode->setSelected( true );
}

void DFGWidget::openPresetDoc( const char *nodeName )
{
  FabricCore::DFGExec &exec = m_uiController->getExec();
  QString uiDocUrl = exec.getNodeMetadata( nodeName, "uiDocUrl" );
  if(uiDocUrl.length() == 0 && exec.getNodeType(nodeName) == FabricCore::DFGNodeType_Inst)
  {
    FabricCore::DFGExec subExec = exec.getSubExec( nodeName );
    uiDocUrl = subExec.getMetadata( "uiDocUrl" );
  }
  Util::DocUrl::openUrl(uiDocUrl);
}

void DFGWidget::splitFromPreset( const char *nodeName )
{
  FabricCore::DFGExec &exec = m_uiController->getExec();
  FabricCore::DFGExec subExec = exec.getSubExec( nodeName );
  subExec.maybeSplitFromPreset();
}

static void DFGPresentPresetSaveFailedDialog( QWidget *parent, QString pathname )
{
  QMessageBox msg(
    QMessageBox::Warning,
    "Fabric Warning", 
      "The file "
    + pathname
    + " cannot be opened for writing.",
    QMessageBox::NoButton,
    parent
    );
  msg.addButton( "OK", QMessageBox::AcceptRole );
  msg.exec();
}

static void DFGPresetFabricCoreExceptionDialog( QWidget *parent, FabricCore::Exception &e )
{
  QMessageBox msg(
    QMessageBox::Warning,
    "Fabric Warning", 
    e.getDesc_cstr(),
    QMessageBox::NoButton,
    parent
    );
  msg.addButton("OK", QMessageBox::AcceptRole);
  msg.exec();
}

void DFGWidget::createPreset( const char *nodeName )
{
  FabricCore::DFGExec &exec = m_uiController->getExec();
  if ( exec.getNodeType( nodeName ) != FabricCore::DFGNodeType_Inst )
    return;
  FabricCore::DFGExec subExec = exec.getSubExec( nodeName );
  if ( subExec.isPreset() )
    return;

  try
  {
    FTL::CStrRef defaultPresetName = nodeName;

    FabricCore::DFGHost &host = m_uiController->getHost();

    DFGSavePresetDialog dialog(
      this,
      m_uiController.get(),
      true,
      defaultPresetName.c_str()
      );
    while ( true )
    {
      if(dialog.exec() != QDialog::Accepted)
        return;

      QString presetName = dialog.name();
      // QString version = dialog.version();
      QString presetDirPath = dialog.location();

      if(presetName.length() == 0 || presetDirPath.length() == 0)
      {
        QMessageBox msg(QMessageBox::Warning, "Fabric Warning",
          "You need to provide a valid name and pick a valid location!",
          QMessageBox::NoButton,
          this);
        msg.addButton("OK", QMessageBox::AcceptRole);
        msg.exec();
        continue;
      }

      if(presetDirPath.startsWith("Fabric.") || presetDirPath.startsWith("Variables.") ||
        presetDirPath == "Fabric" || presetDirPath == "Variables")
      {
        QMessageBox msg(QMessageBox::Warning, "Fabric Warning",
          "You can't save a preset into a factory path (below Fabric).",
          QMessageBox::NoButton,
          this);
        msg.addButton("OK", QMessageBox::AcceptRole);
        msg.exec();
        continue;
      }

      QString pathname =
        DFGUICmdHandler::NewPresetPathname(
          host,
          presetDirPath,
          presetName
          );

      if ( pathname.isEmpty() )
      {
        QMessageBox msg(
          QMessageBox::Warning,
          "Fabric Error",
            "The preset directory '"
          + presetDirPath
          + "' does not have an associated path and so the preset cannot be saved.",
          QMessageBox::NoButton,
          this);
        msg.addButton( "OK", QMessageBox::AcceptRole );
        msg.exec();
        continue;
      }

      QString presetPath;
      if ( !presetDirPath.isEmpty() )
      {
        presetPath = presetDirPath;
        presetPath += '.';
      }
      presetPath += presetName;

      if ( host.isPresetDir( presetPath.toUtf8().constData() ) )
      {
        QMessageBox msg(
          QMessageBox::Warning,
          "Fabric Error",
            "The path '"
          + presetPath
          + "' is a preset directory and cannot be overwritten.",
          QMessageBox::NoButton,
          this);
        msg.addButton( "OK", QMessageBox::AcceptRole );
        msg.exec();
        continue;
      }

      bool updatePresetGuid = false;
      if ( host.isPresetFile( presetPath.toUtf8().constData() ) )
      {
        FabricCore::String origPresetGUID = subExec.getOrigPresetGUID();
        FTL::StrRef origPresetGUIDStr(
          origPresetGUID.getCStr(), origPresetGUID.getSize()
          );

        FabricCore::String existingPresetGUID =
          host.getPresetFileGUID( presetPath.toUtf8().constData() );
        FTL::StrRef existingPresetGUIDStr(
          existingPresetGUID.getCStr(), existingPresetGUID.getSize()
          );

        if ( origPresetGUIDStr == existingPresetGUIDStr )
        {
          QMessageBox msg(
            QMessageBox::Warning,
            "Fabric Warning",
              "Are you sure you want to overwrite the file '"
            + pathname
            + "'?",
            QMessageBox::NoButton,
            this
            );
          msg.addButton( "Cancel", QMessageBox::RejectRole );
          QAbstractButton *okButton =
            msg.addButton( "Yes", QMessageBox::AcceptRole );
          msg.exec();
          QAbstractButton *button = msg.clickedButton();
          if ( button != okButton )
            continue;
          updatePresetGuid = true;
        }
        else
        {
          QMessageBox msg(
            QMessageBox::Warning,
            "Fabric Warning",
              "The preset '"
            + presetPath
            + "' already exists."
            + "\nReuse its preset GUID or create a new one?"
            + "\nNOTE: This will overwrite the file '"
            + pathname
            + "'!",
            QMessageBox::NoButton,
            this
            );
          msg.addButton( "Cancel", QMessageBox::RejectRole );
          QAbstractButton *reuseButton =
            msg.addButton( "Reuse GUID", QMessageBox::YesRole );
          QAbstractButton *createButton =
            msg.addButton( "Create New GUID", QMessageBox::NoRole );
          msg.exec();
          QAbstractButton *button = msg.clickedButton();
          if ( button != reuseButton && button != createButton )
            continue;
          updatePresetGuid = button == reuseButton;
        }
      }

      pathname =
        m_uiController->cmdCreatePreset(
          nodeName,
          presetDirPath.toUtf8().constData(),
          presetName.toUtf8().constData(),
          updatePresetGuid
          );
      if ( pathname.isEmpty() )
      {
        DFGPresentPresetSaveFailedDialog( this, pathname );
        continue;
      }

      emit newPresetSaved( pathname );
      // update the preset search paths within the controller
      m_uiController->onVariablesChanged();
      break;
    }
  }
  catch ( FabricCore::Exception e )
  {
    DFGPresetFabricCoreExceptionDialog( this, e );
  }
}

void DFGWidget::updateOrigPreset( const char *nodeName )
{
  QMessageBox msgBox(
    QMessageBox::Warning,
    "Fabric Engine",
    "",
    QMessageBox::NoButton,
    this);

  msgBox.setText( "Are you sure you want to update the original preset?" );
  msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton( QMessageBox::Yes );
  if (msgBox.exec() == QMessageBox::No)
      return;

  FabricCore::DFGExec &exec = m_uiController->getExec();
  if ( exec.getNodeType( nodeName ) != FabricCore::DFGNodeType_Inst )
    return;
  FabricCore::DFGExec subExec = exec.getSubExec( nodeName );
  FabricCore::String origPresetGUID = subExec.getOrigPresetGUID();
  if ( origPresetGUID.getSize() == 0 )
    return;
  FabricCore::DFGHost host = exec.getHost();
  FabricCore::String presetPath =
    host.getPresetPathForPresetGUID( origPresetGUID.getCStr() );
  if ( presetPath.getSize() == 0 )
    return;
  FTL::StrRef presetPathStr( presetPath.getCStr(), presetPath.getSize() );
  std::pair<FTL::StrRef, FTL::StrRef> presetPathSplit =
    presetPathStr.rsplit( '.' );
  FTL::StrRef presetDirStr = presetPathSplit.first;
  FTL::StrRef presetNameStr = presetPathSplit.second;

  try
  {
    QString pathname =
      m_uiController->cmdCreatePreset(
        nodeName,
        QString::fromUtf8( presetDirStr.data(), presetDirStr.size() ),
        QString::fromUtf8( presetNameStr.data(), presetNameStr.size() ),
        true // updateOrigPreset
        );
    if ( pathname.isEmpty() )
      DFGPresentPresetSaveFailedDialog( this, pathname );
  }
  catch ( FabricCore::Exception e )
  {
    DFGPresetFabricCoreExceptionDialog( this, e );
  }
}

void DFGWidget::exportGraph( const char *nodeName )
{
  FabricCore::DFGExec &exec = m_uiController->getExec();
  if ( exec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst )
    return;

  FabricCore::DFGExec subExec = exec.getSubExec( nodeName );

  // Create a new binding from a copy of the subExec
  FabricCore::DFGHost &host = m_uiController->getHost();
  FabricCore::DFGBinding newBinding = host.createBindingFromJSON(
    subExec.exportJSON().getCString() );
  FabricCore::DFGExec newBindingExec = newBinding.getExec();

  QString title;
  if ( newBindingExec.isPreset() )
  {
    title = newBindingExec.getTitle();
    if ( title.toLower().endsWith(".canvas") )
      title = title.left( title.length() - 7 );
  }
  else title = nodeName;

  FTL::CStrRef uiNodeColor = exec.getNodeMetadata( nodeName, "uiNodeColor" );
  if(!uiNodeColor.empty())
    newBindingExec.setMetadata("uiNodeColor", uiNodeColor.c_str(), true, true);
  FTL::CStrRef uiHeaderColor = exec.getNodeMetadata( nodeName, "uiHeaderColor" );
  if(!uiHeaderColor.empty())
    newBindingExec.setMetadata("uiHeaderColor", uiHeaderColor.c_str(), true, true);
  FTL::CStrRef uiTextColor = exec.getNodeMetadata( nodeName, "uiTextColor" );
  if(!uiTextColor.empty())
    newBindingExec.setMetadata("uiTextColor", uiTextColor.c_str(), true, true);
  FTL::CStrRef uiTooltip = exec.getNodeMetadata( nodeName, "uiTooltip" );
  if(!uiTooltip.empty())
    newBindingExec.setMetadata("uiTooltip", uiTooltip.c_str(), true, true);
  FTL::CStrRef uiDocUrl = exec.getNodeMetadata( nodeName, "uiDocUrl" );
  if(!uiDocUrl.empty())
    newBindingExec.setMetadata("uiDocUrl", uiDocUrl.c_str(), true, true);
  FTL::CStrRef uiAlwaysShowDaisyChainPorts = exec.getNodeMetadata( nodeName, "uiAlwaysShowDaisyChainPorts" );
  if(!uiAlwaysShowDaisyChainPorts.empty())
    newBindingExec.setMetadata("uiAlwaysShowDaisyChainPorts", uiAlwaysShowDaisyChainPorts.c_str(), true, true);

  QString lastPresetFolder = title;
  if(getSettings())
  {
    lastPresetFolder = getSettings()->value("DFGWidget/lastPresetFolder").toString();
    lastPresetFolder += "/" + title;
  }

  QString filter = "Canvas Preset (*.canvas)";
  QString filePath = QFileDialog::getSaveFileName(this, "Export Graph", lastPresetFolder, filter, &filter);
  if(filePath.length() == 0)
    return;
  if(filePath.toLower().endsWith(".canvas.canvas"))
    filePath = filePath.left(filePath.length() - 7);

  if(getSettings())
  {
    QDir dir(filePath);
    dir.cdUp();
    getSettings()->setValue( "DFGWidget/lastPresetFolder", dir.path() );
  }

  std::string filePathStr = filePath.toUtf8().constData();

  try
  {
    // copy all defaults
    for(unsigned int i=0;i<newBindingExec.getExecPortCount();i++)
    {
      char const *newBindingExecPortName = newBindingExec.getExecPortName(i);

      std::string pinPath = nodeName;
      pinPath += ".";
      pinPath += newBindingExecPortName;

      FTL::StrRef rType = exec.getNodePortResolvedType(pinPath.c_str());
      if(rType.size() == 0 || rType.find('$') != rType.end())
        continue;
      if(rType.size() == 0 || rType.find('$') != rType.end())
        rType = subExec.getExecPortResolvedType(i);
      if(rType.size() == 0 || rType.find('$') != rType.end())
        rType = subExec.getExecPortTypeSpec(i);
      if(rType.size() == 0 || rType.find('$') != rType.end())
        continue;

      FabricCore::RTVal val =
        exec.getInstPortResolvedDefaultValue(pinPath.c_str(), rType.data());

      if( val.isValid() ) {
        newBindingExec.setPortDefaultValue( newBindingExecPortName, val, false );

        // Reflect port values as binding args
        newBinding.setArgValue( newBindingExecPortName, val, false );
        newBindingExec.setExecPortMetadata( newBindingExecPortName, DFG_METADATA_UIPERSISTVALUE, "true" );
      }
    }

    std::string json = newBinding.exportJSON().getCString();
    FILE * file = fopen(filePathStr.c_str(), "wb");
    if(file)
    {
      fwrite(json.c_str(), json.length(), 1, file);
      fclose(file);
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
}

void DFGWidget::explodeNode( const char *nodeName, bool clearCurrentSelection, bool selectNewNodes )
{
  QList<QString> newNodeNames =
    m_uiController->cmdExplodeNode( QString::fromUtf8( nodeName ) );

  if (clearCurrentSelection)
    m_uiGraph->clearSelection();

  if (selectNewNodes)
    for (int i=0;i<newNodeNames.size();i++)
    {
      if ( GraphView::Node *uiNode = m_uiGraph->node( newNodeNames.at(i) ) )
        uiNode->setSelected( true );
    }
}

void DFGWidget::keyPressEvent(QKeyEvent * event)
{
  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  if ( event->key() == Qt::Key_Z
    && !keyMod.testFlag(Qt::ShiftModifier)
    && !keyMod.testFlag(Qt::AltModifier)
    && !keyMod.testFlag(Qt::ControlModifier)
    && !event->isAutoRepeat())
  {
    event->accept();

    if (   getUIGraph()->nodes().size() == 0
        || m_uiController->allNodesAreVisible() )
    {
      m_uiGraphZoomBeforeQuickZoom = -1;
      getGraphViewWidget()->setUiGraphZoomBeforeQuickZoom( m_uiGraphZoomBeforeQuickZoom );
    }
    else
    {
      FTL::CStrRef uiGraphZoomStr =
        m_uiController->getExec().getMetadata( "uiGraphZoom" );
      FTL::JSONStrWithLoc jsonStrWithLoc( uiGraphZoomStr );
      FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
        FTL::JSONValue::Decode( jsonStrWithLoc )
        );
      if ( jsonValue )
      {
        FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
        m_uiGraphZoomBeforeQuickZoom = jsonObject->getFloat64( FTL_STR("value") );
        getGraphViewWidget()->setUiGraphZoomBeforeQuickZoom( m_uiGraphZoomBeforeQuickZoom );
      }
      m_uiController->frameAllNodes();
    }

    return;
  }
  Parent::keyPressEvent(event);  
}

template<typename Ty>
Ty clamp( Ty const &v, Ty const &lo, Ty const &hi )
{
  return std::min( std::max( v, lo ), hi );
}

void DFGWidget::keyReleaseEvent(QKeyEvent * event)
{
  if ( event->key() == Qt::Key_Z
    && !event->isAutoRepeat()
    && isQuickZoomActive() )
  {
    event->accept();

    GraphView::GraphViewWidget *graphViewWidget = getGraphViewWidget();
    if (m_uiGraphZoomBeforeQuickZoom > 0)
    {
      QPoint globalPos = QCursor::pos();
      QRect graphViewWidgetRect = graphViewWidget->geometry();
      QPoint graphViewWidgetPos = graphViewWidget->mapFromGlobal( globalPos );
      // [pz 20160724] Constraint point to widget geometry
      graphViewWidgetPos = QPoint(
        clamp(
          graphViewWidgetPos.x(),
          graphViewWidgetRect.left(),
          graphViewWidgetRect.right()
          ),
        clamp(
          graphViewWidgetPos.y(),
          graphViewWidgetRect.top(),
          graphViewWidgetRect.bottom()
          )
        );
      QPointF scenePos = graphViewWidget->mapToScene( graphViewWidgetPos );
      GraphView::Graph *graph = graphViewWidget->graph();
      GraphView::MainPanel *mainPanel = graph->mainPanel();
      QPointF mainPanelPos = mainPanel->mapFromScene( scenePos );
      mainPanel->performZoom( m_uiGraphZoomBeforeQuickZoom, mainPanelPos );

      // center Canvas pan.
      QPointF center = graphViewWidgetRect.center() - graphViewWidgetPos;
      QPointF newPan = mainPanel->canvasPan() + center;
      mainPanel->setCanvasPan(newPan);
    }

    m_uiGraphZoomBeforeQuickZoom = 0;
    graphViewWidget->setUiGraphZoomBeforeQuickZoom( m_uiGraphZoomBeforeQuickZoom );
    graphViewWidget->update();

    return;
  }
  Parent::keyReleaseEvent(event);  
}

void DFGWidget::onBubbleEditRequested(FabricUI::GraphView::Node * node)
{
  if ( !m_isEditable )
    return;

  QString text;
  bool visible = true;
  bool collapsed = false;

  GraphView::NodeBubble * bubble = node->bubble();
  if ( bubble )
  {
    text = bubble->text();
    visible = bubble->isVisible();
    collapsed = bubble->isCollapsed();
    bubble->show();
    bubble->expand();
  }

  DFGGetTextDialog dialog(this, text);
  if ( dialog.exec() == QDialog::Accepted )
  {
    if ( !text.isEmpty() || !dialog.text().isEmpty() )
    {
      m_uiController->cmdSetNodeComment(
        QString::fromUtf8( node->name().c_str() ),
        dialog.text()
        );
      m_uiController->setNodeCommentExpanded(
        QString::fromUtf8( node->name().c_str() ),
        true
        );
    }
    else
    {
      bubble->hide();
    }
  }
  else if ( bubble )
  {
    if ( collapsed )
      bubble->collapse();
    bubble->setVisible( visible );
  }
}

void DFGWidget::onRevealPresetInExplorer(char const *nodeName)
{
  FabricCore::DFGExec &exec = m_uiController->getExec();
  QString presetPath = DFGBindingUtils::getPresetPathFromNode(exec, QString(nodeName));
  if(!presetPath.isEmpty())
    emit revealPresetInExplorer(presetPath);
}

void DFGWidget::onToggleBlockCompilations()
{
  getDFGController()->setBlockCompilations(!this->getUIController()->getHost().areCompsBlocked());
  getUIGraph()->setCompsBlockedOverlayVisibility(this->getUIController()->getHost().areCompsBlocked());
}

void DFGWidget::onToggleDimConnections()
{
  m_uiGraph->config().dimConnectionLines = !m_uiGraph->config().dimConnectionLines;
  std::vector<GraphView::Connection *> connections = m_uiGraph->connections();
  for(size_t i=0;i<connections.size();i++)
    connections[i]->update();
  if(getSettings()) getSettings()->setValue( "DFGWidget/dimConnectionLines", m_uiGraph->config().dimConnectionLines );
}

void DFGWidget::onToggleConnectionShowTooltip()
{
  m_uiGraph->config().connectionShowTooltip = !m_uiGraph->config().connectionShowTooltip;
  std::vector<GraphView::Connection *> connections = m_uiGraph->connections();
  for(size_t i=0;i<connections.size();i++)
    connections[i]->enableToolTip(m_uiGraph->config().connectionShowTooltip);
  if(getSettings()) getSettings()->setValue( "DFGWidget/connectionShowTooltip", m_uiGraph->config().connectionShowTooltip );
}

void DFGWidget::onToggleHighlightConnectionTargets()
{
  m_uiGraph->config().highlightConnectionTargets = !m_uiGraph->config().highlightConnectionTargets;
  if(getSettings()) getSettings()->setValue( "DFGWidget/highlightConnectionTargets", m_uiGraph->config().highlightConnectionTargets );
}

void DFGWidget::onToggleConnectionDrawAsCurves()
{
  m_uiGraph->config().connectionDrawAsCurves = !m_uiGraph->config().connectionDrawAsCurves;
  std::vector<GraphView::Connection *> connections = m_uiGraph->connections();
  for(size_t i=0;i<connections.size();i++)
    connections[i]->dependencyMoved();
  if(getSettings()) getSettings()->setValue( "DFGWidget/connectionDrawAsCurves", m_uiGraph->config().connectionDrawAsCurves );
}

void DFGWidget::onTogglePortsCentered()
{
  m_uiGraph->config().portsCentered = !m_uiGraph->config().portsCentered;
  m_uiGraph->sidePanel(GraphView::PortType_Input)->updateItemGroupScroll();  
  m_uiGraph->sidePanel(GraphView::PortType_Output)->updateItemGroupScroll();  
  if(getSettings()) getSettings()->setValue( "DFGWidget/portsCentered", m_uiGraph->config().portsCentered );
}

void DFGWidget::onToggleDrawGrid()
{
  m_uiGraph->config().mainPanelDrawGrid = !m_uiGraph->config().mainPanelDrawGrid;
  m_uiGraph->update();
  if(getSettings()) getSettings()->setValue( "DFGWidget/mainPanelDrawGrid", m_uiGraph->config().mainPanelDrawGrid );
}

void DFGWidget::onToggleSnapToGrid()
{
  m_uiGraph->config().mainPanelGridSnap = !m_uiGraph->config().mainPanelGridSnap;
  if(getSettings()) getSettings()->setValue( "DFGWidget/mainPanelGridSnap", m_uiGraph->config().mainPanelGridSnap );
}

void DFGWidget::onToggleSnapToNode()
{
  m_uiGraph->config().mainPanelNodeSnap = !m_uiGraph->config().mainPanelNodeSnap;
  if(getSettings()) getSettings()->setValue( "DFGWidget/mainPanelNodeSnap", m_uiGraph->config().mainPanelNodeSnap );
}

void DFGWidget::onToggleSnapToPort()
{
  m_uiGraph->config().mainPanelPortSnap = !m_uiGraph->config().mainPanelPortSnap;
  if(getSettings()) getSettings()->setValue( "DFGWidget/mainPanelPortSnap", m_uiGraph->config().mainPanelPortSnap );
}

bool DFGWidget::maybeEditNode(
  FabricUI::GraphView::Node * node
  )
{
  if ( node->isBackDropNode() )
    return false;

  try
  {
    FTL::CStrRef nodeName = node->name();

    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.isExecBlock( nodeName.c_str() ) )
      return maybePushExec( nodeName, exec, nodeName );
    else
    {
      switch ( exec.getNodeType( nodeName.c_str() ) )
      {
        case FabricCore::DFGNodeType_Inst:
        {
          FabricCore::DFGExec subExec = exec.getSubExec( nodeName.c_str() );
          return maybePushExec( nodeName, subExec );
        }

        default: break;
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

bool DFGWidget::maybeEditInstBlock(
  FabricUI::GraphView::InstBlock *instBlock
  )
{
  try
  {
    FabricUI::GraphView::Node *node = instBlock->node();
    FTL::CStrRef instName = node->name();
    FTL::CStrRef blockName = instBlock->name();

    FabricCore::DFGExec &exec = m_uiController->getExec();
    FabricCore::DFGExec subExec =
      exec.getInstBlockExec( instName.c_str(), blockName.c_str() );
    return maybePushExec( node->name(), subExec );
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

bool DFGWidget::checkForUnsaved()
{
  if(m_klEditor->isVisible() && m_klEditor->hasUnsavedChanges())
  {
    QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
      "There are unsaved changes\n\nIf you leave the function editor they will be lost.",
      QMessageBox::NoButton,
      this);

    msg.addButton("Save Now", QMessageBox::AcceptRole);
    msg.addButton("OK", QMessageBox::NoRole);
    msg.addButton("Cancel", QMessageBox::RejectRole);

    msg.exec();

    QString clicked = msg.clickedButton()->text();
    if(clicked == "Cancel")
      return false;
    else if(clicked == "Save Now")
    {
      m_klEditor->save();
      if(m_klEditor->hasUnsavedChanges())
        return false;
    }
  }

  return true;
}

bool DFGWidget::maybePushExec(
  FTL::StrRef nodeName,
  FabricCore::DFGExec &exec,
  FTL::StrRef execBlockName
  )
{
  try
  {
    if ( !checkForUnsaved() )
      return false;

    m_priorExecStack.push_back(
      PriorExecStackEntry(
        m_uiController->getExec(),
        std::string( nodeName )
        )
      );
    FabricCore::String execPath = exec.getExecPath();
    m_uiController->setExec(
      FTL::StrRef( execPath.getCStr(), execPath.getSize() ),
      exec,
      execBlockName
      );
  }
  catch ( FabricCore::Exception e )
  {
    printf( "Exception: %s\n", e.getDesc_cstr() );
    return false;
  }
  return true;  
}

bool DFGWidget::maybePopExec( std::string &nodeName )
{
  try
  {
    if ( m_priorExecStack.empty() || !checkForUnsaved() )
      return false;

    FabricCore::DFGExec exec = m_priorExecStack.back().first;
    nodeName = m_priorExecStack.back().second;
    m_priorExecStack.pop_back();
    FabricCore::String execPath = exec.getExecPath();
    m_uiController->setExec(
      FTL::StrRef( execPath.getCStr(), execPath.getSize() ),
      exec
      );
  }
  catch ( FabricCore::Exception e )
  {
    printf( "Exception: %s\n", e.getDesc_cstr() );
    return false;
  }
  return true;  
}

void DFGWidget::onEditSelectedNode()
{
  std::vector<GraphView::Node*> const &nodes = m_uiController->graph()->selectedNodes();
  if ( nodes.size() == 1 )
    onNodeEditRequested( nodes[0] );
}

void DFGWidget::onEditSelectedNodeProperties()
{
  try
  {
    FabricUI::DFG::DFGController *controller = getUIController();
    if ( controller )
    {
      std::vector<GraphView::Node *> nodes = getUIGraph()->selectedNodes();
      if (nodes.size() != 1)
      {
        if (nodes.size() == 0)  controller->log("cannot open node editor: no node selected.");
        else                    controller->log("cannot open node editor: more than one node selected.");
        return;
      }
      GraphView::Node *node = nodes[0];

      FabricCore::DFGExec &exec = controller->getExec();
      std::string oldNodeName = node->name();

      if ( exec.isExecBlock( oldNodeName.c_str() ) )
      {
        DFG::DFGBlockPropertiesDialog dialog(
          this,
          controller,
          oldNodeName.c_str(),
          getConfig(),
          true
          );
        if ( dialog.exec() )
        {
          std::string nodeMetadata;
          {
            FTL::JSONEnc<> metaDataEnc( nodeMetadata );
            FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );

            DFGAddMetaDataPair_Color(
              metaDataObjectEnc,
              "uiNodeColor",
              dialog.getNodeColor()
              );

            DFGAddMetaDataPair_Color(
              metaDataObjectEnc,
              "uiTextColor",
              dialog.getTextColor()
              );

            // [Julien] FE-5246
            // Add or remove the geader colo node metadata
            QColor headerColor;
            if ( dialog.getHeaderColor( headerColor ) )
              DFGAddMetaDataPair_Color(
                metaDataObjectEnc,
                "uiHeaderColor",
                headerColor
                );
            else
              DFGAddMetaDataPair(
                metaDataObjectEnc,
                "uiHeaderColor",
                FTL::StrRef()
                );
          }

          FTL::CStrRef nodeName = node->name();
          controller->cmdEditNode(
            QString::fromUtf8( nodeName.data(), nodeName.size() ),
            dialog.getScriptName(),
            QString::fromUtf8( nodeMetadata.data(), nodeMetadata.size() ),
            QString()
            );  // undoable.
        }
      }
      else
      {
        FabricCore::DFGNodeType nodeType = exec.getNodeType( oldNodeName.c_str() );
        if (   nodeType == FabricCore::DFGNodeType_Var
            || nodeType == FabricCore::DFGNodeType_Get
            || nodeType == FabricCore::DFGNodeType_Set)
        {
          controller->log("the node editor is not available for variable nodes.");
          return;
        }

        bool isEditable = !exec.editWouldSplitFromPreset();
        if (!node->isBackDropNode())
          isEditable = !exec.getSubExec(oldNodeName.c_str()).editWouldSplitFromPreset();

        DFG::DFGNodePropertiesDialog dialog(
          this, 
          controller,
          oldNodeName.c_str(),
          getConfig(),
          true /* setAlphaNum */,
          isEditable,
          node->isBackDropNode()
          );

        if ( dialog.exec() )
        {
          std::string nodeMetadata;
          {
            FTL::JSONEnc<> metaDataEnc( nodeMetadata );
            FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );

            if ( nodeType == FabricCore::DFGNodeType_User )
              DFGAddMetaDataPair(
                metaDataObjectEnc,
                "uiTitle",
                dialog.getText().toUtf8().constData()
                );

            if ( nodeType != FabricCore::DFGNodeType_Inst )
            {
              DFGAddMetaDataPair(
                metaDataObjectEnc,
                "uiTooltip",
                dialog.getToolTip().toUtf8().constData()
                );

              DFGAddMetaDataPair(
                metaDataObjectEnc,
                "uiDocUrl",
                dialog.getDocUrl().toUtf8().constData()
                );

              DFGAddMetaDataPair_Color(
                metaDataObjectEnc,
                "uiNodeColor",
                dialog.getNodeColor()
                );

              DFGAddMetaDataPair_Color(
                metaDataObjectEnc,
                "uiTextColor",
                dialog.getTextColor()
                );

              // [Julien] FE-5246
              // Add or remove the geader colo node metadata
              QColor headerColor;
              if ( dialog.getHeaderColor( headerColor ) )
                DFGAddMetaDataPair_Color(
                  metaDataObjectEnc,
                  "uiHeaderColor",
                  headerColor
                  );
              else
                DFGAddMetaDataPair(
                  metaDataObjectEnc,
                  "uiHeaderColor",
                  FTL::StrRef()
                  );
            }
          }

          std::string execMetadata;
          if ( nodeType == FabricCore::DFGNodeType_Inst )
          {
            FabricCore::DFGExec subExec = exec.getSubExec( oldNodeName.c_str() );
            if ( !subExec.editWouldSplitFromPreset() )
            {
              FTL::JSONEnc<> metaDataEnc( execMetadata );
              FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );

              DFGAddMetaDataPair(
                metaDataObjectEnc,
                "uiTooltip",
                dialog.getToolTip().toUtf8().constData()
                );

              DFGAddMetaDataPair(
                metaDataObjectEnc,
                "uiDocUrl",
                dialog.getDocUrl().toUtf8().constData()
                );

              DFGAddMetaDataPair_Color(
                metaDataObjectEnc,
                "uiNodeColor",
                dialog.getNodeColor()
                );

              DFGAddMetaDataPair_Color(
                metaDataObjectEnc,
                "uiTextColor",
                dialog.getTextColor()
                );

              // [Julien] FE-5246
              // Add or remove the geader colo node metadata
              QColor headerColor;  
              if ( dialog.getHeaderColor( headerColor ) )
                DFGAddMetaDataPair_Color(
                  metaDataObjectEnc,
                  "uiHeaderColor",
                  headerColor
                  );
              else
                DFGAddMetaDataPair(
                  metaDataObjectEnc,
                  "uiHeaderColor",
                  FTL::StrRef()
                  );
            }
          }

          FTL::CStrRef nodeName = node->name();
          controller->cmdEditNode(
            QString::fromUtf8( nodeName.data(), nodeName.size() ),
            dialog.getScriptName(),
            QString::fromUtf8( nodeMetadata.data(), nodeMetadata.size() ),
            QString::fromUtf8( execMetadata.data(), execMetadata.size() )
            );  // undoable.

          // [Julien] FE-5246
          // Force update the header/nody node color
          onExecChanged();
        }
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
}

QSettings * DFGWidget::getSettings()
{
  if( g_settings == NULL )
    g_settings = new QSettings();
  return g_settings;
}

void DFGWidget::setSettings(QSettings * settings)
{
  g_settings = settings;
}

void DFGWidget::onExecPathOrTitleChanged()
{
  m_uiHeader->refresh();
}

void DFGWidget::refreshExtDeps( FTL::CStrRef extDeps )
{
  m_uiHeader->refreshExtDeps( extDeps );
}

void DFGWidget::populateMenuBar(QMenuBar *menuBar, bool addFileMenu, bool addEditMenu, bool addViewMenu, bool addDCCMenu, bool addHelpMenu)
{
  // File menu.
  // [Julien] FE-5244 : Add Save Graph action to the Canvas widget for DCC Integrations
  if (addFileMenu)
  {
    QMenu *fileMenu = NULL;
    fileMenu = menuBar->addMenu(tr("&File"));

    // emit the prefix menu entry requests
    emit additionalMenuActionsRequested("File", fileMenu, true);

    // add separators if required
    if (fileMenu->actions().count() > 0)
      fileMenu->addSeparator();

    connect( fileMenu, SIGNAL( aboutToShow() ), this, SIGNAL( fileMenuAboutToShow() ) );

    // emit the suffix menu entry requests
    emit additionalMenuActionsRequested("File", fileMenu, false);
  }

  // Edit menu.
  if (addEditMenu)
  {
    QMenu *editMenu = menuBar->addMenu(tr("&Edit"));

    // emit the prefix menu entry requests
    emit additionalMenuActionsRequested("Edit", editMenu, true);

    // add separators if required
    if(editMenu->actions().count() > 0)
      editMenu->addSeparator();

    // populate.
    QAction * selectAllNodesAction = new SelectAllNodesAction(this, menuBar);
    editMenu->addAction(selectAllNodesAction);

    QAction * deselectAllNodesAction = new DeselectAllNodesAction(this, menuBar);
    editMenu->addAction(deselectAllNodesAction);

    QAction * cutNodesAction = new CutNodesAction(this, menuBar);
    editMenu->addAction(cutNodesAction);

    QAction * copyNodesAction = new CopyNodesAction(this, menuBar);
    editMenu->addAction(copyNodesAction);

    QAction * pasteNodesAction = new PasteNodesAction(this, menuBar);
    editMenu->addAction(pasteNodesAction);

    // emit the prefix menu entry requests
    emit additionalMenuActionsRequested("Edit", editMenu, false);
  }

  // View menu.
  if (addViewMenu)
  {
    QMenu *viewMenu = menuBar->addMenu(tr("&View"));

    // emit the prefix menu entry requests
    emit additionalMenuActionsRequested("View", viewMenu, true);

    // add separators if required
    if(viewMenu->actions().count() > 0)
      viewMenu->addSeparator();

    // block graph compilations.
    QAction * blockCompilationsAction = new BlockCompilationsAction(this, menuBar);
    blockCompilationsAction->setCheckable(true);
    blockCompilationsAction->setChecked(false);
    blockCompilationsAction->setShortcutContext(Qt::WindowShortcut);
    viewMenu->addAction(blockCompilationsAction);
    viewMenu->addSeparator();

    // view -> graph view submenu
    QMenu *graphViewMenu = viewMenu->addMenu(tr("Graph View"));

    QAction * dimLinesAction = graphViewMenu->addAction("Dim Connections");
    dimLinesAction->setCheckable(true);
    dimLinesAction->setChecked(m_uiGraph->config().dimConnectionLines);
    QObject::connect(dimLinesAction, SIGNAL(triggered()), this, SLOT(onToggleDimConnections()));

    QAction * connectionShowTooltipAction = graphViewMenu->addAction("Show Connection Tooltips");
    connectionShowTooltipAction->setCheckable(true);
    connectionShowTooltipAction->setChecked(m_uiGraph->config().connectionShowTooltip);
    QObject::connect(connectionShowTooltipAction, SIGNAL(triggered()), this, SLOT(onToggleConnectionShowTooltip()));

    QAction * highlightConnectionTargetsAction = graphViewMenu->addAction("Highlight Connection Targets");
    highlightConnectionTargetsAction->setCheckable(true);
    highlightConnectionTargetsAction->setChecked(m_uiGraph->config().highlightConnectionTargets);
    QObject::connect(highlightConnectionTargetsAction, SIGNAL(triggered()), this, SLOT(onToggleHighlightConnectionTargets()));

    QAction * connectionDrawAsCurvesAction = graphViewMenu->addAction("Display Connections as Curves");
    connectionDrawAsCurvesAction->setCheckable(true);
    connectionDrawAsCurvesAction->setChecked(m_uiGraph->config().connectionDrawAsCurves);
    QObject::connect(connectionDrawAsCurvesAction, SIGNAL(triggered()), this, SLOT(onToggleConnectionDrawAsCurves()));

    QAction * portsCenteredAction = graphViewMenu->addAction("Side Ports Centered");
    portsCenteredAction->setCheckable(true);
    portsCenteredAction->setChecked(m_uiGraph->config().portsCentered);
    QObject::connect(portsCenteredAction, SIGNAL(triggered()), this, SLOT(onTogglePortsCentered()));

    graphViewMenu->addSeparator();

    QAction * displayGrid = graphViewMenu->addAction("Display Grid");
    displayGrid->setCheckable(true);
    displayGrid->setChecked(m_uiGraph->config().mainPanelDrawGrid);
    QObject::connect(displayGrid, SIGNAL(triggered()), this, SLOT(onToggleDrawGrid()));

    graphViewMenu->addSeparator();

    QAction * snapToGrid = graphViewMenu->addAction("Snap to Grid");
    snapToGrid->setCheckable(true);
    snapToGrid->setChecked(m_uiGraph->config().mainPanelGridSnap);
    QObject::connect(snapToGrid, SIGNAL(triggered()), this, SLOT(onToggleSnapToGrid()));

    QAction * snapToNode = graphViewMenu->addAction("Snap to Node Borders and Centers");
    snapToNode->setCheckable(true);
    snapToNode->setChecked(m_uiGraph->config().mainPanelNodeSnap);
    QObject::connect(snapToNode, SIGNAL(triggered()), this, SLOT(onToggleSnapToNode()));

    QAction * snapToPort = graphViewMenu->addAction("Snap to Ports");
    snapToPort->setCheckable(true);
    snapToPort->setChecked(m_uiGraph->config().mainPanelPortSnap);
    QObject::connect(snapToPort, SIGNAL(triggered()), this, SLOT(onToggleSnapToPort()));

    graphViewMenu->addSeparator();
    
    QAction * toggleLecyTabSearchAction = graphViewMenu->addAction("Use Legacy TabSearch");
    toggleLecyTabSearchAction->setCheckable( true );
    QObject::connect(
      toggleLecyTabSearchAction, SIGNAL( triggered( bool ) ),
      this, SLOT( onToggleLegacyTabSearch( bool ) )
    );
    toggleLecyTabSearchAction->setChecked( this->isUsingLegacyTabSearch() );

    // emit the suffix menu entry requests
    emit additionalMenuActionsRequested("View", viewMenu, false);
  }

  // DCC menu [Fe-6242].
  QMenu *dccMenu = 0;
  if (addDCCMenu)
  {
    dccMenu = menuBar->addMenu(tr("&DCC"));

    // emit the prefix menu entry requests
    emit additionalMenuActionsRequested("DCC", dccMenu, true);

    // add separators if required
    if(dccMenu->actions().count() > 0)
      dccMenu->addSeparator();

    // emit the suffix menu entry requests
    emit additionalMenuActionsRequested("View", dccMenu, false);
  }

  // Help menu.
  if (addHelpMenu)
  {
    QMenu *helpMenu = menuBar->addMenu(tr("&Help"));

    // populate.
    helpMenu->addAction(new OpenUrlAction(menuBar, "Canvas User Guide",         "http://docs.fabric-engine.com/FabricEngine/latest/HTML/CanvasUserGuide/index.html#canvasuserguide"));
    helpMenu->addAction(new OpenUrlAction(menuBar, "Canvas Keyboard Shortcuts", "http://docs.fabric-engine.com/FabricEngine/latest/HTML/CanvasUserGuide/shortcuts.html"));
    helpMenu->addAction(new OpenUrlAction(menuBar, "KL Programming Guide",      "http://docs.fabric-engine.com/FabricEngine/latest/HTML/KLProgrammingGuide/index.html#klpg"));
    helpMenu->addSeparator();
    helpMenu->addAction(new AboutFabricAction(this, menuBar));
  }
}

void DFGWidget::onExecChanged()
{
  if ( m_router )
  {
    m_uiController->setRouter( 0 );
    delete m_router;
    m_router = 0;
  }

  FabricCore::DFGExec &exec = m_uiController->getExec();
  if ( exec.isValid() )
  {
    m_uiGraph = new GraphView::Graph( NULL, m_dfgConfig.graphConfig );
    m_uiGraph->setController(m_uiController.get());
    m_uiController->setGraph(m_uiGraph);

    QObject::connect(
      m_uiGraph, SIGNAL(bubbleEditRequested(FabricUI::GraphView::Node*)), 
      this, SLOT(onBubbleEditRequested(FabricUI::GraphView::Node*))
    );

    // FE-6926  : Shift + double-clicking in an empty space "Goes up"
    QObject::connect(
      m_uiGraph, SIGNAL(goUpPressed()),
      this, SLOT(onGoUpPressed())
    );
    
    onExecSplitChanged();

    // [Julien] FE-5264
    // [Mootz] FE-8556
    // Before initializing the graph, sets the dimConnectionLines, portsCentered, etc. properties
    if(getSettings()) 
    {
#define MAYBE_READ_CONFIG_VALUE( name ) m_uiGraph->config().name = getSettings()->value( "DFGWidget/" #name, m_uiGraph->config().name ).toBool();

      MAYBE_READ_CONFIG_VALUE( dimConnectionLines )
      MAYBE_READ_CONFIG_VALUE( connectionShowTooltip )
      MAYBE_READ_CONFIG_VALUE( highlightConnectionTargets )
      MAYBE_READ_CONFIG_VALUE( connectionDrawAsCurves )
      MAYBE_READ_CONFIG_VALUE( portsCentered )
      MAYBE_READ_CONFIG_VALUE( mainPanelDrawGrid )
      MAYBE_READ_CONFIG_VALUE( mainPanelGridSnap )
      MAYBE_READ_CONFIG_VALUE( mainPanelNodeSnap )
      MAYBE_READ_CONFIG_VALUE( mainPanelPortSnap )
    }
    m_uiGraph->initialize();

    m_router =
      static_cast<DFGNotificationRouter *>( m_uiController->createRouter() );
    m_uiController->setRouter(m_router);
  
    m_uiGraph->setGraphContextMenuCallback( &graphContextMenuCallback, this );
    m_uiGraph->setNodeContextMenuCallback( &nodeContextMenuCallback, this );
    m_uiGraph->setPortContextMenuCallback( &portContextMenuCallback, this );
    m_uiGraph->setPinContextMenuCallback( &pinContextMenuCallback, this );
    m_uiGraph->setFixedPortContextMenuCallback( &fixedPortContextMenuCallback, this );
    m_uiGraph->setConnectionContextMenuCallback( &connectionContextMenuCallback, this );
    m_uiGraph->setSidePanelContextMenuCallback( &sidePanelContextMenuCallback, this );
    m_uiGraph->setCompsBlockedOverlayVisibility(m_uiController->getHost().areCompsBlocked());

    if ( !m_uiController->getExecBlockName().empty() )
    {
      m_uiGraphViewWidget->hide();
      m_errorsWidget->focusNone();
    }
    else if(exec.getType() == FabricCore::DFGExecType_Graph)
    {
      m_uiGraphViewWidget->show();
      m_uiGraphViewWidget->setFocus();
      m_errorsWidget->focusBinding();
    }
    else if(exec.getType() == FabricCore::DFGExecType_Func)
    {
      m_uiGraphViewWidget->hide();
      m_errorsWidget->focusExec();
    }

    QString filePath = getenv("FABRIC_DIR");
    filePath += "/Resources/PoweredByFabric_black.png";
    m_uiGraph->setupBackgroundOverlay(QPointF(1, -70), filePath);

    emit onGraphSet(m_uiGraph);
  }
  else
  {
    m_uiGraph = NULL;
    m_uiController->setGraph(NULL);
    m_isEditable = false;
  }

  m_uiGraphViewWidget->setGraph(m_uiGraph);

  if ( m_uiGraph )
  {
    try
    {
      m_router->onGraphSet();
    }
    catch(FabricCore::Exception e)
    {
      printf( "%s\n", e.getDesc_cstr() );
    }

    // FE-4277
    emit onGraphSet(m_uiGraph);
  }

  this->tabSearchBlockToggleChanged();

  m_uiController->updateNodeErrors();

  emit execChanged();
}

void DFGWidget::reloadStyles()
{
  QString styleSheet = LoadFabricStyleSheet( "FabricUI.qss" );
  if ( !styleSheet.isEmpty() )
    setStyleSheet( styleSheet );
}

void DFGWidget::onExecSelected(
  FTL::CStrRef execPath,
  int line,
  int column
  )
{
  FabricCore::DFGBinding binding = m_uiController->getBinding();
  FabricCore::DFGExec rootExec = binding.getExec();
  FabricCore::DFGExec exec = rootExec.getSubExec( execPath.c_str() );
  maybePushExec( FTL::StrRef(), exec );
}

void DFGWidget::onNodeSelected(
  FTL::CStrRef execPath,
  FTL::CStrRef nodeName,
  int line,
  int column
  )
{
  FabricCore::DFGBinding binding = m_uiController->getBinding();
  FabricCore::DFGExec rootExec = binding.getExec();
  FabricCore::DFGExec exec = rootExec.getSubExec( execPath.c_str() );
  if ( maybePushExec( FTL::StrRef(), exec ) )
  {
    QApplication::processEvents(); // Let graph view resize etc.
    m_uiController->focusNode( nodeName );
  }
}

void DFGWidget::onExecSplitChanged()
{
  FabricCore::DFGExec &exec = m_uiController->getExec();
  if ( exec.isValid() )
  {
    m_isEditable = !exec.editWouldSplitFromPreset();
    FabricCore::DFGBinding &binding = m_uiController->getBinding();
    FTL::StrRef bindingEditable = binding.getMetadata( "editable" );
    if ( bindingEditable == "false" )
      m_isEditable = false;
    if ( m_uiGraph )
      m_uiGraph->setEditable( m_isEditable );
  }
  this->tabSearchBlockToggleChanged();
}

void DFGWidget::replaceBinding(
  FabricCore::DFGBinding &binding
  )
{
  m_priorExecStack.clear();
  FabricCore::DFGExec exec = binding.getExec();
  m_uiController->setBindingExec( binding, FTL::StrRef(), exec );
}

void DFGWidget::onNodeEditRequested(FabricUI::GraphView::Node *node)
{
  try
  {
    FTL::CStrRef nodeName = node->name();

    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.isExecBlock( nodeName.c_str() ) )
      maybeEditNode( node );
    else
    {
      switch ( exec.getNodeType( nodeName.c_str() ) )
      {
        case FabricCore::DFGNodeType_Inst:
        {
          if ( node->isHighlighted() )
          {
            maybeEditNode( node );
            return;
          }

          unsigned instBlockCount = node->instBlockCount();
          for ( unsigned instBlockIndex = 0;
            instBlockIndex < instBlockCount; ++instBlockIndex )
          {
            FabricUI::GraphView::InstBlock *instBlock =
              node->instBlockAtIndex( instBlockIndex );
            if ( instBlock->isHighlighted() )
            {
              maybeEditInstBlock( instBlock );
              return;
            }
          }
        }
        break;

        default: break;
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
}

void DFGWidget::onReloadStyles()
{
  qDebug() << "Reloading Fabric stylesheets";
  reloadStyles();
  emit stylesReloaded();
}

DFGWidgetProxyStyle::DFGWidgetProxyStyle( QStyle* style )
  : QProxyStyle( style )
{
}

void DFGWidgetProxyStyle::drawControl(
  ControlElement element,
  const QStyleOption * option,
  QPainter * painter,
  const QWidget * widget
  ) const
{
  if ( element == QStyle::CE_MenuItem )
  {
    QStyleOptionMenuItem const &opt =
      *static_cast<QStyleOptionMenuItem const *>( option );
    if ( opt.menuItemType == QStyleOptionMenuItem::Normal
      && !( opt.state & QStyle::State_Enabled ) )
    {
      QStyleOptionMenuItem optCopy( opt );
      optCopy.state = opt.state | QStyle::State_Enabled;
      QProxyStyle::drawControl( element, &optCopy, painter, widget );
      return;
    }
  }

  QProxyStyle::drawControl( element, option, painter, widget );
}
