// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGLogWidget__
#define __UI_DFG_DFGLogWidget__

#include <QWidget>
#include <QPlainTextEdit>

#include "DFGConfig.h"
#include "DFGController.h"

namespace FabricUI
{

  namespace DFG
  {

    class DFGLogWidgetPlainTextEdit : public QPlainTextEdit
    {
      Q_OBJECT

    public:

      DFGLogWidgetPlainTextEdit(QWidget * parent)
      : QPlainTextEdit(parent)
      {}

      void initFontPointSizeMembers()
      {
        m_fontPointSizeOriginal = this->font().pointSizeF();
        m_fontPointSizeCurrent = m_fontPointSizeOriginal;
      }

      void applyFontPointSize()
      {
        m_fontPointSizeCurrent = std::max(1.0, std::min(32.0, m_fontPointSizeCurrent));
        char styleSheet[128];
        sprintf(styleSheet, "font-size: %gpt;", m_fontPointSizeCurrent);
        setStyleSheet(styleSheet);
      }

      virtual void keyPressEvent(QKeyEvent * event)
      {
        if (event->modifiers().testFlag(Qt::ControlModifier))
        {
          if (   event->key() == Qt::Key_Plus
              || event->key() == Qt::Key_Minus
              || event->key() == Qt::Key_0)
          {
            if      (event->key() == Qt::Key_Plus)  m_fontPointSizeCurrent++;
            else if (event->key() == Qt::Key_Minus) m_fontPointSizeCurrent--;
            else                                    m_fontPointSizeCurrent = m_fontPointSizeOriginal;
            applyFontPointSize();
          }
          event->accept();
        }
        else
          QPlainTextEdit::keyPressEvent(event);
      }

      virtual void wheelEvent(QWheelEvent *event)
      {
        if (event->modifiers().testFlag(Qt::ControlModifier))
        {
          m_fontPointSizeCurrent += 0.0125 * event->delta();
          applyFontPointSize();
          event->accept();
        }
        else
          QPlainTextEdit::wheelEvent(event);
      }

    private:

      qreal m_fontPointSizeOriginal;
      qreal m_fontPointSizeCurrent;
    };

    class DFGLogWidget : public QWidget
    {
      Q_OBJECT

    public:

      DFGLogWidget( const DFGConfig & config );
      virtual ~DFGLogWidget();

      static void log(const char * message);
      static void logError(const char * message);
      static void logWarning(const char * message);
      static void logInfo(const char * message);
      static void logDebug(const char * message);

      static void callback(
        void * userData, 
        FabricCore::ReportSource source,
        FabricCore::ReportLevel level,
        char const * stringData,
        unsigned int stringLength
        );

      virtual void keyPressEvent(QKeyEvent * event);
      virtual void mouseDoubleClickEvent(QMouseEvent * event);
      virtual void mousePressEvent(QMouseEvent * event);
      virtual void mouseReleaseEvent(QMouseEvent * event);

      static void setLogFunc(DFGController::LogFunc func);

    public slots:

      void clear();
      void showContextMenu( QPoint const &pos );

    private:

      DFGLogWidgetPlainTextEdit * m_text;
      DFGConfig m_config;

      static DFGController::LogFunc s_logFunc;
      static std::vector<std::string> sUnconsumedMessages;
      static std::vector<DFGLogWidget*> sLogWidgets;
    };

  };

};

#endif // __UI_DFG_DFGLogWidget__
