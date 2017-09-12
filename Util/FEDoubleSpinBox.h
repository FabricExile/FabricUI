//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_UTIL_FEDOUBLESPINBOX_H
#define FABRICUI_UTIL_FEDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include <FabricUI/Util/StringUtil.h>

namespace FabricUI {
namespace Util {

class FEDoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT

private:

  qreal m_last;

public:

  FEDoubleSpinBox( QWidget *parent = 0 )
    : QDoubleSpinBox( parent )
  {
    init();
  }

signals:

  void valueModified( double value );

protected:

  void init()
  {
    connect(
      this, SIGNAL(returnPressed()),
      this, SLOT(onReturnPressed())
      );
  }

  void checkValue()
  {
    double cur = value();
    if ( m_last != cur )
    {
      m_last = cur;
      emit valueModified( cur );
    }
  }

  virtual void focusInEvent( QFocusEvent *event );

  virtual void focusOutEvent( QFocusEvent *event );

  virtual void keyPressEvent( QKeyEvent *event );

protected slots:

  void onReturnPressed()
  {
    checkValue();
  }
};

} // namespace Util
} // namespace FabricUI 

#endif // FABRICUI_UTIL_FEDOUBLESPINBOX_H
