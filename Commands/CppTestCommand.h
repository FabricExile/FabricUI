//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_CPP_TEST_COMMAND_REGISTRY__
#define __UI_CPP_TEST_COMMAND_REGISTRY__

#include "KLCommandRegistry.h"
#include "RTValCommandManager.h"
#include "BaseScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

namespace FabricUI {
namespace Commands {

/**
  For unit-tests.
*/
class CppTestScriptableCommand : public BaseScriptableCommand 
{
  Q_OBJECT

  public:
    CppTestScriptableCommand() 
      : BaseScriptableCommand()
    {
      BaseScriptableCommand::declareArg(
        "arg_1",
        CommandArgFlags::OPTIONAL_ARG,
        "arg_1_default_Cpp");
    }

    virtual ~CppTestScriptableCommand() 
    {
    }
 
    virtual bool canUndo()
    {
      return true;
    }
 
    virtual bool doIt()
    {
      return true;
    }
 
    virtual bool undoIt()
    {
      std::cout << "Undo " << getName().toUtf8().constData() << std::endl;
      return true;
    }
 
    virtual bool redoIt()
    {
      std::cout << "Redo " << getName().toUtf8().constData() << std::endl;
      return true;
    }

    virtual QString getHelp() 
    {
      return getName();
    }
};

class CppTestMetaCommand : public CppTestScriptableCommand 
{
  Q_OBJECT
  
  public:
    CppTestMetaCommand() 
      : CppTestScriptableCommand()
    {
    }

    virtual ~CppTestMetaCommand() 
    {
    }
 
    virtual bool doIt()
    {
      QMap<QString, QString> args;
      args["arg_1"] = "string_Cpp";

      CommandManager::getCommandManager()->createCommand(
        "cppTestScriptableCommand", 
        args);

      CommandManager::getCommandManager()->createCommand(
        "cppTestScriptableCommand", 
        args);

      return true;
    }
};

class CppToPythonTestCommand
{
  public:
    CppToPythonTestCommand()
    {
    }

    /// Register C++ commands to create them in python.
    static void RegisterCppTestCommands() 
    {
      CommandFactory<CppTestScriptableCommand>::Register("cppTestScriptableCommand");
      CommandFactory<CppTestMetaCommand>::Register("cppTestMetaCommand");
    }

    /// Creates C++ commands from C++.
    static void CreateCppTestCommands() 
    {
      QMap<QString, QString> args;
      args["arg_1"] = "string_Cpp";

      CommandManager::getCommandManager()->createCommand(
        "cppTestScriptableCommand", 
        args);

      CommandManager::getCommandManager()->createCommand(
        "cppTestMetaCommand");
    }

    /// Creates python commands from C++.
    static void CreatePythonTestCommands() 
    {
      QMap<QString, QString> args;
      args["arg_1"] = "string_Cpp";

      CommandManager::getCommandManager()->createCommand(
        "testScriptableCmd", 
        args);

      CommandManager::getCommandManager()->createCommand(
        "testMetaCmd");
    }

    /// Register the KL commands in KL.
    /// see Ext/Builtin/FabbricInterfaces/Commands/KLTestCommands.kl.
    static void RegisterKLTestCommands() 
    {
      try
      {
        KLCommandRegistry *registry = qobject_cast<KLCommandRegistry *>(
          CommandRegistry::getCommandRegistry());

        FabricCore::RTVal CppToKLTestCommand = FabricCore::RTVal::Create(
          Application::FabricApplicationStates::GetAppStates()->getContext(), 
          "CppToKLTestCommand", 
          0, 
          0);

        CppToKLTestCommand.callMethod(
          "", 
          "registerKLTestCommands", 
          0, 
          0);  

        // Synchronise the stack
        registry->synchronizeKL();
      }    

      catch(FabricCore::Exception e)
      {
        Application::FabricException::Throw(
          "CppToPythonTestCommand::RegisterKLTestCommands",
          e.getDesc_cstr(),
          "",
          Application::FabricException::LOG);
      }
    }

    /// Creates KL commands from KL.
    static void CreateKLTestCommandsInKL() 
    {
      try
      {
        FabricCore::RTVal CppToKLTestCommand = FabricCore::RTVal::Create(
          Application::FabricApplicationStates::GetAppStates()->getContext(), 
          "CppToKLTestCommand", 
          0, 
          0);

        CppToKLTestCommand.callMethod(
          "", 
          "createKLTestCommands", 
          0, 
          0);  
      }

      catch(FabricCore::Exception e)
      {
        Application::FabricException::Throw(
          "CppToPythonTestCommand::CreateKLTestCommandsInKL",
          e.getDesc_cstr(),
          "",
          Application::FabricException::LOG);
      }
    }

    /// Creates KL commands from C++.
    static void CreateKLTestCommands() 
    {
      try
      {
        RTValCommandManager *manager = (RTValCommandManager *)(
          CommandManager::getCommandManager());

        manager->CommandManager::createCommand("klTestCommand");
        manager->CommandManager::createCommand("klTestMetaCommand");
 
        FabricCore::RTVal strRTVal = FabricCore::RTVal::ConstructString(
          Application::FabricApplicationStates::GetAppStates()->getContext(), 
          "string_Cpp");

        FabricCore::RTVal floatRTVal = FabricCore::RTVal::ConstructFloat64(
          Application::FabricApplicationStates::GetAppStates()->getContext(), 
          4.555f);
     
        QMap<QString, FabricCore::RTVal> args;
        args["arg_1"] = strRTVal;
        args["arg_2"] = floatRTVal;

        manager->createCommand(
          "klTestScriptableCommand", 
          args);

        manager->CommandManager::createCommand(
          "klTestScriptableMetaCommand");
      }

      catch(FabricCore::Exception e)
      {
        Application::FabricException::Throw(
          "CppToPythonTestCommand::CreateKLTestCommands",
          e.getDesc_cstr(),
          "",
          Application::FabricException::LOG);
      }
    }
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_CPP_TEST_COMMAND_REGISTRY__
