//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "FabricException.h"

using namespace FabricUI;
using namespace Application;

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
  std::cerr << m_message.c_str() << std::endl;
}

const char* FabricException::what() const throw()
{
  return m_message.c_str();
}
