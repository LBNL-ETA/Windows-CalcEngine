cmake_minimum_required( VERSION 2.8.11 )

project( WindowsCalculationEngine )

option( BUILD_TESTING "Build testing targets" ON )

include( cmake/ProjectMacros.cmake )
include( cmake/internal_utils.cmake )
include( cmake/CompilerFlags.cmake )

if( BUILD_TESTING )
	# include(CMakeLists-GTest.txt)
	configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
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
  		
    # include_directories( third_party/gtest/include )
    include_directories( ${CMAKE_BINARY_DIR}/googletest-src/include )
    # add_subdirectory( third_party/gtest )
endif ()

add_subdirectory( src )