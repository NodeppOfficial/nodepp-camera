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
    
        while( devices[0].is_available() ){
            console::log( devices[0].get_frame()->data );
        coDelay( 100 ); } devices[0].close();
    
    coStop
    });

}
```

## Compilation
```bash
g++ -o main main.cpp -I./include -luvc -lpthread -lusb-1.0
```