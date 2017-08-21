/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_VIEWPORT_EVENT_FILTER__
#define __UI_VIEWPORT_EVENT_FILTER__

#include <QEvent>
#include <QObject>

namespace FabricUI {
namespace Viewports {

class ViewportWidget;

class ViewportEventFilter : public QObject 
{
  Q_OBJECT

  public:
    ViewportEventFilter(
      ViewportWidget *viewport
      );

    bool eventFilter(
      QObject *object, 
      QEvent *event
      );

  private:
    ViewportWidget *m_viewport;
};
  
} // namespace Viewports
} // namespace FabricUI

#endif // __UI_VIEWPORT_EVENT_FILTER__
