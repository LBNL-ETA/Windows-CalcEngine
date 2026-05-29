cmake_minimum_required(VERSION 3.5)

set(target_name FenestrationCommon)

# first include header files of this project itself

# make sure to connect to header files of common variables
# include_directories( src ${CMAKE_SOURCE_DIR}/Common/src )

# collect all source files and add them to the SOURCES list
file(GLOB SOURCES_CPP "src/*.cpp")
file(GLOB SOURCES_HPP "src/*.hpp")

LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})

add_library(${target_name} STATIC ${SOURCES})

# SquareMatrix.cpp includes <mimalloc.h> on Windows and calls mi_malloc/mi_free
# for the matrix buffers only (no global new/delete override, no mimalloc in any
# header). The dependency is therefore PRIVATE: consumers link mimalloc-static
# transitively for the symbols but never see its include directories. The
# mimalloc-static target is declared in the top-level CMakeLists.txt under
# `if(WIN32)`, so it only exists on Windows builds.
if(WIN32 AND TARGET mimalloc-static)
    target_link_libraries(${target_name} PRIVATE mimalloc-static)
endif()

# Install will be used by master projects to get information on destination of library files
install(TARGETS ${target_name}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

if (BUILD_WCE_TESTING)
    include_directories(../helper)
    include_directories(include)

    # Collect test headers and sources from helper and unit tests
    file(GLOB test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "../helper/*.hpp"
        "../helper/*.cpp"
        "tst/units/*.cpp"
    )

    CREATE_TEST_TARGETS_WCE(${target_name} "${test_src}" "")
endif ()

warning_level_update_wce()