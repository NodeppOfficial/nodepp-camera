/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_CAMERA_CAMERA
#define NODEPP_CAMERA_CAMERA

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/expected.h>
#include <libuvc/libuvc.h>
#include <nodepp/nodepp.h>
#include <nodepp/expected.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { enum camera_frame_format {
    FORMAT_UNKNOWN = 0,  FORMAT_ANY = 0,
    FORMAT_UNCOMPRESSED, FORMAT_COMPRESSED,
    FORMAT_YUYV,         FORMAT_UYVY,
    FORMAT_RGB,          FORMAT_BGR,
    FORMAT_MJPEG,        FORMAT_H264,
    FORMAT_GRAY8,        FORMAT_GRAY16,
    FORMAT_BY8,          FORMAT_BA81,
    FORMAT_SGRBG8,       FORMAT_SGBRG8,
    FORMAT_SRGGB8,       FORMAT_SBGGR8,
    FORMAT_NV12,         FORMAT_P010,
    FORMAT_COUNT,
};}

namespace nodepp { struct camera_frame_t {
    int   height , width, type;
    uint  count=0; void*  data;
    ulong size;
};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class camera_t {
protected:

    enum STATE {
         CAMERA_STATE_UNKNOWN   = 0b00000000,
         CAMERA_STATE_USED      = 0b00000001,
         CAMERA_STATE_CLOSED    = 0b01000000,
         CAMERA_STATE_RECORDING = 0b10000000
    };

    struct NODE {

        uvc_device_handle_t *hdl = nullptr;
        uvc_context_t       *ctx = nullptr;
        uvc_device_t        *dev = nullptr;
        string_t             err ;
        camera_frame_t       frm ;
        ulong              stamp = 0;
        int                state = 0;

       ~NODE ( ) {
        if( hdl ){ uvc_close       ( hdl ); }
        if( dev ){ uvc_unref_device( dev ); }
        if( ctx ){ uvc_exit        ( ctx ); }}

    };  ptr_t <NODE> obj;

    static void frame_callback( uvc_frame_t *raw_frame, void *user_data ) {

        if( user_data==nullptr || raw_frame==nullptr ){ return; }

        auto self = (camera_t *)( user_data );
        
        if( self->is_closed() ) /*-*/ { return; }
        if( raw_frame->data_bytes==0 ){ return; }

        camera_frame_t &frame = self->obj->frm;

        frame.type  = raw_frame->frame_format; 
        frame.size  = raw_frame->data_bytes;
        frame.height= raw_frame->height;
        frame.width = raw_frame->width;
        frame.data  = raw_frame->data;
        frame.count = 0;

        self->obj->stamp = process::now(); 
        self->onData.emit( frame );

    }

    bool error( uvc_error state ) const noexcept { switch( state ) {
        case UVC_ERROR_IO:             obj->err="Input/output error";      return true ; break;
        case UVC_ERROR_INVALID_PARAM:  obj->err="Invalid Parameter";       return true ; break;
        case UVC_ERROR_ACCESS:         obj->err="Access denied";           return true ; break;
        case UVC_ERROR_NO_DEVICE:      obj->err="No such device";          return true ; break;
        case UVC_ERROR_NOT_FOUND:      obj->err="Entity not found";        return true ; break;
        case UVC_ERROR_BUSY:           obj->err="Resource busy";           return true ; break;
        case UVC_ERROR_TIMEOUT:        obj->err="Operation timed out";     return true ; break;
        case UVC_ERROR_NO_MEM:         obj->err="Insufficient memory";     return true ; break;
        case UVC_ERROR_NOT_SUPPORTED:  obj->err="Operation not supported"; return true ; break;
        case UVC_ERROR_INVALID_DEVICE: obj->err="Device not supported";    return true ; break;
        case UVC_ERROR_INVALID_MODE:   obj->err="Mode not supported";      return true ; break;
        case UVC_ERROR_OTHER:          obj->err="something went wrong";    return true ; break;
        default:                       obj->err.clear();                   return false; break;
    }}

    enum uvc_frame_format get_type( uint type ) const noexcept { switch( type ) {
        case FORMAT_UNCOMPRESSED: return UVC_FRAME_FORMAT_UNCOMPRESSED; break;
        case FORMAT_COMPRESSED:   return UVC_FRAME_FORMAT_COMPRESSED;   break;
        case FORMAT_UNKNOWN:      return UVC_FRAME_FORMAT_UNKNOWN;      break;
        case FORMAT_SGRBG8:       return UVC_FRAME_FORMAT_SGRBG8;       break;
        case FORMAT_SGBRG8:       return UVC_FRAME_FORMAT_SGBRG8;       break;
        case FORMAT_SRGGB8:       return UVC_FRAME_FORMAT_SRGGB8;       break;
        case FORMAT_SBGGR8:       return UVC_FRAME_FORMAT_SBGGR8;       break;
        case FORMAT_GRAY16:       return UVC_FRAME_FORMAT_GRAY16;       break;
        case FORMAT_GRAY8:        return UVC_FRAME_FORMAT_GRAY8;        break;
        case FORMAT_MJPEG:        return UVC_FRAME_FORMAT_MJPEG;        break;
        case FORMAT_COUNT:        return UVC_FRAME_FORMAT_COUNT;        break;
        case FORMAT_YUYV:         return UVC_FRAME_FORMAT_YUYV;         break;
        case FORMAT_UYVY:         return UVC_FRAME_FORMAT_UYVY;         break;
        case FORMAT_H264:         return UVC_FRAME_FORMAT_H264;         break;
        case FORMAT_BA81:         return UVC_FRAME_FORMAT_BA81;         break;
        case FORMAT_NV12:         return UVC_FRAME_FORMAT_NV12;         break;
        case FORMAT_P010:         return UVC_FRAME_FORMAT_P010;         break;
        case FORMAT_RGB:          return UVC_FRAME_FORMAT_RGB;          break;
        case FORMAT_BGR:          return UVC_FRAME_FORMAT_BGR;          break;
        case FORMAT_BY8:          return UVC_FRAME_FORMAT_BY8;          break;
        default:                  return UVC_FRAME_FORMAT_ANY;          break; 
    }   /*---------------------*/ return UVC_FRAME_FORMAT_ANY; }

    void kill() const noexcept { 
        if( is_closed () ){ return ; } 
        uvc_stop_streaming( obj->hdl );
        obj->state &=~ STATE::CAMERA_STATE_RECORDING;
    }

public:

    event_t<camera_frame_t> onData ;
    event_t<> /*---------*/ onClose;
    event_t<> /*---------*/ onDrain;
    event_t<except_t> /*-*/ onError;

    /*─······································································─*/

    expected_t<camera_t,except_t>
    get_device( uchar_16 vid, uchar_16 pid, const char* serial ) const noexcept {

        if( error( uvc_init( &obj->ctx, nullptr ) ) )
          { return except_t( obj->err ); }

        if( error( uvc_find_device( obj->ctx, &obj->dev, vid, pid, serial ) ) ) 
          { return except_t( obj->err ); }

        if( error( uvc_open( obj->dev, &obj->hdl ) ) )
          { return except_t( obj->err ); }

        obj->state = STATE::CAMERA_STATE_USED;
        obj->stamp = process::now()   ;
        
    return *this; }

    /*─······································································─*/
    
    camera_t() : obj( new NODE() ) {}

   ~camera_t() { if( obj.count() > 1 ){ return; } free(); }

    /*─······································································─*/

    bool is_available() const noexcept { return obj->state & STATE::CAMERA_STATE_USED && ( process::now() - obj->stamp ) < 3000 ; }

    bool is_recording() const noexcept { return obj->state & STATE::CAMERA_STATE_RECORDING; }

    bool    is_closed() const noexcept { return !is_available(); }

    /*─······································································─*/
    
    void stop_streaming() const noexcept { close(); }

    void close() const noexcept { 
        if( is_closed   () ){ return; }
        if( is_recording() ){ kill(); }
        obj->state = STATE::CAMERA_STATE_CLOSED;
        onDrain.emit(); free(); 
    }

    /*─······································································─*/

    int get_produc_id() const noexcept {
    uvc_device_descriptor_t *descriptor;
        
        if( uvc_get_device_descriptor( obj->dev, &descriptor ) )
          { return -1; }  
          
        auto resp = descriptor->idProduct;
        uvc_free_device_descriptor(descriptor); return resp;
    };

    int get_vendor_id() const noexcept {
    uvc_device_descriptor_t *descriptor;
        
        if( uvc_get_device_descriptor( obj->dev, &descriptor ) )
          { return -1; }  
          
        auto resp = descriptor->idVendor;
        uvc_free_device_descriptor(descriptor); return resp;
    };
    
    /*─······································································─*/

    string_t get_device_product() const noexcept {
    uvc_device_descriptor_t *descriptor; char *desc_str = nullptr;
        
        if( uvc_get_device_descriptor( obj->dev, &descriptor ) )
          { return nullptr; }

        auto resp = string_t( descriptor->product, strlen( descriptor->product ) );
        uvc_free_device_descriptor(descriptor); return resp;
    };

    string_t get_device_manufacturer() const noexcept {
    uvc_device_descriptor_t *descriptor; char *desc_str = nullptr;
        
        if( uvc_get_device_descriptor( obj->dev, &descriptor ) )
          { return nullptr; }

        auto resp = string_t( descriptor->manufacturer, strlen( descriptor->manufacturer ) );
        uvc_free_device_descriptor(descriptor); return resp;
    };

    string_t get_device_serial() const noexcept {
    uvc_device_descriptor_t *descriptor; char *desc_str = nullptr;
        
        if( uvc_get_device_descriptor( obj->dev, &descriptor ) )
          { return nullptr; }

        auto resp = string_t( descriptor->serialNumber, strlen( descriptor->serialNumber ) );
        uvc_free_device_descriptor(descriptor); return resp;
    };
    
    /*─······································································─*/

    template < class... T > int
    start_streaming( int type, int width, int height, int fps ) const noexcept {

        if( !is_available() )
          { onError.emit( except_t( "camera has been closed" ) ); return -1; } 

        if( obj->state & STATE::CAMERA_STATE_RECORDING )
          { onError.emit( except_t( "camera is been used" ) );    return -1; } 

            obj->state|= STATE::CAMERA_STATE_RECORDING;
            auto self  = type::bind( this );
            uvc_stream_ctrl_t ctrl;

        if( error( uvc_get_stream_ctrl_format_size( obj->hdl, &ctrl, get_type(type), width, height, fps ) ) || 
            error( uvc_start_streaming /*------*/ ( obj->hdl, &ctrl, frame_callback, (void*) &self, 0   ) ) 
        ) { onError.emit( except_t( obj->err ) ); return -1; }

        process::add( coroutine::add( COROUTINE(){
        coBegin

            while ( self->is_available() && self->is_recording() ) {
            if( self->obj->frm.count > 1 ){ return 1; }
            if( self->obj->frm.size == 0 ){ return 1; }
                self->onData.emit( self->obj->frm );
                self->obj->frm.count++; coNext;
            }

        coFinish
        }));

    return 1; }
    
    /*─······································································─*/

    void free() const noexcept {

        if( is_recording() ){ kill(); }
        if( is_available() ){ onDrain.emit(); obj->state = STATE::CAMERA_STATE_CLOSED; }

        onDrain.clear(); onData.clear();
        onError.clear(); onClose.emit();

    }
    
    /*─······································································─*/

    camera_frame_t get_frame() const noexcept { return obj->frm; }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace camera {

    inline expected_t<ptr_t<camera_t>,except_t> 
    scan () {
         uvc_context_t *ctx ; queue_t<camera_t> que;
    do { uvc_device_t **devs;

        if( uvc_init(&ctx, NULL)!=0 ) /*-------*/ { break; }
        if( uvc_get_device_list( ctx, &devs )!=0 ){ break; }
        if( devs[0] == nullptr ) /*------------*/ { break; }

        ulong x=0; while( devs[x]!=nullptr ){ uvc_device_descriptor_t *desc;

            if( uvc_get_device_descriptor( devs[x], &desc )!=0 ){ continue; }

            auto cam = camera_t().get_device( 
                desc->idVendor , /*--------------*/
                desc->idProduct, desc->serialNumber
            );

            if( cam.has_value() ){ que.push( cam.value() ); }

        /*---*/ uvc_free_device_descriptor( desc );
        x++ ; } uvc_free_device_list   ( devs, x );

        if( que.empty() ){ break; }

        /*---*/ uvc_exit(ctx); return que.data();
    } while(0); uvc_exit(ctx); return except_t( "no cameras founded" ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif