//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_UTIL_FELINEEDIT_H
#define FABRICUI_UTIL_FELINEEDIT_H

#include <QLineEdit>
#include <FabricUI/Util/StringUtil.h>

namespace FabricUI {
namespace Util {

class FELineEdit : public QLineEdit
{
  Q_OBJECT

private:

  QString m_last;

public:

  FELineEdit( QWidget *parent = 0 )
    : QLineEdit( parent )
  {
    init();
  }

  FELineEdit( QString text, QWidget *parent = 0 )
    : QLineEdit( text, parent )
  {
    init();
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

} // namespace Util
} // namespace FabricUI 

#endif // FABRICUI_UTIL_FELINEEDIT_H
