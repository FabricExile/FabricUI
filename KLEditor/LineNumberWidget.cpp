// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "LineNumberWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <string>

#include <stdio.h>

using namespace FabricUI::KLEditor;

LineNumberWidget::LineNumberWidget(QWidget * parent, const EditorConfig & config)
: QWidget(parent)
{
  m_config = config;
  m_lineOffset = 0;
  m_fontPointSize = -1;
  setFontPointSize(0);

  setContentsMargins(0, 0, 0, 0);
}

LineNumberWidget::~LineNumberWidget()
{
}

unsigned int LineNumberWidget::lineOffset() const
{
  return m_lineOffset;
}

void LineNumberWidget::setLineOffset(unsigned int offset)
{
  if(offset != m_lineOffset)
  {
    m_lineOffset = offset;
    update();
  }
}

void LineNumberWidget::setFontPointSize(qreal fontPointSize)
{
  if (fontPointSize != m_fontPointSize)
  {
    m_fontPointSize = fontPointSize;
    QFont font = m_config.lineNumberFont;
    if (m_fontPointSize > 0)
      font.setPointSizeF(m_fontPointSize);
    int maxWidth = QFontMetrics(font).width("0000") + 6;
    setMinimumWidth(maxWidth);
    setMaximumWidth(maxWidth);
    update();
  }
}

void LineNumberWidget::paintEvent(QPaintEvent * event)
{
  QPainter painter(this);
  
  painter.fillRect(event->rect(), m_config.lineNumberBackgroundColor);

  QFont font = m_config.lineNumberFont;
  if (m_fontPointSize > 0)
    font.setPointSizeF(m_fontPointSize);
  painter.setFont(font);
  painter.setPen(m_config.lineNumberFontColor);
  QFontMetrics fontMetrics(font);

  int width = event->rect().width();
  int height = event->rect().height();

  int offset = fontMetrics.lineSpacing();
  int extraOffset = 0;

#if defined(FABRIC_OS_DARWIN)
  extraOffset = 1;
    
  if(m_lineOffset != 0)
  {
    offset -= 2;
  }
  else
  {
    offset += 1;
  }
#else
  if(m_lineOffset != 0)
  {
    offset -= 4;
  }
#endif

  int line = m_lineOffset + 1;
  char buffer[128];
  while(offset < height)
  {
    std::string paddingNumber;
#ifdef _WIN32
    itoa(line, buffer, 10);
#else
    snprintf(buffer, 128, "%d", line);
#endif
    paddingNumber = buffer;
    while(paddingNumber.length() < 4)
      paddingNumber = " " + paddingNumber;

    int lineWidth = fontMetrics.width(paddingNumber.c_str());
    painter.drawText(QPoint(width - 2 - lineWidth, offset), paddingNumber.c_str());
    offset += fontMetrics.lineSpacing() + extraOffset;
    line++;
  }

  QWidget::paintEvent(event);
}
