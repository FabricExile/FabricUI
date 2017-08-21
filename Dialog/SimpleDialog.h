//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SIMPLE_DIALOG__
#define __UI_SIMPLE_DIALOG__

#include <QDialog>
#include <QString>
#include <QWidget>

namespace FabricUI {
namespace Dialog {

class SimpleDialog : public QDialog 
{
  Q_OBJECT

  public:
    SimpleDialog(
    	QString const&title,
    	QString const&text,
      QWidget* parent = 0
      );

    virtual ~SimpleDialog();   
};

} // namespace Dialog
} // namespace FabricUI

#endif // __UI_SIMPLE_DIALOG__
