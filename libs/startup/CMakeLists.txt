set(SUBMODULE_NAME startup)
set(SUBMODULE_TYPE STATIC)

set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/STM32F429ZITx.ld)

add_library(${SUBMODULE_NAME} ${SUBMODULE_TYPE})

target_sources(${SUBMODULE_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/source/startup_stm32f429zi.cpp
)

target_include_directories(${SUBMODULE_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_options(${SUBMODULE_NAME} INTERFACE
    -T${LINKER_SCRIPT}
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

set_target_properties(${SUBMODULE_NAME} PROPERTIES
    LINKER_SCRIPT ${LINKER_SCRIPT}
)
