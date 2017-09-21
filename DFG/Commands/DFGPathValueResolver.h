//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_PATH_VALUE_RESOLVER__
#define __UI_DFG_PATH_VALUE_RESOLVER__

#include <FabricUI/Commands/BasePathValueResolver.h>

namespace FabricUI {
namespace DFG {

class DFGPathValueResolver : public Commands::BasePathValueResolver
{
  /**
    DFGPathValueResolver specializes BasePathValueResolver for DFG graphs. 
    It can resolves DFG ports, args and vars from their absolute paths in the DFG graph.
  */  
  
  Q_OBJECT
  
  public:
    DFGPathValueResolver();
 
    virtual ~DFGPathValueResolver();
   
    /// Implementation of BasePathValueResolver.
    virtual void registrationCallback(
      QString const&name,
      void *userData
      );

    /// Implementation of BasePathValueResolver.
    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual void setValue(
      FabricCore::RTVal pathValue
      );

    struct DFGPortPaths {
      public:
        QString portName; 
        QString blockName;
        QString nodeName;
        QString execPath;
        QString id;
        int arrayIndex; // If array element

        DFGPortPaths() {
          arrayIndex = -1;
        }

        bool isArrayElement() {
          return arrayIndex > -1;
        }

        bool isExecBlockPort();

        bool isExecArg();

        QString getRelativePortPath();

        QString getAbsolutePortPath(
          bool addBindingID = true
          );

        QString getFullItemPath(
          bool addBindingID = true
          );

        QString getAbsoluteNodePath(
          bool addBindingID = true
          );
    };

    /// Type of DFG item.
    enum DFGType { DFGUnknow, DFGPort, DFGArg, DFGVar };
      
    /// Gets the DFG item type (DFGUnknow, DFGPort, DFGVar)
    /// and the DFGPortPaths if the item is a port.
    FabricCore::DFGExec getDFGPortPathsAndType(
      FabricCore::RTVal pathValue,
      DFGPortPaths &dfgPortPaths,
      DFGType &dfgType
      );  

    FabricCore::DFGBinding getDFGBinding() const;

  public slots:
    /// Update the binding.
    virtual void onBindingChanged(
      FabricCore::DFGBinding const &binding
      );

  private:
    /// Removes the bindingID or the solverID from the path if set.
    QString getPathWithoutBindingOrSolverID(
      FabricCore::RTVal pathValue,
      int &arrayIndex,
      bool removeArrayElement = true
      );

    /// Gets the executable and DFGPortPaths
    /// from the pathValue.
    FabricCore::DFGExec getDFGPortPaths(
      FabricCore::RTVal pathValue, 
      DFGPortPaths &dfgPortPaths
      );

    /// Casts the path the a humain readable format.
    /// Replaces the bindingID by the solverID if it exists.
    /// If not (only one binding), removes the bindingID from the path.
    void castPathToHRFormat(
      FabricCore::RTVal pathValue
      );

    /// Solver ID
    QString m_id;
    FabricCore::DFGBinding m_binding;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_PATH_VALUE_RESOLVER__
