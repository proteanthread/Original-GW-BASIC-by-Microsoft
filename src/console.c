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
#include "console.h"
#include "gw_sdl2.h"
#include "gwbasic.h"
#include <stdio.h>
#include <string.h>

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
    // Check SDL key buffer first
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
    if (fileType == FILE_TYPE_PIPE || fileType == FILE_TYPE_CHAR || fileType == FILE_TYPE_UNKNOWN) {
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

void gw_console_read_line(char *buf, size_t max_len) {
    if (fgets(buf, max_len, stdin)) {
        size_t len = strlen(buf);
        while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
            buf[--len] = '\0';
        }
    } else {
        buf[0] = '\0';
    }
}
