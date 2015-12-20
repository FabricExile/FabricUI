/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGLIGHTMANAGERDIALOG_H__
#define __UI_SCENEHUB_SGLIGHTMANAGERDIALOG_H__

#include <QtCore/QPoint>
#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtGui/QAction>
#include <QtGui/QFrame>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <FabricUI/SceneHub/Managers/SGBaseManagerDialog.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SGLightManagerDialog : public SGBaseManagerDialog
    {
      Q_OBJECT

      public:
        SGLightManagerDialog(QWidget*, FabricCore::Client*, FabricCore::RTVal);
        virtual ~SGLightManagerDialog() {};
        
      public slots:
        void showColorDialog();
        void updateLightProperties();
 
      private:
        QPoint m_glWidgetScreenPos;
        QPushButton *m_colorButton;
        QPushButton *m_closeButton;
        QSlider *m_intensitySlider;
    };
  };
};

#endif // __UI_SCENEHUB_SGLIGHTMANAGERDIALOG_H__
