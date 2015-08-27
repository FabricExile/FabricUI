// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NodeHeaderButton.h"
#include "NodeHeader.h"
#include "Node.h"
#include "Graph.h"

#include <QtGui/QPainter>

#ifdef FABRICUI_TIMERS
  #include <Util/Timer.h>
#endif

using namespace FabricUI::GraphView;

std::map<QString, QPixmap> NodeHeaderButton::s_pixmaps;

NodeHeaderButton::NodeHeaderButton(NodeHeader * parent, QString name, QString icon, int rotation)
: QGraphicsWidget(parent)
{
  m_nodeHeader = parent;
  m_name = name;
  m_icon = icon;
  m_rotation = 0;

  QString filePath = m_nodeHeader->node()->graph()->config().nodeToolbarIconDir + m_icon;

  int pos = 0;
  QRegExp rx("\\$\\{([^\\}]+)\\}");
  rx.setMinimal(true);
  while((pos = rx.indexIn(filePath, pos)) != -1)
  {
      QString capture = rx.cap(1);
      const char * envVar = getenv(capture.toAscii());
      if(envVar)
      {
        QString replacement = envVar;
        filePath.replace("${" + capture + "}", replacement);
      }
      pos += rx.matchedLength() + 2;
  }

  std::map<QString, QPixmap>::iterator it = s_pixmaps.find(icon);
  if(it != s_pixmaps.end())
    m_pixmap = it->second;
  else
  {
    m_pixmap = QPixmap(filePath);
    s_pixmaps.insert(std::pair<QString, QPixmap>(icon, m_pixmap));
  
    if(m_pixmap.width() == 0)
    {
      printf("Nodetoolbar: Pixmap not found: '%s'\n", filePath.toUtf8().constData());
      return;
    }
  }

  setMinimumWidth(m_pixmap.width());
  setMaximumWidth(m_pixmap.width());
  setMinimumHeight(m_pixmap.height());
  setMaximumHeight(m_pixmap.height());

  setRotation(rotation);
}

void NodeHeaderButton::setRotation(int rot)
{
  // todo
}


void NodeHeaderButton::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
#ifdef FABRICUI_TIMERS
  Util::TimerPtr timer = Util::Timer::getTimer("FabricUI::NodeHeaderButton");
  timer->resume();
#endif

  QGraphicsWidget::paint(painter, option, widget);
}