//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecBlockPorts.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecBlocks.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecPorts.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_NLSPorts.h>
#include <FabricUI/DFG/PortEditor/DFGPEWidget_Elements.h>
#include <FabricUI/DFG/PortEditor/DFGPEWidget_Exec.h>
#include <FabricUI/Util/LoadPixmap.h>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

namespace FabricUI {
namespace DFG {

DFGPEWidget_Exec::DFGPEWidget_Exec(
  DFGWidget *dfgWidget,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  QWidget *parent
  )
  : QFrame( parent )
  , m_dfgWidget( dfgWidget )
  , m_plusIcon( QIcon( LoadPixmap( "DFGPlus.png" ) ) )
{
  setObjectName( "DFGPEWidget_Exec" );

  m_tabWidget = new QTabWidget;

  m_layout = new QVBoxLayout;
  m_layout->setContentsMargins( 0, 0, 0, 0 );
  m_layout->addWidget( m_tabWidget );
  setLayout( m_layout );

  setExec( binding, execPath, exec );
}

DFGPEWidget_Exec::~DFGPEWidget_Exec()
{
}

void DFGPEWidget_Exec::setExec(
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec
  )
{
  m_tabWidget->clear();
  if ( m_execNotifier )
    disconnect( m_execNotifier.data(), NULL, this, NULL );

  if ( !exec.isValid() )
    return;
  m_binding = binding;
  m_execPath = execPath;
  m_execPathQS = QString::fromUtf8( execPath.data(), execPath.size() );
  m_exec = exec;
  m_execNotifier = DFGExecNotifier::Create( exec );

  DFGPEModel *execPortsModel =
    new DFGPEModel_ExecPorts(
      m_dfgWidget->getDFGController()->getCmdHandler(),
      binding,
      execPath,
      exec,
      m_execNotifier
      );
  DFGPEWidget_Elements *execPortsWidget =
    new DFGPEWidget_Elements(
      m_dfgWidget,
      execPortsModel
      );
  m_tabWidget->addTab( execPortsWidget, "Ports" );

  DFGPEModel *nlsPortsModel =
    new DFGPEModel_NLSPorts(
      m_dfgWidget->getDFGController()->getCmdHandler(),
      binding,
      m_execPath,
      exec,
      m_execNotifier
      );
  DFGPEWidget_Elements *nlsPortsWidget =
    new DFGPEWidget_Elements(
      m_dfgWidget,
      nlsPortsModel
      );
  m_tabWidget->addTab( nlsPortsWidget, "Locals" );

  if ( exec.allowsBlocks() )
  {
    connect(
      m_execNotifier.data(), SIGNAL(execBlockInserted(unsigned, FTL::CStrRef)),
      this, SLOT(onExecBlockInserted(unsigned, FTL::CStrRef))
      );
    connect(
      m_execNotifier.data(), SIGNAL(execBlockRenamed(unsigned, FTL::CStrRef, FTL::CStrRef)),
      this, SLOT(onExecBlockRenamed(unsigned, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_execNotifier.data(), SIGNAL(execBlockRemoved(unsigned, FTL::CStrRef)),
      this, SLOT(onExecBlockRemoved(unsigned, FTL::CStrRef))
      );

    DFGPEModel *execBlocksModel =
      new DFGPEModel_ExecBlocks(
        m_dfgWidget->getDFGController()->getCmdHandler(),
        binding,
        execPath,
        exec,
        m_execNotifier
        );
    DFGPEWidget_Elements *execBlocksWidget =
      new DFGPEWidget_Elements(
        m_dfgWidget,
        execBlocksModel
        );
    m_tabWidget->addTab( execBlocksWidget, "Blocks" );

    unsigned execBlockCount = exec.getExecBlockCount();
    for ( unsigned execBlockIndex = 0;
      execBlockIndex < execBlockCount; ++execBlockIndex )
    {
      FTL::CStrRef execBlockName = exec.getExecBlockName( execBlockIndex );
      onExecBlockInserted( execBlockIndex, execBlockName );
    }
  }
}

void DFGPEWidget_Exec::onExecBlockInserted(
  unsigned blockIndex,
  FTL::CStrRef blockName
  )
{
  QString desc = "Block '";
  desc += QString::fromUtf8( blockName.data(), blockName.size() );
  desc += '\'';

  DFGPEModel *execBlockPortsModel =
    new DFGPEModel_ExecBlockPorts(
      m_dfgWidget->getDFGController()->getCmdHandler(),
      m_binding,
      m_execPath,
      m_exec,
      m_execNotifier,
      blockName
      );

  DFGPEWidget_Elements *execBlockPorts =
    new DFGPEWidget_Elements(
      m_dfgWidget,
      execBlockPortsModel
      );

  m_tabWidget->insertTab( 2 + blockIndex, execBlockPorts, desc );
}

void DFGPEWidget_Exec::onExecBlockRenamed(
  unsigned blockIndex,
  FTL::CStrRef oldBlockName,
  FTL::CStrRef newBlockName
  )
{
  QString desc = "Block '";
  desc += QString::fromUtf8( newBlockName.data(), newBlockName.size() );
  desc += '\'';
  m_tabWidget->setTabText( 2 + blockIndex, desc );
}

void DFGPEWidget_Exec::onExecBlockRemoved(
  unsigned blockIndex,
  FTL::CStrRef blockName
  )
{
  m_tabWidget->removeTab( 2 + blockIndex );
}

} // namespace DFG
} // namespace FabricUI
