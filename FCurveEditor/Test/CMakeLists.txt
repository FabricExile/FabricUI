set( qssIn ${SGDir}/Native/FabricUI/FabricUI.qss )
set( qssOut ${CMAKE_BINARY_DIR}/FabricUI.qss )
add_custom_target(
  qss
  COMMAND ${CMAKE_COMMAND} -E copy ${qssIn} ${qssOut}
)

function( AddTest TestFile )
  set( Test "${CMAKE_CURRENT_LIST_DIR}/${TestFile}" )
  get_filename_component( TestName "${Test}" NAME_WE )
  add_executable( ${TestName} ${Test} )
  target_link_libraries( ${TestName} PRIVATE FCurveEditor )
  add_dependencies( ${TestName} qss )
endfunction()

AddTest( 01_RuledGraphicsViewTest.cpp )
if( Models_AnimXCpp )
  AddTest( 02_FCurveEditorTest.cpp )
  target_link_libraries( 02_FCurveEditorTest PRIVATE AnimXFCurveModel )
endif()
if( Models_AnimXKL )
  AddTest( 03_RTValFCurveEditorTest.cpp )
  target_link_libraries( 03_RTValFCurveEditorTest PRIVATE RTValAnimXFCurveModel )
endif()
if( Models_AnimXKLUndoRedo )
  AddTest( 04_RTValFCurveUndoRedoTest.cpp )
  target_link_libraries( 04_RTValFCurveUndoRedoTest PRIVATE RTValAnimXFCurveDFGController )
endif()

if( MSVC_VERSION )
  message( STATUS
    "You might need to add the following definitions to "
    "Visual Studio's debug environment :\n"
    "PATH=${QtDir}/bin;${FabricStageDir}/bin;%PATH%\n"
    "FABRIC_EXTS_PATH=${FabricStageDir}/Exts\n"
    "FABRIC_DIR=${FabricStageDir}"
  )
endif()
