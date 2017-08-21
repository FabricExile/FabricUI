// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_KLEditor_LineNumberWidget__
#define __UI_KLEditor_LineNumberWidget__

#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QWidget>
#include "EditorConfig.h"

namespace FabricUI
{

  namespace KLEditor
  {

    class LineNumberWidget : public QWidget
    {
      Q_OBJECT
    
    public:
    
      LineNumberWidget(QWidget * parent, const EditorConfig & config = EditorConfig());
      virtual ~LineNumberWidget();

      unsigned int lineOffset() const;

      virtual void paintEvent(QPaintEvent * event);

    public slots:
      void setLineOffset(unsigned int offset);
      void setFontPointSize(qreal fontPointSize);

    private:
      EditorConfig m_config;
      unsigned int m_lineOffset;
      qreal m_fontPointSize;
    };

  };

};

#endif // __UI_KLEditor_LineNumberWidget__
