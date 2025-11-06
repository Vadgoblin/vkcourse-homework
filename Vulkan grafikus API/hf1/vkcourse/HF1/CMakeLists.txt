add_executable(05_moving
        hf1.cpp

        cube.cpp
        cube.h
    grid.cpp
    grid.h
        cylinder.cpp
)

target_include_directories(05_moving
    PRIVATE ${Vulkan_INCLUDE_DIRS} ${glfw_INCLUDE_DIRS} ${EXTERNALS_BUILD_DIR}
)

target_link_libraries(05_moving
    PRIVATE glfw Vulkan::Vulkan imgui vkcourse
)

add_shaders(05_moving
    triangle_in.vert SPV_triangle_in_vert
    triangle_in.frag SPV_triangle_in_frag
    grid.vert SPV_grid_vert
    grid.frag SPV_grid_frag
)
