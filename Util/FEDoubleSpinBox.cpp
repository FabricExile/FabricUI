//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "FEDoubleSpinBox.h"
#include <QFocusEvent>

using namespace FabricUI::Util;

void FEDoubleSpinBox::focusInEvent( QFocusEvent *event )
{
  if ( event->reason() != Qt::PopupFocusReason )
    m_last = value();
  QDoubleSpinBox::focusInEvent( event );
}

void FEDoubleSpinBox::focusOutEvent( QFocusEvent *event )
{
  if ( event->reason() != Qt::PopupFocusReason )
    checkValue();
  QDoubleSpinBox::focusOutEvent( event );
}

void FEDoubleSpinBox::keyPressEvent( QKeyEvent *event )
{
  if ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter )
  {
    clearFocus();
    event->accept();
    return;
  }

  if ( event->key() == Qt::Key_Escape )
  {
    setValue(m_last);
    clearFocus();
    event->accept();
    return;
  }

  QDoubleSpinBox::keyPressEvent( event );
}
