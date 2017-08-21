//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_VELINEEDIT_H
#define FABRICUI_VALUEEDITOR_VELINEEDIT_H

#include <QLineEdit>
#include <QIntValidator>
#include <FabricUI/Util/StringUtil.h>

namespace FabricUI {
namespace ValueEditor {

class VELineEdit : public QLineEdit
{
  Q_OBJECT

private:

  QString m_last;

public:

  VELineEdit( QWidget *parent = 0 )
    : QLineEdit( parent )
  {
    init();
  }

  VELineEdit( QString text, QWidget *parent = 0 )
    : QLineEdit( text, parent )
  {
    init();
  }

  void setValidatorInt()
  {
    setValidator(new QIntValidator(this));
  }

  void setValidatorDouble()
  {
    setValidator(FabricUI::Util::newDoubleValidator(this));
  }

signals:

  void textModified( QString text );

protected:

  void init()
  {
    connect(
      this, SIGNAL(returnPressed()),
      this, SLOT(onReturnPressed())
      );
  }

  void checkText()
  {
    QString cur = text();
    if ( m_last != cur )
    {
      m_last = cur;
      emit textModified( cur );
    }
  }

  virtual void focusInEvent( QFocusEvent *event );

  virtual void focusOutEvent( QFocusEvent *event );

  virtual void keyPressEvent( QKeyEvent *event );

protected slots:

  void onReturnPressed()
  {
    checkText();
    selectAll();
  }
};

} // namespace ValueEditor
} // namespace FabricUI

#endif // FABRICUI_VALUEEDITOR_VELINEEDIT_H
