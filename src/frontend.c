#include <SDL.h>
#ifdef CHIP8_GPU_RENDER
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#endif
#include "frontend.h"
#include "log.h"

#define SCREEN_SCALE 20

static SDL_Window* window = NULL;
#ifdef CHIP8_GPU_RENDER
static SDL_GLContext gl_context;
#endif
static SDL_Renderer* renderer = NULL;
static SDL_Texture* buffer = NULL;

#define COLOR(R,G,B) {.a = 0xFF, .r = R, .g = G, .b = B }

static color_t BLANK = COLOR(0x2E, 0x34, 0x40);
static color_t ON    = COLOR(0x4C, 0x56, 0x6A);

bool should_quit = false;

uint32_t fps_interval = 1000; // 1000ms = 1 sec
uint32_t sdl_lastframe = 0;
uint32_t sdl_numframes = 0;
uint32_t sdl_fps = 0;
char sdl_wintitle[50] = "chip8-redux 00 FPS";

void frontend_init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        logfatal("SDL couldn't initialize! %s", SDL_GetError());
    }

    window = SDL_CreateWindow("chip8-redux",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_X * SCREEN_SCALE,
                              SCREEN_Y * SCREEN_SCALE,
                              SDL_WINDOW_SHOWN
#ifdef CHIP8_GPU_RENDER
                              | SDL_WINDOW_OPENGL
#endif
                              );

#ifdef CHIP8_GPU_RENDER
    gl_context = SDL_GL_CreateContext(window);

    if (gl_context == NULL) {
        logfatal("SDL couldn't create OpenGL context! %s", SDL_GetError());
    }

    int gl_version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

    if (gl_version == 0) {
        logfatal("Failed to initialize Glad context");
    }

    printf("OpenGL initialized.\n");
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    glViewport(0, 0, SCREEN_X * SCREEN_SCALE, SCREEN_Y * SCREEN_SCALE);
    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
#endif

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, SCREEN_X, SCREEN_Y);

    SDL_RenderSetScale(renderer, SCREEN_SCALE, SCREEN_SCALE);

    if (renderer == NULL) {
        logfatal("SDL couldn't create a renderer! %s", SDL_GetError());
    }
}

void handle_event(SDL_Event* event) {
    switch (event->type) {
        case SDL_QUIT:
            should_quit = true;
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    should_quit = true;
                    break;
            }
            break;
    }
}

void on_frame_end(u64 (*screen)[SCREEN_Y], bool* should_update_texture) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        handle_event(&event);
    }

    if (*should_update_texture) {
        color_t screen_data[SCREEN_Y][SCREEN_X];
        for (int y = 0; y < SCREEN_Y; y++) {
            u64 row = (*screen)[y];
            for (int x = 0; x < SCREEN_X; x++) {
                screen_data[y][x] = (row & ((u64)1 << 63)) ? ON : BLANK;
                row <<= 1;
            }
        }
        SDL_UpdateTexture(buffer, NULL, screen_data, SCREEN_X * sizeof(color_t));
        SDL_RenderCopy(renderer, buffer, NULL, NULL);
        *should_update_texture = false;
    }

    SDL_RenderPresent(renderer);

    sdl_numframes++;
    uint32_t ticks = SDL_GetTicks();
    if (sdl_lastframe < ticks - fps_interval) {
        sdl_lastframe = ticks;
        sdl_fps = sdl_numframes;
        sdl_numframes = 0;
        snprintf(sdl_wintitle, sizeof(sdl_wintitle), "chip8-redux %02d FPS", sdl_fps);
        SDL_SetWindowTitle(window, sdl_wintitle);
    }
}
