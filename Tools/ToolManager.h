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
		/// Checks if the RTVal resolved at `itemPath` can be edited with a PathValueTool.
		/// \param itemPath The path to resolve
		static bool canCreatePathValueTool(
			QString const& itemPath
			);

		/// Checks if the input RTVal can be edited with a PathValueTool.
		/// \param value The RTVal to edit (can be a PathValue)
		static bool canCreatePathValueTool(
			FabricCore::RTVal value
			);

		/// Creates and returns a PathValueTool that resolves the path `itemPath`.
		/// \param itemPath The path to resolves.
		static FabricCore::RTVal createPathValueTool(
			QString const& itemPath
			);

		/// Creates and returns a PathValueTool that resolves the PathValue `pathValue`.
		/// \param itemPath The pathValue to resolves.
		static FabricCore::RTVal createPathValueTool(
			FabricCore::RTVal pathValue
			);

		/// Gets a PathValueTool that resolves the path `itemPath`.
		/// \param itemPath The path to resolves.
		static FabricCore::RTVal getPathValueTool(
			QString const& itemPath
			);
 		
 		/// Gets a PathValueTool that resolves the PathValue `pathValue`.
		/// \param itemPath The pathValue to resolves.
		static FabricCore::RTVal getPathValueTool(
			FabricCore::RTVal pathValue
			);

		/// Deletes all PathValueTool.
		static void deleteAllPathValueTools();

		/// Deletes a PathValueTool that resolves the path `itemPath`.
		/// \param itemPath The path to resolves.
		static void deletePathValueTool(
			QString const& itemPath
			);

		/// Deletes a PathValueTool that resolves the PathValue `pathValue`.
		/// \param itemPath The pathValue to resolves.
		static void deletePathValueTool(
			FabricCore::RTVal pathValue
			);

		/// Sets a new path to a PathValueTool.
		/// The resolved values must be of the same type.
		/// \param oldToolPath The old path (to retrieve the tool).
		/// \param newToolPath The new path to resolves.
		static void renamePathValueToolPath(
			QString const& oldToolPath,
			QString const& newToolPath
			);

		/// Sets a new path to a PathValueTool.
		/// The resolved values must be of the same type.
		/// \param oldPathValue The old pathValue (to retrieve the tool).
		/// \param newPathValue The new pathValue to resolves.
		static void renamePathValueToolPath(
			FabricCore::RTVal oldPathValue,
			FabricCore::RTVal newPathValue
			);

		/// Sets the value of a PathValueTool from the resolved RTVal target.
		/// \param itemPath The path to resolves.
		static void setPathValueToolValue(
			QString const& itemPath
			);

		/// Sets the value of a PathValueTool from the resolved RTVal target.
		/// \param itemPath The pathValue to resolves.
		static void setPathValueToolValue(
			FabricCore::RTVal pathValue
			);
};

} // namespace Tools
} // namespace FabricUI

#endif // __UI_TOOL_MANAGER__
