//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_NLSPorts.h>
#include <FTL/JSONValue.h>

namespace FabricUI {
namespace DFG {

DFGPEModel_NLSPorts::DFGPEModel_NLSPorts(
  DFGUICmdHandler *cmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  QSharedPointer<DFG::DFGExecNotifier> execNotifier
  )
  : DFGPEModel( "local", "Local" )
  , m_cmdHandler( cmdHandler )
  , m_binding( binding )
  , m_execPathQS( QString::fromUtf8( execPath.data(), execPath.size() ) )
  , m_exec( exec )
  , m_notifier( execNotifier )
{
  connect(
    m_notifier.data(), SIGNAL(editWouldSplitFromPresetMayHaveChanged()),
    this, SLOT(onEditWouldSplitFromPresetMayHaveChanged())
    );
  connect(
    m_notifier.data(), SIGNAL(nlsPortInserted(unsigned, FTL::CStrRef, FTL::JSONObject const *)),
    this, SLOT(onNLSPortInserted(unsigned, FTL::CStrRef, FTL::JSONObject const *))
    );
  connect(
    m_notifier.data(), SIGNAL(nlsPortRenamed(unsigned, FTL::CStrRef, FTL::CStrRef)),
    this, SLOT(onNLSPortRenamed(unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(), SIGNAL(nlsPortTypeSpecChanged(unsigned, FTL::CStrRef, FTL::CStrRef)),
    this, SLOT(onNLSPortTypeSpecChanged(unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(), SIGNAL(nlsPortRemoved(unsigned, FTL::CStrRef)),
    this, SLOT(onNLSPortRemoved(unsigned, FTL::CStrRef))
    );
  connect(
    m_notifier.data(), SIGNAL(nlsPortsReordered(FTL::ArrayRef<unsigned>)),
    this, SLOT(onNLSPortsReordered(FTL::ArrayRef<unsigned>))
    );

  init();
}

bool DFGPEModel_NLSPorts::computeIsReadOnly()
{
  return m_exec.editWouldSplitFromPreset();
}

int DFGPEModel_NLSPorts::getElementCount()
{
  return m_exec.getNLSPortCount();
}

QString DFGPEModel_NLSPorts::getElementName( int index )
{
  FTL::CStrRef result = m_exec.getNLSPortName( index );
  return QString::fromUtf8( result.data(), result.size() );
}

FabricCore::DFGPortType DFGPEModel_NLSPorts::getElementPortType( int index )
{
  return FabricCore::DFGPortType_IO;
}

QString DFGPEModel_NLSPorts::getElementTypeSpec( int index )
{
  FTL::CStrRef result = m_exec.getNLSPortTypeSpec( index );
  return QString::fromUtf8( result.data(), result.size() );
}

bool DFGPEModel_NLSPorts::isElementReadOnlyImpl( int index )
{
  return false;
}

void DFGPEModel_NLSPorts::insertElement(
  int index,
  QString desiredPortName,
  FabricCore::DFGPortType portType,
  QString portTypeSpec
  )
{
  m_cmdHandler->dfgDoAddNLSPort(
    m_binding,
    m_execPathQS,
    m_exec,
    desiredPortName,
    portTypeSpec,
    QString(), // portToConnect
    QString(), // extDep
    QString() // metaData
    );
}

void DFGPEModel_NLSPorts::inspectElement(
  int index,
  DFGWidget *dfgWidget
  )
{
  assert( false );
}

void DFGPEModel_NLSPorts::renameElement(
  int index,
  QString newPortName
  )
{
  QString oldPortName = getElementName( index );

  m_cmdHandler->dfgDoEditPort(
    m_binding,
    m_execPathQS,
    m_exec,
    oldPortName,
    newPortName,
    FabricCore::DFGPortType_IO,
    getElementTypeSpec( index ),
    QString(), // extDep
    QString() // uiMetadata
    );
}

void DFGPEModel_NLSPorts::setElementPortType(
  int index,
  FabricCore::DFGPortType portType
  )
{
  assert( false );
}

void DFGPEModel_NLSPorts::setElementTypeSpec(
  int index,
  QString newPortTypeSpec
  )
{
  QString portName = getElementName( index );

  m_cmdHandler->dfgDoEditPort(
    m_binding,
    m_execPathQS,
    m_exec,
    portName,
    portName,
    FabricCore::DFGPortType_IO,
    newPortTypeSpec,
    QString(), // extDep
    QString() // uiMetadata
    );
}

void DFGPEModel_NLSPorts::removeElements(
  QList<int> indices
  )
{
  qSort( indices );
  for ( int i = indices.size(); i--; )
  {
    int index = indices[i];
    m_cmdHandler->dfgDoRemovePort(
      m_binding,
      m_execPathQS,
      m_exec,
      QStringList( getElementName( index ) )
      );
  }
}

void DFGPEModel_NLSPorts::reorderElements(
  QList<int> newIndices
  )
{
  m_cmdHandler->dfgDoReorderNLSPorts(
    m_binding,
    m_execPathQS,
    m_exec,
    QString(), // itemPath
    newIndices
    );
}

void DFGPEModel_NLSPorts::onEditWouldSplitFromPresetMayHaveChanged()
{
  updateIsReadOnly();
}

void DFGPEModel_NLSPorts::onNLSPortInserted(
  unsigned portIndex,
  FTL::CStrRef portName,
  FTL::JSONObject const *portDesc
  )
{
  FTL::StrRef typeSpec = portDesc->getStringOrEmpty( FTL_STR("typeSpec") );
  emit elementInserted(
    portIndex,
    QString::fromUtf8( portName.data(), portName.size() ),
    FabricCore::DFGPortType_IO,
    QString::fromUtf8( typeSpec.data(), typeSpec.size() )
    );
}

void DFGPEModel_NLSPorts::onNLSPortRenamed(
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName
  )
{
  emit elementRenamed(
    portIndex,
    QString::fromUtf8( newPortName.data(), newPortName.size() )
    );
}

void DFGPEModel_NLSPorts::onNLSPortTypeSpecChanged(
  unsigned portIndex,
  FTL::CStrRef portName,
  FTL::CStrRef newPortTypeSpec
  )
{
  emit elementTypeSpecChanged(
    portIndex,
    QString::fromUtf8( newPortTypeSpec.data(), newPortTypeSpec.size() )
    );
}

void DFGPEModel_NLSPorts::onNLSPortRemoved(
  unsigned portIndex,
  FTL::CStrRef portName
  )
{
  emit elementRemoved( portIndex );
}

void DFGPEModel_NLSPorts::onNLSPortsReordered(
  FTL::ArrayRef<unsigned> newOrder
  )
{
  QList<int> newIndices;
  for ( size_t i = 0; i < newOrder.size(); ++i )
    newIndices.push_back( newOrder[i] );
  emit elementsReordered( newIndices );
}

} // namespace DFG
} // namespace FabricUI
