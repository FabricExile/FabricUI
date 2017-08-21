// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_FabricSplashScreen__
#define __UI_FabricSplashScreen__

#include <QSplashScreen>

namespace FabricUI
{
  class FabricSplashScreen : public QSplashScreen 
  {
    Q_OBJECT

  public:

    FabricSplashScreen(const QPixmap & pixmap);
    virtual ~FabricSplashScreen();

    void setMessage(QString message);

    static FabricSplashScreen * getSplashScreen(bool create=false);
    static void showSplashScreen();
    static void hideSplashScreen();

  private:

    static FabricSplashScreen * s_splashScreen;
    static unsigned int s_bracketCount;
  };

  class FabricSplashScreenBracket
  {
  public:
    FabricSplashScreenBracket(bool enabled = true)
    : m_enabled(enabled)
    {
      if(m_enabled)
        FabricSplashScreen::showSplashScreen();
    }

    ~FabricSplashScreenBracket()
    {
      if(m_enabled)
        FabricSplashScreen::hideSplashScreen();
    }
  private:
    bool m_enabled;
  };
};

#endif // __UI_BaseDialog__
