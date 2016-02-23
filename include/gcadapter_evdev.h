#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <libgcadapter.h>

bool recieved_sigint = false;
struct timeval last_update;
int uinput[GC_ADAPTER_PORTS];
gc_pad_state_t state[GC_ADAPTER_PORTS];
gc_pad_state_t last_state[GC_ADAPTER_PORTS];
struct input_event event;

int evdev_map_buttons[] = 
{
   BTN_NORTH,
   GC_PAD_BTN_Y,
   BTN_SOUTH,
   GC_PAD_BTN_A,
   BTN_EAST,
   GC_PAD_BTN_X,
   BTN_WEST,
   GC_PAD_BTN_B,
   BTN_START,
   GC_PAD_BTN_START,
   BTN_DPAD_UP,
   GC_PAD_BTN_DPAD_UP,
   BTN_DPAD_DOWN,
   GC_PAD_BTN_DPAD_DOWN,
   BTN_DPAD_LEFT,
   GC_PAD_BTN_DPAD_LEFT,
   BTN_DPAD_RIGHT,
   GC_PAD_BTN_DPAD_RIGHT,
   BTN_TL,
   GC_PAD_BTN_L,
   BTN_TR,
   GC_PAD_BTN_R,
   BTN_TR2,
   GC_PAD_BTN_Z
};
