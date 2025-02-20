/* SubEffects 0.3.0v debug.h */
/*
 * MIT License
 *
 * Copyright (c) 2022 Mikko Johannes Heinänen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

/* Manual config for our use case */
#define CONF_ENABLE_TERMINAL_COLOR 1

#define CONF_DEBUG_COLOR WHITE
#define CONF_INFO_COLOR CYAN
#define CONF_WARN_COLOR YELLOW
#define CONF_ERROR_COLOR RED

#define CONF_ENABLE_DEBUG 1
#define CONF_ENABLE_INFO 1
#define CONF_ENABLE_WARN 1
#define CONF_ENABLE_ERROR 1

#define CONF_APPEND_DEBUG_LEVEL 1
#define PRINT_FILE_NAME 0
#define CONF_FLUSH_DEBUG 1

typedef enum
{
    no_newline,
    newline,
    newline_args
} newline_t;

/**
 * @brief High-intensity colors
 * @note https://i.stack.imgur.com/KTSQa.png
 *
 */
typedef enum
{
    GREY = 8,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PINK,
    CYAN,
    WHITE
} terminal_colors;

typedef enum
{
    error = CONF_ERROR_COLOR,
    warn = CONF_WARN_COLOR,
    info = CONF_INFO_COLOR,
    debug = CONF_DEBUG_COLOR,
} debug_level_t;

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

/* F() macro to mimic Arduino's F() macro. In C, we just use the string literal directly */
#ifndef F
#define F(str) (str)
#endif

/* Easily change debug output*/
#define DEBUG_OUT(...) \
    printf(__VA_ARGS__);

/* Same as DEBUG_OUT, but without double colon ; */
#define DEBUG_OUT_NC(...) \
    printf(__VA_ARGS__)

#define DEBUG_OUT_FLUSH() \
    fflush(stdout);

/* If defined. Appends file name to the debug output */
#if PRINT_FILE_NAME == 1
#define FILE_NAME_PRINT()    \
    do {                     \
        DEBUG_OUT("%s", __FILENAME__); \
        DEBUG_OUT(F("   ")); \
    } while (0)
#else
#define FILE_NAME_PRINT()
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

/**
 * @brief Sets terminal to debug_level_t color
 *
 * @param level     "error level"
 * @param reset_color when true restores terminal color to default
 */
void set_terminal_color(debug_level_t level, bool reset_color);

/**
 * @brief Converted debug_print for C.
 *
 * @param debug_level  Debug level (error, warn, info, debug)
 * @param newline      newline flag. If true a newline (\n\r) is printed.
 * @param fmt          Format string for the message
 * @param ...          Format arguments
 */
void debug_print(debug_level_t debug_level, newline_t newline, const char *fmt, ...);

#if CONF_ENABLE_DEBUG == 1
#define DEBUG(...) debug_print(debug, newline, __VA_ARGS__)
#else
#define DEBUG(...)
#endif

#if CONF_ENABLE_ERROR == 1
#define ERROR(...) debug_print(error, newline, __VA_ARGS__)
#else
#define ERROR(...)
#endif

#if CONF_ENABLE_INFO == 1
#define INFO(...) debug_print(info, newline, __VA_ARGS__)
#else
#define INFO(...)
#endif

#if CONF_ENABLE_WARN == 1
#define WARN(...) debug_print(warn, newline, __VA_ARGS__)
#else
#define WARN(...)
#endif

#pragma GCC diagnostic pop
#endif