#include <stdint.h>

#ifndef GCTYPES_H
#define GCTYPES_H

typedef enum
{
    GC_PAD_BTN_NONE = (1 << 0),
    GC_PAD_BTN_DPAD_LEFT = (1 << 1),
    GC_PAD_BTN_DPAD_RIGHT = (1 << 2),
    GC_PAD_BTN_DPAD_DOWN = (1 << 3),
    GC_PAD_BTN_DPAD_UP = (1 << 4),
    GC_PAD_BTN_Z = (1 << 5),
    GC_PAD_BTN_R = (1 << 6),
    GC_PAD_BTN_L = (1 << 7),
    GC_PAD_BTN_A = (1 << 8),
    GC_PAD_BTN_B = (1 << 9),
    GC_PAD_BTN_X = (1 << 10),
    GC_PAD_BTN_Y = (1 << 11),
    GC_PAD_BTN_START = (1 << 12)
} 
gc_pad_btn_t;

typedef struct
{
    uint8_t x, y;
} 
gc_pad_axis_t;

#define GC_PAD_STICK_CENTER 128

#define GC_PAD_BTN_DOWN(state, btn) (state.buttons & btn) == btn

#define GC_PAD_AXIS_CENTERED(axis) \
        axis.x == GC_PAD_STICK_CENTER && \
        axis.Y == GC_PAD_STICK_CENTER

#define GC_PAD_RESET_STATE(state) \
        state.buttons = 0; \
        state.stick.x = GC_PAD_STICK_CENTER; \
        state.stick.y = GC_PAD_STICK_CENTER; \
        state.c_stick.x = GC_PAD_STICK_CENTER; \
        state.c_stick.y = GC_PAD_STICK_CENTER; \
        state.triggers.x = 0; \
        state.triggers.y = 0;
    
#define GC_PAD_STATES_EQUAL(st1, st2) \
        st1.buttons == st2.buttons && \
        st1.stick.x == st2.stick.x && \
        st1.stick.y == st2.stick.y && \
        st1.c_stick.x == st2.c_stick.x && \
        st1.c_stick.y == st2.c_stick.y && \
        st1.triggers.x == st2.triggers.x && \
        st1.triggers.y == st2.triggers.y
        
#define GC_PAD_AXISES_EQUAL(ax1, ax2) \
        ax1.x == ax2.x && \
        ax1.y == ax2.y

typedef struct
{
    gc_pad_btn_t buttons;
    gc_pad_axis_t stick, c_stick, triggers;
} 
gc_pad_state_t;

typedef enum
{
    GC_PAD_NONE = 0,
    GC_PAD_WIRED = 1,
    GC_PAD_WIRELESS = 2
}
gc_pad_t;

#endif
