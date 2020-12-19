#include <stdio.h>
#include "chip8.h"
#include "frontend.h"

int main(int argc, char** argv) {
    chip8_init("BC_test.ch8");

    frontend_init();
    chip8_run();
}