# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/external/glfw")
  file(MAKE_DIRECTORY "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/external/glfw")
endif()
file(MAKE_DIRECTORY
  "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/_deps/glfw-build"
  "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix"
  "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/tmp"
  "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
  "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src"
  "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/ssd/egyetem/Vulkan grafikus API/hf1/vkcourse/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
