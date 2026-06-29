# Nodepp Camera Wrapper

This repository contains the camera.h wrapper class for accessing USB Video Class (UVC) compliant cameras within the Nodepp Project environment. It utilizes the libuvc library to provide a clean, asynchronous C++ interface for discovery, control, and frame streaming.

## Dependencies & CMake Integration
```bash
#libuvc-dev
🪟: pacman -S mingw-w64-x86_64-libuvc
🐧: sudo apt install libuvc-dev
```
```bash
include(FetchContent)

FetchContent_Declare(
	nodepp
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp)

FetchContent_Declare(
	nodepp-camera
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-camera
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp-camera)

#[...]

target_link_libraries( #[...]
	PUBLIC nodepp nodepp-camera #[...]
)
```

## Build & Run
```bash
g++ -o main main.cpp -I./include -luvc -lpthread -lusb-1.0; ./main
```

## License
**Nodepp-Camera** is distributed under the MIT License. See the LICENSE file for more details.