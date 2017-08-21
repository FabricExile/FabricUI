//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_FACTORY__
#define __UI_FACTORY__
 
#include <QMap>
#include <QObject>
#include <QString>

namespace FabricUI {
namespace Util {

/**
  Factory is a framework to associate a C++ class with a unique  
  name so object of this class can be constructed from it. 

  It has two components:
  - factories that wrapp the C++ object construction.
  - registries that registered a factory under a given unqiue-name.
*/

class Factory
{
  /**
    Factory is a base class for any factories. The method 'create' creates the C++ 
    object and must be overrideen. Factory also has a generic 'userData' argument. 
    It's used to store optional data that can be passed to the object afterward.
  */

  public:
    virtual ~Factory() {}

    /// Creates the object, returns it as a void pointer.
    /// To override.
    virtual void* create(
      void* args = 0
      ) = 0;

    /// Gets the type of the object that
    /// the factory creates.
    virtual QString getType() = 0;

    /// Gets the user data (may be null).
    virtual void* getUserData() = 0;
};

class BaseFactoryRegistry : public QObject 
{
  /**
    BaseFactoryRegistry is a base class to registers/unregisters factories.
    When a factory is created, it must be registered in a registery.
    New registry should inherites this class.  
  */

  Q_OBJECT

  public:
    BaseFactoryRegistry();

    virtual ~BaseFactoryRegistry();
    
    /// Gets a factory.
    /// Returns null if no factory is registered under 'name'.
    /// \param name Name of the factory.
    Factory* getFactory(
      QString const&name
      );

    /// Checks is the registry has a factory.
    /// \param name Name of the factory.
    bool hasFactory(
      QString const&name
      );

    /// Registers a factory once.
    /// \param name Name of the factory.
    /// \param factory Pointer to the factory.
    virtual void registerFactory(
      QString const&name, 
      Factory *factory
      );

    /// Unregisters a factory.
    /// \param name Name of the factory.
    virtual void unregisterFactory(
      QString const&name 
      );

    /// Unregisters a factory.
    /// \param factory A pointer to the factory.
    void unregisterFactory(
      Factory *factory
      );

  signals:
    /// Emitted when a factory is registered.
    /// \param name Name of the factory.
    /// \param factory Pointer to the factory.
    void factoryRegistered(
      QString const&name, 
      Factory *factory
      );

    /// Emitted when a factory is unregistered.
    /// \param name Name of the factory.
    void factoryUnregistered(
      QString const&name
      );

  private:
    /// Dictionaries of registered factories. 
    QMap<QString, Factory*> m_factories;
};

// From https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
#ifdef __GNUG__

static const unsigned int FRONT = sizeof("FabricUI::Util::GetTypeHelper<") - 1u;
static const unsigned int BACK = sizeof(">::GetType") - 1u;
 
template <typename T>
struct GetTypeHelper
{
  static const char* GetType(void)
  {
    static const size_t size = sizeof(__PRETTY_FUNCTION__) - FRONT - BACK;
    static char typeName[size] = {};
    memcpy(typeName, __PRETTY_FUNCTION__ + FRONT, size - 1u);
    return typeName;
  }
};

#else

static const unsigned int FRONT = sizeof("FabricUI::Util::GetTypeHelper<") - 1u;
static const unsigned int BACK = sizeof(">::GetType") - 1u;

template <typename T>
struct GetTypeHelper
{
  static const char* GetType(void)
  {
    static const size_t size = sizeof(__FUNCTION__) - FRONT - BACK;
    static char type[size] = {};
    memcpy(type, __FUNCTION__ + FRONT, size - 1u);
    return type;
  }
};

#endif
  
template<typename T> 
class TemplateFactory : public Factory
{
  /**
    TemplateFactory creates a new object from it's type directly.
    It's the best approach found so far.   
  
    Usage: 
      // Defines a new class.
      class MyClass 
      {
        ...
      };

      // Get the registry.
      FactoryRegistry *registry = ....;
      
      // Create a factory to construct objects  
      // of type 'MyClass' and register it.
      Factory<MyClass>::Register(
        registry,
        "myClass");

      // Get the factory in the registry
      Factory *factory = registry->getFactory(
        "myClass");

      // Construct a new object if type 'MyClass'
      MyClass *myObject = (MyClass *)factory->create();
  */
  public:
    /// Constructor.
    /// \param userData The user data.
    TemplateFactory(
      void* userData) 
      : m_userData(userData)
    {
    }

    virtual ~TemplateFactory() 
    { 
    } 

    /// Creates the factory and registers it in a registry.
    /// \param registry BaseFactoryRegistry owning the factory.
    /// \param name Name of the factory, should be unique.
    /// \param userData The user data.
    static void Register(
      BaseFactoryRegistry *registry,
      QString const&name,
      void* userData = 0) 
    {
      registry->registerFactory(
        name, 
        new TemplateFactory(userData)
        );
    }

    /// Removes the factory from the registry.
    /// \param registry BaseFactoryRegistry owning the factory.
    /// \param name Name of the factory, should be unique.
    static void Unregister(
      BaseFactoryRegistry *registry,
      QString const&name ) {
      TemplateFactory* factory = (TemplateFactory*)registry->getFactory(
        name
      );
      if( factory ) {
        // Note: the following also deletes the factory
        registry->unregisterFactory(
          name
        );
      }
    }

    /// Implementation of Factory.
    virtual void* create(
      void* args = 0) 
    { 
      return new T();
    } 
    
    /// Implementation of Factory.
    virtual QString getType()
    {
      return FabricUI::Util::GetTypeHelper<T>::GetType();
    }

    /// Implementation of Factory.
    virtual void* getUserData() 
    { 
      return m_userData;
    } 

    private:
      void* m_userData;
};

} // namespace Util
} // namespace FabricUI

#endif // __UI_FACTORY__
