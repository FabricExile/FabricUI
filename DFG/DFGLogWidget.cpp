// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DFGLogWidget.h"
#include <QHBoxLayout>
#include <QMenu>
#include <QVBoxLayout>

#include <FabricUI/Util/LoadFabricStyleSheet.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGController::LogFunc DFGLogWidget::s_logFunc = NULL;
std::vector<DFGLogWidget*> DFGLogWidget::sLogWidgets;
std::vector<std::string> DFGLogWidget::sUnconsumedMessages;

DFGLogWidget::DFGLogWidget( const DFGConfig & config )
  : m_config( config )
{
  setObjectName( "DFGLogWidget" );
  
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);
  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  m_text = new DFGLogWidgetPlainTextEdit(this);
  m_text->setFont( m_config.fixedFont );
  m_text->initFontPointSizeMembers();
  m_text->setReadOnly(true);
  m_text->setContextMenuPolicy( Qt::CustomContextMenu );
  connect(
    m_text, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(showContextMenu(const QPoint&))
    );
  layout->addWidget(m_text);

  QString styleSheet = LoadFabricStyleSheet( "FabricUI.qss" );
  if ( !styleSheet.isEmpty() )
    setStyleSheet( styleSheet );

  sLogWidgets.push_back(this);

  for(size_t i=0;i<sUnconsumedMessages.size();i++)
    log(sUnconsumedMessages[i].c_str());
  sUnconsumedMessages.clear();
}

DFGLogWidget::~DFGLogWidget()
{
  size_t index = sLogWidgets.size();
  for(size_t i=0;i<sLogWidgets.size();i++)
  {
    if(sLogWidgets[i] == this)
    {
      index = i;
      break;
    }
  }

  if(index < sLogWidgets.size())
  {
    std::vector<DFGLogWidget*>::iterator it = sLogWidgets.begin();
    sLogWidgets.erase(it + index);
  }
}

void DFGLogWidget::log(const char * message)
{
  callback( NULL, FabricCore::ReportSource_System,
            FabricCore::ReportLevel_Info, message, 0 );
}

void DFGLogWidget::callback(
  void * userData, 
  FabricCore::ReportSource source,
  FabricCore::ReportLevel level,
  char const * stringData,
  uint32_t stringLength
  )
{
  if(sLogWidgets.size() > 0)
  {
    // set the default text color and the keyword-color pairs.
    QColor defaultTextColor(233, 233, 233);

    std::vector<std::pair<QString, QColor> > keywords;

    keywords.push_back(std::make_pair("errors", QColor(255, 20, 10)));
    keywords.push_back(std::make_pair("error", QColor(255, 20, 10)));

    keywords.push_back(std::make_pair("warnings", QColor(230, 230, 10)));
    keywords.push_back(std::make_pair("warning", QColor(230, 230, 10)));

    keywords.push_back(std::make_pair("[fabric:mt]", QColor(137, 208, 231)));
    keywords.push_back(std::make_pair("[st]", QColor(137, 208, 231)));

    keywords.push_back(std::make_pair("fabric engine", QColor(42, 183, 229)));
    keywords.push_back(std::make_pair("fabric canvas", QColor(42, 183, 229)));
    keywords.push_back(std::make_pair("KL stack trace:", QColor(42, 183, 229)));

    keywords.push_back(std::make_pair("frogs", QColor(0, 255, 0)));
    keywords.push_back(std::make_pair("frog", QColor(0, 255, 0)));

    keywords.push_back(std::make_pair("teapots", QColor(255, 0, 255)));
    keywords.push_back(std::make_pair("teapot", QColor(255, 0, 255)));

    // create a QString from the data and set the global
    // flags indicating if the string contains the error
    // and/or warning words.
    QString s(stringData);
    bool messageContainsErrors   = (s.indexOf("error",   0, Qt::CaseInsensitive) >= 0);
    bool messageContainsWarnings = (s.indexOf("warning", 0, Qt::CaseInsensitive) >= 0);

    // split the string using the keywords.
    QStringList messages;
    while (true)
    {
      int sIndex = -1;
      int kIndex = -1;
      for (size_t i=0;i<keywords.size();i++)
      {
        int index = s.indexOf(keywords[i].first, 0, Qt::CaseInsensitive);
        if (index < 0)
          continue;
        if (sIndex < 0 || index < sIndex)
        {
          sIndex = index;
          kIndex = i;
        }
      }
      if (sIndex < 0)
      {
        if (s.size())
          messages.push_back(s);
        break;
      }
      else
      {
        if (sIndex)
          messages.push_back(s.mid(0, sIndex));
        messages.push_back(s.mid(sIndex, keywords[kIndex].first.size()));
        s = s.mid(sIndex + keywords[kIndex].first.size());
      }
    }

    // insert the colorized strings into
    // the log widgets' plain text edits.
    for(size_t i=0;i<sLogWidgets.size();i++)
    {
      QPlainTextEdit &t = *sLogWidgets[i]->m_text;

      QTextCharFormat format = t.currentCharFormat();
      for (int j=0;j<messages.size();j++)
      {
        bool messageIsKeyword = false;
        QColor textColor(defaultTextColor);
        for (size_t k=0;k<keywords.size();k++)
        {
          if (keywords[k].first.compare(messages[j], Qt::CaseInsensitive))
            continue;
          textColor = keywords[k].second;
          messageIsKeyword = true;
          break;
        }
        if (messageContainsErrors)
        {
          if (!messageIsKeyword)
          {
            textColor.setRed  (255);
            textColor.setGreen( 20);
            textColor.setBlue ( 10);
          }
        }
        else if (messageContainsWarnings)
        {
          textColor.setRed  (230);
          textColor.setGreen(230);
          textColor.setBlue ( 10);
        }
        if (j == 0)
          t.appendPlainText("");
        format.setForeground(textColor);
        t.setCurrentCharFormat(format);
        t.moveCursor(QTextCursor::End);
        t.setCurrentCharFormat(format);
        t.insertPlainText(messages[j]);
      }

      // [FE-6563] scroll to the last line.
      QTextCursor cursor = t.textCursor();
      cursor.movePosition(QTextCursor::End);
      t.setTextCursor(cursor);
      t.ensureCursorVisible();
    }
  }
  else
  {
    sUnconsumedMessages.push_back(stringData);
  }

  if(s_logFunc)
    (*s_logFunc)(stringData);
  else
    printf("%s\n", stringData);
}

void DFGLogWidget::keyPressEvent(QKeyEvent * event)
{
  event->accept();
}

void DFGLogWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
  if(event->button() == Qt::MiddleButton)
  {
    // [FABMODO-3]
    event->accept();
    return;
  }

  if(event->button() == Qt::RightButton)
  {
    // [FABMODO-3]
    event->accept();
    return;
  }

  QWidget::mouseDoubleClickEvent(event);
}

void DFGLogWidget::mousePressEvent(QMouseEvent * event)
{
  if(event->button() == Qt::MiddleButton)
  {
    // [FABMODO-3]
    event->accept();
    return;
  }

  if(event->button() == Qt::RightButton)
  {
    // [FABMODO-3]
    event->accept();
    return;
  }

  QWidget::mousePressEvent(event);
}

void DFGLogWidget::mouseReleaseEvent(QMouseEvent * event)
{
  QWidget::mouseReleaseEvent(event);
}

void DFGLogWidget::setLogFunc(DFGController::LogFunc func)
{
  s_logFunc = func;
}

void DFGLogWidget::showContextMenu( QPoint const &pos )
{
  QMenu *menu = m_text->createStandardContextMenu();

  menu->addSeparator();

  QAction *clearAction = new QAction( "Clear", menu );
  connect(
    clearAction, SIGNAL(triggered()),
    this, SLOT(clear())
    );
  menu->addAction( clearAction );

  menu->popup( m_text->mapToGlobal(pos) );
}

void DFGLogWidget::clear()
{
  m_text->clear();
}
