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
        TextureManager.cpp
        TextureManager.h
        primitives/CirnoPrism.cpp
        primitives/CirnoPrism.h
        LightManager.cpp
        LightManager.h
        LightningPass.cpp
        LightningPass.h
        ObjectManager.cpp
        ObjectManager.h
)

target_include_directories(hf1
    PRIVATE ${Vulkan_INCLUDE_DIRS} ${glfw_INCLUDE_DIRS} ${EXTERNALS_BUILD_DIR}
)

target_link_libraries(hf1
    PRIVATE glfw Vulkan::Vulkan imgui vkcourse
)

add_shaders(hf1
        shaders/shader.vert SPV_shader_in_vert
        shaders/shader.frag SPV_shader_in_frag
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