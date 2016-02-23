#include <stdbool.h>
#include <gctypes.h>

#ifndef LIBGCADAPTER_H
#define LIBGCADAPTER_H

#define LIBGCADAPTER_VERSION 110
#define LIBGCADAPTER_VERSION_STR "1.1"

#define GC_ADAPTER_USB_VID 0x057e
#define GC_ADAPTER_USB_PID 0x0337
#define GC_ADAPTER_PORTS 4
#define GC_ADAPTER_PAYLOAD_SIZE 37

typedef struct
{
    /* public */
    bool open;
    gc_pad_t pad[GC_ADAPTER_PORTS];
    gc_pad_state_t state[GC_ADAPTER_PORTS];
    uint8_t rumble[GC_ADAPTER_PORTS];
    
    /* private, don't use these */
    /* use them for debugging if you want though */
    uint8_t usb_recv_data[GC_ADAPTER_PAYLOAD_SIZE];
    void* usb_context;
    void* usb_device_handle;
    uint8_t usb_endpoint_read;
    uint8_t usb_endpoint_write;
}
gc_adapter_t;

int gc_adapter_get_version();
bool gc_adapter_initialize(gc_adapter_t** adapter);
void gc_adapter_update(gc_adapter_t* adapter);
void gc_adapter_poll(gc_adapter_t* adapter);
void gc_adapter_set_rumble(gc_adapter_t* adapter, int port, uint8_t rumble);
void gc_adapter_free(gc_adapter_t* adapter);

#endif
