//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_TOOL_MANAGER_
#define __UI_TOOL_MANAGER_

#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Tools {

class ToolManager
{
	public:
		/// Checks if the RTVal resolved at `itemPath` can be edited with a ToolManager.
		/// \param itemPath The path to resolve
		static bool canCreateTool(
			QString const& itemPath
			);

		/// Checks if the input RTVal can be edited with a ToolManager.
		/// \param value The RTVal to edit (can be a PathValue)
		static bool canCreateTool(
			FabricCore::RTVal value
			);

		/// Creates and returns a ToolManager that resolves the path `itemPath`.
		/// \param itemPath The path to resolves.
		static FabricCore::RTVal createTool(
			QString const& itemPath
			);

		/// Creates and returns a ToolManager that resolves the PathValue `pathValue`.
		/// \param itemPath The pathValue to resolves.
		static FabricCore::RTVal createTool(
			FabricCore::RTVal pathValue
			);

		/// Gets a ToolManager that resolves the path `itemPath`.
		/// \param itemPath The path to resolves.
		static FabricCore::RTVal getTool(
			QString const& itemPath
			);
 		
 		/// Gets a ToolManager that resolves the PathValue `pathValue`.
		/// \param itemPath The pathValue to resolves.
		static FabricCore::RTVal getTool(
			FabricCore::RTVal pathValue
			);

		/// Deletes a ToolManager that resolves the path `itemPath`.
		/// \param itemPath The path to resolves.
		static void deleteTool(
			QString const& itemPath
			);

		/// Deletes a ToolManager that resolves the PathValue `pathValue`.
		/// \param itemPath The pathValue to resolves.
		static void deleteTool(
			FabricCore::RTVal pathValue
			);

		/// Sets a new path to a ToolManager.
		/// The resolved values must be of the same type.
		/// \param oldToolPath The old path (to retrieve the tool).
		/// \param newToolPath The new path to resolves.
		static void setToolPath(
			QString const& oldToolPath,
			QString const& newToolPath
			);

		/// Sets a new path to a ToolManager.
		/// The resolved values must be of the same type.
		/// \param oldPathValue The old pathValue (to retrieve the tool).
		/// \param newPathValue The new pathValue to resolves.
		static void setToolPath(
			FabricCore::RTVal oldPathValue,
			FabricCore::RTVal newPathValue
			);

		/// Sets the value of a ToolManager from the resolved RTVal target.
		/// \param itemPath The path to resolves.
		static void setToolValue(
			QString const& itemPath
			);

		/// Sets the value of a ToolManager from the resolved RTVal target.
		/// \param itemPath The pathValue to resolves.
		static void setToolValue(
			FabricCore::RTVal pathValue
			);
};

} // namespace Tools
} // namespace FabricUI

#endif // __UI_TOOL_MANAGER__
