set(SUBMODULE_TESTS_NAME tests_app)
set(SUBMODULE_TESTS_DEPS app systickdrv mmr)

include(GoogleTest)

file(GLOB SUBMODULE_TESTS_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_executable(${SUBMODULE_TESTS_NAME})

target_include_directories(${SUBMODULE_TESTS_NAME}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../api
)

target_sources(${SUBMODULE_TESTS_NAME}
    PRIVATE
        ${SUBMODULE_TESTS_SRC_FILES}
)

target_link_libraries(${SUBMODULE_TESTS_NAME} PRIVATE ${SUBMODULE_TESTS_DEPS} gtest_main gtest)

gtest_discover_tests(${SUBMODULE_TESTS_NAME})

if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    target_compile_options(${SUBMODULE_TESTS_NAME} PRIVATE --coverage)
    target_link_options(${SUBMODULE_TESTS_NAME} PRIVATE --coverage)
endif()
