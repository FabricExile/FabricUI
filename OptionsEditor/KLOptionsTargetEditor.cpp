//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValDictModelItem.h"
#include "RTValArrayModelItem.h"
#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include <FabricUI/Util/QtUtil.h>
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Viewports/ViewportWidget.h>
#include "KLOptionsTargetModelItem.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

KLOptionsTargetEditor::KLOptionsTargetEditor(
  QString title)
  : BaseRTValOptionsEditor(title, 0)
{
  resetModel();
}

KLOptionsTargetEditor::~KLOptionsTargetEditor() 
{
}

BaseRTValModelItem* KLOptionsTargetEditor::constructModel(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor *editor,
  RTVal options) 
{
  FABRIC_CATCH_BEGIN();

  options = RTValUtil::toRTVal(options);

  // It's always possible that the options no longer exist (eg: tool was detructed)
  if( options.isValid() ) {
    if( options.isDict() )
      return new RTValDictModelItem(
        name,
        path,
        editor,
        options );

    else if( options.isArray() )
      return new RTValArrayModelItem(
        name,
        path,
        editor,
        options );

    else
      return new KLOptionsTargetModelItem(
        name,
        path,
        editor,
        options );
  }

  FABRIC_CATCH_END("KLOptionsTargetEditor::constructModel");

  return 0;
}

void KLOptionsTargetEditor::updateModel(
  RTVal options) 
{  
  FABRIC_CATCH_BEGIN();

  m_model->setRTValOptions( 
    RTValUtil::toRTVal(
      OptionsEditorHelpers::getKLOptionsTargetOptions(m_title)
      )
    );

  FABRIC_CATCH_END("KLOptionsTargetEditor::updateModel");
}

void KLOptionsTargetEditor::refreshKLOptions() {
  if( m_model )
    updateModel( RTVal() );
  else
    resetModel( RTVal() );
}

void KLOptionsTargetEditor::resetModel(
  RTVal options) 
{
  FABRIC_CATCH_BEGIN();
  
  BaseRTValOptionsEditor::resetModel(
    OptionsEditorHelpers::getKLOptionsTargetOptions( m_title )
    );
  
  FABRIC_CATCH_END("KLOptionsTargetEditor::resetModel");
}

QDockWidget* KLOptionsTargetEditor::create( 
  QString editorID,
  QString title,
  QString groupeName,
  QMainWindow* mainWindow) 
{
  if( mainWindow == 0 )
    Application::FabricException::Throw(
      "CreateKLOptionsTargetEditor",
      "mainWindow is null" );

  QDockWidget *dock = new QDockWidget(
    title,
    mainWindow );

  dock->setObjectName( editorID );

  BaseRTValOptionsEditor *optionsEditor = new KLOptionsTargetEditor(
    editorID );

  dock->setWidget( optionsEditor );

  mainWindow->addDockWidget(
    Qt::RightDockWidgetArea,
    dock,
    Qt::Vertical);

  Viewports::ViewportWidget *viewport = Util::QtUtil::getQWidget<Viewports::ViewportWidget>();
  if( viewport == 0 )
    Application::FabricException::Throw(
      "CreateKLOptionsTargetEditor",
      "Viewport is null" );

  QObject::connect(
    viewport,
    SIGNAL( initComplete() ),
    optionsEditor,
    SLOT( resetModel() )
    );

  QObject::connect(
    optionsEditor,
    SIGNAL( updated() ),
    viewport,
    SLOT( redraw() )
    );

  return dock;
}

QDockWidget* KLOptionsTargetEditor::create( 
  QString editorID,
  QString title,
  QString groupeName) 
{
  QMainWindow* mainWindow = Util::QtUtil::getMainWindow();
  return KLOptionsTargetEditor::create( editorID, title, groupeName, mainWindow );
}
