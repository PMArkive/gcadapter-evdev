#include <gcadapter_evdev.h>

void sigint(int sig)
{
    recieved_sigint = true;
}

char port_to_char(gc_adapter_t* adapter, int port)
{
    return adapter->pad[port] == GC_PAD_NONE ? '-' : '#';
}

bool write_button_event(int btn, struct input_event* event, int uinput_fd, bool down)
{
    memset(event, 0, sizeof(*event));
    (*event).type = EV_KEY;
    (*event).code = btn;
    (*event).value = down;
    write(uinput_fd, event, sizeof(*event));
    return true;
}

bool write_axis_event(gc_pad_axis_t axis, int target_axis_x, int target_axis_y, int x_shift_by, int y_shift_by, bool invert_y, struct input_event* event, int uinput_fd)
{
    memset(event, 0, sizeof(*event));
    (*event).type = EV_ABS;
    (*event).code = target_axis_x;
    // we shift the value by about a quarter of our 'deadzone'.
    (*event).value = axis.x + x_shift_by;
    write(uinput_fd, event, sizeof(*event));
    (*event).type = EV_ABS;
    (*event).code = target_axis_y;
    (*event).value = axis.y + y_shift_by;
    if(invert_y)
        (*event).value = 255 - (*event).value;
    write(uinput_fd, event, sizeof(*event));
    return true;
}

void initialize_port(gc_adapter_t* adapter, int port)
{
    int uinput_fd = open("/dev/uinput", O_WRONLY);
    
    if(uinput_fd < 0)
    {
        uinput_fd = open("/dev/input/uinput", O_WRONLY);
        
        if(uinput_fd < 0)
        {
            return;
        }
    }
    
    uinput[port] = uinput_fd;
    
    struct uinput_user_dev evdev_device;
    memset(&evdev_device, 0, sizeof(evdev_device));
    
    snprintf(evdev_device.name, UINPUT_MAX_NAME_SIZE, "GameCube Controller (libgcadapter %s, Adapter %p, Port %i)", LIBGCADAPTER_VERSION_STR, adapter, port + 1);
    evdev_device.id.bustype = BUS_VIRTUAL;
    evdev_device.id.vendor = GC_ADAPTER_USB_VID;
    evdev_device.id.product = GC_ADAPTER_USB_PID;
    evdev_device.id.version = LIBGCADAPTER_VERSION;
    
    ioctl(uinput[port], UI_SET_EVBIT, EV_KEY);
    
    for(int i = 0; i < sizeof(evdev_map_buttons); i += 2)
        ioctl(uinput[port], UI_SET_KEYBIT, evdev_map_buttons[i]);
        
    ioctl(uinput[port], UI_SET_EVBIT, EV_ABS);
   
    for(int i = ABS_X; i <= ABS_RZ; i++)
    {
        ioctl(uinput[port], UI_SET_ABSBIT, i);
        evdev_device.absmin[i] = 32;
        evdev_device.absmax[i] = 255 - 32;
    }
    
    write(uinput[port], &evdev_device, sizeof(evdev_device));
    ioctl(uinput[port], UI_DEV_CREATE);
    
    GC_PAD_RESET_STATE(last_state[port]);
    state[port] = adapter->state[port];
    write_axis_event(state[port].stick, ABS_X, ABS_Y, 0, 8, true, &event, uinput[port]);
    write_axis_event(state[port].c_stick, ABS_Z, ABS_RX, 2, -2, true, &event, uinput[port]);
    write_axis_event(state[port].triggers, ABS_RY, ABS_RZ, -8, -8, false, &event, uinput[port]);
}

void update_port(gc_adapter_t* adapter, int port)
{
    state[port] = adapter->state[port];
    bool written_this_poll = false;
    for(int i = 1; i < sizeof(evdev_map_buttons); i += 2)
    {
        int value = -1;
        if(GC_PAD_BTN_DOWN(state[port], evdev_map_buttons[i]) && !(GC_PAD_BTN_DOWN(last_state[port], evdev_map_buttons[i])))
            value = 1;
        if(!(GC_PAD_BTN_DOWN(state[port], evdev_map_buttons[i])) && GC_PAD_BTN_DOWN(last_state[port], evdev_map_buttons[i]))
            value = 0;
        if(value != -1)
            written_this_poll = write_button_event(evdev_map_buttons[i - 1], &event, uinput[port], (bool)value);
    }
    if(!(GC_PAD_AXISES_EQUAL(state[port].stick, last_state[port].stick)))
        written_this_poll = write_axis_event(state[port].stick, ABS_X, ABS_Y, 0, 8, true, &event, uinput[port]);
    if(!(GC_PAD_AXISES_EQUAL(state[port].c_stick, last_state[port].c_stick)))
        written_this_poll = write_axis_event(state[port].c_stick, ABS_Z, ABS_RX, 2, -2, true, &event, uinput[port]);
    if(!(GC_PAD_AXISES_EQUAL(state[port].triggers, last_state[port].triggers)))
        written_this_poll = write_axis_event(state[port].triggers, ABS_RY, ABS_RZ, -8, -8, false, &event, uinput[port]);
    if(written_this_poll)
    {
        memset(&event, 0, sizeof(event));
        event.type = EV_SYN;
        event.code = SYN_REPORT;
        write(uinput[port], &event, sizeof(event));
    }
    last_state[port] = state[port];
}

void close_port(gc_adapter_t* adapter, int port)
{
    ioctl(uinput[port], UI_DEV_DESTROY);
    close(uinput[port]);
    uinput[port] = 0;
}

int main(int argc, char** argv)
{
    printf("Wii U GameCube Adapter interface for evdev.\n");
    
    if(LIBGCADAPTER_VERSION != gc_adapter_get_version())
    {
        printf("Invalid version of libgcadapter detected (not %s).\n", LIBGCADAPTER_VERSION_STR);
        return -1;
    }
    
    printf("Using libgcadapter %s.\n", LIBGCADAPTER_VERSION_STR);
    
    last_update.tv_sec = 0;
    last_update.tv_usec = 0;
    
    for(int i = 0; i < GC_ADAPTER_PORTS; i++)
    {
        uinput[i] = 0;
    }
    
    signal(SIGINT, sigint);
    
    gc_adapter_t* adapter;
    
    if(gc_adapter_initialize(&adapter))
    {
        while(!recieved_sigint)
        {
            struct timeval update;
            gettimeofday(&update, NULL);
            double update_ms = (update.tv_sec * 1000.0) + (update.tv_usec / 1000.0);
            double last_update_ms = (last_update.tv_sec * 1000.0) + (last_update.tv_usec / 1000.0);
            
            if(update_ms - last_update_ms >= 100.0)
            {
                gc_adapter_update(adapter);
                
                if(adapter->open)
                    printf("\rAdapter connected (%p) %c %c %c %c", 
                    adapter->usb_device_handle, 
                    port_to_char(adapter, 0),
                    port_to_char(adapter, 1),
                    port_to_char(adapter, 2),
                    port_to_char(adapter, 3));
                else
                    printf("\rWaiting for adapter...              ");
                    
                fflush(stdout);
                last_update = update;
            }
            
            gc_adapter_poll(adapter);
            
            for(int i = 0; i < GC_ADAPTER_PORTS; i++)
            {
                if(uinput[i] == 0 && adapter->pad[i] != GC_PAD_NONE)
                    initialize_port(adapter, i);
                else if(uinput[i] != 0 && adapter->pad[i] == GC_PAD_NONE)
                    close_port(adapter, i);
                if(uinput[i] != 0 && adapter->pad[i] != GC_PAD_NONE)
                    update_port(adapter, i);
            }
    
            sched_yield();
        }
        
        printf("\n");
        
        for(int i = 0; i < GC_ADAPTER_PORTS; i++)
        {
            if(uinput[i] != 0)
            {
                close_port(adapter, i);
            }
        }
        
        gc_adapter_free(adapter);
        return 0;
    }
    else
    {
        printf("Could not initialize libgcadapter.");
        return -1;
    }
}
