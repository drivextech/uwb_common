cmake_minimum_required(VERSION 3.0.8)
project(uwb_common VERSION 0.1.0)

option(SELF_TEST_ENABLE "If is standalone project? (not part of some project)" OFF)

include(CTest)
enable_testing()

if(NOT TARGET dxt_common AND SELF_TEST_ENABLE)
    execute_process(
        COMMAND git clone https://github.com/drivextech/dxt_common
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    add_subdirectory(${PROJECT_SOURCE_DIR}/dxt_common)
    include_directories(${PROJECT_SOURCE_DIR}/dxt_common)
endif()
unset(SELF_TEST_ENABLE CACHE)


include_directories(${PROJECT_SOURCE_DIR})

add_library(uwb_common
    mac_frame/mac_frame.c)
target_link_libraries(uwb_common dxt_common)
add_dependencies(uwb_common dxt_common)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
