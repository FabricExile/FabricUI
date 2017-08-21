//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_FABRIC_EXCEPTION__
#define __UI_FABRIC_EXCEPTION__

#include <string>
#include <QString>
#include <iostream>
#include <exception>
#include <FabricCore.h>
#include <FTL/JSONEnc.h>
#include <FTL/JSONDec.h>
#include <FTL/JSONValue.h>

namespace FabricUI {
namespace Application {

class FabricException : public std::exception
{
  /**
    FabricException is the exception class for Fabric applications.
    It provides a static method `Throw` that propagates the exceptions
    in order to provide the complete code-path of the error. 

    It defines a macro FABRIC_CATCH_BEGIN() - FABRIC_CATCH_END() that 
    catchs and then re-throws: 
    - JSON Exception
    - Fabric Exception
    - FabricCore Exception
  */
  public: 
    static int NOTHING;
    static int THROW;
    static int LOG;

    FabricException(
      QString const&message
      );
    
    virtual ~FabricException() throw();

    /// Throws and/or logs a FabricException.
    /// \param method Name of the method that fails.
    /// \param error The error to throw/log.
    /// \param childError A child error
    /// \param flag (THROW, LOG)
    static void Throw(
      QString const&method,
      QString const&error = QString(),
      QString const&childError = QString(),
      int flag = THROW
      );

    /// Logs the errors, to overide.
    /// The default implementation logs
    /// the error using std::cerr.
    virtual void log() const throw();

    /// Implementation of std::exception.
    virtual const char* what() const throw();

  protected:
    std::string m_message;
};

} // namespace Commands
} // namespace FabricUI

#define FABRIC_CATCH_BEGIN() \
  try {

#define FABRIC_CATCH_END(methodName) \
  } \
  catch (FabricCore::Exception &e) \
  { \
    FabricUI::Application::FabricException::Throw( \
      methodName, \
      "Caught Core Exception", \
      e.getDesc_cstr() \
      ); \
  } \
  catch (FTL::JSONException &je) \
  { \
    FabricUI::Application::FabricException::Throw( \
      methodName, \
      "Caught JSON Exception", \
      je.getDescCStr() \
      ); \
  } \
  catch (FabricUI::Application::FabricException &e) \
  { \
    FabricUI::Application::FabricException::Throw( \
      methodName, \
      "Caught App Exception", \
      e.what() \
      ); \
  } 

#endif // __UI_FABRIC_EXCEPTION__
