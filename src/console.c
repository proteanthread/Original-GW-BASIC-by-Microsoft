/**
 * Original GW-BASIC Interpreter Port (C17)
 * 
 * -----------------------------------------------------------------------------
 * MAINTENANCE & EXTENSION GUIDELINES
 * -----------------------------------------------------------------------------
 * 1. WHAT CAN BE CHANGED:
 *    - Console sizing, output formatting, or ANSI color/escape sequences.
 *    - Logic inside statement handlers to optimize standard BASIC behaviors.
 *    - Math functions (tuning logic for trigonometric or random values).
 * 
 * 2. WHAT CANNOT BE CHANGED:
 *    - Keyword/token byte mapping tables (essential for loading tokenized BAS binaries).
 *    - Segmented memory layout simulation structures.
 *    - Core mathematical parsing precedence chain (eval descent hierarchy).
 * 
 * 3. EXPECTED BEHAVIOR:
 *    - Console and terminal input/output handling, line-editing, and ASCII character reading.
 *    - Buffered keystroke queue management and terminal escape sequence rendering.
 * 
 * 4. WHAT TO DO IF SOMETHING BREAKS:
 *    - Check variable tables, default variable type states, and stack pointers.
 *    - Cross-reference token layouts with original detokenization specifications.
 *    - Run diagnostic verification script to identify isolated error line numbers.
 * -----------------------------------------------------------------------------
 */
#define CONSOLE_C_INTERNAL
#include "console.h"
#include "gw_sdl2.h"
#include "gwbasic.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#endif

void gw_console_write_char(char c) {
    putchar(c);
    fflush(stdout);
    
    if (g_state) {
        uint32_t fg = GW_PALETTE[g_state->fg_color % 16];
        uint32_t bg = GW_PALETTE[g_state->bg_color % 16];
        gw_sdl2_set_text_color(fg, bg);
    }
    gw_sdl2_write_char(c);
}

char gw_console_read_char(void) {
    // Poll SDL2 events to populate keyboard buffer
    gw_sdl2_poll_events();
    
    int code = gw_sdl2_get_key();
    if (code > 0) {
        return (char)code;
    }
    
#ifdef _WIN32
    if (_kbhit()) {
        return (char)_getch();
    }
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD fileType = GetFileType(hStdin);
    if (fileType == FILE_TYPE_DISK) {
        int ch = fgetc(stdin);
        if (ch != EOF) return (char)ch;
    } else if (fileType == FILE_TYPE_PIPE || fileType == FILE_TYPE_CHAR || fileType == FILE_TYPE_UNKNOWN) {
        DWORD bytesAvail = 0;
        if (PeekNamedPipe(hStdin, NULL, 0, NULL, &bytesAvail, NULL) && bytesAvail > 0) {
            char ch = 0;
            DWORD bytesRead = 0;
            if (ReadFile(hStdin, &ch, 1, &bytesRead, NULL) && bytesRead > 0) {
                return ch;
            }
        }
    }
#else
    // Non-blocking Unix stdin check
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    char ch = 0;
    fd_set rfds;
    struct timeval tv = {0, 0};
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    
    if (select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv) > 0) {
        read(STDIN_FILENO, &ch, 1);
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
    return 0;
}

int gw_printf(const char *format, ...) {
    char buffer[2048];
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    int in_ansi = 0;
    for (int i = 0; i < result && buffer[i] != '\0'; i++) {
        char c = buffer[i];
        if (in_ansi) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                in_ansi = 0;
            }
            continue;
        }
        if (c == '\033') {
            in_ansi = 1;
            continue;
        }
        gw_console_write_char(c);
    }
    return result;
}

void gw_console_read_line(char *buf, size_t max_len) {
#ifdef NO_SDL2
    if (fgets(buf, max_len, stdin)) {
        size_t len = strlen(buf);
        while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
            buf[--len] = '\0';
        }
    }
    return;
#else
    size_t idx = 0;
    buf[0] = '\0';
    
    uint32_t last_blink = 0;
    int cursor_visible = 0;
    
    // Clear key buffer first
    while (gw_sdl2_get_key() > 0) {}
    
    while (1) {
        gw_sdl2_poll_events();
        
        uint32_t current_ticks = gw_sdl2_ticks();
        if (current_ticks - last_blink >= 250) {
            cursor_visible = !cursor_visible;
            last_blink = current_ticks;
            gw_sdl2_write_char_cursor(cursor_visible);
        }
        
        int code = gw_sdl2_get_key();
        if (code > 0) {
            // Clear cursor before modifying text
            if (cursor_visible) {
                gw_sdl2_write_char_cursor(0);
                cursor_visible = 0;
            }
            
            if (code == 13 || code == '\n') { // Enter
                buf[idx] = '\0';
                gw_console_write_char('\n');
                break;
            } else if (code == 8 || code == 127) { // Backspace
                if (idx > 0) {
                    idx--;
                    gw_console_write_char('\b');
                }
            } else if (code == 9) { // Tab
                if (idx < max_len - 1) {
                    buf[idx++] = '\t';
                    gw_console_write_char('\t');
                }
            } else if (code >= 32 && code <= 126) { // Printable characters
                if (idx < max_len - 1) {
                    buf[idx++] = (char)code;
                    gw_console_write_char((char)code);
                }
            }
        }
        
        gw_sdl2_delay(10);
    }
#endif
}
