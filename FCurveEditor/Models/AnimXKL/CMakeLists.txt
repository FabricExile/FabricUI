add_library( RTValAnimXFCurveModel
  ${CMAKE_CURRENT_LIST_DIR}/RTValAnimXFCurveModel.cpp
)
QtMocProcess( "${CMAKE_CURRENT_LIST_DIR}/RTValAnimXFCurveModel.h" RTValAnimXFCurveModel )
add_definitions(-DFEC_SHARED)
target_include_directories( RTValAnimXFCurveModel PUBLIC
  ${SGDir}/Core/Clients/CAPI
)

target_link_libraries( RTValAnimXFCurveModel PUBLIC
  FCurveEditor
  ${FabricStageDir}/lib/FabricCore-2.7.lib
)
