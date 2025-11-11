#ifndef __cplusplus
#error This header requires a C++ compiler!
#elif __cplusplus < 201103L
#error This header requires at least C++11
#endif

#pragma once

#include <cstdint>

// Modern C++ type aliases (C++11+)
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

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