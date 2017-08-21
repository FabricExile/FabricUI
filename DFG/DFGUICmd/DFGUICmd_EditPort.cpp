// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_EditPort.h>

#include <FTL/JSONValue.h>
#include <FTL/JSONException.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_EditPort::appendDesc( QString &desc )
{
  desc += "Edit ";
  appendDesc_PortPath( m_oldPortPath, desc );
}

void DFGUICmd_EditPort::invoke( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNewPortName =
    invoke(
      getExecPath().toUtf8().constData(),
      m_oldPortPath.toUtf8().constData(),
      m_desiredNewPortName.toUtf8().constData(),
      m_portType,
      m_typeSpec.toUtf8().constData(),
      m_extDep.toUtf8().constData(),
      m_uiMetadata.toUtf8().constData(),
      coreUndoCount
      );
  m_actualNewPortName =
    QString::fromUtf8( actualNewPortName.data(), actualNewPortName.size() );
}

FTL::CStrRef DFGUICmd_EditPort::invoke(
  FTL::CStrRef execPath,
  FTL::CStrRef oldPortPath,
  FTL::CStrRef desiredNewPortName,
  FabricCore::DFGPortType portType,
  FTL::CStrRef typeSpec,
  FTL::CStrRef extDep,
  FTL::CStrRef uiMetadata,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGBinding &binding = getBinding();
  FabricCore::DFGExec &exec = getExec();

  if ( !extDep.empty() )
  {
    exec.addExtDep( extDep.c_str() );
    ++coreUndoCount;
  }

  FTL::CStrRef oldPortName = oldPortPath.rsplit('.').second;
  FTL::CStrRef newPortName;
  std::string newPortPath;
  if ( !desiredNewPortName.empty()
    && desiredNewPortName != oldPortName )
  {
    newPortName = exec.renameExecPort(
      oldPortPath.c_str(),
      desiredNewPortName.c_str()
      );
    FTL::CStrRef::Split split = oldPortPath.rsplit('.');
    newPortPath = split.first;
    if ( !newPortPath.empty() )
      newPortPath += '.';
    newPortPath += newPortName;
    ++coreUndoCount;
  }
  else
  {
    newPortPath = oldPortPath;
    newPortName = oldPortPath.rsplit('.').second;
  }

  bool isOneLevelPath = newPortPath.find('.') == newPortPath.npos;
  bool isExecPort = isOneLevelPath && exec.isExecPort( newPortPath.c_str() );

  if ( isExecPort || !isOneLevelPath )
  {
    FabricCore::DFGPortType oldPortType =
      exec.getOutsidePortType( newPortPath.c_str() );
    if ( portType != oldPortType )
    {
      exec.setOutsidePortType( newPortPath.c_str(), portType );
      ++coreUndoCount;
    }
  }
  
  // Only set type & value if the type is different (else we loose the value even if the type didn't change!)
  FTL::CStrRef oldTypeSpec = exec.getPortTypeSpec( newPortPath.c_str() );
  if ( !typeSpec.empty() && oldTypeSpec != typeSpec )
  {
    exec.setPortTypeSpec(
      newPortPath.c_str(),
      typeSpec.c_str()
      );
    ++coreUndoCount;

    if ( isExecPort
      && execPath.empty()
      && !typeSpec.empty()
      && typeSpec.find('$') == typeSpec.end() )
    {
      FabricCore::DFGHost host = binding.getHost();
      FabricCore::Context context = host.getContext();

      FabricCore::RTVal argValue =
        FabricCore::RTVal::Construct(
          context,
          typeSpec.c_str(),
          0,
          0
          );
      binding.setArgValue(
        newPortName.c_str(),
        argValue,
        true
        );
      ++coreUndoCount;
    }
  }

  if ( !uiMetadata.empty() )
  {
    try
    {
      FTL::JSONStrWithLoc swl( uiMetadata );
      FTL::OwnedPtr<FTL::JSONObject> jo(
        FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
        );

      for ( FTL::JSONObject::const_iterator it = jo->begin();
        it != jo->end(); ++it )
      {
        FTL::CStrRef key = it->key();
        FTL::CStrRef value = it->value()->getStringValue();
        exec.setPortMetadata(
          newPortPath.c_str(),
          key.c_str(),
          !value.empty()? value.c_str(): NULL,
          true
          );
        ++coreUndoCount;
      }
    }
    catch ( FTL::JSONException e )
    {
      printf("DFGUICmd_EditPort: JSON exception: '%s'\n", e.getDescCStr());
    }
  }

  return newPortName;
}

FABRIC_UI_DFG_NAMESPACE_END
