#!/bin/sh

Target=('fabricui_commands_CommandHelpers_wrapper' 'fabricui_commands_commandmanager_wrapper' 'fabricui_commands_rtvalcommandmanager_wrapper' 'fabricui_commands_klcommandmanager_wrapper' 'fabricui_commands_commandregistry_wrapper' 'fabricui_commands_klcommandregistry_wrapper' 'fabricui_commands_basescriptablecommand_wrapper' 	'fabricui_commands_basertvalscriptablecommand_wrapper')

for ((i=0; i<${#Target[*]}; i++));
do
 echo ${Target[i]}

 diff -u ./build/Windows/x86_64/Release/Native/FabricUI/pyside/python2.7/FabricUI.ori/${Target[i]}.cpp \
./build/Windows/x86_64/Release/Native/FabricUI/pyside/python2.7/FabricUI/${Target[i]}.cpp > \
./Native/FabricUI/shiboken/${Target[i]}.diff 2>&1

done

