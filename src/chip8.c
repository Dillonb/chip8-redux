#include "chip8.h"
#include "log.h"
#include "frontend.h"
#include <string.h>
#include <stdio.h>

#define MAX_PROGRAM_SIZE 3583
#define INSTRUCTIONS_PER_FRAME 100
typedef void(*chip8_instr_t)(u16 instr);

chip8_t state;
chip8_instr_t instr_lut[0x10000];

#define INC_PC do { state.pc += 2; } while(false)
#define INSTR_X (((instr) & 0x0F00) >> 8)
#define INSTR_Y (((instr) & 0x00F0) >> 4)
#define Vx state.V[INSTR_X]
#define Vy state.V[INSTR_Y]
#define VF state.V[0xF]
#define INSTR_NNN ((instr) & 0x0FFF)
#define INSTR_N ((instr) & 0x000F)
#define INSTR_KK ((instr) & 0x00FF)


INLINE u16 read_u16(u16 addr) {
    u16 value;
    memcpy(&value, &state.mem[addr], sizeof(u16));
    return be16toh(value);
}

INLINE u8 read_u8(u16 addr) {
    return state.mem[addr];
}

INLINE void write_u8(u16 addr, u8 value) {
    state.mem[addr] = value;
}

void show_frame() {
    for (int i = 0; i < SCREEN_X + 2; i++) {
        printf("-");
    }
    printf("\n");
    for (int y = 0; y < SCREEN_Y; y++) {
        printf("|");
        for (int x = 0; x < SCREEN_X; x++) {
            if (state.screen[y][x]) {
                printf("█");
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }
    for (int i = 0; i < SCREEN_X + 2; i++) {
        printf("-");
    }
    printf("\n");
}

void instr_cls(u16 instr) {
    memset(state.screen, 0, sizeof(state.screen));
    state.screen_updated = true;
    INC_PC;
}

void instr_ret(u16 instr) {
    state.pc = state.stack[state.sp];
    state.sp--;
}

void instr_invalid(u16 instr) {
    logfatal("Invalid instruction 0x%04X", instr);
}

void instr_unknown(u16 instr) {
    logfatal("Unknown instruction 0x%04X", instr);
}

void instr_jp(u16 instr) {
    state.pc = INSTR_NNN;
}

void instr_call(u16 instr) {
    state.sp++;
    state.stack[state.sp] = state.pc;
    state.pc = INSTR_NNN;
}

void instr_se_imm(u16 instr) {
    if (Vx == INSTR_KK) {
        INC_PC;
    }
    INC_PC;
}

void instr_sne_imm(u16 instr) {
    if (Vx != INSTR_KK) {
        INC_PC;
    }
    INC_PC;
}

void instr_se_xy(u16 instr) {
    if (Vx == Vy) {
        INC_PC;
    }
    INC_PC;
}

void instr_ld_imm(u16 instr) {
    Vx = INSTR_KK;
    INC_PC;
}

void instr_add_imm(u16 instr) {
    Vx = Vx + INSTR_KK;
    INC_PC;
}

void instr_ld_xy(u16 instr) {
    Vx = Vy;
    INC_PC;
}

void instr_or_xy(u16 instr) {
    Vx |= Vy;
    INC_PC;
}

void instr_and_xy(u16 instr) {
    Vx &= Vy;
    INC_PC;
}

void instr_xor_xy(u16 instr) {
    Vx ^= Vy;
    INC_PC;
}

void instr_add_xy(u16 instr) {
    logfatal("instr_add_xy");
}

void instr_sub_xy(u16 instr) {
    VF = (Vx > Vy) ? 1 : 0;
    Vx -= Vy;
    INC_PC;
}

void instr_shr_xy(u16 instr) {
    VF = Vx & 1;
    Vx >>= 1;
    INC_PC;
}

void instr_subn_xy(u16 instr) {
    VF = (Vy > Vx) ? 1 : 0;
    Vy -= Vx;
    INC_PC;
}

void instr_shl_xy(u16 instr) {
    VF = ((Vx & 0x80) == 0x80) ? 1 : 0;
    Vx <<= 1;
    INC_PC;
}

void instr_sne_xy(u16 instr) {
    logfatal("instr_sne_xy");
}

void instr_ld_i_imm(u16 instr) {
    state.index = INSTR_NNN;
    INC_PC;
}

void instr_jp_v0_ofs(u16 instr) {
    logfatal("instr_jp_v0_ofs");
}

void instr_rnd(u16 instr) {
    logfatal("instr_rnd");
}

void instr_drw(u16 instr) {
    bool collision = false;

    state.screen_updated = true;

    for (int yofs = 0; yofs < INSTR_N; yofs++) {
        u8 row = read_u8(state.index + yofs);
        for (int xofs = 7; xofs >= 0; xofs--) {
            bool* pixel = &state.screen[(Vy + yofs) % SCREEN_Y][(Vx + xofs) % SCREEN_X];

            bool newpixel = (row & 1) ^*pixel;
            row >>= 1;

            if (*pixel && !newpixel) {
                collision = true;
            }

            *pixel = newpixel;
        }
    }

    VF = collision ? 1 : 0;
    INC_PC;
}

void instr_skp(u16 instr) {
    logfatal("instr_skp");
}

void instr_sknp(u16 instr) {
    logfatal("instr_sknp");
}

void instr_ld_x_dt(u16 instr) {
    logfatal("instr_ld_x_dt");
}

void instr_ld_x_k(u16 instr) {
    logfatal("instr_ld_x_k");
}

void instr_ld_dt_x(u16 instr) {
    logfatal("instr_ld_dt_x");
}

void instr_ld_st_x(u16 instr) {
    logfatal("instr_ld_st_x");
}

void instr_add_i_x(u16 instr) {
    state.index += Vx;
    INC_PC;
}

void instr_ld_f_x(u16 instr) {
    // address of sprite containing the digit contained in Vx
    state.index = Vx * 5;
    INC_PC;
}

void instr_ld_b_x(u16 instr) {
    u8 hundreds = Vx / 100;
    u8 tens = (Vx / 10) % 10;
    u8 ones = Vx % 10;
    write_u8(state.index + 0, hundreds);
    write_u8(state.index + 1, tens);
    write_u8(state.index + 2, ones);
    INC_PC;
}

void instr_ld_i_x(u16 instr) {
    for (int i = 0; i <= INSTR_X; i++) {
        write_u8(state.index + i, state.V[i]);
    }
    INC_PC;
}

void instr_ld_x_i(u16 instr) {
    for (int i = 0; i <= INSTR_X; i++) {
        state.V[i] = read_u8(state.index + i);
    }
    INC_PC;
}

chip8_instr_t chip8_decode(u16 instr) {
    switch (instr & 0xF000) {
        case 0x0000: {
            switch (instr) {
                case 0x00E0:
                    return instr_cls;
                case 0x00EE:
                    return instr_ret;
                default:
                    return instr_invalid;
            }
        }
        case 0x1000: return instr_jp;
        case 0x2000: return instr_call;
        case 0x3000: return instr_se_imm;
        case 0x4000: return instr_sne_imm;
        case 0x5000: {
            if ((instr & 0xF) == 0) {
                return instr_se_xy;
            } else {
                return instr_invalid;
            }
        }
        case 0x6000: return instr_ld_imm;
        case 0x7000: return instr_add_imm;
        case 0x8000: {
            switch (instr & 0x000F) {
                case 0x0: return instr_ld_xy;
                case 0x1: return instr_or_xy;
                case 0x2: return instr_and_xy;
                case 0x3: return instr_xor_xy;
                case 0x4: return instr_add_xy;
                case 0x5: return instr_sub_xy;
                case 0x6: return instr_shr_xy;
                case 0x7: return instr_subn_xy;
                case 0xE: return instr_shl_xy;

                default:
                    return instr_invalid;
            }
        }
        case 0x9000: {
            if ((instr & 0xF) == 0) {
                return instr_sne_xy;
            } else {
                return instr_invalid;
            }
        }
        case 0xA000: return instr_ld_i_imm;
        case 0xB000: return instr_jp_v0_ofs;
        case 0xC000: return instr_rnd;
        case 0xD000: return instr_drw;
        case 0xE000: {
            switch (instr & 0x00FF) {
                case 0x9E: return instr_skp;
                case 0xA1: return instr_sknp;
                default: return instr_invalid;
            }
        }
        case 0xF000: {
            switch (instr & 0xFF) {
                case 0x07: return instr_ld_x_dt;
                case 0x0A: return instr_ld_x_k;
                case 0x15: return instr_ld_dt_x;
                case 0x18: return instr_ld_st_x;
                case 0x1E: return instr_add_i_x;
                case 0x29: return instr_ld_f_x;
                case 0x33: return instr_ld_b_x;
                case 0x55: return instr_ld_i_x;
                case 0x65: return instr_ld_x_i;
                default: return instr_invalid;
            }
        }
        default:
            return instr_unknown;
    }
}

void chip8_run() {
    while (!should_quit) {
        for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
            u16 instr = read_u16(state.pc);
            instr_lut[instr](instr);
        }
        on_frame_end(&state.screen, &state.screen_updated);
    }
}

void chip8_init(char* rom_path) {
    memset(&state.mem, 0, sizeof(chip8_t));

    u8 chip8_chars[] = {
            // Characters 0-F sprites
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(&state.mem, &chip8_chars, sizeof(chip8_chars));

    FILE* rom = fopen(rom_path, "rb");
    if (!rom) {
        logfatal("Failed to load %s", rom_path);
    }

    fread(&(state.mem[0x200]), 1, MAX_PROGRAM_SIZE, rom);

    state.pc = 0x200;

    for (int instr = 0x0000; instr <= 0xFFFF; instr++) {
        instr_lut[instr] = chip8_decode(instr);
    }
}
