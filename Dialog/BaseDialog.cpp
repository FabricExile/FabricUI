// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include "BaseDialog.h"
#include <FabricUI/Util/LoadFabricStyleSheet.h>

using namespace FabricUI;
using namespace Dialog;

BaseDialog::BaseDialog(
  QWidget *parent)
: QDialog(parent)
{
  setMinimumHeight(10);
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));

  setContentsMargins(0, 0, 0, 0);

  QVBoxLayout *mainlayout = new QVBoxLayout();
  this->setLayout( mainlayout );
  mainlayout->setContentsMargins( 5, 5, 5, 5 );
  mainlayout->setSpacing(2);

  // Inputs
  QWidget *inputsWidget = new QWidget();
  inputsWidget->setObjectName( "inputsWidget" );
  inputsWidget->setContentsMargins( 0, 0, 0, 0 );

  m_inputsLayout = new QVBoxLayout;
  m_inputsLayout->setContentsMargins( 0, 0, 0, 0 );
  m_inputsLayout->setSpacing(2);
  inputsWidget->setLayout(m_inputsLayout);

  // Buttons
  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  buttonsLayout->setContentsMargins( 0, 6, 0, 0 );
  buttonsLayout->setSpacing( 2 );

  QWidget *buttonsWidget = new QWidget();
  buttonsWidget->setObjectName( "buttonsWidget" );
  buttonsWidget->setContentsMargins( 0, 0, 0, 0 );
  buttonsWidget->setLayout( buttonsLayout );
  buttonsWidget->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding) );
  
  QPushButton *okButton = new QPushButton( "OK", buttonsWidget );
  QPushButton *cancelButton = new QPushButton( "Cancel", buttonsWidget );
  okButton->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
  cancelButton->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

  QObject::connect( okButton, SIGNAL(clicked()), this, SLOT(accept()) );
  QObject::connect( cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );

  buttonsLayout->addStretch( 2 );
  buttonsLayout->addWidget( okButton );
  buttonsLayout->addWidget( cancelButton );
  
  mainlayout->addWidget(inputsWidget);
  mainlayout->addWidget( buttonsWidget );
  mainlayout->addStretch( 2 );
}

BaseDialog::~BaseDialog()
{
}

QVBoxLayout * BaseDialog::inputsLayout()
{
  return m_inputsLayout;
}
