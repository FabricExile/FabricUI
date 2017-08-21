cmake_minimum_required(VERSION 2.6)
project( FCurveEditor )

set( FCurveEditorDir ${CMAKE_CURRENT_LIST_DIR} )
set( SGDir ${FCurveEditorDir}/../../.. )
set( FabricOSName ${CMAKE_SYSTEM_NAME} )
if( APPLE )
  set( FabricOSName "Darwin" )
endif()
set( ThirdPartyDir ${SGDir}/ThirdParty/PreBuilt/${FabricOSName}/x86_64 )
option( WithQtDebugLibs "Compile against the debug version of Qt's libs" OFF )

if( APPLE)
  set( ThirdPartyPrefix "stdlib-libc++/" )
endif()
if( WIN32 )
  set( ThirdPartyPrefix "VS2013/" )
endif()
set( QtDir ${ThirdPartyDir}/${ThirdPartyPrefix}Release/qt/4.8.7 CACHE PATH "" )
if( NOT EXISTS ${QtDir}/bin )
  message( SEND_ERROR "Please specify QtDir (didn't find QtDir/bin" )
endif()

function( QtMocProcess Headers Target )
  set( OutDir ${CMAKE_BINARY_DIR}/Moc/${Target} )
  set( OutFiles "" ) # to be used outside of the function
  foreach( Header ${Headers} )
    get_filename_component( FileName ${Header} NAME_WE )
    string( MD5 PathHash "${Header}" ) # HACK : hash the full path to avoid duplicates
    set( OutFile "${OutDir}/${FileName}_${PathHash}_moc.cpp" )
    if( NOT EXISTS ${OutFile} )
      file( WRITE ${OutFile} )
    endif()
    add_custom_command(
      OUTPUT ${OutFile}
      COMMAND ${QtDir}/bin/moc
        ${Header}
        -o${OutFile}
      DEPENDS ${Header}
    )
    set( OutFiles ${OutFiles} ${OutFile} )
  endforeach()
  target_sources( ${Target} PRIVATE ${Headers} ${OutFiles} )
  source_group( QtMocOutput FILES ${OutFiles} )
endfunction( QtMocProcess )

file( GLOB FCurveEditorHeaders "${FCurveEditorDir}/*.h" )
file( GLOB FCurveEditorCpps "${FCurveEditorDir}/*.cpp" )
add_library( FCurveEditor ${FCurveEditorCpps} )
QtMocProcess( "${FCurveEditorHeaders}" FCurveEditor )

target_include_directories( FCurveEditor PUBLIC
  ${SGDir}
  ${SGDir}/Native
  ${QtDir}/include
  ${QtDir}/include/QtCore
  ${QtDir}/include/QtGui
  ${QtDir}/include/ActiveQt
  ${QtDir}/include/QtWidgets
  ${QtDir}/include/QtOpenGL
)

if( WIN32 )

  set( UsingQt5Bool 0 )
  set( QtLibPrfx "" )
  set( QtLibSfx "4" )
  if( EXISTS "${QtDir}/lib/Qt5Gui.lib" ) # TODO : Unix
    set( UsingQt5Bool 1 )
    set( QtLibPrfx "5" )
    set( QtLibSfx "" )
  endif()

  set( libSfx "${QtLibSfx}" )
  if( WithQtDebugLibs )
    set( libSfx "d${libSfx}" )
  endif()

  target_link_libraries( FCurveEditor PUBLIC
    ${QtDir}/lib/Qt${QtLibPrfx}Gui${libSfx}.lib
    ${QtDir}/lib/Qt${QtLibPrfx}Core${libSfx}.lib
    ${QtDir}/lib/Qt${QtLibPrfx}OpenGL${libSfx}.lib
    )
  if( UsingQt5Bool )
    target_link_libraries( FCurveEditor PUBLIC
      ${QtDir}/lib/Qt${QtLibPrfx}Widgets${libSfx}.lib
    )
  endif()
elseif( APPLE )
  target_link_libraries( FCurveEditor PUBLIC
    ${QtDir}/lib/QtGui.framework/QtGui
    ${QtDir}/lib/QtCore.framework/QtCore
    ${QtDir}/lib/QtOpenGL.framework/QtOpenGL
  )  
else()
  target_link_libraries( FCurveEditor PUBLIC
    ${QtDir}/lib/libQtGui.so
    ${QtDir}/lib/libQtCore.so
    ${QtDir}/lib/libQtOpenGL.so
  )
endif()

set( FabricStageDir "${SGDir}/stage/${CMAKE_SYSTEM_NAME}/x86_64/Release" )

include( ${FCurveEditorDir}/Test/CMakeLists.txt )
option( Models_AnimXCpp "C++ AnimX Model" OFF )
if( Models_AnimXCpp )
  include( ${FCurveEditorDir}/Models/AnimXCpp/CMakeLists.txt )
endif()
option( Models_AnimXKL "KL AnimX Model" OFF )
if( Models_AnimXKL )
  include( ${FCurveEditorDir}/Models/AnimXKL/CMakeLists.txt )
endif()
option( Models_AnimXKLUndoRedo "KL AnimX Model with Undo-Redo" OFF )
if( Models_AnimXKLUndoRedo )
  include( ${FCurveEditorDir}/Models/DFG/CMakeLists.txt )
endif()
