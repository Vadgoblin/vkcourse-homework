add_executable(hf1
        hf1.cpp

        PipelineUtils.h
        PipelineUtils.cpp
        primitives/BasePrimitive.h
        primitives/BasePrimitive.cpp
        primitives/Cube.cpp
        primitives/Cube.h
        primitives/Cylinder.cpp
        primitives/Cylinder.h
        primitives/Grid.cpp
        primitives/Grid.h
        ObjectManager.cpp
        ObjectManager.h
        debug.h
        debug.cpp
        primitives/Sphere.cpp
        primitives/Sphere.h
        primitives/ITransformable.cpp
        primitives/ITransformable.h
)

target_include_directories(hf1
    PRIVATE ${Vulkan_INCLUDE_DIRS} ${glfw_INCLUDE_DIRS} ${EXTERNALS_BUILD_DIR}
)

target_link_libraries(hf1
    PRIVATE glfw Vulkan::Vulkan imgui vkcourse
)

add_shaders(hf1
        shaders/triangle_in.vert SPV_triangle_in_vert
        shaders/triangle_in.frag SPV_triangle_in_frag
        shaders/grid.vert SPV_grid_vert
        shaders/grid.frag SPV_grid_frag
)
