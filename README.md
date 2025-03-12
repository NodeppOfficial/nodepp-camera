# NODEPP-CAMERA
Read USB Camera's Data in Nodepp

## Dependencies
```bash
#libuvc-dev
🪟: pacman -S mingw-w64-x86_64-libuvc
🐧: sudo apt install libuvc-dev
```

## Example
```cpp
#include <nodepp/nodepp.h>
#include <camera/camera.h>

using namespace nodepp;

void onMain() {

    auto devices = camera::scan();

    devices[0].start_recording( UVC_FRAME_FORMAT_YUYV, 640, 480, 30 );

    process::add([=](){
    coStart
    
        while( devices[0].is_available() ){ do {
               auto frame = devices[0].get_frame();
               if ( frame== nullptr ) { break; }
               coonsole::log( frame->data ); 
        } while(0); coNext; } devices[0].close();
    
    coStop
    });

}
```

## Project
[raylib-camera-example](https://github.com/EDBCREPO/raylib-camera-example)

## Compilation
```bash
g++ -o main main.cpp -I./include -luvc -lpthread -lusb-1.0
```
