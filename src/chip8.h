#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#include <stdint.h>
#include <stdbool.h>

#define INLINE static inline __attribute__((always_inline))

#define SCREEN_X 64
#define SCREEN_Y 32

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct chip8 {
    u8 mem[0x1000];

    u16 pc;
    u8 sp;
    u16 index;
    u8 delay;
    u8 sound;
    u8 V[16];
    u16 stack[16];

    bool screen[SCREEN_Y][SCREEN_X];
    bool screen_updated;
} chip8_t;

extern chip8_t state;

void chip8_init();

void chip8_run();

#endif //CHIP8_CHIP8_H
