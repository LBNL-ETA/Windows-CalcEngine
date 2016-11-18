cmake_minimum_required( VERSION 2.8.7 )

set( target_name Windows-CalcEngine )

project( ${target_name} )

include( cmake/ProjectMacros.cmake )
include( cmake/internal_utils.cmake )
include( cmake/CompilerFlags.cmake )

# google test will not be created by this project if this is not main project. Main project is expected to handle google test.
get_directory_property(hasParent PARENT_DIRECTORY)

if(hasParent)
	set( BUILD_TESTING OFF )
	set( SINGLE_PROJECT ON )
else()
	option( BUILD_TESTING "Build testing targets" ON )
	option( SINGLE_PROJECT "Build windows library as single project" OFF )
endif()

if( BUILD_TESTING )
	# include(CMakeLists-GTest.txt)
	configure_file(CMakeLists.txt.in ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
	execute_process(COMMAND ${CMAKE_COMMAND} --build .
  		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
  		
  	# Prevent GoogleTest from overriding our compiler/linker options
	# when building with Visual Studio
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
 
	# Add googletest directly to our build. This adds
	# the following targets: gtest, gtest_main, gmock
	# and gmock_main
	add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
    	${CMAKE_BINARY_DIR}/googletest-build)
 
	# Now simply link your own targets against gtest, gmock,
	# etc. as appropriate
  		
    include_directories( ${CMAKE_BINARY_DIR}/googletest-src/include )
endif ()

# foreach( _variableName ${SOURCES})
#   message( STATUS "${_variableName}" )
# endforeach()

if( NOT ${SINGLE_PROJECT} )
  add_subdirectory( src )
else()
  file( GLOB SOURCES_CPP "src/Common/src/*.cpp" )
  file( GLOB SOURCES_HPP "src/Common/src/*.hpp" )
  LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  file( GLOB SOURCES_CPP "src/Gases/src/*.cpp" )
  file( GLOB SOURCES_HPP "src/Gases/src/*.hpp" )
  LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  file( GLOB SOURCES_CPP "src/Tarcog/src/*.cpp" )
  file( GLOB SOURCES_HPP "src/Tarcog/src/*.hpp" )
  LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  file( GLOB SOURCES_CPP "src/SpectralAveraging/src/*.cpp" )
  file( GLOB SOURCES_HPP "src/SpectralAveraging/src/*.hpp" )
  LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  file( GLOB SOURCES_CPP "src/Viewer/src/*.cpp" )
  file( GLOB SOURCES_HPP "src/Viewer/src/*.hpp" )
  LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  file( GLOB SOURCES_CPP "src/SingleLayerOptics/src/*.cpp" )
  file( GLOB SOURCES_HPP "src/SingleLayerOptics/src/*.hpp" )
  LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  file( GLOB SOURCES_CPP "src/MultiLayerOptics/src/*.cpp" )
  file( GLOB SOURCES_HPP "src/MultiLayerOptics/src/*.hpp" )
  LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  file( GLOB SOURCES_CPP "src/Chromogenics/src/*.cpp" )
  file( GLOB SOURCES_HPP "src/Chromogenics/src/*.hpp" )
  LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  add_library( ${target_name} STATIC ${SOURCES} )
  
  if( BUILD_TESTING )
  	include_directories( src/Common/src )
  	include_directories( src/Gases/src )
  	include_directories( src/Tarcog/src )  	
  	include_directories( src/SpectralAveraging/src )
  	include_directories( src/Viewer/src )
  	include_directories( src/SingleLayerOptics/src )
  	include_directories( src/MultiLayerOptics/src )
  	
  	file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Common/tst/*.cpp" )
    LIST( APPEND test_src ${all_test_src} )
    
    file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Gases/tst/*.cpp" )
    LIST( REMOVE_ITEM all_test_src "src/Gases/tst/main.cpp")
    LIST( APPEND test_src ${all_test_src} )
    
    file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Tarcog/tst/*.cpp" )
    LIST( REMOVE_ITEM all_test_src "src/Tarcog/tst/main.cpp")
    LIST( APPEND test_src ${all_test_src} )
    
    file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/SpectralAveraging/tst/*.cpp" )
    LIST( REMOVE_ITEM all_test_src "src/SpectralAveraging/tst/main.cpp")
    LIST( APPEND test_src ${all_test_src} )
    
    file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Viewer/tst/*.cpp" )
    LIST( REMOVE_ITEM all_test_src "src/Viewer/tst/main.cpp")
    LIST( APPEND test_src ${all_test_src} )
    
    file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/SingleLayerOptics/tst/*.cpp" )
    LIST( REMOVE_ITEM all_test_src "src/SingleLayerOptics/tst/main.cpp")
    LIST( APPEND test_src ${all_test_src} )
    
    file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/MultiLayerOptics/tst/*.cpp" )
    LIST( REMOVE_ITEM all_test_src "src/MultiLayerOptics/tst/main.cpp")
    LIST( APPEND test_src ${all_test_src} )
    
    CREATE_TEST_TARGETS( ${target_name} "${test_src}" "" )
  endif()
  
endif()

config_compiler_and_linker()