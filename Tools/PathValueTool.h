//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_PATH_VALUE_TOOL_
#define __UI_PATH_VALUE_TOOL_

#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Tools {

class PathValueTool
{
	public:
		static bool canCreateTool(
			QString const& toolPath
			);

		static bool canCreateTool(
			FabricCore::RTVal pathValue
			);

		static FabricCore::RTVal createTool(
			QString const& toolPath
			);

		static FabricCore::RTVal createTool(
			FabricCore::RTVal pathValue
			);

		static FabricCore::RTVal getTool(
			QString const& toolPath
			);
 
		static FabricCore::RTVal getTool(
			FabricCore::RTVal pathValue
			);

		static void deleteTool(
			QString const& toolPath
			);

		static void deleteTool(
			FabricCore::RTVal pathValue
			);

		static void setToolPath(
			QString const& oldToolPath,
			QString const& newToolPath
			);

		static void setToolPath(
			FabricCore::RTVal oldPathValue,
			FabricCore::RTVal newPathValue
			);

		static void setToolValue(
			QString const& toolPath
			);

		static void setToolValue(
			FabricCore::RTVal pathValue
			);
};

} // namespace Tools
} // namespace FabricUI

#endif // __UI_PATH_VALUE_TOOL__
