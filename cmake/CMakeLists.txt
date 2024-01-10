set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

############
##	GTest	##
############
if( DOWNLOAD_GTEST STREQUAL ON ) # Do not need gtest since it will be processed by E+
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/gtest.CMakeLists.txt.in
		${CMAKE_BINARY_DIR}/gtest-download/CMakeLists.txt)
	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gtest-download )
	if(result)
		message(FATAL_ERROR "CMake step for gtest failed: ${result}")
	endif()
	execute_process(COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gtest-download )
	if(result)
		message(FATAL_ERROR "Build step for gtest failed: ${result}")
	endif()

	set(gtest_NAMESPACE "gtest" CACHE INTERNAL "Namespace for gtest")

		# Prevent GoogleTest from overriding our compiler/linker options
	# when building with Visual Studio
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

	# Add googletest directly to our build. This adds
	# the following targets: gtest, gtest_main, gmock
	# and gmock_main
	add_subdirectory(
		${CMAKE_BINARY_DIR}/gtest-src
		${CMAKE_BINARY_DIR}/gtest-build)

	# Now simply link your own targets against gtest, gmock,
	# etc. as appropriate
	include_directories(
		${CMAKE_BINARY_DIR}/gtest-src/googletest/include
		${CMAKE_BINARY_DIR}/gtest-src/googlemock/include)

	message(STATUS "Subproject: Gflag...DONE")
endif ()

##############
##	GFLAGS	##
##############
message(STATUS "Subproject: GFlags...")
# Download and unpack gflags at configure time
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/gflags.CMakeLists.txt.in
	${CMAKE_BINARY_DIR}/gflags-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gflags-download)
if(result)
	message(FATAL_ERROR "CMake step for gflags failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gflags-download)
if(result)
	message(FATAL_ERROR "Build step for gflags failed: ${result}")
endif()

set(GFLAGS_NAMESPACE "gflags")
set(GFLAGS_INSTALL_STATIC_LIBS ON)
set(GFLAGS_INSTALL_HEADERS ON)
#set(GFLAGS_IS_SUBPROJECT TRUE)
add_subdirectory(
	${CMAKE_BINARY_DIR}/gflags-src
	${CMAKE_BINARY_DIR}/gflags-build)
include_directories( ${CMAKE_BINARY_DIR}/gflags-build/include )
message(STATUS "Subproject: Gflag...DONE")

############
##	GLOG	##
############
message(STATUS "Subproject: GLog...")
# Download and unpack glog at configure time
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/glog.CMakeLists.txt.in
	${CMAKE_BINARY_DIR}/glog-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/glog-download)
if(result)
	message(FATAL_ERROR "CMake step for glog failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/glog-download)
if(result)
	message(FATAL_ERROR "Build step for glog failed: ${result}")
endif()

set(gflags_NAMESPACE "gflags" CACHE INTERNAL "Namespace for gflags")
add_subdirectory(
	${CMAKE_BINARY_DIR}/glog-src
	${CMAKE_BINARY_DIR}/glog-build)
include_directories( ${CMAKE_BINARY_DIR}/glog-build/include )
message(STATUS "Subproject: Glog...DONE")

############
##	BLAS	##
############
message(STATUS "Subproject: BLAS...")
# Download and unpack BLAS at configure time
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/blas.CMakeLists.txt.in
	${CMAKE_BINARY_DIR}/blas-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/blas-download)
if(result)
	message(FATAL_ERROR "CMake step for blas failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/blas-download)
if(result)
	message(FATAL_ERROR "Build step for blas failed: ${result}")
endif()

set(blas_NAMESPACE "blas" CACHE INTERNAL "Namespace for BLAS")
add_subdirectory(
	${CMAKE_BINARY_DIR}/blas-src
	${CMAKE_BINARY_DIR}/blas-build)
include_directories(
	${CMAKE_BINARY_DIR}/blas-src/include
	${CMAKE_BINARY_DIR}/blas-build/include)
message(STATUS "Subproject: blas...DONE")

##############
##	LAPACK	##
##############
message(STATUS "Subproject: LAPACK...")
# Download and unpack LAPACK at configure time
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/lapack.CMakeLists.txt.in
	${CMAKE_BINARY_DIR}/lapack-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/lapack-download)
if(result)
	message(FATAL_ERROR "CMake step for lapack failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/lapack-download)
if(result)
	message(FATAL_ERROR "Build step for lapack failed: ${result}")
endif()

set(lapack_NAMESPACE "lapack" CACHE INTERNAL "Namespace for LAPACK")
add_subdirectory(
	${CMAKE_BINARY_DIR}/lapack-src
	${CMAKE_BINARY_DIR}/lapack-build)
include_directories(
	${CMAKE_BINARY_DIR}/lapack-src/LAPACKE/include
	${CMAKE_BINARY_DIR}/lapack-src/CBLAS/include
	${CMAKE_BINARY_DIR}/lapack-build/include)
message(STATUS "Subproject: lapack...DONE")

#############
##	Eigen	##
#############
message(STATUS "Subproject: Eigen...")
# Download and unpack Eigen at configure time
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/eigen.CMakeLists.txt.in
	${CMAKE_BINARY_DIR}/eigen-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/eigen-download)
if(result)
	message(FATAL_ERROR "CMake step for eigen failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/eigen-download)
if(result)
	message(FATAL_ERROR "Build step for eigen failed: ${result}")
endif()

set(eigen_NAMESPACE "eigen" CACHE INTERNAL "Namespace for Eigen")
add_subdirectory(
	${CMAKE_BINARY_DIR}/eigen-src
	${CMAKE_BINARY_DIR}/eigen-build)
include_directories( ${CMAKE_BINARY_DIR}/eigen-src )
message(STATUS "Subproject: eigen...DONE")

##################
##	SuiteSparse	##
##################
# message(STATUS "Subproject: SuiteSparse...")
# # Download and unpack SuiteSparse at configure time
# configure_file(
# 	${CMAKE_CURRENT_SOURCE_DIR}/suitesparse.CMakeLists.txt.in
# 	${CMAKE_BINARY_DIR}/suitesparse-download/CMakeLists.txt)
# execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
# 	RESULT_VARIABLE result
# 	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/suitesparse-download)
# if(result)
# 	message(FATAL_ERROR "CMake step for suitesparse failed: ${result}")
# endif()
# execute_process(COMMAND ${CMAKE_COMMAND} --build .
# 	RESULT_VARIABLE result
# 	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/suitesparse-download)
# if(result)
# 	message(FATAL_ERROR "Build step for suitesparse failed: ${result}")
# endif()
#
# set(suitesparse_NAMESPACE "suitesparse" CACHE INTERNAL "Namespace for SuiteSparse")
# add_subdirectory(
# 	${CMAKE_BINARY_DIR}/suitesparse-src
# 	${CMAKE_BINARY_DIR}/suitesparse-build)
# message(STATUS "Subproject: suitesparse...DONE")

####################
##	Ceres Solver	##
####################
message(STATUS "Subproject: Ceres Solver...")
# Download and unpack Ceres Solver at configure time
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/ceres.CMakeLists.txt.in
	${CMAKE_BINARY_DIR}/ceres-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ceres-download)
if(result)
	message(FATAL_ERROR "CMake step for ceres failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ceres-download)
if(result)
	message(FATAL_ERROR "Build step for ceres failed: ${result}")
endif()

set(ceres_NAMESPACE "ceres" CACHE INTERNAL "Namespace for Ceres Solver")
# Eigen's and Ceres' `uninstall` targets collide as both are added as
# subdirectories. [Eigen's `uninstall` target cannot be
# excluded](https://gitlab.com/libeigen/eigen/-/issues/1892). However, Ceres'
# one can with `PROVIDE_UNINSTALL_TARGET`.
set(PROVIDE_UNINSTALL_TARGET OFF)
add_subdirectory(
	${CMAKE_BINARY_DIR}/ceres-src
	${CMAKE_BINARY_DIR}/ceres-build)
include_directories(
	${CMAKE_BINARY_DIR}/ceres-src/include
	${CMAKE_BINARY_DIR}/ceres-build/include)
message(STATUS "Subproject: ceres...DONE")
