#include "debug.h"

void set_terminal_color(debug_level_t level, bool reset_color)
{
#if CONF_ENABLE_TERMINAL_COLOR == 1
    if (reset_color)
    {
        DEBUG_OUT(F("\033[39m\033[49m"));
        return;
    }

    DEBUG_OUT(F("\033[38;5;"));
    printf("%d", (int)level);
    DEBUG_OUT(F("m"));
#endif
    return;
}

void debug_print(debug_level_t debug_level, newline_t newline, const char *fmt, ...)
{
    set_terminal_color(debug_level, false);

    FILE_NAME_PRINT();

#if CONF_APPEND_DEBUG_LEVEL == 1
    switch (debug_level)
    {
    case debug:
        DEBUG_OUT(F("DEBUG: "));
        break;

    case info:
        DEBUG_OUT(F("INFO: "));
        break;

    case warn:
        DEBUG_OUT(F("WARNING: "));
        break;

    case error:
        DEBUG_OUT(F("ERROR: "));
        break;
    }
#endif

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    set_terminal_color(debug_level, true);

    if (newline)
        DEBUG_OUT(F("\n\r"));

#if CONF_FLUSH_DEBUG == 1
    DEBUG_OUT_FLUSH();
#endif
}