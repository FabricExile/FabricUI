//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGVEEditorOwner.h"

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/ModelItems/BindingModelItem.h>
#include <FabricUI/ModelItems/ExecBlockModelItem.h>
#include <FabricUI/ModelItems/GetModelItem.h>
#include <FabricUI/ModelItems/InstBlockModelItem.h>
#include <FabricUI/ModelItems/InstModelItem.h>
#include <FabricUI/ModelItems/SetModelItem.h>
#include <FabricUI/ModelItems/VarModelItem.h>
#include <FabricUI/ValueEditor/BaseViewItem.h>
#include <FabricUI/ValueEditor/ItemMetadata.h>
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>
#include <FabricUI/DFG/DFGVEEditorContextualMenu.h>
#include <iostream>
using namespace FabricUI;
using namespace DFG;
using namespace ModelItems;

DFGVEEditorOwner::DFGVEEditorOwner( DFGWidget * dfgWidget )
  : m_dfgWidget(dfgWidget)
  , m_setGraph( NULL )
  , m_notifProxy( NULL )
{
  m_valueEditor->setContextMenuPolicy(Qt::CustomContextMenu);
  m_valueEditor->setMouseTracking(true);

  connect(
    m_valueEditor,
    SIGNAL(itemOveredChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
    this,
    SLOT(onItemOveredChanged(QTreeWidgetItem*, QTreeWidgetItem*))
    );
}

DFGVEEditorOwner::~DFGVEEditorOwner()
{
}

void DFGVEEditorOwner::initConnections()
{
  VEEditorOwner::initConnections();

  connect(
    getDFGController(),
    SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
    this,
    SLOT(onControllerBindingChanged(FabricCore::DFGBinding const &))
    );

  connect(
    getDFGController(), SIGNAL( argsChanged() ),
    this, SLOT( onStructureChanged() )
    );
  connect(  // [FE-6010]
    this, SIGNAL( modelItemRenamed( FabricUI::ValueEditor::BaseModelItem* ) ),
    this, SLOT( onStructureChanged() )
    );

  connect(
    getDfgWidget(), SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
    this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
    );

  connect( getDfgWidget(), SIGNAL( onGraphSet( FabricUI::GraphView::Graph* ) ),
                    this, SLOT( onGraphSet( FabricUI::GraphView::Graph* ) ) );

  connect(
    m_valueEditor, 
    SIGNAL(customContextMenuRequested(const QPoint &)), 
    this, 
    SLOT(onCustomContextMenu(const QPoint &))
    );

  onGraphSet( getDfgWidget()->getUIGraph() );
}

FabricUI::DFG::DFGWidget * DFGVEEditorOwner::getDfgWidget()
{
  return m_dfgWidget;
}

FabricUI::DFG::DFGController * DFGVEEditorOwner::getDFGController()
{
  return getDfgWidget()->getDFGController();
}

void DFGVEEditorOwner::setModelRoot(
  FabricUI::DFG::DFGController *dfgController,
  FabricUI::ModelItems::BindingModelItem *bindingModelItem
  )
{
  if ( m_notifProxy )
  {
    m_notifProxy->setParent( NULL );
    delete m_notifProxy;
    m_notifProxy = NULL;
  }

  m_subNotifier.clear();
  m_notifier.clear();

  delete m_modelRoot;
  m_modelRoot = bindingModelItem;

  if ( bindingModelItem )
  {
    m_notifier = dfgController->getBindingNotifier();

    DFGVEEditorOwner_BindingNotifProxy *notifProxy =
      new DFGVEEditorOwner_BindingNotifProxy( this, this );
    m_notifProxy = notifProxy;

    connect(
      m_notifier.data(),
      SIGNAL( argInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argRemoved( unsigned, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgRemoved( unsigned, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argValueChanged( unsigned, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgValueChanged( unsigned, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argsReordered( FTL::ArrayRef<unsigned> ) ),
      notifProxy,
      SLOT( onBindingArgsReordered( FTL::ArrayRef<unsigned> ) )
      );

    FabricCore::DFGBinding binding = dfgController->getBinding();
    FabricCore::DFGExec rootExec = binding.getExec();

    m_subNotifier = DFG::DFGExecNotifier::Create( rootExec );

    connect(
      m_subNotifier.data(),
      SIGNAL(execPortMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecPortMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
      );
  }

  emit replaceModelRoot( m_modelRoot );
}

void DFGVEEditorOwner::setModelRoot(
  FabricCore::DFGExec exec,
  FTL::CStrRef itemPath,
  FabricUI::ModelItems::ItemModelItem *nodeModelItem
  )
{
  if ( m_notifProxy )
  {
    m_notifProxy->setParent( NULL );
    delete m_notifProxy;
    m_notifProxy = NULL;
  }

  m_notifier.clear();
  m_subNotifier.clear();

  delete m_modelRoot;
  m_modelRoot = nodeModelItem;

  if ( nodeModelItem )
  {
    m_notifier = DFG::DFGExecNotifier::Create( exec );
    connect(
      m_notifier.data(),
      SIGNAL(nodeRenamed(FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecNodeRenamed(FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodeRemoved(FTL::CStrRef)),
      this,
      SLOT(onExecNodeRemoved(FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortInserted(FTL::CStrRef, unsigned, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortInserted(FTL::CStrRef, unsigned, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortsReordered(FTL::CStrRef, FTL::ArrayRef<unsigned>)),
      this,
      SLOT(onExecNodePortsReordered(FTL::CStrRef, FTL::ArrayRef<unsigned>))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(instBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onInstBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(instBlockRemoved(FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onInstBlockRemoved(FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(instBlockPortInserted(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef)),
      this,
      SLOT(onInstBlockPortInserted(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(instBlockPortRenamed(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onInstBlockPortRenamed(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(instBlockPortRemoved(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef)),
      this,
      SLOT(onInstBlockPortRemoved(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(instBlockPortsReordered(FTL::CStrRef, FTL::CStrRef, FTL::ArrayRef<unsigned>)),
      this,
      SLOT(onInstBlockPortsReordered(FTL::CStrRef, FTL::CStrRef, FTL::ArrayRef<unsigned>))
      );
    connect(
      m_notifier.data(),
      SIGNAL(instBlockPortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onInstBlockPortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(instBlockPortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onInstBlockPortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL( portsConnected( FTL::CStrRef, FTL::CStrRef ) ),
      this,
      SLOT( onExecPortsConnectedOrDisconnected( FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( portsDisconnected( FTL::CStrRef, FTL::CStrRef ) ),
      this,
      SLOT( onExecPortsConnectedOrDisconnected( FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( refVarPathChanged( FTL::CStrRef, FTL::CStrRef ) ),
      this,
      SLOT( onExecRefVarPathChanged( FTL::CStrRef, FTL::CStrRef ) )
      );

    if ( !exec.isExecBlock( itemPath.c_str() )
      && ( exec.isInstBlock( itemPath.c_str() )
        || exec.getNodeType( itemPath.c_str() ) == FabricCore::DFGNodeType_Inst ) )
    {
      FabricCore::DFGExec subExec = exec.getSubExec( itemPath.c_str() );

      m_subNotifier = DFG::DFGExecNotifier::Create( subExec );

      connect(
        m_subNotifier.data(),
        SIGNAL(execPortMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
        this,
        SLOT(onExecPortMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
        );
      connect(
        m_subNotifier.data(),
        SIGNAL(execPortDefaultValuesChanged(FTL::CStrRef)),
        this,
        SLOT(onExecPortDefaultValuesChanged(FTL::CStrRef))
        );
    }
  }

  emit replaceModelRoot( m_modelRoot );
}

void DFGVEEditorOwner::onControllerBindingChanged(
  FabricCore::DFGBinding const &newBinding
  )
{
  onSidePanelInspectRequested();
}

void DFGVEEditorOwner::onSidePanelInspectRequested()
{
  FabricUI::DFG::DFGController *dfgController = getDFGController();
  FabricCore::DFGBinding binding = dfgController->getBinding();
  std::string path = dfgController->getExecPath();
  if ( path.empty() )
  {
    setModelRoot(
      dfgController,
      new FabricUI::ModelItems::BindingModelItem(
        dfgController->getCmdHandler(),
        binding
        )
      );
  }
  else if ( dfgController->getExec().isInstBlockExec() )
  {
    // [pzion 20160519] For now, simply cannot inspect block instance

    setModelRoot( dfgController, NULL );

    // std::string blockName = instName;
    // instName = SplitLast( path );
    // if ( !path.empty() )
    //   exec = rootExec.getSubExec( path.c_str() );
    // else
    //   exec = rootExec;
    // exec = exec.getInstBlockExec( instName.c_str(), blockName.c_str() );
  }
  else
  {
    FabricCore::DFGBinding &binding = dfgController->getBinding();
    FabricCore::DFGExec rootExec = binding.getExec();

    std::string instName = SplitLast( path );

    FabricCore::DFGExec exec;
    if ( !path.empty() )
      exec = rootExec.getSubExec( path.c_str() );
    else
      exec = rootExec;

    // We always show the instantiated values (ie, what
    // we would see if we were outside this node and clicked on it)
    FabricUI::DFG::DFGUICmdHandler *dfgUICmdHandler =
      dfgController->getCmdHandler();

    setModelRoot(
      exec,
      instName,
      new FabricUI::ModelItems::InstModelItem(
        dfgUICmdHandler,
        binding,
        path,
        exec,
        instName
        )
      );
  }
}

void DFGVEEditorOwner::onNodeInspectRequested(
  FabricUI::GraphView::Node *node
  )
{
  if (node->isBackDropNode())
    return;

  FabricUI::DFG::DFGController *dfgController =
    getDFGController();

  FabricUI::DFG::DFGUICmdHandler *dfgUICmdHandler =
    dfgController->getCmdHandler();
  FabricCore::DFGBinding &binding = dfgController->getBinding();
  FTL::CStrRef execPath = dfgController->getExecPath();
  FabricCore::DFGExec &exec = dfgController->getExec();
  FTL::CStrRef nodeName = node->name();

  // TODO: Check for re-inspecting the same node, and don't rebuild
  FabricUI::ModelItems::ItemModelItem *nodeModelItem = 0;

  if ( exec.isExecBlock( nodeName.c_str() ) )
  {
    nodeModelItem =
      new FabricUI::ModelItems::ExecBlockModelItem(
        dfgUICmdHandler,
        binding,
        execPath,
        exec,
        nodeName
        );
  }
  else
  {
    FabricCore::DFGNodeType type = exec.getNodeType( nodeName.c_str() );
    switch (type)
    {
      case FabricCore::DFGNodeType_Inst:
        nodeModelItem =
          new FabricUI::ModelItems::InstModelItem(
            dfgUICmdHandler,
            binding,
            execPath,
            exec,
            nodeName
            );
        break;
      
      case FabricCore::DFGNodeType_Var:
        nodeModelItem =
          new FabricUI::ModelItems::VarModelItem(
            dfgUICmdHandler,
            binding,
            execPath,
            exec,
            nodeName
            );
        break;
      
      case FabricCore::DFGNodeType_Get:
        nodeModelItem =
          new FabricUI::ModelItems::GetModelItem(
            dfgUICmdHandler,
            binding,
            execPath,
            exec,
            nodeName
            );
        break;
      
      case FabricCore::DFGNodeType_Set:
        nodeModelItem =
          new FabricUI::ModelItems::SetModelItem(
            dfgUICmdHandler,
            binding,
            execPath,
            exec,
            nodeName
            );
        break;
      
      case FabricCore::DFGNodeType_User:
        break;
      
      default:
        assert( 0 && "Implement Me" );
        break;
    }
  }
  setModelRoot( exec, nodeName, nodeModelItem );
}

void DFGVEEditorOwner::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name
  )
{
  assert( m_modelRoot );

  try
  {
    if (name == NULL)
    {
      // TODO: Update all children
      //int nChildren = m_modelRoot->NumChildren();
    }
    else
    {
      ValueEditor::BaseModelItem* changingChild = m_modelRoot->getChild( name, false );
      if (changingChild != NULL)
      {
        QVariant val = changingChild->getValue();
        changingChild->emitModelValueChanged( val );
      }
    }
  }
  catch (FabricCore::Exception e)
  {
    emit log( e.getDesc_cstr() );
  }
}

void DFGVEEditorOwner::onExecPortDefaultValuesChanged(
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot );

  try
  {
    if( ValueEditor::BaseModelItem *changingChild =
      m_modelRoot->getChild( portName, false ) )
    {
      QVariant val = changingChild->getValue();
      changingChild->emitModelValueChanged( val );
    }
  }
  catch (FabricCore::Exception e)
  {
    emit log( e.getDesc_cstr() );
  }
}

void DFGVEEditorOwner::onExecNodePortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot );
  assert( m_modelRoot->isItem() );
  ItemModelItem *nodeModelItem =
    static_cast<ItemModelItem *>( m_modelRoot );

  if ( nodeModelItem->getItemPath() != nodeName )
    return;

  try
  {
    if( ValueEditor::BaseModelItem *changingChild =
      m_modelRoot->getChild( portName, false ) )
    {
      QVariant val = changingChild->getValue();
      changingChild->emitModelValueChanged( val );
    }
  }
  catch (FabricCore::Exception e)
  {
    emit log( e.getDesc_cstr() );
  }
}

void DFGVEEditorOwner::onInstBlockPortDefaultValuesChanged(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot->isInst() );
  InstModelItem *instModelItem = static_cast<InstModelItem *>( m_modelRoot );

  if ( FabricUI::ValueEditor::BaseModelItem *childModelItem =
    instModelItem->getChild( blockName ) )
  {
    assert( childModelItem->isInstBlock() );
    InstBlockModelItem *instBlockModelItem =
      static_cast<InstBlockModelItem *>( childModelItem );

    try
    {
      if( ValueEditor::BaseModelItem *changingChild =
        instBlockModelItem->getChild( portName, false ) )
      {
        QVariant val = changingChild->getValue();
        changingChild->emitModelValueChanged( val );
      }
    }
    catch (FabricCore::Exception e)
    {
      emit log( e.getDesc_cstr() );
    }
  }
}

void DFGVEEditorOwner::onExecNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName
  )
{
  assert( m_modelRoot );
  assert( m_modelRoot->isItem() );
  ItemModelItem *nodeModelItem =
    static_cast<ItemModelItem *>( m_modelRoot );
  if ( nodeModelItem->getItemPath() != nodeName )
    return;

  try
  {
    if( ValueEditor::BaseModelItem *changingChild =
      m_modelRoot->getChild( portName, false ) )
    {
      emit modelItemTypeChange( changingChild, newResolveTypeName.c_str() );
    }
  }
  catch (FabricCore::Exception e)
  {
    emit log( e.getDesc_cstr() );
  }
}

void DFGVEEditorOwner::onInstBlockPortResolvedTypeChanged(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName
  )
{
  assert( m_modelRoot->isInst() );
  InstModelItem *instModelItem = static_cast<InstModelItem *>( m_modelRoot );

  if ( FabricUI::ValueEditor::BaseModelItem *childModelItem =
    instModelItem->getChild( blockName ) )
  {
    assert( childModelItem->isInstBlock() );
    InstBlockModelItem *instBlockModelItem =
      static_cast<InstBlockModelItem *>( childModelItem );

    try
    {
      if( ValueEditor::BaseModelItem *changingChild =
        instBlockModelItem->getChild( portName, false ) )
      {
        emit modelItemTypeChange( changingChild, newResolveTypeName.c_str() );
      }
    }
    catch (FabricCore::Exception e)
    {
      emit log( e.getDesc_cstr() );
    }
  }
}

void DFGVEEditorOwner::onOutputsChanged()
{
  VEEditorOwner::onOutputsChanged();
  if (m_modelRoot == NULL)
    return;

  // We need to update all -out- values to reflect the
  // result of the new calculation
  FabricUI::ModelItems::ChildVec::iterator itr =
    m_modelRoot->GetChildItrBegin();
  FabricUI::ModelItems::ChildVec::iterator end =
    m_modelRoot->GetChildItrEnd();
  for (; itr != end; itr++)
  {
    ValueEditor::BaseModelItem *childModelItem = *itr;
    if( ValueEditor::ItemMetadata *childItemMetadata = childModelItem->getMetadata() )
    {
      FTL::CStrRef vePortType =
        childItemMetadata->getString( ValueEditor::ItemMetadata::VEPortTypeKey.c_str() );
      if ( vePortType != FTL_STR("In") )
      {
        QVariant val = childModelItem->getValue();
        childModelItem->emitModelValueChanged( val );
      }
    }
  }
}

void DFGVEEditorOwner::onBindingArgInserted( unsigned index, FTL::CStrRef name, FTL::CStrRef type )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isBinding() )
  {
    emit modelItemInserted( m_modelRoot, int( index ), name.c_str() );
  }
}

void DFGVEEditorOwner::onBindingArgTypeChanged( unsigned index, FTL::CStrRef name, FTL::CStrRef newType )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isBinding() )
  {
    ValueEditor::BaseModelItem *changingChild = m_modelRoot->getChild( name, false );
    if ( changingChild != NULL )
      emit modelItemTypeChange( changingChild, newType.c_str() );
  }
}

void DFGVEEditorOwner::onBindingArgRemoved( unsigned index, FTL::CStrRef name )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isBinding() )
  {
    BindingModelItem *bindingModelItem =
      static_cast<BindingModelItem *>( m_modelRoot );
    if ( ValueEditor::BaseModelItem* removedChild =
      m_modelRoot->getChild( name, false ) )
    {
      emit modelItemRemoved( removedChild );
      bindingModelItem->childRemoved( index, name );
    }
  }
}

void DFGVEEditorOwner::onBindingArgsReordered( FTL::ArrayRef<unsigned> newOrder )
{
  assert( m_modelRoot );

  // The array will specify the new order of our base arrays children
  // We will need to keep track of 
  QList<int> newIntOrder;
  #if QT_VERSION >= 0x040800
    newIntOrder.reserve( newOrder.size() );
  #endif
  for (size_t i = 0; i < newOrder.size(); i++)
    newIntOrder.push_back( int( newOrder[i] ) );

  //m_modelRoot->reorderChildren( newIntOrder );
  emit modelItemChildrenReordered( m_modelRoot, newIntOrder );
}

void DFGVEEditorOwner::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName
  )
{
  assert( m_modelRoot );

  if( ValueEditor::BaseModelItem *changingChild =
    m_modelRoot->onPortRenamed(
      argIndex,
      oldArgName,
      newArgName
      ) )
    emit modelItemRenamed( changingChild );
}

void DFGVEEditorOwner::onExecNodePortInserted(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isItem() )
  {
    emit modelItemInserted( m_modelRoot, int( portIndex ), portName.c_str() );
  }
}

void DFGVEEditorOwner::onInstBlockPortInserted(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot );
  assert( m_modelRoot->isInst() );
  InstModelItem *instModelItem = static_cast<InstModelItem *>( m_modelRoot );

  if ( FabricUI::ValueEditor::BaseModelItem *childModelItem =
    instModelItem->getChild( blockName ) )
  {
    assert( childModelItem->isInstBlock() );
    InstBlockModelItem *instBlockModelItem =
      static_cast<InstBlockModelItem *>( childModelItem );

    emit modelItemInserted( instBlockModelItem, int( portIndex ), portName.c_str() );
  }
}

void DFGVEEditorOwner::onExecNodePortRenamed(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName
  )
{
  assert( m_modelRoot );

  if( ValueEditor::BaseModelItem *changingChild =
    m_modelRoot->onPortRenamed(
      portIndex,
      oldPortName,
      newPortName
      ) )
    emit modelItemRenamed( changingChild );
}

void DFGVEEditorOwner::onInstBlockPortRenamed(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName
  )
{
  assert( m_modelRoot->isInst() );
  InstModelItem *instModelItem = static_cast<InstModelItem *>( m_modelRoot );

  if ( FabricUI::ValueEditor::BaseModelItem *childModelItem =
    instModelItem->getChild( blockName ) )
  {
    assert( childModelItem->isInstBlock() );
    InstBlockModelItem *instBlockModelItem =
      static_cast<InstBlockModelItem *>( childModelItem );

    if( ValueEditor::BaseModelItem *changingChild =
      instBlockModelItem->onPortRenamed(
        portIndex,
        oldPortName,
        newPortName
        ) )
      emit modelItemRenamed( changingChild );
  }
}

void DFGVEEditorOwner::onExecNodePortRemoved(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isItem() )
  {
    ItemModelItem *itemModelItem =
      static_cast<ItemModelItem *>( m_modelRoot );
    ValueEditor::BaseModelItem* removedChild = m_modelRoot->getChild( portName, false );
    if ( removedChild != NULL )
    {
      emit modelItemRemoved( removedChild );
      itemModelItem->childRemoved( portIndex, portName );
    }
  }
}

void DFGVEEditorOwner::onInstBlockPortRemoved(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot->isInst() );
  InstModelItem *instModelItem = static_cast<InstModelItem *>( m_modelRoot );

  if ( FabricUI::ValueEditor::BaseModelItem *childModelItem =
    instModelItem->getChild( blockName ) )
  {
    assert( childModelItem->isInstBlock() );
    InstBlockModelItem *instBlockModelItem =
      static_cast<InstBlockModelItem *>( childModelItem );

    if ( ValueEditor::BaseModelItem* removedChild =
      instBlockModelItem->getChild( portName, false ) )
    {
      emit modelItemRemoved( removedChild );
      instBlockModelItem->childRemoved( portIndex, portName );
    }
  }
}

void DFGVEEditorOwner::onInstBlockPortsReordered(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  FTL::ArrayRef<unsigned> newOrder
  )
{
  assert( m_modelRoot->isInst() );
  InstModelItem *instModelItem = static_cast<InstModelItem *>( m_modelRoot );

  if ( FabricUI::ValueEditor::BaseModelItem *childModelItem =
    instModelItem->getChild( blockName ) )
  {
    assert( childModelItem->isInstBlock() );
    InstBlockModelItem *instBlockModelItem =
      static_cast<InstBlockModelItem *>( childModelItem );

    QList<int> newIntOrder;
#if QT_VERSION >= 0x040800
    newIntOrder.reserve( newOrder.size() );
#endif
    for (size_t i = 0; i < newOrder.size(); i++)
      newIntOrder.push_back( int( newOrder[i] ) );

    emit modelItemChildrenReordered( instBlockModelItem, newIntOrder );
  }
}

void DFGVEEditorOwner::onExecNodePortsReordered(
  FTL::CStrRef nodeName,
  FTL::ArrayRef<unsigned> newOrder
  )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isItem() )
  {
    QList<int> newIntOrder;
    #if QT_VERSION >= 0x040800
      newIntOrder.reserve( newOrder.size() );
    #endif
    for (size_t i = 0; i < newOrder.size(); i++)
      newIntOrder.push_back( int( newOrder[i] ) );

    emit modelItemChildrenReordered( m_modelRoot, newIntOrder );
  }
}

void DFGVEEditorOwner::onExecPortMetadataChanged(
  FTL::CStrRef portName,
  FTL::CStrRef key,
  FTL::CStrRef value
  )
{
  // Find the appropriate execPort
  assert( m_modelRoot );

  if ( key == FTL_STR("uiPersistValue") )
    return;

  ValueEditor::BaseModelItem* changingChild = m_modelRoot->getChild( portName, false );
  // Only update if the change isn't coming from the child itself
    if ( changingChild != NULL && !changingChild->isSettingMetadata() )
  {
    // Our changing metadata could mean a changing type
    emit modelItemTypeChange( changingChild, "" );
  }
}

void DFGVEEditorOwner::onExecNodeRemoved(
  FTL::CStrRef nodeName
  )
{
  assert( m_modelRoot );
  assert( m_modelRoot->isItem() );
  ItemModelItem *itemModelItem =
    static_cast<ItemModelItem *>( m_modelRoot );
  if ( itemModelItem->getItemPath() == nodeName )
  {
    emit modelItemRemoved( m_modelRoot );
    onSidePanelInspectRequested();
  }
}

void DFGVEEditorOwner::onExecNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName
  )
{
  assert( m_modelRoot );

  ItemModelItem *itemModelItem =
    static_cast<ItemModelItem *>( m_modelRoot );
  if ( itemModelItem->getName() == oldNodeName )
  {
    itemModelItem->onRenamed( oldNodeName, newNodeName );
    emit modelItemRenamed( itemModelItem );
  }
}

void DFGVEEditorOwner::onInstBlockRemoved(
  FTL::CStrRef instName,
  FTL::CStrRef blockName
  )
{
  assert( m_modelRoot );
  assert( m_modelRoot->isInst() );
  InstModelItem *instModelItem = static_cast<InstModelItem *>( m_modelRoot );

  if ( FabricUI::ValueEditor::BaseModelItem *childModelItem =
    instModelItem->getChild( blockName ) )
  {
    assert( childModelItem->isInstBlock() );
    InstBlockModelItem *instBlockModelItem =
      static_cast<InstBlockModelItem *>( childModelItem );

    emit modelItemRemoved( instBlockModelItem );
  }
}

void DFGVEEditorOwner::onInstBlockRenamed(
  FTL::CStrRef instName,
  FTL::CStrRef oldBlockName,
  FTL::CStrRef newBlockName
  )
{
  assert( m_modelRoot );
  assert( m_modelRoot->isInst() );
  InstModelItem *instModelItem = static_cast<InstModelItem *>( m_modelRoot );

  if ( FabricUI::ValueEditor::BaseModelItem *childModelItem =
    instModelItem->getChild( oldBlockName ) )
  {
    assert( childModelItem->isInstBlock() );
    InstBlockModelItem *instBlockModelItem =
      static_cast<InstBlockModelItem *>( childModelItem );

    instBlockModelItem->onRenamed( oldBlockName, newBlockName );

    emit modelItemRenamed( instBlockModelItem );
  }
}

void DFGVEEditorOwner::onExecPortsConnectedOrDisconnected(
  FTL::CStrRef srcPortPath,
  FTL::CStrRef dstPortPath
  )
{
  assert( m_modelRoot );

  if ( m_modelRoot->isItem() )
  {
    ItemModelItem *itemModelItem =
      static_cast<ItemModelItem *>( m_modelRoot );

    FTL::CStrRef::Split srcPortPathSplit = srcPortPath.split('.');
    if ( srcPortPathSplit.first == itemModelItem->getItemPath() )
      if ( ValueEditor::BaseModelItem *srcChild =
        itemModelItem->getDescendant( srcPortPathSplit.second ) )
        emit modelItemTypeChange( srcChild, "" );

    FTL::CStrRef::Split dstPortPathSplit = dstPortPath.split('.');
    if ( dstPortPath.split('.').first == itemModelItem->getItemPath() )
      if ( ValueEditor::BaseModelItem *dstChild =
        itemModelItem->getDescendant( dstPortPathSplit.second ) )
        emit modelItemTypeChange( dstChild, "" );
  }
}

void DFGVEEditorOwner::onExecRefVarPathChanged(
  FTL::CStrRef refName,
  FTL::CStrRef newVarPath
  )
{
  assert( m_modelRoot );

  if ( m_modelRoot->isItem() )
  {
    ItemModelItem *itemModelItem =
      static_cast<ItemModelItem *>( m_modelRoot );
    if ( itemModelItem->isRef() )
    {
      RefModelItem *refModelItem =
        static_cast<RefModelItem *>( itemModelItem );
      if ( refModelItem->getItemPath() == refName )
      {
        if( ValueEditor::BaseModelItem *changingChild =
          refModelItem->getChild( FTL_STR("varPath"), false ) )
        {
          QVariant val = changingChild->getValue();
          changingChild->emitModelValueChanged( val );
        }
      }
    }
  }
}

void DFGVEEditorOwner::onGraphSet( FabricUI::GraphView::Graph * graph )
{
  if(graph != m_setGraph)
  {
    connect( 
      graph, SIGNAL( sidePanelInspectRequested() ),
      this, SLOT( onSidePanelInspectRequested() )
      );
    connect(
      graph, SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
      this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
      );

    
    onSidePanelInspectRequested();

    m_setGraph = graph;
  }
}

void DFGVEEditorOwner::onCustomContextMenu(const QPoint &point)
{
  QTreeWidgetItem *qTreeItem = m_valueEditor->itemAt(point);
  if(qTreeItem)
  {
    ValueEditor::VETreeWidgetItem *veTreeItem = static_cast<ValueEditor::VETreeWidgetItem *>(qTreeItem);
    if(veTreeItem)
      DFGVEEditorContextualMenu::create(
        m_valueEditor, 
        point,
        veTreeItem);
  }
}

void DFGVEEditorOwner::onItemOveredChanged( QTreeWidgetItem * oldItem, QTreeWidgetItem * newItem ) {

  if(newItem != 0)
  {
    ValueEditor::VETreeWidgetItem *newVETreeItem = static_cast<ValueEditor::VETreeWidgetItem *>(newItem);
    if(newVETreeItem && DFGVEEditorContextualMenu::canCreate(newVETreeItem))
    {
      // std::cout << "DFGVEEditorOwner::onItemOvered " << "Add New " << std::endl;
    }
  }

  if(oldItem != 0)
  {
    ValueEditor::VETreeWidgetItem *oldVETreeItem = static_cast<ValueEditor::VETreeWidgetItem *>(oldItem);
    if(oldVETreeItem && DFGVEEditorContextualMenu::canCreate(oldVETreeItem))
    {
      //std::cout << "DFGVEEditorOwner::onItemOvered " << "Remove old " << std::endl;
    }
  }
}

void DFGVEEditorOwner_BindingNotifProxy::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name
  )
{
  m_dst->onBindingArgValueChanged( index, name );
}

void DFGVEEditorOwner_BindingNotifProxy::onBindingArgInserted(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef type
  )
{
  m_dst->onBindingArgInserted( index, name, type );
}

void DFGVEEditorOwner_BindingNotifProxy::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName
  )
{
  m_dst->onBindingArgRenamed( argIndex, oldArgName, newArgName );
}

void DFGVEEditorOwner_BindingNotifProxy::onBindingArgRemoved(
  unsigned index,
  FTL::CStrRef name
  )
{
  m_dst->onBindingArgRemoved( index, name );
}

void DFGVEEditorOwner_BindingNotifProxy::onBindingArgTypeChanged(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef newType
  )
{
  m_dst->onBindingArgTypeChanged( index, name, newType );
}

void DFGVEEditorOwner_BindingNotifProxy::onBindingArgsReordered(
  FTL::ArrayRef<unsigned> newOrder
  )
{
  m_dst->onBindingArgsReordered( newOrder );
}
