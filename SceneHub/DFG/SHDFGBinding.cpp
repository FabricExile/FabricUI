/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include "SHDFGBinding.h"
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGWidget.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;
 

SHDFGBinding::SHDFGBinding(
  DFGBinding &binding, 
  DFG::DFGController *controller,
  SHStates *shStates) 
  : m_binding(binding)
  , m_controller(controller) 
  , m_shStates(shStates)
{ 
  connectBindingNotifier();
  m_computeContextVal = RTVal::Create(controller->getClient(), "ComputeContextRTValWrapper", 0, 0);
}

void SHDFGBinding::setCanvasOperator(RTVal &canvasOperator) {
  try 
  {
    m_dfgCanvasOperator = canvasOperator;
    if( m_dfgCanvasOperator.isValid() ) 
    {
      RTVal dfgBindingVal = m_dfgCanvasOperator.callMethod("DFGBinding", "getDFGBinding", 0, 0);
      DFGBinding binding = dfgBindingVal.getDFGBinding();
      m_controller->getDFGWidget()->replaceBinding(binding);
    } 
    else 
    {
      // return to the standard binding
      m_controller->getDFGWidget()->replaceBinding(m_binding);
    }
  }
  catch(Exception e) 
  {
    printf("SceneHubWindow::setCanvasOperator: exception: %s\n", e.getDesc_cstr());
  }
  connectBindingNotifier();
}

RTVal SHDFGBinding::getCanvasOperator() { 
  return m_dfgCanvasOperator; 
}

RTVal SHDFGBinding::getCanvasOperatorParameterObject() {
  try 
  {
    if(m_dfgCanvasOperator.isValid()) 
    {
      RTVal parameterObject = m_dfgCanvasOperator.callMethod("SGObject", "getParameterContainer", 0, 0);
      if( parameterObject.callMethod("Boolean", "isValid", 0, 0 ).getBoolean())
        return parameterObject;
    }
  }
  catch(Exception e) 
  {
    printf("SceneHubWindow::getCanvasOperatorParameterObject: exception: %s\n", e.getDesc_cstr());
  }
  return RTVal();
}

void SHDFGBinding::setMainBinding(DFGBinding &binding) {
  m_dfgCanvasOperator = RTVal();
  m_binding = binding;
}

QList<bool> SHDFGBinding::setDirty() {
  QList<bool> dirtyList;
  // accepted
  dirtyList.append(m_dfgCanvasOperator.isValid());
  // refresh
  dirtyList.append(false);
  if(dirtyList[0])
  {
    try
    {
      //Ignore: this is from runtime KL Bindings computation (we are already redrawing)
      if(!m_computeContextVal.callMethod("Boolean", "hasExecutingKLDFGBinding", 0, 0 ).getBoolean())
      {
        dirtyList[1] = true;
        m_dfgCanvasOperator.callMethod("", "setDirty", 0, 0);
      }
    }
    catch(Exception e)
    {
      printf("SHDFGBinding::setDirty: exception: %s\n", e.getDesc_cstr());
    }
  }
  return dirtyList;
}

void SHDFGBinding::onArgInserted(unsigned index, FTL::CStrRef name, FTL::CStrRef typeName) {
  if(!m_dfgCanvasOperator.isValid()) return;
  try 
  {
    m_dfgCanvasOperator.callMethod("", "updatePropertySetFromBinding", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHDFGBinding::onArgInserted: exception: %s\n", e.getDesc_cstr());
  }
  emit sceneChanged();
}

void SHDFGBinding::onArgRemoved(unsigned index, FTL::CStrRef name) {
  if(!m_dfgCanvasOperator.isValid()) return;
  try 
  {
    m_dfgCanvasOperator.callMethod("", "updatePropertySetFromBinding", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHDFGBinding::getAssetLibraryRoot: exception: %s\n", e.getDesc_cstr());
  }
  emit sceneChanged();
}

void SHDFGBinding::onArgTypeChanged(unsigned index, FTL::CStrRef name, FTL::CStrRef newTypeName) {
  if(!m_dfgCanvasOperator.isValid())  return;
  try 
  {
    m_dfgCanvasOperator.callMethod("", "updatePropertySetFromBinding", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHDFGBinding::onArgTypeChanged: exception: %s\n", e.getDesc_cstr());
  }
  emit sceneChanged();
}

void SHDFGBinding::onInspectChanged() {
  RTVal canvasOperator = m_shStates->getInspectedSGCanvasOperator();
  setCanvasOperator(canvasOperator);
}

void SHDFGBinding::connectBindingNotifier() {
  QSharedPointer<DFG::DFGBindingNotifier> notifier = m_controller->getBindingNotifier();
  QObject::connect(notifier.data(), SIGNAL(argInserted(unsigned, FTL::CStrRef, FTL::CStrRef)), this, SLOT(onArgInserted( unsigned, FTL::CStrRef, FTL::CStrRef)));
  QObject::connect(notifier.data(), SIGNAL(argRemoved(unsigned, FTL::CStrRef)), this, SLOT(onArgRemoved( unsigned, FTL::CStrRef)));
  QObject::connect(notifier.data(), SIGNAL(argTypeChanged(unsigned, FTL::CStrRef, FTL::CStrRef)), this, SLOT(onArgTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef)));
}

void SHDFGBinding::onDriveNodeInputPorts(RTVal event) {
  try 
  {
    QString toolPath(event.maybeGetMember("dfgToolPath").getStringCString());    

    if(toolPath != "") 
    {
      DFGBinding binding = m_controller->getBinding();
      DFGExec exec = binding.getExec();

      if(exec.hasVar(toolPath.toUtf8().constData()))
      {
        QStringList list = toolPath.split(".");
        QString contenerPath = list[list.size() - 2];

        QString subExecParentPath;
        for(int i=0; i<list.size()-2; ++i) 
          subExecParentPath += list[i] + ".";
        subExecParentPath = subExecParentPath.left(subExecParentPath.lastIndexOf("."));
        if(subExecParentPath == "")
          subExecParentPath = ".";

        QString subExecPath = toolPath.left(toolPath.lastIndexOf("."));
        DFGExec subExec = exec.getSubExec(subExecPath.toUtf8().constData());
        DFGExec subExecParent = exec.getSubExec(subExecParentPath.toUtf8().constData());
 
        RTVal toolVal = exec.getVarValue(toolPath.toUtf8().constData());
        RTVal target = toolVal.callMethod("LegacyDFGToolTarget", "getTarget", 0, 0);
        RTVal toolData = target.callMethod("LegacyDFGToolData", "getToolData", 0, 0);

        bool bakeValue = toolData.callMethod("Boolean", "bakeValue", 0, 0).getBoolean();  
        int portCount = toolData.callMethod("UInt32", "getPortCount", 0, 0).getUInt32();
        
        for(int i=0; i<portCount; ++i)
        {
          RTVal index = RTVal::ConstructUInt32(m_controller->getClient(), i);
          QString portName(toolData.callMethod("String", "getPortAtIndex", 1, &index).getStringCString()); 
          QString portPath = contenerPath + "." + portName;

          if(subExec.haveExecPort(portName.toUtf8().constData()))
          {
            if(subExec.getExecPortType(portName.toUtf8().constData()) == DFGPortType_In)
            {
              RTVal args[2] = { index, RTVal::ConstructBoolean(m_controller->getClient(), false) };
              RTVal rtVal = toolData.callMethod("RTVal", "getRTValAtPortIndex", 2, args);
              QString valType(rtVal.callMethod("String", "type", 0, 0).getStringCString());

              unsigned int portIndexInExec = subExec.getExecPortIndex(portName.toUtf8().constData());
              if(subExec.isExecPortResolvedType(portIndexInExec, valType.toUtf8().constData()))   
              {
                RTVal val = RTVal::Construct(m_controller->getClient(), valType.toUtf8().constData(), 1, &rtVal);

                if(!bakeValue)
                  subExecParent.setPortDefaultValue(portPath.toUtf8().constData(), val, false);
                else
                {
                  args[1] = RTVal::ConstructBoolean(m_controller->getClient(), true);
                  RTVal prevRTVal = toolData.callMethod("RTVal", "getRTValAtPortIndex", 2, args);
                  RTVal prevVal = RTVal::Construct(m_controller->getClient(), valType.toUtf8().constData(), 1, &prevRTVal);

                  subExecParent.setPortDefaultValue(portPath.toUtf8().constData(), prevVal, false);
                  m_controller->getCmdHandler()->dfgDoSetPortDefaultValue(binding, subExecParentPath, subExecParent, portPath, val);
                }
              }

              else
              {
                QString portType(subExec.getExecPortResolvedType(portName.toUtf8().constData()));
                printf("SHGLRenderer::driveNodeInputPorts: Warning : "
                  "The port: %s 's type %s doesn't match with the target manipulated type %s\n", 
                  portName.toUtf8().constData(), 
                  portType.toUtf8().constData(),
                  valType.toUtf8().constData());
              }
            }

            else
            {
              printf("SHGLRenderer::driveNodeInputPorts: Warning: "
                "The driven port is not an InputPort\n");
            }
          }

          else
          {
            printf("SHGLRenderer::driveNodeInputPorts: Warning : "
              "The port: %s doesn't exist\n", portName.toUtf8().constData());        
          }
        }
      }
      else
      {
        printf("SHGLRenderer::driveNodeInputPorts: Warning : "
          "The tool: %s is not a variable\n", toolPath.toUtf8().constData());    
      }
    }
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::driveNodeInputPorts: exception: %s\n", e.getDesc_cstr());
  }
}
