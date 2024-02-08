cmake_minimum_required(VERSION 3.5)

# Require out-of-source builds as suggested on
# https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Use find_package everywhere, no-op if it's a target
macro(find_package)
	if(NOT TARGET ${ARGV0} AND NOT TARGET ${ARGV0}::${ARGV0})
		_find_package(${ARGV})
	else()
		if(TARGET ${ARGV0}::${ARGV0})
			get_target_property(TGT_VER ${ARGV0}::${ARGV0} VERSION)
			set(TGT ${ARGV0}::${ARGV0})
		else()
			get_target_property(TGT_VER ${ARGV0} VERSION)
			set(TGT ${ARGV0})
		endif()
		message(STATUS "Found ${ARGV0}: CMake Target ${TGT} (found version \"${TGT_VER}\")")
		set(${ARGV0}_FOUND TRUE)
	endif()
endmacro()


project(Windows-CalcEngine VERSION 1.0.33 LANGUAGES CXX)

set(target_name ${PROJECT_NAME})

#set( target_name Windows-CalcEngine )

set (CMAKE_CXX_STANDARD 17)

add_definitions(-DMULTITHREADING)

include( cmake/WCEProjectMacros.cmake )
include( cmake/WCEInternalUtils.cmake )
include( cmake/WCECompilerFlags.cmake )

# google test will not be created by this project if this is not main project. Main project is expected to handle google test.
get_directory_property(hasParent PARENT_DIRECTORY)

if(hasParent)
  set( BUILD_WCE_TESTING OFF )
  set( DOWNLOAD_GTEST OFF )
  #if( BUILD_TESTING STREQUAL ON ) # EnergyPlus testing is ON
  #  set( BUILD_WCE_TESTING ON )
  #endif()
  set( SINGLE_PROJECT ON )
  
  # If external project requested part of Windows-CalcEngine, then do not compile it as single project but as those parts
  if( BUILD_WCE_COMMON OR BUILD_WCE_GASES OR BUILD_WCE_THERMAL OR BUILD_WCE_OPTICAL )
		#set( SINGLE_PROJECT OFF )
	else()
	  # Nothing has been set from outside project. Compile complete project in that case.
		set( BUILD_WCE_COMMON ON )
		set( BUILD_WCE_GASES ON )
		set( BUILD_WCE_VIEWER ON )
		set( BUILD_WCE_THERMAL ON )
		set( BUILD_WCE_OPTICAL ON )
	endif()
else()
	option( BUILD_WCE_TESTING "Build testing targets" ON )
	option( SINGLE_PROJECT "Build windows library as single project" OFF )
	option( BUILD_WCE_COMMON "Build Common Library" ON )
	option( BUILD_WCE_GASES "Build Gas Calculations Library" ON )
	option( BUILD_WCE_VIEWER "Build Viewer Calculation Library" ON )
	option( BUILD_WCE_THERMAL "Build Thermal Calculations Library" ON )
	option( BUILD_WCE_OPTICAL "Build Optical Calculations Library" ON )
	
	set( DOWNLOAD_GTEST ON )
endif()

if( ${BUILD_WCE_GASES} )
	set( BUILD_WCE_COMMON ON )
endif()

if( ${BUILD_WCE_THERMAL} )
	set( BUILD_WCE_COMMON ON )
	set( BUILD_WCE_GASES ON )
	set( BUILD_WCE_VIEWER ON )
endif()

if( ${BUILD_WCE_OPTICAL} )
	set( BUILD_WCE_COMMON ON )
	set( BUILD_WCE_VIEWER ON )
endif()

# ?
include(CTest)
include(GNUInstallDirs)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Build Dependencies
add_subdirectory(cmake)

# Install
install(EXPORT CMakeExternalTargets
	NAMESPACE CMakeExternal::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CMakeExternal
	COMPONENT Devel)
include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/CMakeExternalConfig.cmake.in
	"${PROJECT_BINARY_DIR}/CMakeExternalConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CMakeExternal"
	NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
	"${PROJECT_BINARY_DIR}/CMakeExternalConfigVersion.cmake"
	COMPATIBILITY SameMajorVersion)
install(
	FILES
	"${PROJECT_BINARY_DIR}/CMakeExternalConfig.cmake"
	"${PROJECT_BINARY_DIR}/CMakeExternalConfigVersion.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CMakeExternal"
	COMPONENT Devel)

# foreach( _variableName ${SOURCES})
#   message( STATUS "${_variableName}" )
# endforeach()

if( NOT ${SINGLE_PROJECT} )
  add_subdirectory( src )
else()
	if( ${BUILD_WCE_COMMON} )
  	file( GLOB SOURCES_CPP "src/Common/src/*.cpp" )
  	file( GLOB SOURCES_HPP "src/Common/src/*.hpp" )
  	LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  endif()
  
  if( ${BUILD_WCE_GASES} )
  	file( GLOB SOURCES_CPP "src/Gases/src/*.cpp" )
  	file( GLOB SOURCES_HPP "src/Gases/src/*.hpp" )
  	LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  endif()
  
  if( ${BUILD_WCE_VIEWER} )
  	file( GLOB SOURCES_CPP "src/Viewer/src/*.cpp" )
  	file( GLOB SOURCES_HPP "src/Viewer/src/*.hpp" )
  	LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  endif()
  
  if( ${BUILD_WCE_THERMAL} )
  	file( GLOB SOURCES_CPP "src/Tarcog/src/*.cpp" )
  	file( GLOB SOURCES_HPP "src/Tarcog/src/*.hpp" )
  	LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  endif()
  
  if( ${BUILD_WCE_OPTICAL} )
  	file( GLOB SOURCES_CPP "src/SpectralAveraging/src/*.cpp" )
  	file( GLOB SOURCES_HPP "src/SpectralAveraging/src/*.hpp" )
  	LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  	file( GLOB SOURCES_CPP "src/SingleLayerOptics/src/*.cpp" )
  	file( GLOB SOURCES_HPP "src/SingleLayerOptics/src/*.hpp" )
  	LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  
  	file( GLOB SOURCES_CPP "src/MultiLayerOptics/src/*.cpp" )
  	file( GLOB SOURCES_HPP "src/MultiLayerOptics/src/*.hpp" )
  	LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})
  endif()
  
  add_library( ${target_name} STATIC ${SOURCES} )
  
  if( ${BUILD_WCE_COMMON} )
	target_include_directories(${target_name} PUBLIC "src/Common/include")
  endif()

  if( ${BUILD_WCE_GASES} )
	target_include_directories(${target_name} PUBLIC "src/Gases/include")
  endif()

  if( ${BUILD_WCE_VIEWER} )
	target_include_directories(${target_name} PUBLIC "src/Viewer/include")
  endif()

  if( ${BUILD_WCE_THERMAL} )
	target_include_directories(${target_name} PUBLIC "src/Tarcog/include")
  endif()

  if( ${BUILD_WCE_OPTICAL} )
	target_include_directories(${target_name} PUBLIC "src/SpectralAveraging/include")
	target_include_directories(${target_name} PUBLIC "src/SingleLayerOptics/include")
	target_include_directories(${target_name} PUBLIC "src/MultiLayerOptics/include")
  endif()
  
  if( BUILD_WCE_TESTING )
  
	if( ${BUILD_WCE_COMMON} )
  		include_directories( src/Common/include )
  		file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Common/tst/units/*.cpp" )
    	LIST( APPEND test_src ${all_test_src} )
  	endif()
  	
  	if( ${BUILD_WCE_VIEWER} )
  		include_directories( src/Viewer/include )
  		file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Viewer/tst/units/*.cpp" )
    	LIST( REMOVE_ITEM all_test_src "src/Viewer/tst/units/main.cpp")
    	LIST( APPEND test_src ${all_test_src} )
  	endif()
    
    if( ${BUILD_WCE_GASES} )
    	include_directories( src/Gases/include )
    	file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Gases/tst/units/*.cpp" )
    	LIST( REMOVE_ITEM all_test_src "src/Gases/tst/units/main.cpp")
    	LIST( APPEND test_src ${all_test_src} )
    endif()
    
    if( ${BUILD_WCE_THERMAL} )
  		include_directories( src/Tarcog/include )
    	file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Tarcog/tst/units/*.cpp" )
    	LIST( REMOVE_ITEM all_test_src "src/Tarcog/tst/units/main.cpp")
    	LIST( APPEND test_src ${all_test_src} )
    endif()
    
    if( ${BUILD_WCE_OPTICAL} )
      include_directories( src/SpectralAveraging/include )  	
      file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/SpectralAveraging/tst/*.cpp" )
      LIST( REMOVE_ITEM all_test_src "src/SpectralAveraging/tst/main.cpp")
      LIST( APPEND test_src ${all_test_src} )
      
      include_directories( src/SingleLayerOptics/include )
      file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/SingleLayerOptics/tst/*.cpp" )
      LIST( REMOVE_ITEM all_test_src "src/SingleLayerOptics/tst/main.cpp")
      LIST( APPEND test_src ${all_test_src} )
      
      include_directories( src/MultiLayerOptics/include )
      file( GLOB all_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/MultiLayerOptics/tst/*.cpp" )
      LIST( REMOVE_ITEM all_test_src "src/MultiLayerOptics/tst/main.cpp")
      LIST( APPEND test_src ${all_test_src} )
    endif()
    
    CREATE_TEST_TARGETS_WCE( ${target_name} "${test_src}" "" )
  endif()
  
endif()

config_compiler_and_linker_wce()
