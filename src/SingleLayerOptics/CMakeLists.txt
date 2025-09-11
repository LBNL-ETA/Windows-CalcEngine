cmake_minimum_required(VERSION 3.5)

set(target_name SingleLayerOptics)

# first include header files of this project itself
# include_directories( include )

# make sure to connect to header files of common variables
include_directories(../Common/include)
include_directories(../Viewer/include)
include_directories(../SpectralAveraging/include)

# collect all source files and add them to the SOURCES list
file(GLOB SOURCES_CPP "src/*.cpp")
file(GLOB SOURCES_HPP "src/*.hpp")

LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP})

# Generates the static library from the SOURCES
add_library(${target_name} STATIC ${SOURCES})

target_link_libraries(${target_name} ${LINK_TO_Common})
target_link_libraries(${target_name} ${LINK_TO_Viewer})
target_link_libraries(${target_name} ${LINK_TO_SpectralAveraging})

# Install will be used by master projects to get information on destination of library files
install(TARGETS ${target_name}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

if (BUILD_WCE_TESTING)
    # common helper files for the testing
    include_directories(../helper)

    # Collect helper files
    file(GLOB helper_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "../helper/*.hpp" "../helper/*.cpp")
    list(APPEND test_src ${helper_test_src})

    # Include main test files
    include_directories(include)
    file(GLOB unit_test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "tst/units/*.cpp")
    list(APPEND test_src ${unit_test_src})

    # Create test targets
    CREATE_TEST_TARGETS_WCE(${target_name} "${test_src}" "")

    # Set test data directory
    set(TEST_DATA_DIR_SINGLE_LAYER_OPTICS "${CMAKE_CURRENT_SOURCE_DIR}/tst")
    target_compile_definitions(${target_name}_tests PRIVATE TEST_DATA_DIR_SINGLE_LAYER_OPTICS="${TEST_DATA_DIR_SINGLE_LAYER_OPTICS}")
endif ()

warning_level_update_wce()