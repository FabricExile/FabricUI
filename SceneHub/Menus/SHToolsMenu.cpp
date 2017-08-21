/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include "SHToolsMenu.h"
#include <QKeySequence>

using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;
using namespace Menus;

SHToolsMenu::SHToolsMenu(
  SHGLRenderer* shGLRenderer, 
  QWidget *parent)
  : Menus::BaseMenu(shGLRenderer->getClient(), "Tools", parent)
  , m_shGLRenderer(shGLRenderer) {
  QObject::connect(this, SIGNAL(aboutToShow()), this, SLOT(onConstructMenu()));
  m_delimiter = "\t";
}

SHToolsMenu::~SHToolsMenu() {
}

inline void GetBasicInfo(
  RTVal description, 
  QString &name, 
  QString &key, 
  bool &isEnabled) 
{
  name = QString(description.maybeGetMember("name").getStringCString());
  key = QString(QKeySequence(description.maybeGetMember("enableKey").getUInt32()).toString());
  isEnabled = description.maybeGetMember("isEnabled").getBoolean();
}

inline QString GetLegacyHandlerExclusivity(RTVal description) {
  unsigned int type(description.maybeGetMember("handlerExclusivity").getUInt32());
  return  type == 0 ? "Disable None" : 
          type == 1 ? "Disable Others" : "Disable All";
}

inline void CreateToolAction(
  QString actionText, 
  bool isEnabled, 
  QMenu *parent, 
  QMenu *menu) 
{
  QAction *action = menu->addAction(actionText);
  QObject::connect(action, SIGNAL(triggered()), parent, SLOT(onActiveTool()));
  action->setCheckable(true);
  if(isEnabled) action->setChecked(true);
}

inline void CreateDispatcherAction(
  QString actionText, 
  bool isEnabled, 
  QMenu *parent, 
  QMenu *menu) 
{
  QAction *action = menu->addAction(actionText);
  QObject::connect(action, SIGNAL(triggered()), parent, SLOT(onActiveDispatcher()));
  action->setCheckable(true);
  if(isEnabled) action->setChecked(true);
}

inline void ConstructToolDescriptionMenu(
  RTVal descriptionIn, 
  Client client,
  QString delimiter,
  SHToolsMenu *shToolsMenu,
  QMenu *menu) 
{
  try 
  {
    QString type(descriptionIn.callMethod("String", "type", 0, 0).getStringCString());
    RTVal description = RTVal::Construct(client, type.toUtf8().constData(), 1, &descriptionIn);

    bool isEnabled; QString name, key;
    GetBasicInfo(description, name, key, isEnabled);
    //QString typeStr = GetLegacyHandlerExclusivity(description);

    RTVal toolModes = description.maybeGetMember("modes");
    if(!toolModes.isValid() || (toolModes.isValid() && toolModes.getArraySize() == 0))
      CreateToolAction(
        QString(name /*+ delimiter + typeStr*/ + delimiter + key), 
        isEnabled,
        shToolsMenu, 
        menu);    
    
    else if(toolModes.isValid() && toolModes.getArraySize() > 0)
    {
      QMenu *toolMenu = new Menus::BaseMenu(client, name /*+ delimiter + typeStr*/, shToolsMenu);
       
      CreateToolAction(
        QString("Enable Key" + delimiter + key), 
        isEnabled,
        shToolsMenu, 
        toolMenu);

      for(unsigned int j=0; j<toolModes.getArraySize(); ++j)
      {
        RTVal modeTemp = toolModes.getArrayElement(j);
        QString modeType(modeTemp.callMethod("String", "type", 0, 0).getStringCString());
        RTVal mode = RTVal::Construct(client, modeType.toUtf8().constData(), 1, &modeTemp);

        bool isModeEnabled; QString modeName, modekey;
        GetBasicInfo(mode, modeName, modekey, isModeEnabled);
        
        CreateToolAction(
          QString(modeName + delimiter + modekey), 
          isModeEnabled,
          shToolsMenu, 
          toolMenu);
      }

      menu->addMenu(toolMenu);
    }
  }
  catch(Exception e)
  {
    printf("SHToolsMenu::constructMenu: exception: %s\n", e.getDesc_cstr());
  }
}

inline void ConstructLegacyDispatcherDescriptionMenu(
  RTVal descriptionIn, 
  Client client,
  QString delimiter,
  SHToolsMenu *shToolsMenu,
  QMenu *menu) 
{
  try 
  {
    QString type(descriptionIn.callMethod("String", "type", 0, 0).getStringCString());
    RTVal description = RTVal::Construct(client, type.toUtf8().constData(), 1, &descriptionIn);

    bool isEnabled; QString name, key;
    GetBasicInfo(description, name, key, isEnabled);

    QMenu *toolMenu = 0;
    if(name == "ToolDispatcher")
      toolMenu = shToolsMenu;
    else
      toolMenu = new Menus::BaseMenu(client, name /*+ delimiter + typeStr*/, shToolsMenu);
     
    CreateDispatcherAction(
      QString("Enable Key" + delimiter + "Shift+" + delimiter + key), 
      isEnabled,
      shToolsMenu, 
      toolMenu);   

    RTVal descriptionList = description.maybeGetMember("handlers");
    for(unsigned int i=0; i<descriptionList.getArraySize(); ++i)
    {
      RTVal descr = descriptionList.getArrayElement(i);
      QString type(descr.callMethod("String", "type", 0, 0).getStringCString());

      if(type == "ToolDescription") 
        ConstructToolDescriptionMenu(
          descr, 
          client, 
          delimiter,
          shToolsMenu,
          toolMenu);

      else if (type == "LegacyDispatcherDescription")
        ConstructLegacyDispatcherDescriptionMenu(
          descr, 
          client, 
          delimiter,
          shToolsMenu,
          toolMenu);
    }
    
    if(name != "ToolDispatcher")
      menu->addMenu(toolMenu);
  }
  catch(Exception e)
  {
    printf("SHToolsMenu::constructMenu: exception: %s\n", e.getDesc_cstr());
  }
}

void SHToolsMenu::constructMenu() {
  try 
  {
    if(!m_client.isValid())
      m_client = m_shGLRenderer->getClient();

    if(!m_client.isValid()) 
      return;

    RTVal description = m_shGLRenderer->getDescription();
    if(description.isValid())
      ConstructLegacyDispatcherDescriptionMenu(
        description, 
        m_client, 
        m_delimiter,
        this,
        this);
  } 
  catch(Exception e)
  {
    printf("SHToolsMenu::constructMenu: exception: %s\n", e.getDesc_cstr());
  }
}

void SHToolsMenu::onConstructMenu() {
  clear();
  constructMenu();
}

inline QKeyEvent CreateQKeyEvent(
  QAction *action, 
  QString delimiter,
  bool activeDispatcher) {
  //From the sender action text : name \tab type \tab Key
  //gets the toolKey only
  QStringList temp = QString(action->text()).split(delimiter);
  QString toolKey = temp[temp.size()-1].trimmed();
  //From the toolKey, create a key event
  QKeySequence seq(toolKey);
  
  return QKeyEvent(
    QEvent::KeyPress, 
    seq[0], 
    (activeDispatcher) ? Qt::ShiftModifier : Qt::NoModifier
  );
}

void SHToolsMenu::onActiveTool() {
  QAction *action = qobject_cast<QAction *>(QObject::sender());
  QKeyEvent qtKey = CreateQKeyEvent(action, m_delimiter, false);
  //Viewport 0
  m_shGLRenderer->onEvent(0, &qtKey, false);
}

void SHToolsMenu::onActiveDispatcher() {
  QAction *action = qobject_cast<QAction *>(QObject::sender());
  QKeyEvent qtKey = CreateQKeyEvent(action, m_delimiter, true);
  m_shGLRenderer->onEvent(0, &qtKey, false);
}
