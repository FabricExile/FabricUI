// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_BASE_DIALOG__
#define __UI_BASE_DIALOG__

#include <QDialog>
#include <QVBoxLayout>

namespace FabricUI {
namespace Dialog {

class BaseDialog : public QDialog 
{
  Q_OBJECT

  public:
    BaseDialog(
      QWidget* parent
      );

    virtual ~BaseDialog();      

    virtual QVBoxLayout* inputsLayout();

  protected:
    QVBoxLayout * m_inputsLayout;
};

} // namespace Dialog
} // namespace FabricUI

#endif // __UI_BASE_DIALOG__
