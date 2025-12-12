add_executable(hf1
        hf1.cpp

        primitives/BasePrimitive.h
        primitives/BasePrimitive.cpp
        primitives/Cube.cpp
        primitives/Cube.h
        primitives/Cylinder.cpp
        primitives/Cylinder.h
        primitives/Grid.cpp
        primitives/Grid.h
        scene.cpp
        scene.h
        debug.h
        debug.cpp
        primitives/Sphere.cpp
        primitives/Sphere.h
        primitives/ITransformable.cpp
        primitives/ITransformable.h
        containers/ObjectGroup.cpp
        containers/ObjectGroup.h
        primitives/IDrawable.h
        entities/BaseEntity.h
        entities/RotatingCube.cpp
        entities/RotatingCube.h
        entities/RotatingCube.h
        primitives/Cone.cpp
        primitives/Cone.h
        entities/RotatingTetrahedron.cpp
        entities/RotatingTetrahedron.h
        entities/PistonWithBouncingBall.cpp
        entities/PistonWithBouncingBall.h
        entities/OrbitingHelicopter.cpp
        entities/OrbitingHelicopter.h
        ModelPushConstant.h
        PipelineWrapper.cpp
        PipelineWrapper.h
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
