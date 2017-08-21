set( AnimXDir
  "${ThirdPartyDir}/${ThirdPartyPrefix}Release/animx/1.0.0"
  CACHE PATH "Directory of Autodesk's AnimX curve library"
)
if( NOT EXISTS ${AnimXDir}/include )
  message(SEND_ERROR "Please specify AnimXDir (where /include/ is)" )
  set( AnimXLib "" )
endif()

set( AnimXLib ${AnimXDir}/lib/AnimX.lib
  CACHE FILEPATH ""
)

add_library( AnimXFCurveModel
  ${CMAKE_CURRENT_LIST_DIR}/AnimXFCurveModel.cpp
)
QtMocProcess( "${CMAKE_CURRENT_LIST_DIR}/AnimXFCurveModel.h" AnimXFCurveModel )
target_include_directories( AnimXFCurveModel PUBLIC
  ${AnimXDir}/include/
)
target_link_libraries( AnimXFCurveModel PUBLIC
  ${AnimXLib}
  FCurveEditor
)
