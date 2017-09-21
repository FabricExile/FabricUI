//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "FabricException.h"

using namespace FabricUI;
using namespace Application;

FabricLog* FabricLog::s_log = 0;

FabricLog::FabricLog()
{
  if(s_log != 0)
    FabricException::Throw(
      "FabricLog::FabricLog",
      "the singleton has been set already");

  s_log = this;
}

FabricLog::~FabricLog()
{
  if( s_log == this )
    s_log = 0;
}

FabricLog* FabricLog::get() 
{
  if(s_log == 0)
    return new FabricLog();
  return s_log;
}

void FabricLog::log(
  QString const&message) 
{
  std::cerr 
    << message.toUtf8().constData() 
    << std::endl;
}

int FabricException::NOTHING = 0;
int FabricException::THROW = 1;
int FabricException::LOG = 2;

FabricException::FabricException(
  QString const&message)
  : m_message(message.toUtf8().constData())
{
}

FabricException::~FabricException() throw() {}

void FabricException::Throw(
  QString const&method,
  QString const&error,
  QString const&childError,
  int flag)
{
  QString cmdError;

  if(!method.isEmpty())
    cmdError += "" + method + ", error: " + error;
  
  if(!childError.isEmpty()) 
    cmdError += "\n" + childError;

  if(flag & LOG)
  {
    FabricException exception(cmdError);
    exception.log();
  }
 
  if(flag & THROW)
    throw FabricException(cmdError);
}

void FabricException::log() const throw()
{
  FabricLog::get()->log(
    m_message.c_str()
    );
}

const char* FabricException::what() const throw()
{
  return m_message.c_str();
}
