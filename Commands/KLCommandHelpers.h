//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND_HELPERS__
#define __UI_KL_COMMAND_HELPERS__

#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Commands {

/**
	Helpers for KLCommand and KLScriptableCommands.
*/

class KLCommandHelpers
{
  public:
    /// Gets the KL command manager singleton.
    static FabricCore::RTVal getKLCommandManager();

    /// Gets the KL command registry singleton.
    static FabricCore::RTVal getKLCommandRegistry();

    /// Gets the name of a KL commands.
    /// Throws an exception if an error occurs.
    static QString getKLCommandName(
      FabricCore::RTVal klCmd
      );

    /// Checks if the KL command is undoable.
    /// Throws an exception if an error occurs.
    static bool canKLCommandUndo(
      FabricCore::RTVal klCmd
      );

    /// Checks if the KL command is undoable.
    /// Throws an exception if an error occurs.
    static bool canKLCommandLog(
      FabricCore::RTVal klCmd
      );

    /// Executes a KL commands.
    /// Throws an exception if an error occurs.
    static bool doKLCommand(
      FabricCore::RTVal klCmd
      );

    /// Undoes the current KL command.
    /// Throws an exception if an error occurs.
    static bool undoKLCommand();

    /// Redoes the current KL command.
    /// Throws an exception if an error occurs.
    static bool redoKLCommand();

    /// Gets a KL command's description.
    /// Throws an exception if an error occurs.
    static QString getKLCommandHelp(
      FabricCore::RTVal klCmd
      );

    /// Gets a KL command's history description.
    /// Throws an exception if an error occurs.
    static QString getKLCommandHistoryDesc(
      FabricCore::RTVal klCmd
      );

    /// Sets the interaction ID of a KL commands.
    /// Throws an exception if an error occurs.
    static void setKLCommandCanMergeID(
      FabricCore::RTVal klCmd,
      int canMergeID
      );

    /// Gets the interaction ID of a KL commands.
    /// Throws an exception if an error occurs.
    static int getKLCommandCanMergeID(
      FabricCore::RTVal klCmd
      );

    /// Merges a KL command with another.
    /// Throws an exception if an error occurs.
    static bool canMergeKLCommand(
      FabricCore::RTVal klCmd,
      FabricCore::RTVal prevKlCmd,
      bool &undoPrevAndMergeFirst
      );

    /// Merges a KL command with another.
    /// Throws an exception if an error occurs.
    static void mergeKLCommand(
      FabricCore::RTVal klCmd,
      FabricCore::RTVal prevKlCmd
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_HELPERS__
