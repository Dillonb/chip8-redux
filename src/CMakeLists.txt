SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
find_package(SDL2 REQUIRED)
add_executable(${CHIP8_TARGET} main.c chip8.c chip8.h log.c log.h frontend.c frontend.h)
target_link_libraries(${CHIP8_TARGET} ${SDL2_LIBRARY})