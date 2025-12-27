# VKCourse assignment

![gif of the running program](demo.webp)

To build:
```sh
$ cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug
$ make -C build/ -j4
```

To run:
```sh
$ ./build/bin/hf1
```

# Required packages

Linux (ubuntu package names):
* vulkan-validationlayers
* libvulkan-dev
* libvulkan1
* glslang-tools
* cmake

* (recommended): libglfw3-dev
* If libglfw3-dev is not installed then additionally: libxkbcommon-dev xorg-dev