// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <QDebug>
#include <QRegExp>
#include <QApplication>
#include <QClipboard>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMessageBox>
#include <QTimer>

#include <iostream>

#include <FTL/JSONEnc.h>
#include <FTL/JSONDec.h>
#include <FTL/Str.h>
#include <FTL/Math.h>
#include <FTL/MapCharSingle.h>
#include <FTL/Path.h>

#include <FabricUI/Util/LoadPixmap.h>

#include <FabricUI/GraphView/FixedPort.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/GraphRelaxer.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/NodeHeaderButton.h>
#include <FabricUI/GraphView/NodeHeader.h>

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGErrorsWidget.h>
#include <FabricUI/DFG/DFGLogWidget.h>
#include <FabricUI/DFG/DFGNotificationRouter.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGUIUtil.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGBindingUtils.h>

#include <FabricServices/Persistence/RTValToJSONEncoder.hpp>
#include <FabricUI/DFG/DFGMetaDataHelpers.h>
#include <FabricUI/DFG/Dialogs/DFGEditPortDialog.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;
using namespace FabricUI::GraphView;

DFGController::DFGController(
  GraphView::Graph * graph,
  DFGWidget *dfgWidget,
  FabricCore::Client &client,
  FabricServices::ASTWrapper::KLASTManager * manager,
  DFGUICmdHandler *cmdHandler,
  bool overTakeBindingNotifications
  )
  : GraphView::Controller(graph)
  , m_notificationTimer( new QTimer( this ) )
  , m_dfgWidget( dfgWidget )
  , m_client(client)
  , m_manager(manager)
  , m_cmdHandler( cmdHandler )
  , m_router(0)
  , m_logFunc(0)
  , m_overTakeBindingNotifications( overTakeBindingNotifications )
  , m_updateSignalBlockCount( 0 )
  , m_varsChangedPending( false )
  , m_argsChangedPending( false )
  , m_argValuesChangedPending( false )
  , m_defaultValuesChangedPending( false )
  , m_topoDirtyPending( false )
  , m_dirtyPending( false )
{
  resetTimelinePortIndices();

  m_tabSearchPrefsJSONFilename = FabricCore::GetFabricPrivateDir();
  FTL::PathAppendEntry(
    m_tabSearchPrefsJSONFilename,
    "TabSearch.prefs.json"
    );

  m_notificationTimer->setSingleShot( true );
  connect(
    m_notificationTimer, SIGNAL(timeout()),
    this, SLOT(onNotificationTimer())
    );

  m_router = NULL;
  m_logFunc = NULL;
  m_presetDictsUpToDate = false;

  QObject::connect(this, SIGNAL(topoDirty()), this, SLOT(onTopoDirty()));
  QObject::connect(this, SIGNAL(varsChanged()), this, SLOT(onVariablesChanged()));

  QObject::connect(this, SIGNAL(topoDirty()), this, SIGNAL(varsChangedImplicitly()));
  QObject::connect(this, SIGNAL(varsChanged()), this, SIGNAL(varsChangedImplicitly()));
}

DFGController::~DFGController()
{
}

void DFGController::setHostBindingExec(
  FabricCore::DFGHost &host,
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::StrRef execBlockName
  )
{
  m_host = host;

  setBindingExec( binding, execPath, exec, execBlockName );

  emit hostChanged();
}

void DFGController::setBindingExec(
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::StrRef execBlockName
  )
{
  assert( m_dfgWidget->priorExecStackIsEmpty() );

  if ( m_binding.isValid() )
    m_bindingNotifier.clear();

  m_binding = binding;
  
  if ( m_binding.isValid() )
  {
    m_bindingNotifier = DFGBindingNotifier::Create( m_binding );

    connect(
      m_bindingNotifier.data(),
      SIGNAL(dirty()),
      this,
      SLOT(onBindingDirty())
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(topoDirty()),
      this,
      SLOT(onBindingTopoDirty())
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argInserted(unsigned, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onBindingArgInserted(unsigned, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argTypeChanged(unsigned, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onBindingArgTypeChanged(unsigned, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argRemoved(unsigned, FTL::CStrRef)),
      this,
      SLOT(onBindingArgRemoved(unsigned, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argsReordered(FTL::ArrayRef<unsigned>)),
      this,
      SLOT(onBindingArgsReordered(FTL::ArrayRef<unsigned>))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argValueChanged(unsigned, FTL::CStrRef)),
      this,
      SLOT(onBindingArgValueChanged(unsigned, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(varInserted(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onBindingVarInserted(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(varRemoved(FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onBindingVarRemoved(FTL::CStrRef, FTL::CStrRef))
      );
  }

  setExec( execPath, exec, execBlockName );

  updateErrors();

  emit bindingChanged( m_binding );
  emitTopoDirty();
  this->resetTimelinePortIndices();
  this->updateTimelinePortIndices();
}

void DFGController::setExec(
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::StrRef execBlockName
  )
{
  if ( m_exec.isValid() )
    m_ancestorExecNotifiers.clear();

  m_execPath = execPath;
  m_exec = exec;
  m_execBlockName = execBlockName;
 
  m_presetDictsUpToDate = false;

  if ( m_binding.isValid()
    && m_exec.isValid()
    && !m_execPath.empty() )
  {
    FabricCore::DFGExec ancestorExec = m_binding.getExec();
    FTL::StrRef::Split split = FTL::StrRef( m_execPath ).split('.');
    for (;;)
    {
      QSharedPointer<DFGExecNotifier> ancestorExecNotifier =
        DFGExecNotifier::Create( ancestorExec );

      connect(
        ancestorExecNotifier.data(),
        SIGNAL(nodeRenamed(FTL::CStrRef, FTL::CStrRef)),
        this,
        SLOT(onParentExecNodeRenamed(FTL::CStrRef, FTL::CStrRef))
        );

      m_ancestorExecNotifiers.append( ancestorExecNotifier );

      if ( split.second.empty() )
        break;
      FTL::StrRef instName = split.first;
      split = split.second.split('.');
      FTL::StrRef maybeInstBlockName = split.first;

      std::string subPath = instName;
      subPath += '.';
      subPath += maybeInstBlockName;
      if ( ancestorExec.isInstBlock( subPath.c_str() ) )
      {
        ancestorExec = ancestorExec.getSubExec( subPath.c_str() );
        split = split.second.split('.');
      }
      else
      {
        subPath[instName.size()] = '\0';
        ancestorExec = ancestorExec.getSubExec( subPath.c_str() );
      }
    }
  }

  emit execChanged();
}

void DFGController::refreshExec()
{
  emit execChanged();
}

DFGNotificationRouter * DFGController::getRouter()
{
  return m_router;
}

void DFGController::setRouter(DFGNotificationRouter * router)
{
  if ( m_router )
  {
    QObject::disconnect(
      this, SIGNAL(execChanged()),
      m_router, SLOT(onExecChanged())
      );
  }

  m_router = router;

  if ( m_router )
  {
    QObject::connect(
      this, SIGNAL(execChanged()),
      m_router, SLOT(onExecChanged())
      );
  }
}

bool DFGController::validPresetSplit() const
{
  if(isViewingRootGraph())
    return true;

  if(!m_exec.isValid())
    return true;

  if(!m_exec.editWouldSplitFromPreset())
    return true;

  // QMessageBox msgBox;
  // msgBox.setText( "You are about to split this node from the preset. Are you sure?" );
  // msgBox.setInformativeText( "The node will no longer be referencing the preset." );
  // msgBox.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
  // msgBox.setDefaultButton( QMessageBox::Cancel );
  // switch(msgBox.exec())
  // {
  //   case QMessageBox::Ok:
  //     return true;
  //   case QMessageBox::Cancel:
  //     return false;
  // }

  return false;
}

std::string DFGController::gvcDoCopy()
{
  return copy();
}

void DFGController::gvcDoPaste(
  bool mapPositionToMouseCursor
  )
{
  cmdPaste( mapPositionToMouseCursor );
}

bool DFGController::gvcDoRemoveNodes(
  FTL::ArrayRef<GraphView::Node *> nodes
  )
{
  if ( !nodes.empty() )
  {
    QStringList nodeNames;
    nodeNames.reserve( nodes.size() );
    for ( unsigned i = 0; i < nodes.size(); ++i )
    {
      FTL::CStrRef nodeName = nodes[i]->name();
      nodeNames.push_back(
        QString::fromUtf8( nodeName.data(), nodeName.size() )
        );
    }

    cmdRemoveNodes( nodeNames );
  }
  return true;
}

QString DFGController::cmdAddBackDrop(
  QString title,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  QString nodeName =
    m_cmdHandler->dfgDoAddBackDrop(
      getBinding(),
      getExecPath_QS(),
      getExec(),
      title,
      pos
      );
  return nodeName;
}

QString DFGController::cmdEditNode(
  QString oldName,
  QString desiredNewName,
  QString nodeMetadata,
  QString execMetadata
  )
{
  if(!validPresetSplit())
    return oldName;
  
  return m_cmdHandler->dfgDoEditNode(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    oldName,
    desiredNewName,
    nodeMetadata,
    execMetadata
    );
}

void DFGController::cmdSetNodeComment(
  QString nodeName,
  QString comment
  )
{
  if ( !validPresetSplit() )
    return;

  m_cmdHandler->dfgDoSetNodeComment(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeName,
    comment
    );
}

void DFGController::setNodeCommentExpanded(
  QString nodeName,
  bool expanded
  )
{
  QByteArray nodeNameBA = nodeName.toUtf8();
  char const *nodeNameCStr = nodeNameBA.constData();
  m_exec.setItemMetadata(
    nodeNameCStr,
    "uiCommentExpanded",
    expanded? "true": "",
    false,
    false
    );
}

bool DFGController::gvcDoAddInstFromPreset(
  QString presetPath,
  QPointF pos
  )
{
  cmdAddInstFromPreset(
    presetPath,
    pos
    );

  return true;
}

void DFGController::gvcDoAddPort(
  QString desiredPortName,
  GraphView::PortType portType,
  QString typeSpec,
  GraphView::ConnectionTarget *connectWith,
  QString extDep,
  QString metaData
  )
{
  FabricCore::DFGPortType dfgPortType = PortTypeToDFGPortType( portType );

  std::string connectWithPath;
  if ( connectWith )
    connectWithPath = connectWith->path();
  cmdAddPort(
    desiredPortName,
    dfgPortType,
    typeSpec,
    QString::fromUtf8( connectWithPath.data(), connectWithPath.size() ),
    extDep,
    metaData
    );
}

void DFGController::gvcDoRenameExecPort(
  QString oldName,
  QString desiredPortName,
  QString execPath
)
{
  cmdRenameExecPort(
    oldName,
    desiredPortName,
    execPath
  );
}

void DFGController::gvcDoRenameNode(
  GraphView::Node* node,
  QString newName
)
{
  cmdEditNode(
    node->name_QS(),
    newName,
    "",
    ""
  );
}

void DFGController::gvcDoSetNodeCommentExpanded(
  GraphView::Node *node,
  bool expanded
  )
{
  FTL::CStrRef nodeName = node->name();
  setNodeCommentExpanded(
    QString::fromUtf8( nodeName.data(), nodeName.size() ),
    expanded
    );
}

QString DFGController::cmdRenameExecPort(
  QString oldName,
  QString desiredNewName,
  QString execPath
  )
{
  if(!validPresetSplit())
    return QString();

  QString result = m_cmdHandler->dfgDoRenamePort(
    getBinding(),
    getExecPath_QS(),
    getExec().getSubExec( execPath.toUtf8().constData() ),
    oldName,
    desiredNewName
    );

  if ( !result.isEmpty() )
    emit argsChanged();

  return result;
}

bool DFGController::gvcDoAddConnections(
  std::vector<GraphView::ConnectionTarget *> const &srcs,
  std::vector<GraphView::ConnectionTarget *> const &dsts
  )
{
  QStringList srcPaths;
  QStringList dstPaths;

  for (size_t i=0;i<srcs.size();i++)
  {
    GraphView::ConnectionTarget *src = srcs[i];
    std::string srcPath;
    if(src->targetType() == GraphView::TargetType_Pin)
      srcPath = ((GraphView::Pin*)src)->path();
    else if(src->targetType() == GraphView::TargetType_Port)
      srcPath = ((GraphView::Port*)src)->path();
    else if(src->targetType() == GraphView::TargetType_FixedPort)
      srcPath = ((GraphView::FixedPort*)src)->path();
    else if(src->targetType() == GraphView::TargetType_InstBlockPort)
      srcPath = ((GraphView::InstBlockPort*)src)->path();
    srcPaths.push_back( QString::fromUtf8( srcPath.data(), srcPath.size() ) );
  }

  for (size_t i=0;i<dsts.size();i++)
  {
    GraphView::ConnectionTarget *dst = dsts[i];
    std::string dstPath;
    if(dst->targetType() == GraphView::TargetType_Pin)
      dstPath = ((GraphView::Pin*)dst)->path();
    else if(dst->targetType() == GraphView::TargetType_Port)
      dstPath = ((GraphView::Port*)dst)->path();
    else if(dst->targetType() == GraphView::TargetType_FixedPort)
      dstPath = ((GraphView::FixedPort*)dst)->path();
    else if(dst->targetType() == GraphView::TargetType_InstBlockPort)
      dstPath = ((GraphView::InstBlockPort*)dst)->path();
    dstPaths.push_back( QString::fromUtf8( dstPath.data(), dstPath.size() ) );
  }

  cmdConnect( srcPaths, dstPaths );
  
  return true;
}

bool DFGController::gvcDoRemoveConnections(
  std::vector<GraphView::ConnectionTarget *> const &srcs,
  std::vector<GraphView::ConnectionTarget *> const &dsts
  )
{
  QStringList srcPaths;
  QStringList dstPaths;

  for (size_t i=0;i<srcs.size();i++)
  {
    GraphView::ConnectionTarget *src = srcs[i];
    std::string srcPath;
    if(src->targetType() == GraphView::TargetType_Pin)
      srcPath = ((GraphView::Pin*)src)->path();
    else if(src->targetType() == GraphView::TargetType_Port)
      srcPath = ((GraphView::Port*)src)->path();
    else if(src->targetType() == GraphView::TargetType_FixedPort)
      srcPath = ((GraphView::FixedPort*)src)->path();
    else if(src->targetType() == GraphView::TargetType_InstBlockPort)
      srcPath = ((GraphView::InstBlockPort*)src)->path();
    srcPaths.push_back( QString::fromUtf8( srcPath.data(), srcPath.size() ) );
  }

  for (size_t i=0;i<dsts.size();i++)
  {
    GraphView::ConnectionTarget *dst = dsts[i];
    std::string dstPath;
    if(dst->targetType() == GraphView::TargetType_Pin)
      dstPath = ((GraphView::Pin*)dst)->path();
    else if(dst->targetType() == GraphView::TargetType_Port)
      dstPath = ((GraphView::Port*)dst)->path();
    else if(dst->targetType() == GraphView::TargetType_FixedPort)
      dstPath = ((GraphView::FixedPort*)dst)->path();
    else if(dst->targetType() == GraphView::TargetType_InstBlockPort)
      dstPath = ((GraphView::InstBlockPort*)dst)->path();
    dstPaths.push_back( QString::fromUtf8( dstPath.data(), dstPath.size() ) );
  }

  cmdDisconnect( srcPaths, dstPaths );

  return true;
}

void DFGController::cmdSetCode( QString code )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoSetCode(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    code
    );
}

QString DFGController::reloadCode()
{
  if(!validPresetSplit())
    return QString();

  FabricCore::DFGExec &func = getExec();
  if ( func.getType() != FabricCore::DFGExecType_Func )
    return QString();

  char const * filePath = func.getImportPathname();
  FILE * file = fopen(filePath, "rb");
  if(file)
  {
    fseek( file, 0, SEEK_END );
    int fileSize = ftell( file );
    rewind( file );

    char * buffer = (char*) malloc(fileSize + 1);
    buffer[fileSize] = '\0';

    fread(buffer, 1, fileSize, file);

    fclose(file);

    std::string json = buffer;
    free(buffer);

    try
    {
      FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(json.c_str());
      if(jsonVar.isDict())
      {
        const FabricCore::Variant * klCodeVar = jsonVar.getDictValue("code");
        if(klCodeVar)
        {
          if(klCodeVar->isString())
          {
            QString klCode = QString::fromUtf8(
              klCodeVar->getString_cstr()
              );
            cmdSetCode( klCode );
            return klCode;
          }
        }
      }
    }
    catch(FabricCore::Exception e)
    {
      logError(e.getDesc_cstr());
    }
  }    
  return QString();
}

bool DFGController::zoomCanvas(
  float zoom
  )
{
  if ( FTL::IsNaN( zoom ) || FTL::IsInf( zoom ) )
    return false;

  try
  {
    FabricCore::DFGExec &exec = getExec();

    std::string json;
    {
      FTL::JSONEnc<> enc( json );
      FTL::JSONObjectEnc<> objEnc( enc );
      {
        FTL::JSONEnc<> zoomEnc( objEnc, FTL_STR("value") );
        FTL::JSONFloat64Enc<> zoomS32Enc( zoomEnc, zoom );
      }
    }

    exec.setMetadata("uiGraphZoom", json.c_str(), false, false);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

bool DFGController::panCanvas(QPointF pan)
{
  try
  {
    FabricCore::DFGExec &exec = getExec();

    std::string json;
    {
      FTL::JSONEnc<> enc( json );
      FTL::JSONObjectEnc<> objEnc( enc );
      {
        FTL::JSONEnc<> xEnc( objEnc, FTL_STR("x") );
        FTL::JSONFloat64Enc<> xS32Enc( xEnc, pan.x() );
      }
      {
        FTL::JSONEnc<> yEnc( objEnc, FTL_STR("y") );
        FTL::JSONFloat64Enc<> yS32Enc( yEnc, pan.y() );
      }
    }

    // qDebug() << "panCanvas " << json.c_str();
    exec.setMetadata("uiGraphPan", json.c_str(), false, false);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

bool DFGController::relaxNodes(QStringList paths)
{
  if(paths.length() == 0)
  {
    const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
    for(unsigned int i=0;i<nodes.size();i++)
    {
      if(nodes[i]->type() != GraphView::QGraphicsItemType_Node)
        continue;
      paths.append(nodes[i]->name().c_str());
    }
  }

  std::vector<GraphView::Node*> rootNodes;
  for(int i=0;i<paths.length();i++)
  {
    if ( GraphView::Node *uiNode = graph()->node( paths[i] ) )
      rootNodes.push_back( uiNode );
  }

  if(rootNodes.size() == 0)
    return false;

  std::vector<GraphView::Node*> nodes = rootNodes[0]->upStreamNodes_deprecated(true, rootNodes);
  if(nodes.size() <= 1)
    return false;

  GraphView::GraphRelaxer relaxer(graph()->config());
  for(unsigned int i=0;i<nodes.size();i++)
  {
    QRectF rect = nodes[i]->boundingRect();
    relaxer.addNode(nodes[i]->name().c_str(), nodes[i]->topLeftGraphPos(), nodes[i]->col(), nodes[i]->row(), rect.width(), rect.height());
  }

  std::vector<GraphView::Connection*> connections = graph()->connections();
  for(unsigned int i=0;i<connections.size();i++)
  {
    GraphView::ConnectionTarget * src = connections[i]->src();
    if(src->targetType() != GraphView::TargetType_Pin)
      continue;

    GraphView::ConnectionTarget * dst = connections[i]->dst();
    if(dst->targetType() != GraphView::TargetType_Pin)
      continue;

    GraphView::Pin * srcPin = (GraphView::Pin *)src;
    GraphView::Pin * dstPin = (GraphView::Pin *)dst;

    GraphView::Node * srcNode = srcPin->node();
    GraphView::Node * dstNode = dstPin->node();

    relaxer.addSpring(srcNode->name().c_str(), dstNode->name().c_str());
  }

  relaxer.relax(50);

  QStringList nodeNames;
  nodeNames.reserve( relaxer.numNodes() );
  QList<QPointF> newTopLeftPoss;
  newTopLeftPoss.reserve( relaxer.numNodes() );
  for ( unsigned i=0; i<relaxer.numNodes(); i++ )
  {
    FTL::CStrRef nodeName = relaxer.getName(i);
    nodeNames.push_back( QString::fromUtf8( nodeName.data(), nodeName.size() ) );
    newTopLeftPoss.push_back( relaxer.getPos(i) );
  }

  cmdMoveNodes( nodeNames, newTopLeftPoss );

  return true;
}

bool DFGController::setNodeColor(
  const char * nodeName,
  const char * key,
  QColor color
  )
{
  if(!validPresetSplit())
    return false;

  try
  {
    std::string uiNodeColorString;
    {
      FTL::JSONEnc<> enc( uiNodeColorString );
      FTL::JSONObjectEnc<> objEnc( enc );
      {
        FTL::JSONEnc<> rEnc( objEnc, FTL_STR("r") );
        FTL::JSONSInt32Enc<> rS32Enc( rEnc, color.red() );
      }
      {
        FTL::JSONEnc<> gEnc( objEnc, FTL_STR("g") );
        FTL::JSONSInt32Enc<> gS32Enc( gEnc, color.green() );
      }
      {
        FTL::JSONEnc<> bEnc( objEnc, FTL_STR("b") );
        FTL::JSONSInt32Enc<> bS32Enc( bEnc, color.blue() );
      }
    }

    FabricCore::DFGExec &exec = getExec();
    exec.setNodeMetadata(
      nodeName,
      key,
      uiNodeColorString.c_str(),
      false, 
      true
      );
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

void DFGController::setNodeCollapseState(int collapsedState, GraphView::Node *node) {
  if (node->type() != GraphView::QGraphicsItemType_Node)
    return;
  node->setCollapsedState((GraphView::Node::CollapseState)collapsedState);
  FabricCore::DFGExec &exec = getExec();
  char const * const collapsedStateMetadataValues[GraphView::Node::CollapseState_NumStates] = { "0", "1", "2" };
  exec.setNodeMetadata(node->name().c_str(), "uiCollapsedState", collapsedStateMetadataValues[collapsedState], false, false);
}

void DFGController::setSelectedNodesCollapseState(int collapsedState) {
  collapseSelectedNodes(collapsedState);
  FabricCore::DFGExec &exec = getExec();
  char const * const collapsedStateMetadataValues[GraphView::Node::CollapseState_NumStates] = { "0", "1", "2" };
  const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
  for(unsigned int i=0;i<nodes.size();i++)
  {
    if(nodes[i]->type() != GraphView::QGraphicsItemType_Node)
      continue;
    exec.setNodeMetadata(nodes[i]->name().c_str(), "uiCollapsedState", collapsedStateMetadataValues[collapsedState], false, false);
  }
}

QStringList DFGController::getSelectedNodesName() {
  QStringList selectedNodes;

  const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
  for(unsigned int i=0;i<nodes.size();i++)
    selectedNodes.append(nodes[i]->name().c_str());
  return selectedNodes;
}

QStringList DFGController::getSelectedNodesPath() {
  QStringList selectedNodes;

  QString path(getExecPath_QS());
  if(!path.isEmpty()) path += ".";

  const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
  for(unsigned int i=0;i<nodes.size();i++)
    selectedNodes.append(path + nodes[i]->name().c_str());
  return selectedNodes;
}

std::string DFGController::copy()
{
  std::string json;
  try
  {
    const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();

    if (nodes.size() == 0)
    {
      QApplication::clipboard()->clear();
      return "";
    }

    std::vector<std::string> pathStrs;
    pathStrs.reserve( nodes.size() );

    for ( size_t i = 0; i < nodes.size(); ++i )
    {
      if ( nodes[i]->type() != GraphView::QGraphicsItemType_Node )
        continue;
      pathStrs.push_back( nodes[i]->name() );
    }

    std::vector<char const *> pathCStrs;
    pathCStrs.reserve( pathStrs.size() );
    for ( size_t i = 0; i < pathStrs.size(); ++i )
      pathCStrs.push_back( pathStrs[i].c_str() );

    json =
      m_exec.exportNodesJSON(
        pathCStrs.size(),
        &pathCStrs[0]
        ).getCString();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( json.c_str() );
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return json;
}

void DFGController::cmdCut()
{
  if(!validPresetSplit())
    return;

  try
  {
    const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();

    if (nodes.size() == 0)
    {
      QApplication::clipboard()->clear();
      return;
    }

    std::vector<std::string> pathStrs;
    pathStrs.reserve( nodes.size() );

    for ( size_t i = 0; i < nodes.size(); ++i )
    {
      if ( nodes[i]->type() != GraphView::QGraphicsItemType_Node )
        continue;
      pathStrs.push_back( nodes[i]->name() );
    }

    std::vector<char const *> pathCStrs;
    pathCStrs.reserve( pathStrs.size() );
    for ( size_t i = 0; i < pathStrs.size(); ++i )
      pathCStrs.push_back( pathStrs[i].c_str() );

    std::string json =
      m_exec.exportNodesJSON(
        pathCStrs.size(),
        &pathCStrs[0]
        ).getCString();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( json.c_str() );

    QStringList paths;
    paths.reserve( pathStrs.size() );
    for ( size_t i = 0; i < pathStrs.size(); ++i )
      paths.push_back(
        QString::fromUtf8( pathStrs[i].data(), pathStrs[i].size() )
        );

    cmdRemoveNodes( paths );
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

void DFGController::selectNodes(QList<QString> nodeNames) {
  graph()->clearSelection();
  for ( int i = 0; i < nodeNames.size(); ++i )
  {
    if ( FabricUI::GraphView::Node *node = graph()->node( nodeNames[i] ) )
      node->setSelected( true );
  }
}

void DFGController::cmdPaste(bool mapPositionToMouseCursor)
{
  if(!validPresetSplit())
    return;

  try
  {
    QClipboard *clipboard = QApplication::clipboard();
    QString textToPaste = clipboard->text();
    if ( !textToPaste.isEmpty() )
    {
      QPointF pos(0, 0);
      if ( mapPositionToMouseCursor )
      {
        pos = m_dfgWidget->getGraphViewWidget()->mapToGraph( QCursor::pos() );
      }
      else
      {
        // use average top left node positions as pos.
        std::vector<FabricUI::GraphView::Node *> nodes = m_dfgWidget->getGraphViewWidget()->graph()->selectedNodes();
        if ( nodes.size() > 0 )
        {
          for (size_t i=0;i<nodes.size();i++)
            pos += nodes[i]->topLeftGraphPos();
          pos /= (float)nodes.size();
        }
      }

      // paste.
      QList<QString> pastedNodes =
        m_cmdHandler->dfgDoPaste(
          getBinding(),
          getExecPath_QS(),
          getExec(),
          textToPaste,
          pos
          );

      selectNodes(pastedNodes);
    }
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

bool DFGController::reloadExtensionDependencies(char const * path)
{
  FabricCore::DFGExec exec = getExec();
  FTL::StrRef pathRef(path);
  if(pathRef.size() > 0)
    exec = exec.getSubExec(path);

  if(!exec.isValid())
    return false;

  for(unsigned int i = 0; i < exec.getExtDepCount(); i++ )
  {
    char const * ext = exec.getExtDepName(i);
    std::string version = exec.getExtDepVersion(i).getCString();
    try
    {
      m_client.loadExtension(ext, version.c_str(), true);
    }
    catch(FabricCore::Exception e)
    {
      logError(e.getDesc_cstr());
      return false;
    }
  }

  execute();

  return true;
}

void DFGController::onTopoDirty()
{
  updateErrors();
  updateNodeErrors();
  updateTimelinePortIndices();
  setTimelineValuesToGraph();
}

void DFGController::updateErrors()
{
  m_dfgWidget->getErrorsWidget()->onErrorsMayHaveChanged();
}

void DFGController::updateNodeErrors()
{
  // [pzion 20160209] This will force the Core to ensure errors are up-to-date
  (void)m_binding.hasRecursiveConnectedErrors();

  try
  {
    if ( m_exec.getType() == FabricCore::DFGExecType_Graph )
    {
      GraphView::Graph *uiGraph = graph();

      unsigned nodeCount = m_exec.getNodeCount();
      for(size_t j=0;j<nodeCount;j++)
      {
        char const *nodeName = m_exec.getNodeName(j);
        GraphView::Node *uiNode = uiGraph->nodeFromPath( nodeName );
        if ( !uiNode )
          continue;

        FabricCore::String errorsJSON =
          m_exec.getNodeErrors(
            nodeName,
            true, // recursive
            true  // connectedOnly
            );
        FTL::CStrRef errorsJSONStr( errorsJSON.getCStr(), errorsJSON.getSize() );
        FTL::JSONStrWithLoc strWithLoc( errorsJSONStr );
        FTL::OwnedPtr<FTL::JSONArray> errorsJSONArray(
          FTL::JSONValue::Decode( strWithLoc )->cast<FTL::JSONArray>()
          );
        unsigned errorCount = errorsJSONArray->size();
        if ( errorCount == 0 )
          uiNode->clearError();
        else
        {
          QString fullDesc;
          for(unsigned i=0;i<errorCount;i++)
          {
            if ( i == 3 )
            {
              fullDesc += "\n...";
              break;
            }

            FTL::JSONObject const *error = errorsJSONArray->getObject( i );

            FTL::CStrRef execPath = error->getString( FTL_STR("execPath") );
            FTL::CStrRef nodeName = error->getStringOrEmpty( FTL_STR("nodeName") );
            FTL::CStrRef blockName = error->getStringOrEmpty( FTL_STR("blockName") );
            int32_t line = error->getSInt32Or( FTL_STR("line"), -1 );
            int32_t column = error->getSInt32Or( FTL_STR("column"), -1 );
            FTL::CStrRef desc = error->getString( FTL_STR("desc") );

            QString localDesc;
            if ( !execPath.empty() )
              localDesc += execPath.c_str();
            if ( !nodeName.empty() )
            {
              if ( !localDesc.isEmpty() )
                localDesc += '.';
              localDesc += nodeName.c_str();
            }
            if ( !blockName.empty() )
            {
              if ( !localDesc.isEmpty() )
                localDesc += '.';
              localDesc += blockName.c_str();
            }
            if ( line >= 0 )
            {
              if ( !localDesc.isEmpty() )
                localDesc += ' ';
              localDesc += "(line ";
              localDesc += QString::number( line );
              if ( column >= 0 )
              {
                localDesc += ", column ";
                localDesc += QString::number( column );
              }
              localDesc += ')';
            }
            if ( !localDesc.isEmpty() )
              localDesc += ": ";
            localDesc += desc.c_str();

            if ( !fullDesc.isEmpty() )
              fullDesc += '\n';
            fullDesc += localDesc;
          }
          uiNode->setError( fullDesc );
        }
      }
    }
  }
  catch ( FTL::JSONException e )
  {
    std::cout
      << "Caught exception: "
      << e.getDesc()
      << "\n";
  }

  // [pzion 20150701] Upgrade old backdrops scheme
  static bool upgradingBackDrops = false;
  if ( !upgradingBackDrops )
  {
    upgradingBackDrops = true;
    
    char const * uiBackDropsCStr = m_exec.getMetadata( "uiBackDrops" );
    std::string uiBackDrops;
    if(uiBackDropsCStr != NULL)
      uiBackDrops = uiBackDropsCStr;

    if ( !uiBackDrops.empty() )
    {
      std::pair<FTL::StrRef, FTL::CStrRef> split = FTL::CStrRef( uiBackDrops ).split(',');
      while ( !split.first.empty() )
      {
        std::string uiBackDropKey = split.first;
        try
        {
          char const *nodeName = m_exec.addUser( uiBackDropKey.c_str() );

          FTL::CStrRef uiBackDrop = m_exec.getMetadata( uiBackDropKey.c_str() );
          if ( !uiBackDrop.empty() )
          {
            FTL::JSONStrWithLoc swl( uiBackDrop );
            FTL::OwnedPtr<FTL::JSONObject> jo(
              FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
              );

            m_exec.setNodeMetadata(
              nodeName,
              "uiTitle",
              jo->getString( FTL_STR("title") ).c_str(),
              false,
              true
              );

            FTL::JSONObject const *posJO =
              jo->getObject( FTL_STR("pos") );
            std::string posStr = posJO->encode();
            m_exec.setNodeMetadata(
              nodeName, "uiGraphPos", posStr.c_str(), false, false
              );

            FTL::JSONObject const *sizeJO =
              jo->getObject( FTL_STR("size") );
            std::string sizeStr;
            {
              FTL::JSONEnc<> sizeEnc( sizeStr );
              FTL::JSONObjectEnc<> sizeObjEnc( sizeEnc );
              {
                FTL::JSONEnc<> widthEnc( sizeObjEnc, FTL_STR("w") );
                FTL::JSONFloat64Enc<> widthF64Enc(
                  widthEnc, sizeJO->getFloat64( FTL_STR("width") )
                  );
              }
              {
                FTL::JSONEnc<> heightEnc( sizeObjEnc, FTL_STR("h") );
                FTL::JSONFloat64Enc<> heightF64Enc(
                  heightEnc, sizeJO->getFloat64( FTL_STR("height") )
                  );
              }
            }
            m_exec.setNodeMetadata(
              nodeName, "uiGraphSize", sizeStr.c_str(), false, false
              );

            FTL::JSONObject const *colorJO =
              jo->getObject( FTL_STR("color") );
            std::string colorStr = colorJO->encode();
            m_exec.setNodeMetadata(
              nodeName, "uiNodeColor", colorStr.c_str(), false, true
              );

            m_exec.setMetadata( uiBackDropKey.c_str(), "", false, false );
          }
        }
        catch ( ... )
        {
        }
        split = split.second.split(',');
      }
  
      m_exec.setMetadata( "uiBackDrops", "", false, false );
    }
    upgradingBackDrops = false;
  }
}

void DFGController::processDelayedEvents()
{
  if ( m_notificationTimer->isActive() )
  {
    // stop the timer and call the slot directly.
    // (note: we call the slot directly, because in Qt
    //  one cannot directly emit QTimer::timeout from
    //  outside of the class)
    m_notificationTimer->stop();
    onNotificationTimer();
  }
}

void DFGController::log(const char * message) const
{
  DFGLogWidget::log(message);
  if(m_logFunc)
    (*m_logFunc)(message);
}

void DFGController::logError(const char * message) const
{
  std::string m = "Error: ";
  m += message;
  log(m.c_str());
}

void DFGController::setLogFunc(LogFunc func)
{
  m_logFunc = func;
}

void DFGController::execute()
{
  try
  {
    m_binding.execute();
    emit bindingExecuted();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

void DFGController::cmdSetArgValue(
  QString argName,
  FabricCore::RTVal const &value
  )
{
  if(!validPresetSplit())
    return;

  FabricCore::RTVal currentValue =
    m_binding.getArgValue( argName.toUtf8().constData() );
  if ( !currentValue
    || !currentValue.isExEQTo( value ) )
  {
    m_cmdHandler->dfgDoSetArgValue(
      m_binding,
      argName,
      value.copy()
      );
  }
}

void DFGController::cmdSetPortDefaultValue(
  QString portPath,
  FabricCore::RTVal const &value
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoSetPortDefaultValue(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    portPath,
    value.copy()
    );
}

void DFGController::cmdSetRefVarPath(
  FabricCore::DFGBinding &binding,
  QString execPath,
  FabricCore::DFGExec &exec,
  QString refName,
  QString varPath
  )
{
  if(!validPresetSplit())
    return;

  FTL::CStrRef currentVarPath =
    exec.getRefVarPath( refName.toUtf8().constData() );
  if ( QString::fromUtf8( currentVarPath.c_str() ) != varPath )
  {
    m_cmdHandler->dfgDoSetRefVarPath(
      binding,
      execPath,
      exec,
      refName,
      varPath
      );
  }
}

void DFGController::cmdReorderPorts(
  FabricCore::DFGBinding &binding,
  QString execPath,
  FabricCore::DFGExec &exec,
  QString itemPath,
  QList<int> indices
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoReorderPorts(
    binding,
    execPath,
    exec,
    itemPath,
    indices
    );
}

void DFGController::cmdSetExtDeps(
  QStringList nameAndVers
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoSetExtDeps(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nameAndVers
    );
}

void DFGController::cmdSplitFromPreset()
{
  m_cmdHandler->dfgDoSplitFromPreset(
    getBinding(),
    getExecPath_QS(),
    getExec()
    );
}

bool DFGController::canConnectTo(
  char const *pathA,
  char const *pathB,
  std::string &failureReason
  ) const
{
  try
  {
    FabricCore::DFGStringResult result =
      m_exec.canConnectTo( pathA, pathB, "\n" );
    char const *resultData;
    uint32_t resultSize;
    result.getStringDataAndLength( resultData, resultSize );
    if ( resultSize > 0 )
    {
      failureReason = std::string( resultData, resultSize );
      return false;
    }
    else return true;
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

void DFGController::onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button)
{
  Controller::onNodeHeaderButtonTriggered(button);
  GraphView::Node * node = button->header()->node();  
  if(button->name() == "node_collapse")
  {
    setNodeCollapseState((int)node->collapsedState(), node);
  }
  else if(button->name() == "node_edit")
  {
    emit nodeEditRequested(node);
  }
}

void DFGController::onVariablesChanged()
{
  m_presetDictsUpToDate = false;
}

void DFGController::onBindingDirty()
{
  emitDirty();
}

void DFGController::onBindingArgInserted(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef typeName
  )
{
  emitArgsChanged();
}

void DFGController::onBindingArgTypeChanged(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef newTypeName
  )
{
  emitArgsChanged();
}

void DFGController::onBindingTopoDirty()
{
  emitTopoDirty();
}

void DFGController::onBindingArgRemoved(
  unsigned index,
  FTL::CStrRef name
  )
{
  emitArgsChanged();
}

void DFGController::onBindingArgsReordered(
  FTL::ArrayRef<unsigned> newOrder
  )
{
  emitArgsChanged();
}

void DFGController::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name
  )
{
  emitArgValuesChanged();
}

void DFGController::onBindingVarInserted(
  FTL::CStrRef varName,
  FTL::CStrRef varPath,
  FTL::CStrRef typeName,
  FTL::CStrRef extDep
  )
{
  emitVarsChanged();
}

void DFGController::onBindingVarRemoved(
  FTL::CStrRef varName,
  FTL::CStrRef varPath
  )
{
  emitVarsChanged();
}

FabricServices::SplitSearch::Matches
DFGController::getPresetPathsFromSearch( char const * search )
{
  FTL::StrRef searchRef(search);
  if(searchRef.size() == 0)
    return FabricServices::SplitSearch::Matches();

  updatePresetPathDB();

  // [pzion 20150305] This is a little evil but avoids lots of copying

  std::string stdString = search;

  std::vector<FTL::StrRef> searchSplit;
  FTL::StrSplit<'.'>( stdString, searchSplit, true /*strict*/ );

  FTL::StrRemap< FTL::MapCharSingle<'.', '\0'> >( stdString );

  char const **cStrs = reinterpret_cast<char const **>(
    alloca( searchSplit.size() * sizeof( char const * ) )
    );
  for ( size_t i = 0; i < searchSplit.size(); ++i )
    cStrs[i] = searchSplit[i].data();

  return m_presetPathDict.search( searchSplit.size(), cStrs );
}

void DFGController::appendPresetsAtPrefix(
  std::string &prefixedName,
  FTL::JSONStr &ds
  )
{
  FTL::JSONObjectDec<FTL::JSONStr> jod( ds );
  FTL::JSONEnt<FTL::JSONStr> jeKey, jeVal;
  while ( jod.getNext( jeKey, jeVal ) )
  {
    if ( jeKey.stringIs( FTL_STR("objectType") )
      && jeVal.stringIs( FTL_STR("Preset") ) )
    {
      m_presetPathDictSTL.push_back(
        std::pair<std::string, unsigned>( prefixedName, 0 )
        );
    }
    else if ( jeKey.stringIs( FTL_STR("members") ) )
    {
      FTL::JSONStr ds( jeVal.getRawJSONStr() );
      FTL::JSONObjectDec<FTL::JSONStr> jod( ds );
      FTL::JSONEnt<FTL::JSONStr> jeKey, jeVal;
      while ( jod.getNext( jeKey, jeVal ) )
      {
        size_t oldPrefixedNameSize = prefixedName.size();
        if ( !prefixedName.empty() )
          prefixedName += '.';
        jeKey.stringAppendTo( prefixedName );
        m_presetNameSpaceDictSTL.push_back( prefixedName );

        FTL::JSONStr ds( jeVal.getRawJSONStr() );
        appendPresetsAtPrefix( prefixedName, ds );

        prefixedName.resize( oldPrefixedNameSize );
      }
    }
  }
}

void DFGController::updatePresetPathDB()
{
  if(m_presetDictsUpToDate || !m_host.isValid())
    return;
  m_presetDictsUpToDate = true;

  m_presetNameSpaceDict.clear();
  m_presetPathDict.clear();
  m_presetNameSpaceDictSTL.clear();
  m_presetPathDictSTL.clear();

  // insert fixed results for special nodes
  m_presetPathDictSTL.push_back( std::pair<std::string, unsigned>( "var",      2 ) );
  m_presetPathDictSTL.push_back( std::pair<std::string, unsigned>( "get",      2 ) );
  m_presetPathDictSTL.push_back( std::pair<std::string, unsigned>( "set",      2 ) );
  m_presetPathDictSTL.push_back( std::pair<std::string, unsigned>( "backdrop", 2 ) );

  QStringList variables =
    DFGBindingUtils::getVariableWordsFromBinding(
      m_binding,
      m_execPath.c_str()
      );
  for(int i=0;i<variables.length();i++)
  {
    m_presetPathDictSTL.push_back( std::pair<std::string, unsigned>(
      "get." + std::string(variables[i].toUtf8().constData()), 1
      ) );
    m_presetPathDictSTL.push_back( std::pair<std::string, unsigned>(
      "set." + std::string(variables[i].toUtf8().constData()), 1
      ) );
  }

  FabricCore::String jsonString = m_host.getPresetDesc( "" );
  char const *jsonStrCStr;
  uint32_t jsonStrSize;
  jsonString.getCStrAndSize( jsonStrCStr, jsonStrSize );
  FTL::JSONStr ds( FTL::StrRef( jsonStrCStr, jsonStrSize ) );
  std::string prefixedName;
  appendPresetsAtPrefix( prefixedName, ds );

  for(size_t i=0;i<m_presetNameSpaceDictSTL.size();i++)
    m_presetNameSpaceDict.add(m_presetNameSpaceDictSTL[i].c_str(), '.', m_presetNameSpaceDictSTL[i].c_str());
  for(size_t i=0;i<m_presetPathDictSTL.size();i++)
    m_presetPathDict.add(
      m_presetPathDictSTL[i].first.c_str(),
      '.',
      m_presetPathDictSTL[i].first.c_str(),
      m_presetPathDictSTL[i].second
      );

  m_presetPathDict.loadPrefs( m_tabSearchPrefsJSONFilename.c_str() );
}

DFGNotificationRouter * DFGController::createRouter()
{
  return new DFGNotificationRouter( this );
}

void DFGController::cmdRemoveNodes(
  QStringList nodeNames
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoRemoveNodes(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeNames
    );
}

void DFGController::cmdConnect(
  QStringList srcPaths, 
  QStringList dstPaths
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoConnect(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    srcPaths,
    dstPaths
    );
}

void DFGController::cmdDisconnect(
  QStringList srcPaths, 
  QStringList dstPaths
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoDisconnect(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    srcPaths,
    dstPaths
    );
}

QString DFGController::cmdAddInstWithEmptyGraph(
  QString title,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoAddGraph(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    title,
    pos
    );
}

QString DFGController::cmdAddInstFromJSON(
  QString nodeName,
  QString filePath,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoImportNodeFromJSON(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeName,
    filePath,
    pos
    );
}

QString DFGController::cmdAddInstWithEmptyFunc(
  QString title,
  QString initialCode,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoAddFunc(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    title,
    initialCode,
    pos
    );
}

QString DFGController::cmdAddInstFromPreset(
  QString presetPath,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoInstPreset(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    presetPath,
    pos
    );
}

QString DFGController::cmdAddBlock(
  QString desiredName,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoAddBlock(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    desiredName,
    pos
    );
}

QString DFGController::cmdAddVar(
  QString desiredNodeName,
  QString dataType,
  QString extDep,
  QPointF pos
  )
{
  if ( !validPresetSplit()
     || desiredNodeName.isEmpty()
     || dataType.isEmpty() )
    return QString();

  return m_cmdHandler->dfgDoAddVar(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    desiredNodeName,
    dataType,
    extDep,
    pos
    );
}

QString DFGController::cmdAddGet(
  QString desiredNodeName,
  QString varPath,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoAddGet(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    desiredNodeName,
    varPath,
    pos
    );
}

QString DFGController::cmdAddSet(
  QString desiredNodeName,
  QString varPath,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoAddSet(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    desiredNodeName,
    varPath,
    pos
    );
}

QString DFGController::cmdAddPort(
  QString desiredPortName,
  FabricCore::DFGPortType portType,
  QString typeSpec,
  QString portToConnect,
  QString extDep,
  QString metaData
  )
{
  if(!validPresetSplit())
    return QString();

  return m_cmdHandler->dfgDoAddPort(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    desiredPortName,
    portType,
    typeSpec,
    portToConnect,
    extDep,
    metaData
    );
}

QString DFGController::cmdCreatePreset(
  QString nodeName,
  QString presetDirPath,
  QString presetName,
  bool updateOrigPreset
  )
{
  return m_cmdHandler->dfgDoCreatePreset(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeName,
    presetDirPath,
    presetName,
    updateOrigPreset
    );
}

QString DFGController::cmdEditPort(
  QString oldPortName,
  QString desiredNewPortName,
  FabricCore::DFGPortType portType,
  QString typeSpec,
  QString extDep,
  QString uiMetadata
  )
{
  if(!validPresetSplit())
    return QString();

  return m_cmdHandler->dfgDoEditPort(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    oldPortName,
    desiredNewPortName,
    portType,
    typeSpec,
    extDep,
    uiMetadata
    );
}

void DFGController::cmdRemovePort(
  QStringList portNames
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoRemovePort(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    portNames
    );
}

void DFGController::cmdMoveNodes(
  QStringList nodeNames,
  QList<QPointF> newTopLeftPoss
  )
{
  m_cmdHandler->dfgDoMoveNodes(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeNames,
    newTopLeftPoss
    );
}

void DFGController::cmdResizeBackDropNode(
  QString backDropNodeName,
  QPointF newTopLeftPos,
  QSizeF newSize
  )
{
  m_cmdHandler->dfgDoResizeBackDrop(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    backDropNodeName,
    newTopLeftPos,
    newSize
    );
}

QString DFGController::cmdImplodeNodes(
  QStringList nodeNames,
  QString desiredNodeName
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoImplodeNodes(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeNames,
    desiredNodeName
    );
}

QList<QString> DFGController::cmdExplodeNode(
  QString nodeName
  )
{
  if(!validPresetSplit())
    return QList<QString>();

  return m_cmdHandler->dfgDoExplodeNode(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeName
    );
}

void DFGController::gvcDoMoveNode(
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
  )
{
  if (!node)
    return;

  FTL::CStrRef nodeName = node->name();

  QPointF finalPos = nodeOriginalPos + delta;

  // snap to grid/node/port.
  if (    gridSnapSize > 0
      || (nodeSnapPositions.size() && nodeSnapDistance > 0)
      || (portSnapPositionsSrcY.size() && portSnapPositionsDstY.size() && portSnapDistance > 0)
     )
  {
    QPointF nodeSize = QPointF(nodeOriginalSize.width(), nodeOriginalSize.height());
    
    // snap to grid and/nodes.
    qreal smallestYdelta = 10e+10;
    if (    gridSnapSize > 0
        || (nodeSnapPositions.size() && nodeSnapDistance > 0)
        || (portSnapPositionsSrcY.size() && portSnapPositionsDstY.size() && portSnapDistance > 0)
       )
    {
      std::vector<QPointF> newPos;
      std::vector<QPointF> newPosOffset;
      std::vector<QPointF> newPosSnapped;
      std::vector<QPointF> newPosDelta;
    
      // add topleft, center and bottomright positions to newPos/newPosOffset.
      newPosOffset.push_back(0.0 * nodeSize);
      newPosOffset.push_back(0.5 * nodeSize);
      newPosOffset.push_back(1.0 * nodeSize);
      for (size_t i=0;i<newPosOffset.size();i++)
        newPos.push_back(finalPos + newPosOffset[i]);

      // calculate snapped positions and delta to the previous position.
      for (size_t i = 0; i<newPos.size(); i++)
      {
        // grid.
        if (gridSnapSize > 0)
        {
          newPosSnapped.push_back(snapToGrid(newPos[i], gridSnapSize));
          newPosDelta.push_back(newPos[i] - newPosSnapped[i]);
          newPosDelta[i].setX(fabs(newPosDelta[i].x()));
          newPosDelta[i].setY(fabs(newPosDelta[i].y()));
        }
        else
        {
          newPosSnapped.push_back(newPos[i]);
          newPosDelta.push_back(QPointF(10e+10, 10e+10));
        }

        // node.
        if (nodeSnapDistance > 0)
        {
          for (size_t j = 0; j<nodeSnapPositions.size(); j++)
          {
            qreal dx = fabs(newPos[i].x() - nodeSnapPositions[j].x());
            qreal dy = fabs(newPos[i].y() - nodeSnapPositions[j].y());
            if (dx < nodeSnapDistance && dx < newPosDelta[i].x())
            {
              newPosSnapped[i].setX(nodeSnapPositions[j].x());
              newPosDelta[i].setX(dx);
            }
            if (dy < nodeSnapDistance && dy < newPosDelta[i].y())
            {
              newPosSnapped[i].setY(nodeSnapPositions[j].y());
              newPosDelta[i].setY(dy);
            }
          }
        }
      }

      // get index of closest X and Y snapping position.
      int closestXindex = 0;
      int closestYindex = 0;
     for (size_t i = 0; i<newPosDelta.size(); i++)
     {
        if (newPosDelta[i].x() < newPosDelta[closestXindex].x())  closestXindex = i;
        if (newPosDelta[i].y() < newPosDelta[closestYindex].y())  closestYindex = i;
     }
    
     // set final position from closest snapping position.
     finalPos.setX(newPosSnapped[closestXindex].x() - newPosOffset[closestXindex].x());
     finalPos.setY(newPosSnapped[closestYindex].y() - newPosOffset[closestYindex].y());
    }

    // snap to ports (note: this only affects the y position).
    if (   portSnapPositionsSrcY.size()
        && portSnapPositionsDstY.size()
        && portSnapDistance > 0
        && smallestYdelta > 0
       )
    {
      int closestSrcYindex = -1;
      int closestDstYindex = -1;
      for (size_t i = 0; i<portSnapPositionsSrcY.size() && smallestYdelta > 0; i++)
      {
        for (size_t j = 0; j<portSnapPositionsDstY.size() && smallestYdelta > 0; j++)
        {
          qreal dy = fabs(portSnapPositionsSrcY[i] + delta.y() - portSnapPositionsDstY[j]);
          if (dy < portSnapDistance && dy < smallestYdelta)
          {
            smallestYdelta = dy;
            closestSrcYindex = i;
            closestDstYindex = j;
          }
        }
      }
      if (closestDstYindex >= 0)
      {
        finalPos.setY(nodeOriginalPos.y() + (portSnapPositionsDstY[closestDstYindex] - portSnapPositionsSrcY[closestSrcYindex]));
      }
    }
  }


  if ( allowUndo )
  {
    QStringList nodeNames;
    nodeNames.push_back(QString::fromUtf8(nodeName.data(), nodeName.size()));

    QList<QPointF> newTopLeftPoss;
    newTopLeftPoss.append(finalPos);

    cmdMoveNodes(nodeNames, newTopLeftPoss);
  }
  else
  {
    std::string newPosJSON;
    {
      FTL::JSONEnc<> enc(newPosJSON);
      FTL::JSONObjectEnc<> objEnc(enc);
      {
        FTL::JSONEnc<> xEnc(objEnc, FTL_STR("x"));
        FTL::JSONFloat64Enc<> xS32Enc(xEnc, finalPos.x());
      }
      {
        FTL::JSONEnc<> yEnc(objEnc, FTL_STR("y"));
        FTL::JSONFloat64Enc<> yS32Enc(yEnc, finalPos.y());
      }
    }
    getExec().setItemMetadata(
      nodeName.c_str(),
      "uiGraphPos",
      newPosJSON.c_str(),
      false,
      false
      );
  }
}

void DFGController::gvcDoMoveNodes(
  std::vector<GraphView::Node *> const &nodes,
  std::vector<QPointF> const &nodesOriginalPos,
  QPointF delta,
  float gridSnapSize,
  bool allowUndo
  )
{
  if (   nodes.size() == 0
      || nodes.size() != nodesOriginalPos.size() )
    return;

  // apply the grid snapping to the delta only.
  // Note: this function is designed to move several nodes.
  // For more advanced grid and port snapping use
  // the function void gvcDoMoveNode().
  if (gridSnapSize > 0)
    delta = snapToGrid(delta, gridSnapSize);

  if ( allowUndo )
  {
    QStringList nodeNames;
    QList<QPointF> newTopLeftPoss;
    nodeNames.reserve( nodes.size() );
    newTopLeftPoss.reserve( nodes.size() );
    int i = 0;
    for ( std::vector<GraphView::Node *>::const_iterator it = nodes.begin();
      it != nodes.end(); ++it, ++i )
    {
      GraphView::Node *node = *it;
      FTL::CStrRef nodeName = node->name();
      nodeNames.append( QString::fromUtf8( nodeName.data(), nodeName.size() ) );
      QPointF newPos = nodesOriginalPos[i] + delta;
      newTopLeftPoss.append( newPos );
    }
    
    cmdMoveNodes( nodeNames, newTopLeftPoss );
  }
  else
  {
    int i = 0;
    for ( std::vector<GraphView::Node *>::const_iterator it = nodes.begin();
      it != nodes.end(); ++it, ++i )
    {
      GraphView::Node *node = *it;
      FTL::CStrRef nodeName = node->name();
      QPointF newPos = nodesOriginalPos[i] + delta;
      std::string newPosJSON;
      {
        FTL::JSONEnc<> enc( newPosJSON );
        FTL::JSONObjectEnc<> objEnc( enc );
        {
          FTL::JSONEnc<> xEnc( objEnc, FTL_STR("x") );
          FTL::JSONFloat64Enc<> xS32Enc( xEnc, newPos.x() );
        }
        {
          FTL::JSONEnc<> yEnc( objEnc, FTL_STR("y") );
          FTL::JSONFloat64Enc<> yS32Enc( yEnc, newPos.y() );
        }
      }

      getExec().setItemMetadata(
        nodeName.c_str(),
        "uiGraphPos",
        newPosJSON.c_str(),
        false,
        false
        );
    }
  }
}

void DFGController::gvcDoResizeBackDropNode(
  GraphView::BackDropNode *backDropNode,
  QPointF newTopLeftPos,
  QSizeF newSize,
  float gridSnapSize,
  bool gridSnapTop,
  bool gridSnapBottom,
  bool gridSnapLeft,
  bool gridSnapRight,
  bool allowUndo
  )
{
  if (gridSnapSize > 0)
    if (gridSnapTop || gridSnapBottom || gridSnapLeft || gridSnapRight)
    {
      QPointF newBottomRightPos = newTopLeftPos + QPointF(newSize.width(), newSize.height());
      if (gridSnapTop)
        newTopLeftPos.setY(gridSnapSize * qRound(newTopLeftPos.ry() / gridSnapSize));
      if (gridSnapBottom)
        newBottomRightPos.setY(gridSnapSize * qRound(newBottomRightPos.ry() / gridSnapSize));
      if (gridSnapLeft)
        newTopLeftPos.setX(gridSnapSize * qRound(newTopLeftPos.rx() / gridSnapSize));
      if (gridSnapRight)
        newBottomRightPos.setX(gridSnapSize * qRound(newBottomRightPos.rx() / gridSnapSize));
      newSize.setWidth(newBottomRightPos.x() - newTopLeftPos.x());
      newSize.setHeight(newBottomRightPos.y() - newTopLeftPos.y());
    }

  if ( allowUndo )
  {
    cmdResizeBackDropNode(
      QString::fromUtf8( backDropNode->name().c_str() ),
      newTopLeftPos,
      newSize
      );
  }
  else
  {
    {
      std::string newPosJSON;
      {
        FTL::JSONEnc<> enc( newPosJSON );
        FTL::JSONObjectEnc<> objEnc( enc );
        {
          FTL::JSONEnc<> xEnc( objEnc, FTL_STR("x") );
          FTL::JSONFloat64Enc<> xS32Enc( xEnc, newTopLeftPos.x() );
        }
        {
          FTL::JSONEnc<> yEnc( objEnc, FTL_STR("y") );
          FTL::JSONFloat64Enc<> yS32Enc( yEnc, newTopLeftPos.y() );
        }
      }

      getExec().setNodeMetadata(
        backDropNode->name().c_str(),
        "uiGraphPos",
        newPosJSON.c_str(),
        false,
        false
        );
    }

    {
      std::string newSizeJSON;
      {
        FTL::JSONEnc<> enc( newSizeJSON );
        FTL::JSONObjectEnc<> objEnc( enc );
        {
          FTL::JSONEnc<> wEnc( objEnc, FTL_STR("w") );
          FTL::JSONFloat64Enc<> wS32Enc( wEnc, newSize.width() );
        }
        {
          FTL::JSONEnc<> hEnc( objEnc, FTL_STR("h") );
          FTL::JSONFloat64Enc<> hS32Enc( hEnc, newSize.height() );
        }
      }

      getExec().setNodeMetadata(
        backDropNode->name().c_str(),
        "uiGraphSize",
        newSizeJSON.c_str(),
        false,
        false
        );
    }
  }
}

void DFGController::setBlockCompilations( bool blockCompilations )
{
  if ( blockCompilations )
    m_host.blockComps();
  else
  {
    m_host.unblockComps();
    emitArgsChanged();
    emit varsChanged();
  }
}

void DFGController::focusNode( FTL::StrRef nodeName )
{
  if ( m_exec.getType() == FabricCore::DFGExecType_Graph )
  {
    GraphView::Graph *gvGraph = graph();
    gvGraph->clearSelection();
    if ( GraphView::Node *gvNode = gvGraph->node( nodeName ) )
    {
      gvNode->setSelected( true );
      frameSelectedNodes();
    }
  }
}

void DFGController::onNotificationTimer()
{
  if ( m_varsChangedPending )
  {
    m_varsChangedPending = false;
    emit varsChanged();
  }
  if ( m_argsChangedPending )
  {
    m_argsChangedPending = false;
    emit argsChanged();
  }
  if ( m_argValuesChangedPending )
  {
    m_argValuesChangedPending = false;
    emit argValuesChanged();
  }
  if ( m_defaultValuesChangedPending )
  {
    m_defaultValuesChangedPending = false;
    emit defaultValuesChanged();
  }
  if ( m_topoDirtyPending )
  {
    m_topoDirtyPending = false;
    emit topoDirty();

    // [pzion 20160405] FE-6269
    // topoDirty implies dirty
    m_dirtyPending = false;
  }
  if ( m_dirtyPending )
  {
    m_dirtyPending = false;
    emit dirty();
  }
}

void DFGController::gvcDoMoveExecPort(
  QString srcName,
  QString dstName
  )
{
  if ( !validPresetSplit() )
    return;

  QList<int> indices;

  unsigned execPortCount = m_exec.getExecPortCount();
  unsigned srcPortIndex =
    m_exec.getExecPortIndex( srcName.toUtf8().constData() );
  for ( unsigned i = 0; i < execPortCount; ++i )
  {
    FTL::CStrRef execPortName = m_exec.getExecPortName( i );
    if ( i == srcPortIndex )
      continue;
    if ( !dstName.isEmpty()
      && dstName == execPortName.c_str() )
      indices.append( srcPortIndex );
    indices.append( i );
  }
  if ( dstName.isEmpty() )
    indices.append( srcPortIndex );

  // Ensure that the permutation is non-trivial
  bool trivial = true;
  for ( unsigned i = 0; i < execPortCount; ++i )
  {
    if ( indices[i] != int(i) )
    {
      trivial = false;
      break;
    }
  }
  if ( !trivial )
    m_cmdHandler->dfgDoReorderPorts(
      m_binding,
      QString::fromUtf8( m_execPath.data(), m_execPath.size() ),
      m_exec,
      QString(), // itemPath
      indices
      );
}

QString DFGController::gvcGetCurrentExecPath()
{
  return getExecPath_QS();
}

bool DFGController::gvcCurrentExecIsInstBlockExec()
{
  return m_exec.isInstBlockExec();
}

void DFGController::savePrefs()
{
  if ( m_presetDictsUpToDate )
    m_presetPathDict.savePrefs( m_tabSearchPrefsJSONFilename.c_str() );
}

void DFGController::onParentExecNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName
  )
{
  m_execPath = m_exec.getExecPath().getCString();
  m_dfgWidget->onExecPathOrTitleChanged();
}

int DFGController::getTimelinePortIndex( const std::string& name )
{
  int index = -1;
  try
  {
    FabricCore::DFGExec graph = getExec();
    unsigned portCount = graph.getExecPortCount();
    for( unsigned i = 0; i < portCount; i++ )
    {
      if( graph.getExecPortType( i ) == FabricCore::DFGPortType_Out )
        continue;
      FTL::CStrRef portName = graph.getExecPortName( i );
      if( portName != name )
        continue;
      if( !graph.isExecPortResolvedType( i, "SInt32" )
        && !graph.isExecPortResolvedType( i, "UInt32" )
        && !graph.isExecPortResolvedType( i, "Float32" )
        && !graph.isExecPortResolvedType( i, "Float64" ) )
        continue;
      index = int( i );
      break;
    }
  }
  catch( FabricCore::Exception e )
  {
    log( e.getDesc_cstr() );
  }
  return index;
}

void DFGController::setTimelinePortValue( int portIndex, float value )
{
  if( portIndex == -1 )
    return;

  try
  {
    FabricCore::DFGBinding binding = this->getBinding();
    FabricCore::DFGExec exec = binding.getExec();
    FabricCore::Context ctxt = binding.getHost().getContext();

    if( exec.isExecPortResolvedType( portIndex, "SInt32" ) )
      binding.setArgValue(
        portIndex,
        FabricCore::RTVal::ConstructSInt32( ctxt, int(value) ),
        false
      );
    else if( exec.isExecPortResolvedType( portIndex, "UInt32" ) )
      binding.setArgValue(
        portIndex,
        FabricCore::RTVal::ConstructUInt32( ctxt, int(value) ),
        false
      );
    else if( exec.isExecPortResolvedType( portIndex, "Float32" ) )
      binding.setArgValue(
        portIndex,
        FabricCore::RTVal::ConstructFloat32( ctxt, value ),
        false
      );
    else if( exec.isExecPortResolvedType( portIndex, "Float64" ) )
      binding.setArgValue(
        portIndex,
        FabricCore::RTVal::ConstructFloat64( ctxt, value ),
        false
      );
  }
  catch( FabricCore::Exception e )
  {
    log( e.getDesc_cstr() );
  }
}

void DFGController::resetTimelinePortIndices()
{
  m_timelinePortIndex = -1;
  m_timelineStartPortIndex = -1;
  m_timelineEndPortIndex = -1;
  m_timelineFrameratePortIndex = -1;
}

void DFGController::updateTimelinePortIndices()
{
  if( this->isViewingRootGraph() )
  {
    m_timelinePortIndex = getTimelinePortIndex( "timeline" );
    m_timelineStartPortIndex = getTimelinePortIndex( "timelineStart" );
    m_timelineEndPortIndex = getTimelinePortIndex( "timelineEnd" );
    m_timelineFrameratePortIndex = getTimelinePortIndex( "timelineFramerate" );
  }
}

void DFGController::onFrameChanged( int frame )
{
  m_timelineFrame = frame;
  this->setTimelinePortValue( m_timelinePortIndex, m_timelineFrame );
}

void DFGController::onTimelineRangeChanged( int start, int end )
{
  m_timelineStart = start;
  m_timelineEnd = end;
  this->setTimelinePortValue( m_timelineStartPortIndex, m_timelineStart );
  this->setTimelinePortValue( m_timelineEndPortIndex, m_timelineEnd );
}

void DFGController::onTimelineTargetFramerateChanged( float frameRate )
{
  m_timelineFramerate = frameRate;
  this->setTimelinePortValue( m_timelineFrameratePortIndex, m_timelineFramerate );
}

void DFGController::setTimelineValuesToGraph()
{
  this->setTimelinePortValue( m_timelinePortIndex, m_timelineFrame );
  this->setTimelinePortValue( m_timelineStartPortIndex, m_timelineStart );
  this->setTimelinePortValue( m_timelineEndPortIndex, m_timelineEnd );
  this->setTimelinePortValue( m_timelineFrameratePortIndex, m_timelineFramerate );
}

static void GetDesiredPortNameAndTypeSpecForConnectionTarget(
  GraphView::ConnectionTarget *target,
  FTL::StrRef &desiredPortNameStr,
  FTL::StrRef &typeSpecStr
)
{
  if( !target )
    return;

  switch( target->targetType() )
  {
  case TargetType_Pin:
  {
    Pin *targetPin = static_cast<Pin *>( target );
    desiredPortNameStr = targetPin->name();
    typeSpecStr = targetPin->dataType();
  }
  break;

  case TargetType_Port:
  {
    Port *targetPort = static_cast<Port *>( target );
    desiredPortNameStr = targetPort->name();
    typeSpecStr = targetPort->dataType();
  }
  break;

  case TargetType_FixedPort:
  {
    FixedPort *targetFixedPort = static_cast<FixedPort *>( target );
    desiredPortNameStr = targetFixedPort->name();
    typeSpecStr = targetFixedPort->dataType();
  }
  break;

  case TargetType_InstBlockPort:
  {
    InstBlockPort *targetInstBlockPort = static_cast<InstBlockPort *>( target );
    desiredPortNameStr = targetInstBlockPort->name();
    typeSpecStr = targetInstBlockPort->dataType();
  }
  break;

  default:
    assert( false );
    break;
  }
}

ExposePortAction::ExposePortAction(
  QObject *parent,
  FabricUI::DFG::DFGController *dfgController,
  GraphView::ConnectionTarget *other,
  GraphView::PortType connectionPortType
)
  : QAction( parent )
  , m_dfgController( dfgController )
  , m_other( other )
  , m_connectionPortType( connectionPortType )
{
  setText( "Expose new port" );
  setIcon( FabricUI::LoadPixmap( "DFGPlus.png" ).scaledToWidth( 20, Qt::SmoothTransformation ) );
  connect(
    this, SIGNAL( triggered() ),
    this, SLOT( onTriggered() )
  );
}

void ExposePortAction::onTriggered()
{
  FTL::StrRef defaultDesiredPortNameStr;
  FTL::StrRef defaultTypeSpecStr;
  GetDesiredPortNameAndTypeSpecForConnectionTarget(
    m_other,
    defaultDesiredPortNameStr,
    defaultTypeSpecStr
  );

  FabricUI::DFG::DFGEditPortDialog dialog(
    m_dfgController->getDFGWidget(),
    m_dfgController->getClient(),
    allowNonInPortType(), // showPortType
    true, // canEditPortType
    m_dfgController->getDFGWidget()->getConfig(),
    true
  );

  dialog.setTitle(
    QString::fromUtf8(
      defaultDesiredPortNameStr.data(),
      defaultDesiredPortNameStr.size()
    )
  );
  dialog.setDataType(
    QString::fromUtf8(
      defaultTypeSpecStr.data(),
      defaultTypeSpecStr.size()
    )
  );
  if( m_connectionPortType == FabricUI::GraphView::PortType_Output )
    dialog.setPortType( "In" );
  else
    dialog.setPortType( "Out" );

  if( dialog.exec() != QDialog::Accepted )
    return;

  std::string metaData;
  {
    FTL::JSONEnc<> metaDataEnc( metaData );
    FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );
    if( dialog.hidden() )
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiHidden", "true" );
    if( dialog.opaque() )
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiOpaque", "true" );

    if( dialog.persistValue() )
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, DFG_METADATA_UIPERSISTVALUE, "true" );

    if( dialog.hasSoftRange() )
    {
      QString range = "(" + QString::number( dialog.softRangeMin() ) + ", " + QString::number( dialog.softRangeMax() ) + ")";
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", range.toUtf8().constData() );
    }
    else
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", "" );

    if( dialog.hasHardRange() )
    {
      QString range = "(" + QString::number( dialog.hardRangeMin() ) + ", " + QString::number( dialog.hardRangeMax() ) + ")";
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", range.toUtf8().constData() );
    }
    else
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", "" );

    if( dialog.hasCombo() )
    {
      QStringList combo = dialog.comboValues();
      QString flat = "(";
      for( int i = 0; i<combo.length(); i++ )
      {
        if( i > 0 )
          flat += ", ";
        flat += "\"" + combo[i] + "\"";
      }
      flat += ")";
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiCombo", flat.toUtf8().constData() );
    }
  }

  QString desiredPortName = dialog.title();
  QString typeSpec = dialog.dataType();
  QString extDep = dialog.extension();
  QString dialogPortType = dialog.portType();
  FabricCore::DFGPortType portType = FabricCore::DFGPortType_Out;
  if( dialogPortType.isEmpty() || dialogPortType == "In" )
    portType = FabricCore::DFGPortType_In;
  else if( dialogPortType == "IO" )
    portType = FabricCore::DFGPortType_IO;

  if( metaData == "{}" )
    metaData = "";

  invokeAddPort(
    desiredPortName,
    portType,
    typeSpec,
    extDep,
    QString::fromUtf8( metaData.data(), metaData.size() )
  );
}

class ExposeInstPortAction : public ExposePortAction
{
public:

  ExposeInstPortAction(
    QObject *parent,
    FabricUI::DFG::DFGController *dfgController,
    GraphView::Node *node,
    GraphView::ConnectionTarget *other,
    GraphView::PortType connectionPortType
  )
    : ExposePortAction( parent, dfgController, other, connectionPortType )
    , m_node( node )
  {
  }

protected:

  virtual void invokeAddPort(
    QString desiredPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString extDep,
    QString metaData
  )
  {
    FabricUI::DFG::DFGUICmdHandler *cmdHandler =
      m_dfgController->getCmdHandler();
    switch( m_node->getNodeType() )
    {
    case Node::NodeType_Inst:
      cmdHandler->dfgDoAddInstPort(
        m_dfgController->getBinding(),
        m_dfgController->getExecPath_QS(),
        m_dfgController->getExec(),
        m_node->name_QS(),
        desiredPortName,
        portType,
        typeSpec,
        m_other ? m_other->path_QS() : QString(),
        PortTypeToDFGPortType( m_connectionPortType ),
        extDep,
        metaData
      );
      break;

    case Node::NodeType_Block:
      cmdHandler->dfgDoAddBlockPort(
        m_dfgController->getBinding(),
        m_dfgController->getExecPath_QS(),
        m_dfgController->getExec(),
        m_node->name_QS(),
        desiredPortName,
        portType,
        typeSpec,
        m_other ? m_other->path_QS() : QString(),
        PortTypeToDFGPortType( m_connectionPortType ),
        extDep,
        metaData
      );
      break;

    default:
      assert( false );
      break;
    }
  }

private:

  GraphView::Node *m_node;
};

class ExposeInstBlockPortAction : public ExposePortAction
{
public:

  ExposeInstBlockPortAction(
    QObject *parent,
    FabricUI::DFG::DFGController *dfgController,
    GraphView::InstBlock *instBlock,
    GraphView::ConnectionTarget *other
  )
    : ExposePortAction( parent, dfgController, other, PortType_Output )
    , m_instBlock( instBlock )
  {
  }

protected:

  virtual bool allowNonInPortType() const
  { return false; }

  virtual void invokeAddPort(
    QString desiredPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString extDep,
    QString metaData
  )
  {
    assert( portType == FabricCore::DFGPortType_In );
    FabricUI::DFG::DFGUICmdHandler *cmdHandler =
      m_dfgController->getCmdHandler();
    cmdHandler->dfgDoAddInstBlockPort(
      m_dfgController->getBinding(),
      m_dfgController->getExecPath_QS(),
      m_dfgController->getExec(),
      m_instBlock->node()->name_QS(),
      m_instBlock->name_QS(),
      desiredPortName,
      typeSpec,
      m_other ? m_other->path_QS() : QString(),
      extDep,
      metaData
    );
  }

private:

  GraphView::InstBlock *m_instBlock;
};

QMenu* DFGController::gvcCreateNodeHeaderMenu(
  Node* node,
  ConnectionTarget *other,
  PortType nodeRole
)
{
  QMenu *menu = new QMenu( this->getDFGWidget() );

  // go through all the node's pins and add
  // those to the menu that can be connected.
  for( unsigned int i = 0; i<node->pinCount(); i++ )
  {
    Pin *pin = node->pin( i );

    if( nodeRole == PortType_Output && pin->portType() == PortType_Output )
      continue; // skip this pin (an output port cannot be connected with another output port).

    if( other )
    {
      if( nodeRole == PortType_Output )
      {
        std::string failureReason;
        if( !other->canConnectTo( pin, failureReason ) )
          continue; // skip this pin (it cannot be connected)
      }
      else if( nodeRole == PortType_Input )
      {
        std::string failureReason;
        if( !pin->canConnectTo( other, failureReason ) )
          continue; // skip this pin (it cannot be connected)
      }
    }

    // construct the label for the menu.
    QString name = pin->name().c_str();
    QString label;
    if( nodeRole == PortType_Input )
    {
      if( pin->portType() == PortType_Input )
        label = name + " =";
      else
        label = name + " >";
    }
    else
    {
      label = "> " + name;
    }

    // create an action using our label and add it to the menu.
    QAction * action = new QAction( label, NULL );
    action->setData( name );
    menu->addAction( action );
  }

  menu->addSeparator();

  QAction *exposeNewPortAction =
    new ExposeInstPortAction(
      menu,
      this,
      node,
      other,
      nodeRole
    );
  exposeNewPortAction->setEnabled(
    node->canEdit()
    && ( !other || other->isRealPort() )
  );
  menu->addAction( exposeNewPortAction );

  // done.
  return menu;
}

QMenu* DFGController::gvcCreateInstBlockHeaderMenu(
  InstBlock *instBlock,
  ConnectionTarget *other,
  PortType nodeRole
)
{
  QMenu *menu = new QMenu( this->getDFGWidget() );

  // go through all the node's pins and add
  // those to the menu that can be connected.
  for( unsigned int i = 0; i<instBlock->instBlockPortCount(); i++ )
  {
    InstBlockPort *instBlockPort = instBlock->instBlockPort( i );

    if( nodeRole == PortType_Output && instBlockPort->portType() == PortType_Output )
      continue; // skip this instBlockPort (an output port cannot be connected with another output port).

    if( other )
    {
      if( nodeRole == PortType_Output )
      {
        std::string failureReason;
        if( !other->canConnectTo( instBlockPort, failureReason ) )
          continue; // skip this instBlockPort (it cannot be connected)
      }
      else if( nodeRole == PortType_Input )
      {
        std::string failureReason;
        if( !instBlockPort->canConnectTo( other, failureReason ) )
          continue; // skip this instBlockPort (it cannot be connected)
      }
    }

    // construct the label for the menu.
    QString name = instBlockPort->name().c_str();
    QString label;
    if( nodeRole == PortType_Input )
    {
      if( instBlockPort->portType() == PortType_Input )
        label = name + " =";
      else
        label = name + " >";
    }
    else
    {
      label = "> " + name;
    }

    // create an action using our label and add it to the menu.
    QAction * action = new QAction( label, NULL );
    action->setData( name );
    menu->addAction( action );
  }

  menu->addSeparator();

  QAction *exposeNewPortAction =
    new ExposeInstBlockPortAction(
      menu,
      this,
      instBlock,
      other
    );
  exposeNewPortAction->setEnabled(
    nodeRole == PortType_Output
    && ( !other || other->isRealPort() )
  );
  menu->addAction( exposeNewPortAction );

  // done.
  return menu;
}

std::string DFGController::gvcEncodeMetadaToPersistValue()
{
  std::string metaData;
  if( this->gvcGetCurrentExecPath().isEmpty() ) // [FE-7700]
  {
    FTL::JSONEnc<> metaDataEnc( metaData );
    FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );
    FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, DFG_METADATA_UIPERSISTVALUE, "true" );
  }
  return metaData;
}
