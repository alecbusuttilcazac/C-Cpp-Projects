#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// ANSI color codes:

    // Regular colors:
    #define COLOUR_RESET   "\033[0m"
    #define COLOUR_BLACK   "\033[30m"
    #define COLOUR_RED     "\033[31m"
    #define COLOUR_GREEN   "\033[32m"
    #define COLOUR_YELLOW  "\033[33m"
    #define COLOUR_BLUE    "\033[34m"
    #define COLOUR_MAGENTA "\033[35m"
    #define COLOUR_CYAN    "\033[36m"
    #define COLOUR_WHITE   "\033[37m"

    // Bright colors:
    #define COLOUR_BRIGHT_BLACK   "\033[90m"
    #define COLOUR_BRIGHT_RED     "\033[91m"
    #define COLOUR_BRIGHT_GREEN   "\033[92m"
    #define COLOUR_BRIGHT_YELLOW  "\033[93m"
    #define COLOUR_BRIGHT_BLUE    "\033[94m"
    #define COLOUR_BRIGHT_MAGENTA "\033[95m"
    #define COLOUR_BRIGHT_CYAN    "\033[96m"
    #define COLOUR_BRIGHT_WHITE   "\033[97m"
//

#ifdef __cplusplus
} // extern "C"
#endif