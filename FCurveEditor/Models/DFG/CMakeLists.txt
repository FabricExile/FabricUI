set( CommandDir  ${SGDir}/Native/FabricUI/Commands )

file( GLOB CommandHeaders "${CommandDir}/*.h" )
file( GLOB CommandCpps "${CommandDir}/*.cpp" )

add_library( FabricUI
  ${SGDir}/Native/FabricUI/Application/FabricException.cpp
  ${SGDir}/Native/FabricUI/Application/FabricApplicationStates.cpp
  ${SGDir}/Native/FabricUI/Util/Factory.cpp
  ${SGDir}/Native/FabricUI/Util/RTValUtil.cpp
  ${SGDir}/Native/FabricUI/DFG/Commands/DFGPathValueResolver.cpp
  ${CommandCpps}
)
set( Headers
  ${CommandHeaders}
  ${SGDir}/Native/FabricUI/Application/FabricException.h
  ${SGDir}/Native/FabricUI/Application/FabricApplicationStates.h
  ${SGDir}/Native/FabricUI/Util/Factory.h
  ${SGDir}/Native/FabricUI/Util/RTValUtil.h
  ${SGDir}/Native/FabricUI/DFG/Commands/DFGPathValueResolver.h
)
QtMocProcess( "${Headers}" FabricUI )

target_include_directories( FabricUI PUBLIC
  ${SGDir}/Native/FabricServices
)
target_link_libraries( FabricUI PUBLIC
  RTValAnimXFCurveModel
)

add_library( DFGAnimXFCurveModel
  ${CMAKE_CURRENT_LIST_DIR}/DFGAnimXFCurveModel.cpp
)
QtMocProcess( "${CMAKE_CURRENT_LIST_DIR}/DFGAnimXFCurveModel.h" DFGAnimXFCurveModel )

target_link_libraries( DFGAnimXFCurveModel PUBLIC
  RTValAnimXFCurveModel
  FabricUI
)
