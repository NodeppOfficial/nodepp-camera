#include <nodepp/nodepp.h>
#include <camera/camera.h>

using namespace nodepp;

void onMain(){

    auto devices = camera::scan();
    if( !devices ){ console::error( devices.error() ); return; }

    auto camera = devices.value()[0];
    console::log("device:", camera.get_device_product());
    
    camera.onData( [=]( camera_frame_t frame ){
        console::log( "Frame received:", frame.width, "x", frame.height, ":", frame.size );
    //  camera.stop_streaming();
    });

    camera.onError([=]( except_t err ){
        console::log( "err:", err.what() );
    });

    camera.start_streaming( FORMAT_YUYV, 640, 480, 30 );

}