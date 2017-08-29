// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGExecHeaderWidget__
#define __UI_DFG_DFGExecHeaderWidget__

#include <FabricCore.h>

#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QToolbar>
#include <QFrame>
#include <QMouseEvent>

#include <FTL/StrRef.h>

#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/Util/FELineEdit.h>
#include <FabricUI/Util/LoadPixmap.h>

class QLabel;

namespace FabricUI
{
  namespace DFG
  {
    class DFGController;

    class ReqExtLineEdit : public Util::FELineEdit
    {
      Q_OBJECT

    public:

      ReqExtLineEdit( QWidget *parent = 0 );

      void setAllowEdits(bool allow);

    protected slots:

      void onEditingFinished();

    protected:

      virtual bool eventFilter(QObject * watched, QEvent * event);

      void init();

    protected slots:

      void onGoUpPressed();

    private:

      bool m_allowEdits;

    };

    class DFGExecHeaderWidget : public QFrame
    {
      Q_OBJECT

    public:

      DFGExecHeaderWidget(
        QWidget * parent,
        DFGController *dfgController,
        const GraphView::GraphConfig & config = GraphView::GraphConfig()
        );
      virtual ~DFGExecHeaderWidget();
      
      void refreshExtDeps( FTL::CStrRef extDeps );

    signals:

      void goUpPressed();
      void reloadPressed();
      void savePressed();
      void extensionLoaded();

    public slots:

      void refresh();
      void onExecChanged();

    protected:

      FTL::CStrRef getExecPath();
      FabricCore::DFGExec &getExec();

      virtual void paintEvent(QPaintEvent * event);

    protected slots:

      void reqExtEditingFinished();
      void reqExtResizeToContent();
      void reqExtSetToolTipFromContent();
      void onSplitFromPresetClicked();

    private:

      DFGController *m_dfgController;
      QLabel *m_execPathLabel;
      QLabel *m_presetNameLabel;
      QLabel *m_presetPathSep;
      QLabel *m_reqExtLabel;
      ReqExtLineEdit *m_reqExtLineEdit;
      QPushButton * m_backButton;
      QPushButton * m_saveButton;
      QPushButton * m_reloadButton;
      QToolButton * m_disableGraphButton;
      QColor m_backgroundColor;
      QPen m_pen;
      QWidget *m_presetSplitWidget;

    };

  };

};

#endif // __UI_DFG_DFGExecHeaderWidget__
