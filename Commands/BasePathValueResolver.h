//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_PATH_VALUE_RESOLVER__
#define __UI_BASE_PATH_VALUE_RESOLVER__

#include <QObject>
#include <QString>
#include <FabricCore.h>
 
namespace FabricUI {
namespace Commands {

class BasePathValueResolver : public QObject
{
  /**
    PathValueResolver are used to resolve the value of the KL PathValue from
    its path. BasePathValueResolver is the base implementation of any resolver.

    When desctructed, the resolver automatically unregisters it-self from the 
    resolver registry.
  */  
  Q_OBJECT
  
  public:
    BasePathValueResolver();

    virtual ~BasePathValueResolver();

    /// Called when the resolver is created.
    /// The userData argument is used to pass optional custom data.
    /// The data is referenced by the registry, and given to the
    /// resolver with this callback.    
    virtual void registrationCallback(
      QString const&name,
      void *userData
      );
 
    /// Checks the resolver knows the `PathValue` path.
    /// Must be overriden.
    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );

    /// Gets the `PathValue` value type.
    /// Must be overriden.
    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    /// Gets the `PathValue` value.
    /// Must be overriden.
    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    /// Sets the `PathValue` value.
    /// Must be overriden.=
    virtual void setValue(
      FabricCore::RTVal pathValue
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_PATH_VALUE_RESOLVER__
