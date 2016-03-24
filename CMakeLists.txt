cmake_minimum_required( VERSION 2.8.9 )

project( WindowsCalculationEngine )

option( BUILD_TESTING "Build testing targets" ON )

include( cmake/ProjectMacros.cmake )
include( cmake/internal_utils.cmake )
include( cmake/CompilerFlags.cmake )

if( BUILD_TESTING )
  include_directories( third_party/gtest/include )
  add_subdirectory( third_party/gtest )
endif ()

add_subdirectory( src )