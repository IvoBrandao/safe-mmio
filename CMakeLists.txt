cmake_minimum_required(VERSION 3.25)

project(Example_Project CXX C ASM)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/tools/cmake/modules)
set(CMAKE_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(Doxygen)
include(PlantUml)

if(CMAKE_CROSSCOMPILING)
    add_subdirectory(libs)
    add_subdirectory(app)

    add_executable(${PROJECT_NAME}.elf app/source/main.cpp)
    target_link_libraries(${PROJECT_NAME}.elf PRIVATE app startup)

    add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
        COMMENT "Generating HEX and BIN files"
    )
else()
    include(FetchContent)
    include(CTest)
    include(CodeCoverage)

    if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    endif()

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(libs)
    add_subdirectory(app)

    add_custom_target(DependencyGraph
        COMMAND cmake --graphviz=DependencyGraph.dot .
        COMMAND dot -Tsvg DependencyGraph.dot -o DependencyGraph.svg
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
