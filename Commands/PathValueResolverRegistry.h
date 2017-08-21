//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_PATH_RESOLVER_REGISTRY__
#define __UI_PATH_RESOLVER_REGISTRY__

#include <QMap>
#include "BasePathValueResolver.h"
#include <FabricUI/Util/Factory.h>

namespace FabricUI {
namespace Commands {

class PathValueResolverRegistry : public Util::BaseFactoryRegistry
{  
  /**
    PathValueResolverRegistry registers C++ resolver-factories and creates resolver
    from them. When specialized in Python, the same registry is shared between C++- 
    Python so resolvers implemented in Python can be created from C++ and vice versa.
      
    The registry is singleton:
    - Get the singleton: PathValueResolverRegistry *registry = PathValueResolverRegistry::getRegistry();
  */  
  
  Q_OBJECT

  public:
    PathValueResolverRegistry();

    virtual ~PathValueResolverRegistry();

    /// Gets the resolver singleton.
    static PathValueResolverRegistry* getRegistry();
    
    /// Checks if the registry has
    /// a resolver named 'name'
    bool hasResolver(
      QString const&name
      );

    /// Gets the resolver name. Returns an empty 
    /// string if the resolver doesn't exist.
    QString getResolverName(
      BasePathValueResolver* resolver
      );

    /// Gets the resolver named 'name'. it must be
    /// registered, returns a null object otherwise.
    virtual BasePathValueResolver* getResolver(
      QString const&name
      );

    /// Gets the resolver that can 
    /// resolves `pathValue`.  
    virtual BasePathValueResolver* getResolver(
      FabricCore::RTVal pathValue
      );
          
    /// Checks if one of the registered   
    /// resolvers knows the `PathValue`.
    bool knownPath(
      FabricCore::RTVal pathValue
      );

    /// Gets the `PathValue` value type.
    QString getType(
      FabricCore::RTVal pathValue
      );

    /// Gets the `PathValue` value.
    void getValue(
      FabricCore::RTVal pathValue
      );

    /// Sets the `PathValue` value.
    void setValue(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BaseFactoryRegistry.
    virtual void unregisterFactory(
      QString const&name 
      );

    /// \internal
    /// Registers a resolver, done when
    /// the factory is it-self registered
    /// \param cmdName The name of the command
    /// \param type Object type
    virtual void registerResolver(
      BasePathValueResolver *resolver,
      QString const&type
      );

    /// \internal
    /// Clears the registered content
    virtual void clear();

  private:
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
    /// PathResolver singleton.
    static PathValueResolverRegistry *s_registry;
    /// PathResolver singleton.
    QMap<QString, BasePathValueResolver*> m_registeredResolvers;
};

template<typename T> 
class PathValueResolverFactory : public Util::TemplateFactory<T>
{
  /**
    PathValueResolverFactory is used to register PathValueResolvers in the PathValueResolverRegistry.
    The resolver is automatically created when registered.

    - Register a resolver: PathValueResolverFactory<cmdType>::Register(name, userData);
  */
  public:
    PathValueResolverFactory(
      void *userData) 
      : Util::TemplateFactory<T>(userData) 
    {
    }

    /// Registers the resolver <T> under the name "name".
    static void Register(
      QString name,
      void *userData=0) 
    {
      Util::TemplateFactory<T>::Register(
        PathValueResolverRegistry::getRegistry(),
        name);

      FabricUI::Util::Factory *factory = PathValueResolverRegistry::getRegistry()->getFactory(name);
      BasePathValueResolver* resolver = (BasePathValueResolver*)factory->create(); 
      resolver->registrationCallback(name, userData);
      PathValueResolverRegistry::getRegistry()->registerResolver(resolver, name);
    }

    /// Registers the resolver <T> under the name "name".
    static void Unregister(
      QString name,
      void *userData = 0 ) {
      // The following will also unregister the resolver
      Util::TemplateFactory<T>::Unregister(
        PathValueResolverRegistry::getRegistry(),
        name );
    }
};

} // namespace PathResolvers
} // namespace FabricUI

#endif // __UI_PATH_RESOLVER_REGISTRY__
