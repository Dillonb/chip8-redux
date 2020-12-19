#include <stdio.h>
#include "chip8.h"
#include "frontend.h"
#include "log.h"

int main(int argc, char** argv) {
    chip8_init("trip8.ch8");
    log_set_verbosity(LOG_VERBOSITY_TRACE);

    frontend_init();
    chip8_run();
}