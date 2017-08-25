//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGAnimXFCurveModel.h"

#include <FabricUI/Commands/CommandManager.h>
#include <FabricUI/Commands/KLCommandRegistry.h> // HACK: remove

#include <assert.h>

using namespace FabricUI::FCurveEditor;

inline void AddKeyValueToArgs( QMap<QString, QString>& args, const Key& k )
{
  args["time"] = QString::number( k.pos.x() );
  args["value"] = QString::number( k.pos.y() );
  args["tanInType"] = QString::number( k.tanInType );
  args["tanInX"] = QString::number( k.tanIn.x() );
  args["tanInY"] = QString::number( k.tanIn.y() );
  args["tanOutType"] = QString::number( k.tanOutType );
  args["tanOutX"] = QString::number( k.tanOut.x() );
  args["tanOutY"] = QString::number( k.tanOut.y() );
}

inline void SynchronizeKLReg()
{
  static_cast<FabricUI::Commands::KLCommandRegistry*>( FabricUI::Commands::KLCommandRegistry::getCommandRegistry() )
    ->synchronizeKL(); // HACK : remove
}

void RTValAnimXFCurveDFGController::setPath( const char* bindingId, const char* dfgPortPath )
{
  m_dfgPath = dfgPortPath;

  // FE-8736 : if the current executable is the root
  // The path has the form '.node.port' or , remove the first '.'
  if(m_dfgPath.mid(0, 1) == ".")
    m_dfgPath = m_dfgPath.mid(1);

  m_dfgPath = "<" + bindingId + "." + m_dfgPath + ">"
}

void RTValAnimXFCurveDFGController::setKey( size_t i, Key h, bool autoTangent )
{
  FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
  SynchronizeKLReg();
  FabricCore::RTVal bRV = FabricCore::RTVal::ConstructBoolean( m_val.getContext(), true );
  const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "", "useIds", 1, &bRV );
  QMap<QString, QString> args;
  args["target"] = m_dfgPath;
  args["id"] = QString::number( i );
  AddKeyValueToArgs( args, h );
  args["interactionEnd"] = m_isInteracting ? "false" : "true";
  args["autoTangent"] = autoTangent ? "true" : "false";
  QString cmdName = "AnimX_SetKeyframe";
  manager->createCommand( cmdName, args, true,
    m_isInteracting ? m_interactionId : FabricUI::Commands::CommandManager::NoCanMergeID );
  m_lastCommand = cmdName;
  m_lastArgs = args;
  emit this->dirty();
}

void RTValAnimXFCurveDFGController::setKey( size_t i, Key k )
{
  this->setKey( i, k, false );
}

void RTValAnimXFCurveDFGController::autoTangent( size_t i )
{
  this->setKey( i, this->getKey( i ), true );
}

inline QString SerializeQS( const size_t* indices, const size_t nbIndices )
{
  QString indicesStr = "[";
  for( size_t i = 0; i < nbIndices; i++ )
  {
    if( i > 0 )
      indicesStr += ",";
    indicesStr += QString::number( indices[i] );
  }
  indicesStr += "]";
  return indicesStr;
}

void RTValAnimXFCurveDFGController::moveKeys( const size_t* indices, const size_t nbIndices, QPointF delta )
{
  FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
  SynchronizeKLReg();
  FabricCore::RTVal bRV = FabricCore::RTVal::ConstructBoolean( m_val.getContext(), true );
  const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "", "useIds", 1, &bRV );
  QMap<QString, QString> args;
  args["target"] = m_dfgPath;
  args["ids"] = SerializeQS( indices, nbIndices );
  args["dx"] = QString::number( delta.x() );
  args["dy"] = QString::number( delta.y() );
  args["interactionEnd"] = m_isInteracting ? "false" : "true";
  QString cmdName = "AnimX_MoveKeyframes";
  manager->createCommand( cmdName, args, true, m_interactionId );
  m_lastCommand = cmdName;
  m_lastArgs = args;
  emit this->dirty();
}

void RTValAnimXFCurveDFGController::addKey( Key k, bool useKey, bool autoTangent )
{
  FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
  SynchronizeKLReg();
  QMap<QString, QString> args;
  args["target"] = m_dfgPath;
  if( useKey )
    AddKeyValueToArgs( args, k );
  args["autoTangent"] = autoTangent ? "true" : "false";
  manager->createCommand( "AnimX_PushKeyframe", args );
  emit this->dirty();
}

void RTValAnimXFCurveDFGController::deleteKey( size_t i )
{
  FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
  SynchronizeKLReg();
  QMap<QString, QString> args;
  args["target"] = m_dfgPath;
  args["id"] = QString::number( i );
  manager->createCommand( "AnimX_RemoveKeyframe", args );
  emit this->dirty();
}

void RTValAnimXFCurveDFGController::deleteKeys( const size_t* indices, const size_t nbIndices )
{
  FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
  SynchronizeKLReg();
  QMap<QString, QString> args;
  args["target"] = m_dfgPath;
  args["ids"] = SerializeQS( indices, nbIndices );
  manager->createCommand( "AnimX_RemoveKeyframes", args );
  emit this->dirty();
}

void RTValAnimXFCurveDFGController::setPreInfinityType( size_t i )
{
  FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
  SynchronizeKLReg();
  QMap<QString, QString> args;
  args["target"] = m_dfgPath;
  args["type"] = QString::number( i );
  manager->createCommand( "AnimX_SetPreInfinityType", args );
  emit this->dirty();
}

void RTValAnimXFCurveDFGController::setPostInfinityType( size_t i )
{
  FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
  SynchronizeKLReg();
  QMap<QString, QString> args;
  args["target"] = m_dfgPath;
  args["type"] = QString::number( i );
  manager->createCommand( "AnimX_SetPostInfinityType", args );
  emit this->dirty();
}

void RTValAnimXFCurveDFGController::onInteractionBegin()
{
  m_interactionId = FabricUI::Commands::CommandManager::getCommandManager()->getNewCanMergeID();
  m_isInteracting = true;
}

void RTValAnimXFCurveDFGController::onInteractionEnd()
{
  m_isInteracting = false;
  if( !m_lastCommand.isEmpty() )
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    SynchronizeKLReg();
    QMap<QString, QString> args = m_lastArgs;
    args["interactionEnd"] = "true";
    manager->createCommand( m_lastCommand, args, true, m_interactionId );
    m_lastCommand = "";
    emit this->dirty();
  }
}
