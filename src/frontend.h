#ifndef CHIP8_FRONTEND_H
#define CHIP8_FRONTEND_H

#include "chip8.h"

typedef union color {
    u32 raw;
    struct {
        u8 a;
        u8 r;
        u8 g;
        u8 b;
    };
} color_t;

extern bool should_quit;

void frontend_init();
void on_frame_end(bool (*screen)[SCREEN_Y][SCREEN_X], bool* should_update_texture);

#endif //CHIP8_FRONTEND_H
