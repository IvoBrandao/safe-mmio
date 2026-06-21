add_subdirectory(mmio)
add_subdirectory(systick)

if(CMAKE_CROSSCOMPILING)
    add_subdirectory(startup)
endif()
