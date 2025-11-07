add_executable(hf1
        hf1.cpp

#        cube.cpp
#        cube.h
        grid.cpp
        grid.h
#        cylinder.cpp
        PipelineUtils.h
        PipelineUtils.cpp
        primitives/BasePrimitive.h
        primitives/BasePrimitive.cpp
        primitives/Cube.cpp
        primitives/Cube.h
        primitives/Cylinder.cpp
        primitives/Cylinder.h
)

target_include_directories(hf1
    PRIVATE ${Vulkan_INCLUDE_DIRS} ${glfw_INCLUDE_DIRS} ${EXTERNALS_BUILD_DIR}
)

target_link_libraries(hf1
    PRIVATE glfw Vulkan::Vulkan imgui vkcourse
)

add_shaders(hf1
    triangle_in.vert SPV_triangle_in_vert
    triangle_in.frag SPV_triangle_in_frag
    grid.vert SPV_grid_vert
    grid.frag SPV_grid_frag
)
