//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QLabel>
#include <QVBoxLayout>
#include "SimpleDialog.h"
#include <FabricUI/Util/LoadFabricStyleSheet.h>
using namespace FabricUI;
using namespace Dialog;

SimpleDialog::SimpleDialog(
  QString const&title,
  QString const&text,
  QWidget* parent)
 : QDialog(parent)
{
  setWindowTitle(title);
  setObjectName("HelpDialog_" + title);
  setStyleSheet(LoadFabricStyleSheet("FabricUI.qss"));
  
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setContentsMargins(0, 0, 0, 0);

  QVBoxLayout *mainlayout = new QVBoxLayout();
  setLayout( mainlayout );
  mainlayout->setContentsMargins( 5, 5, 5, 5 );
  mainlayout->setSpacing(2);

  QLabel *label = new QLabel(text, this);
  label->setContentsMargins( 0, 0, 0, 0 );
  label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
  label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  label->setWordWrap(true);

  QVBoxLayout * inputsLayout = new QVBoxLayout();
  inputsLayout->setContentsMargins( 0, 0, 0, 0 );
  inputsLayout->setSpacing(2);
  label->setLayout(inputsLayout);

  mainlayout->addWidget(label);
  setFixedSize(sizeHint());
}

SimpleDialog::~SimpleDialog()
{
}
