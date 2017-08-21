// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/Util/LoadFabricStyleSheet.h>
#include "FabricSplashScreen.h"

#include <QFont>
#include <QColor>
#include <QApplication>
#include <FTL/StrRef.h>

#include <FabricCore.h>

using namespace FabricUI;

FabricSplashScreen * FabricSplashScreen::s_splashScreen = NULL;
unsigned int FabricSplashScreen::s_bracketCount = 0;

FabricSplashScreen::FabricSplashScreen(const QPixmap & pixmap)
: QSplashScreen(pixmap)
{
  setFont(QFont("Roboto", 8, QFont::Normal));
  setAttribute(Qt::WA_DeleteOnClose);
  setMessage("");
}

FabricSplashScreen::~FabricSplashScreen()
{
}

void FabricSplashScreen::setMessage(QString message)
{
  QString productName, version, copyright;
  productName = FabricCore::GetProductNameStr();
  version = FabricCore::GetVersionWithBuildInfoStr();
  copyright = FabricCore::GetCopyrightStr();

  QString composedMessage = productName + " " + version + "\n" + copyright;
  if(message.length() > 0)
    composedMessage += "\n" + message + " ...";

  showMessage(composedMessage, Qt::AlignLeft | Qt::AlignBottom, QColor(Qt::white));
  QApplication::processEvents();
}

FabricSplashScreen * FabricSplashScreen::getSplashScreen(bool create)
{
  if(s_splashScreen == NULL)
  {
    if(!create)
      return NULL;

    QPixmap pixmap;

    FTL::StrRef FABRIC_DIR = getenv("FABRIC_DIR");
    if(!FABRIC_DIR.empty())
    {
      QString path = FABRIC_DIR.data();
      path += "/Resources/canvas-splash.png";
      pixmap = QPixmap(path);
    }

    s_splashScreen = new FabricSplashScreen(pixmap);
    s_splashScreen->show();
  }
  return s_splashScreen;
}

void FabricSplashScreen::showSplashScreen()
{
  s_bracketCount++;
  getSplashScreen(true);
}

void FabricSplashScreen::hideSplashScreen()
{
  if(s_bracketCount > 0)
  {
    s_bracketCount--;
  }

  if(s_bracketCount == 0)
  {
    if(s_splashScreen == NULL)
      return;
    s_splashScreen->close();
    s_splashScreen = NULL;
  }
}
