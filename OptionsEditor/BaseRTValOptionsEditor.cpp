//
// Copyright(c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseRTValOptionsEditor.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace FabricCore;
using namespace ValueEditor;
using namespace Application;
using namespace OptionsEditor;

BaseRTValOptionsEditor::BaseRTValOptionsEditor(
  QString title,
  RTVal options)
  : VETreeWidget()
  , m_title(title)
  , m_model(0)
{
}

BaseRTValOptionsEditor::~BaseRTValOptionsEditor()
{
  if(m_model != 0) 
  { 
    delete m_model; 
    m_model = 0;
  }
}

BaseRTValModelItem* BaseRTValOptionsEditor::constructModel(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor *editor,
  RTVal options) 
{
  FabricException::Throw(
    "BaseRTValOptionsEditor::constructModel",
    "must be overridden");
  return 0;
}

void BaseRTValOptionsEditor::modelUpdated()
{
  emit updated();
}

void BaseRTValOptionsEditor::resetModel(
  RTVal options) 
{
  if(m_model != 0) 
  { 
    delete m_model; 
    m_model = 0;
  }
   
  m_model = constructModel(
    m_title.toUtf8().constData(),
    "",
    this,
    options);
 
  onSetModelItem(m_model);
}

void BaseRTValOptionsEditor::updateModel(
  RTVal options) 
{
  FabricException::Throw(
    "BaseRTValOptionsEditor::updateModel",
    "must be overridden");
}
