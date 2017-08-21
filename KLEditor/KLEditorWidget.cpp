// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLEditorWidget.h"

#include <QHBoxLayout>

using namespace FabricServices;
using namespace FabricUI::KLEditor;

KLEditorWidget::KLEditorWidget(
  QWidget * parent,
  FabricServices::ASTWrapper::KLASTManager * manager,
  const EditorConfig & config
  )
  : QFrame(parent)
{
  setObjectName( "KLEditorWidget" );

  m_config = config;

  setContentsMargins(0, 0, 0, 0);

  m_lineNumbers = new LineNumberWidget(this, config);
  m_sourceCodeWidget = new KLSourceCodeWidget(this, manager, config);

  QObject::connect(m_sourceCodeWidget, SIGNAL(documentScrolled(unsigned int)), m_lineNumbers, SLOT(setLineOffset(unsigned int)));

  QObject::connect(m_sourceCodeWidget, SIGNAL(fontPointSizeChanged(qreal)), m_lineNumbers, SLOT(setFontPointSize(qreal)));

  QHBoxLayout * layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_lineNumbers);
  layout->addWidget(m_sourceCodeWidget);
  setLayout(layout);
}

KLEditorWidget::~KLEditorWidget()
{
}

KLSourceCodeWidget * KLEditorWidget::sourceCodeWidget()
{
  return m_sourceCodeWidget;
}
