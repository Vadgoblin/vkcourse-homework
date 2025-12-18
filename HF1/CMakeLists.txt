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
        entities/SpinningCirnoPrism.cpp
        entities/SpinningCirnoPrism.h
        entities/PistonWithBouncingBall.cpp
        entities/PistonWithBouncingBall.h
        entities/OrbitingHelicopter.cpp
        entities/OrbitingHelicopter.h
        managers/TextureManager.cpp
        managers/TextureManager.h
        primitives/CirnoPrism.cpp
        primitives/CirnoPrism.h
        managers/LightManager.cpp
        managers/LightManager.h
        render_passes/LightningPass.cpp
        render_passes/LightningPass.h
        managers/ObjectManager.cpp
        managers/ObjectManager.h
        render_passes/PostProcessPass.cpp
        render_passes/PostProcessPass.h
        render_passes/ShadowPass.cpp
        render_passes/ShadowPass.h
)

target_include_directories(hf1
    PRIVATE ${Vulkan_INCLUDE_DIRS} ${glfw_INCLUDE_DIRS} ${EXTERNALS_BUILD_DIR}
)

target_link_libraries(hf1
    PRIVATE glfw Vulkan::Vulkan imgui vkcourse
)

add_shaders(hf1
        shaders/lightning_pass.vert SPV_shader_in_vert
        shaders/lightning_pass.frag SPV_shader_in_frag
        shaders/post_process.vert SPV_post_process_vert
        shaders/post_process.frag SPV_post_process_frag
        shaders/shadow_map.vert SPV_shadow_map_vert
        shaders/shadow_map.frag SPV_shadow_map_frag
)


set(TEXTURE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/HF1/textures")
set(TEXTURE_DEST_DIR "${CMAKE_BINARY_DIR}/bin/HF1/textures")

add_custom_command(
        TARGET hf1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${TEXTURE_SOURCE_DIR}
        ${TEXTURE_DEST_DIR}
        COMMENT "Copying Textures to Binary Directory..."
)