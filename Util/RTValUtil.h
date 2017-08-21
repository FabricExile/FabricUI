//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_UTIL__
#define __UI_RTVAL_UTIL__

#include <QList>
#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Util {

class RTValUtil 
{
  /**
    RTValUtil provides static methods to convert C++ KL RTVal to/from JSON. 
    A C++ KL RTVal is a C++ RTVal wrapping a KL RTVal.
  */

  public:
    /// Checks if the c++ RTVal wraps a KL RTVal.
    static bool isKLRTVal(
      FabricCore::RTVal klRTVal
      );

    /// Gets the KL type of the C++ RTVal.
    /// If the C++ RTVal wraps a KL RTVal, the returned type 
    /// is the type of the object wrapped by the KL RTVal.
    /// To know if a C++ wraps a KL RTVal, use `isKLRTVal`.
    static QString getType(
      FabricCore::RTVal rtVal
      );

    /// Converts a C++ KL RTVal to a C++ RTVal.
    static FabricCore::RTVal toRTVal(
      FabricCore::RTVal klRTVal
      );

    /// Converts a C++ RTVal to a C++ KL RTVal.
    static FabricCore::RTVal toKLRTVal(
      FabricCore::RTVal rtVal
      );

    /// Extracts in JSON the C++ RTVal.
    static QString toJSON(
      FabricCore::RTVal rtVal
      );

    /// Creates a C++ RTVal of KL type 'type' 
    /// and sets it's value from the json.
    static FabricCore::RTVal fromJSON(
      FabricCore::Client client,
      QString const&json,
      QString const&type
      );

    /// Creates a C++ RTVal of KL type 'type' 
    /// and sets it's value from the json.
    static FabricCore::RTVal fromJSON(
      FabricCore::Context ctxt,
      QString const&json,
      QString const&type
      ); 
};

} // namespace FabricUI
} // namespace Util

#endif // __UI_RTVAL_UTIL__
