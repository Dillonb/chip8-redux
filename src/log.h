#ifndef CHIP8_LOG_H
#define CHIP8_LOG_H

#include <stdio.h>
#include <stdlib.h>

extern unsigned int chip8_log_verbosity;

#define COLOR_RED       "\033[0;31m"
#define COLOR_GREEN     "\033[0;32m"
#define COLOR_YELLOW    "\033[0;33m"
#define COLOR_CYAN      "\033[0;36m"
#define COLOR_END       "\033[0;m"

#define LOG_VERBOSITY_WARN 1
#define LOG_VERBOSITY_INFO 2
#define LOG_VERBOSITY_DEBUG 3
#define LOG_VERBOSITY_TRACE 4

#ifdef CHIP8_DEBUG_MODE
#define LOG_ENABLED
#endif


#define log_set_verbosity(new_verbosity) do {chip8_log_verbosity = new_verbosity;} while(0)
#define log_get_verbosity() chip8_log_verbosity

#define logfatal(message,...) do { \
    fprintf(stderr, COLOR_RED "[FATAL] at %s:%d ", __FILE__, __LINE__);\
    fprintf(stderr, message "\n" COLOR_END, ##__VA_ARGS__);\
    exit(EXIT_FAILURE);} while(0)

#define logdie(message,...) do { \
    fprintf(stderr, COLOR_RED "[FATAL] ");\
    fprintf(stderr, message "\n" COLOR_END, ##__VA_ARGS__);\
    exit(EXIT_FAILURE);} while(0)

#define logwarn(message,...) do { if (chip8_log_verbosity >= LOG_VERBOSITY_WARN) {printf(COLOR_YELLOW "[WARN]  " message "\n" COLOR_END, ##__VA_ARGS__);} } while(0)
#define logalways(message,...) do { printf(COLOR_CYAN "[LOG]  " message "\n" COLOR_END, ##__VA_ARGS__); } while(0)
#define unimplemented(condition, message) do { if (condition) { logfatal("UNIMPLEMENTED CASE DETECTED: %s", message); } } while(0)

#ifdef LOG_ENABLED
#define loginfo(message,...) do { if (chip8_log_verbosity >= LOG_VERBOSITY_INFO) {printf(COLOR_CYAN "[INFO]  " message "\n" COLOR_END, ##__VA_ARGS__);} } while(0)
#define loginfo_nonewline(message,...) do { if (chip8_log_verbosity >= LOG_VERBOSITY_INFO) {printf(COLOR_CYAN "[INFO]  " message COLOR_END, ##__VA_ARGS__);} } while(0)
#define logdebug(message,...) do { if (chip8_log_verbosity >= LOG_VERBOSITY_DEBUG) {printf(COLOR_GREEN "[DEBUG] " message "\n" COLOR_END, ##__VA_ARGS__);} } while(0)
#define logtrace(message,...) do { if (chip8_log_verbosity >= LOG_VERBOSITY_TRACE) {printf("[TRACE] " message "\n", ##__VA_ARGS__);} } while(0)


#else

#define loginfo(message,...) do {} while(0)
#define loginfo_nonewline(message,...) do {} while(0)
#define logdebug(message,...) do {} while(0)
#define logtrace(message,...) do {} while(0)

#endif

#endif //CHIP8_LOG_H
