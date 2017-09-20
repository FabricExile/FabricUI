// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>

#include "DFGCombinedWidget.h"
#include <FabricUI/Style/FabricStyle.h>
#include <FabricUI/DFG/Dialogs/DFGNodePropertiesDialog.h>
#include <FabricUI/DFG/DFGVEEditorOwner.h>

using namespace FabricUI::DFG;

DFGCombinedWidget::DFGCombinedWidget(QWidget *parent)
  :QSplitter(parent)
{
  m_manager = NULL;
  m_treeWidget = NULL;
  m_dfgWidget = NULL;
  m_valueEditor = NULL;
  m_hSplitter = NULL;
  m_dfgLogWidget = NULL;
  m_setGraph = NULL;

  setStyle(new FabricUI::Style::FabricStyle());
  setOrientation(Qt::Vertical);
}

void DFGCombinedWidget::initMenu() {

  m_menuBar = new QMenuBar(this);
  m_menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addWidget(m_menuBar);

  // populate the menu bar
  QObject::connect(
    m_dfgWidget, 
    SIGNAL(additionalMenuActionsRequested(QString, QMenu*, bool)), 
    this, SLOT(onAdditionalMenuActionsRequested(QString, QMenu *, bool))
    );

  m_dfgWidget->populateMenuBar(m_menuBar, false /* addFileMenu */, true /* addEditMenu */, true /* addViewMenu */, true /* addDCCMenu */, true /* addHelpMenu */);

  // [FE-5340]
  setCollapsible(indexOf(m_menuBar), false);
  m_menuBar->setMaximumHeight(m_menuBar->sizeHint().height());
}

void DFGCombinedWidget::initTreeView() {

  // [Julien] FE-5252
  // preset library
  // Because of a lack of performances, we don't expose the search tool of the PresetTreeWidget
  m_treeWidget = new DFG::PresetTreeWidget(
    m_dfgWidget->getDFGController(),
    m_config,
    true,
    false,
    false,
    false,
    false,
    true);

  if(m_dfgWidget->isEditable())
  {
    // FE-8381 : Removed variables from the PresetTreeWidget
    //QObject::connect(m_dfgWidget->getUIController(), SIGNAL(varsChanged()), m_treeWidget, SLOT(setModelDirty()) );
    QObject::connect(m_dfgWidget, SIGNAL(newPresetSaved(QString)), m_treeWidget, SLOT(refresh()));
    QObject::connect(m_dfgWidget, SIGNAL(revealPresetInExplorer(QString)), m_treeWidget, SLOT(onExpandToAndSelectItem(QString)) );
}
}

void DFGCombinedWidget::initValueEditor() {
  m_valueEditor = new DFG::DFGVEEditorOwner( m_dfgWidget );
  QObject::connect( m_valueEditor, SIGNAL( log( const char * ) ), this, SLOT( log ( const char * ) ) );
}

void DFGCombinedWidget::initDFG() {

  m_dfgWidget->getUIController()->setLogFunc(DFGLogWidget::log);
  m_dfgLogWidget = new DFGLogWidget( m_config );
  if(m_dfgWidget->isEditable())
  {
    QObject::connect(
      m_dfgWidget, SIGNAL(portEditDialogCreated(FabricUI::DFG::DFGBaseDialog*)),
      this, SLOT(onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog*))
      );
    QObject::connect(
      m_dfgWidget, SIGNAL(portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog*, FTL::JSONObjectEnc<>*)),
      this, SLOT(onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog*, FTL::JSONObjectEnc<>*))
      );
    QObject::connect(
      m_dfgWidget, SIGNAL(nodeInspectRequested(FabricUI::GraphView::Node*)),
      this, SLOT(onNodeInspectRequested(FabricUI::GraphView::Node*))
      );
    QObject::connect(
      m_dfgWidget, SIGNAL(revealPresetInExplorer(QString)),
      this, SLOT(onRevealPresetInExplorer(QString))
      );

    QObject::connect(m_dfgWidget, SIGNAL(onGraphSet(FabricUI::GraphView::Graph*)), 
      this, SLOT(onGraphSet(FabricUI::GraphView::Graph*)));
  }
}

void DFGCombinedWidget::initGL() {
}

void DFGCombinedWidget::initDocks() {
  m_hSplitter->addWidget(m_treeWidget);
  m_hSplitter->addWidget(m_dfgWidget);
  m_hSplitter->addWidget(m_valueEditor->getWidget());
  addWidget(m_hSplitter);
  addWidget(m_dfgLogWidget);
}

void DFGCombinedWidget::init(
  FabricCore::Client &client,
  FabricServices::ASTWrapper::KLASTManager * manager,
  FabricCore::DFGHost &host,
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec,
  DFGUICmdHandler *cmdHandler,
  bool overTakeBindingNotifications,
  DFGConfig config
  )
{
  if(m_dfgWidget)
    return;

  try
  {
    m_mainDFGBinding = binding;
    m_config = config;
    m_client = client;
    m_manager = manager;

    m_dfgWidget = new DFG::DFGWidget(
      this,
      m_client,
      host,
      m_mainDFGBinding,
      execPath,
      exec,
      m_manager,
      cmdHandler,
      m_config,
      overTakeBindingNotifications);

    m_hSplitter = new QSplitter(this);
    m_hSplitter->setOrientation(Qt::Horizontal);
    m_hSplitter->setContentsMargins(0, 0, 0, 0);
    
    initDFG();
    initTreeView();
    initValueEditor();
    initGL();
    initMenu();
    initDocks();
    onGraphSet(m_dfgWidget->getUIGraph());
    m_valueEditor->initConnections();
    setContentsMargins(0, 0, 0, 0);
  }
  catch(FabricCore::Exception e)
  {
    log(e.getDesc_cstr());
    return;
  }

  resize(1000, 500);
  QList<int> s = m_hSplitter->sizes();
  s[0] = 0;
  s[1] = 1000;
  s[2] = 0;
  m_hSplitter->setSizes(s);

  s = sizes();
  s[0] = 10;
  s[1] = 500;
  s[2] = 0;
  setSizes(s);
}

DFGCombinedWidget::~DFGCombinedWidget()
{
  delete m_valueEditor;
}

QWidget* FabricUI::DFG::DFGCombinedWidget::getDfgValueEditor()
{
  return m_valueEditor->getWidget();
}

void DFGCombinedWidget::onGraphSet(FabricUI::GraphView::Graph * graph)
{
  if(graph != m_setGraph)
  {
    QObject::connect(graph, SIGNAL(nodeEditRequested(FabricUI::GraphView::Node*)),
      this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node*)));

    QObject::connect(graph, SIGNAL(nodeInspectRequested(FabricUI::GraphView::Node*)),
      this, SLOT(onNodeInspectRequested(FabricUI::GraphView::Node*)));
    
    m_setGraph = graph;
  }
}

void DFGCombinedWidget::onNodeInspectRequested(FabricUI::GraphView::Node * node)
{
  if ( node->isBackDropNode() )
    return;

  QList<int> s = m_hSplitter->sizes();
  if(s[2] == 0)
  {
    s[2] = (int)(float(s[1]) * 0.25f);
    s[1] -= s[2];
    m_hSplitter->setSizes(s);
  }

  node->graph()->clearInspection();
  node->setInspected(true);
}

void DFGCombinedWidget::onRevealPresetInExplorer(QString nodeName)
{
  if ( nodeName.isEmpty() )
    return;

  QList<int> s = m_hSplitter->sizes();
  if(s[0] == 0)
  {
    s[0] = (int)(float(s[1]) * 0.25f);
    s[1] -= s[0];
    m_hSplitter->setSizes(s);
  }
}

void DFGCombinedWidget::onNodeEditRequested(
  FabricUI::GraphView::Node *node
  )
{
  m_dfgWidget->onNodeEditRequested( node );
}

void DFGCombinedWidget::setLogFunc(DFGController::LogFunc func)
{
  DFGLogWidget::setLogFunc(func);
}

void DFGCombinedWidget::onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog)
{
  emit portEditDialogCreated(dialog);
}

void DFGCombinedWidget::onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData)
{
  emit portEditDialogInvoked(dialog, additionalMetaData);
}

void DFGCombinedWidget::log(const char * message) const
{
  if(m_dfgWidget)
    m_dfgWidget->getUIController()->log(message);
  printf("%s\n", message);
}

void DFGCombinedWidget::onAdditionalMenuActionsRequested(QString name, QMenu * menu, bool prefix)
{
  // [Julien] FE-5244 : Add Save Graph action to the Canvas widget for DCC Integrations
  // Don't construct the edit menu if called from DCC
  // The loading/saving of graphs is DCC specialized and called with commands
  if(name == "File")
  {
    if(prefix)
    {
      // QAction * loadGraphAction = menu->addAction("Load Graph...");
      // loadGraphAction->setShortcut(QKeySequence::Open);
      // QAction * saveGraphAsAction = menu->addAction("Save Graph As...");
      // saveGraphAsAction->setShortcut(QKeySequence::SaveAs);
      // QObject::connect(loadGraphAction, SIGNAL(triggered()), this, SLOT(onLoadGraph()));
      // QObject::connect(saveGraphAsAction, SIGNAL(triggered()), this, SLOT(onSaveGraphAs()));
    }
  }
  else if(name == "Edit")
  {
    if(prefix)
    {
      QAction *undoAction = menu->addAction("Undo");
      undoAction->setShortcut( QKeySequence::Undo );
      // [Julien] When using shortcut in Qt, set the flag WidgetWithChildrenShortcut so the shortcut is specific to the widget
      undoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
      menu->addAction( undoAction );
      QAction *redoAction = menu->addAction("Redo");
      redoAction->setShortcut( QKeySequence::Redo );
      // [Julien] When using shortcut in Qt, set the flag WidgetWithChildrenShortcut so the shortcut is specific to the widget
      redoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
      menu->addAction( redoAction );

      QObject::connect(undoAction, SIGNAL(triggered()), this, SLOT(onUndo()));
      QObject::connect(redoAction, SIGNAL(triggered()), this, SLOT(onRedo()));
    }
  }
  else if(name == "View")
  {
    // todo: here we might add view options for the canvas graph
  }
  else if(name == "DCC")
  {
    if(prefix)
    {
      QAction *selectCanvasNodeInDCCAction = menu->addAction("Select Canvas Node/Item");
      menu->addAction( selectCanvasNodeInDCCAction );
      QObject::connect(selectCanvasNodeInDCCAction, SIGNAL(triggered()), this, SLOT(onSelectCanvasNodeInDCC()));

      menu->addSeparator();

      QAction *importGraphInDCCAction = menu->addAction("Load Graph");
      menu->addAction( importGraphInDCCAction );
      QObject::connect(importGraphInDCCAction, SIGNAL(triggered()), this, SLOT(onImportGraphInDCC()));

      QAction *exportGraphInDCCAction = menu->addAction("Save Graph");
      menu->addAction( exportGraphInDCCAction );
      QObject::connect(exportGraphInDCCAction, SIGNAL(triggered()), this, SLOT(onExportGraphInDCC()));
    }
  }
}
