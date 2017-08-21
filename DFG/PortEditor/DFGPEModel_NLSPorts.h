//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef _DFGPEModel_NLSPorts_h
#define _DFGPEModel_NLSPorts_h

#include <FTL/StrRef.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel.h>
#include <FabricUI/DFG/DFGExecNotifier.h>

namespace FabricUI {
namespace DFG {

class DFGUICmdHandler;

class DFGPEModel_NLSPorts : public DFGPEModel
{
  Q_OBJECT

public:

  DFGPEModel_NLSPorts(
    DFGUICmdHandler *cmdHandler,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    QSharedPointer<DFG::DFGExecNotifier> execNotifier
    );

  virtual bool canInspectElements() /*override*/ { return false; }
  virtual bool hasPortType() /*override*/ { return false; }
  virtual bool hasTypeSpec() /*override*/ { return true; }
  virtual bool allowReordering() /*override*/ { return true; }

  virtual int getElementCount() /*override*/;
  virtual QString getElementName( int index ) /*override*/;
  virtual FabricCore::DFGPortType getElementPortType( int index ) /*override*/;
  virtual QString getElementTypeSpec( int index ) /*override*/;

  virtual void insertElement(
    int index,
    QString desiredName,
    FabricCore::DFGPortType type,
    QString typeSpec
    ) /*override*/;
  virtual void inspectElement(
    int index,
    DFGWidget *dfgWidget
    ) /*override*/;
  virtual void renameElement(
    int index,
    QString newName
    ) /*override*/;
  virtual void setElementPortType(
    int index,
    FabricCore::DFGPortType type
    ) /*override*/;
  virtual void setElementTypeSpec(
    int index,
    QString newTypeSpec
    ) /*override*/;
  virtual void removeElements(
    QList<int> indices
    ) /*override*/;

  virtual void reorderElements(
    QList<int> newIndices
    ) /*override*/;

protected:

  virtual bool isElementReadOnlyImpl( int index ) /*override*/;
  virtual bool computeIsReadOnly() /*override*/;

protected slots:

  void onEditWouldSplitFromPresetMayHaveChanged();

  void onNLSPortInserted(
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::JSONObject const *portDesc
    );

  void onNLSPortRenamed(
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void onNLSPortTypeSpecChanged(
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::CStrRef newTypeSpec
    );

  void onNLSPortRemoved(
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void onNLSPortsReordered(
    FTL::ArrayRef<unsigned> newOrder
    );

private:

  DFGUICmdHandler *m_cmdHandler;
  FabricCore::DFGBinding m_binding;
  QString m_execPathQS;
  FabricCore::DFGExec m_exec;
  QSharedPointer<DFG::DFGExecNotifier> m_notifier;
};

} // namespace DFG
} // namespace FabricUI

#endif // _DFGPEModel_NLSPorts_h
