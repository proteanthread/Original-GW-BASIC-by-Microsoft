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
 *    - Main entry point of the interpreter, environment initialization, and command-line parsing.
 *    - Shell interactive read-eval-print loop (REPL) and basic command-line switch parser.
 * 
 * 4. WHAT TO DO IF SOMETHING BREAKS:
 *    - Check variable tables, default variable type states, and stack pointers.
 *    - Cross-reference token layouts with original detokenization specifications.
 *    - Run diagnostic verification script to identify isolated error line numbers.
 * -----------------------------------------------------------------------------
 */
#include "gwbasic.h"
#include "gw_sdl2.h"
#include "gw_memory.h"
#include "gw_plugin.h"
#include "tokenizer.h"
#include "interp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

GW_State *g_state = NULL;

void gw_error(int error_code) {
    const char *errors[] = {
        "Success",
        "NEXT without FOR",
        "Syntax error",
        "RETURN without GOSUB",
        "Out of DATA",
        "Illegal function call",
        "Overflow",
        "Out of memory",
        "Undefined line number",
        "Subscript out of range",
        "Duplicate Definition",
        "Division by zero",
        "Illegal direct",
        "Type mismatch",
        "Out of string space",
        "String too long",
        "String formula too complex",
        "Can't continue",
        "Undefined user function",
        "No RESUME",
        "RESUME without error",
        "Missing operand",
        "Line buffer overflow",
        "Device Timeout",
        "Device Fault",
        "FOR without NEXT",
        "Out of Paper",
        "Unprintable error",
        "Disk full"
    };
    
    int size = sizeof(errors) / sizeof(errors[0]);
    const char *err_msg = (error_code >= 0 && error_code < size) ? errors[error_code] : "Unprintable error";
    
    if (g_state && g_state->current_line_num > 0) {
        if (g_state->error_handler_line > 0 && !g_state->in_error_handler) {
            g_state->last_error_code = error_code;
            g_state->last_error_line = g_state->current_line_num;
            g_state->error_line_ptr = g_state->current_line;
            g_state->error_ip = g_state->stmt_ip;
            g_state->in_error_handler = true;
            g_state->error_jump_pending = true;
            return;
        }
        printf("\n%s in %d\n", err_msg, g_state->current_line_num);
        g_state->last_error_code = error_code;
        g_state->last_error_line = g_state->current_line_num;
        g_state->execution_active = false;
    } else {
        printf("\n%s\n", err_msg);
        if (g_state) {
            g_state->execution_active = false;
        }
    }
    
    if (g_state) {
        g_state->execution_active = false;
    }
}

static void init_interpreter_state(void) {
    g_state = (GW_State *)calloc(1, sizeof(GW_State));
    if (!g_state) {
        fprintf(stderr, "Fatal: Out of memory at boot.\n");
        exit(1);
    }
    
    for (int i = 0; i < 26; i++) {
        g_state->default_types[i] = TYPE_SINGLE;
    }
    
    g_state->string_space_size = 1048576;
    g_state->string_space = (char *)malloc(g_state->string_space_size);
    g_state->string_space_free = g_state->string_space_size;
    
    g_state->mem_sys = gw_mem_create(1048576);
    g_state->plugin_mgr = gw_plugin_init();
    
    g_state->fg_color = 7;
    g_state->bg_color = 0;
}



int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    init_interpreter_state();
    
    // Parse machine flags
    GW_MachineType machine = MACHINE_VGA;
    char *load_file = NULL;
    for (int idx = 1; idx < argc; idx++) {
        if (strcmp(argv[idx], "-herc") == 0 || strcmp(argv[idx], "-hercules") == 0) {
            machine = MACHINE_HERCULES;
        } else if (strcmp(argv[idx], "-tandy") == 0) {
            machine = MACHINE_TANDY;
        } else if (strcmp(argv[idx], "-pcjr") == 0) {
            machine = MACHINE_PCJR;
        } else if (strcmp(argv[idx], "-plantronics") == 0) {
            machine = MACHINE_PLANTRONICS;
        } else if (strcmp(argv[idx], "-att") == 0) {
            machine = MACHINE_ATT;
        } else if (strcmp(argv[idx], "-amstrad") == 0) {
            machine = MACHINE_AMSTRAD;
        } else if (strcmp(argv[idx], "-pc98") == 0) {
            machine = MACHINE_PC98;
        } else if (argv[idx][0] != '-') {
            load_file = argv[idx];
        }
    }
    g_state->machine = machine;
    gw_sdl2_set_machine((int)machine);
    gw_sdl2_init(640, 400, "GW-BASIC C17", 0);
    gw_sdl2_set_mode(0, 80);
    printf("GW-BASIC Modernized 64-bit Interpreter (C17)\n");
    
    if (load_file) {
        strncpy(g_state->current_program_path, load_file, sizeof(g_state->current_program_path) - 1);
        g_state->current_program_path[sizeof(g_state->current_program_path) - 1] = '\0';
        printf("Loading file: %s\n", load_file);
        FILE *f = fopen(load_file, "rb");
        if (f) {
            int first = fgetc(f);
            if (first == 0xFF) {
                while (!feof(f)) {
                    uint16_t offset = 0;
                    if (fread(&offset, 2, 1, f) < 1 || offset == 0) break;
                    uint16_t line_num = 0;
                    fread(&line_num, 2, 1, f);
                    uint8_t temp[BUFLEN + 1];
                    int idx = 0;
                    if (!gw_read_binary_line(f, temp, BUFLEN + 1, &idx)) break;
                    
                    char ascii_line[BUFLEN * 4 + 1];
                    gw_detokenize_binary(temp, idx, ascii_line, sizeof(ascii_line));
                    uint8_t crunched[BUFLEN + 1];
                    size_t crunched_len = gw_crunch(ascii_line, crunched, sizeof(crunched));
                    gw_program_insert(g_state, line_num, crunched, crunched_len);
                }
            } else {
                if (first != EOF) ungetc(first, f);
                char file_line[BUFLEN + 1];
                while (fgets(file_line, sizeof(file_line), f)) {
                    size_t flen = strlen(file_line);
                    while (flen > 0 && (file_line[flen - 1] == '\n' || file_line[flen - 1] == '\r')) {
                        file_line[--flen] = '\0';
                    }
                    char *start = file_line;
                    while (*start == ' ' || *start == '\t') start++;
                    char *endptr;
                    long line_num = strtol(start, &endptr, 10);
                    if (endptr != start && line_num >= 0) {
                        while (*endptr == ' ' || *endptr == '\t') endptr++;
                        uint8_t crunched[BUFLEN + 1];
                        size_t crunched_len = gw_crunch(endptr, crunched, sizeof(crunched));
                        gw_program_insert(g_state, line_num, crunched, crunched_len);
                    }
                }
            }
            fclose(f);
            printf("Running...\n");
            gw_run_program(g_state);
            gw_mem_destroy(g_state->mem_sys);
            gw_plugin_cleanup(g_state->plugin_mgr);
            free(g_state->string_space);
            free(g_state);
            gw_sdl2_cleanup();
            return 0;
        } else {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        }
    } else {
        printf("Ok\n");
    }
    
    char line_buf[BUFLEN + 1];
    while (1) {
        gw_sdl2_poll_events();
        
        printf("\nReady\n");
        printf("] ");
        gw_console_read_line(line_buf, sizeof(line_buf));
        
        // Strip newline and carriage return
        size_t len = strlen(line_buf);
        while (len > 0 && (line_buf[len - 1] == '\n' || line_buf[len - 1] == '\r')) {
            line_buf[--len] = '\0';
        }
        
        // Skip leading whitespace
        char *start = line_buf;
        while (*start == ' ' || *start == '\t') start++;
        
        if (strlen(start) == 0) continue;
        
        // Check standard shell commands
        char norm_cmd[10];
        int i;
        for (i = 0; start[i] && i < 9 && !isspace((unsigned char)start[i]); i++) {
            norm_cmd[i] = toupper((unsigned char)start[i]);
        }
        norm_cmd[i] = '\0';
        
        if (strcmp(norm_cmd, "SYSTEM") == 0) {
            break;
        } else if (strcmp(norm_cmd, "NEW") == 0) {
            gw_program_clear(g_state);
            continue;
        } else if (strcmp(norm_cmd, "LIST") == 0) {
            GW_Line *l = g_state->program_head;
            while (l) {
                char text[BUFLEN + 1];
                gw_list(l->tokens, l->length, text, sizeof(text));
                printf("%d %s\n", l->line_num, text);
                l = l->next;
            }
            continue;
        }
        
        // Check if line starts with a line number
        char *endptr;
        long line_num = strtol(start, &endptr, 10);
        if (endptr != start && line_num >= 0) {
            // Line statement: add to program storage
            // Skip spaces after line number
            while (*endptr == ' ' || *endptr == '\t') endptr++;
            
            uint8_t crunched[BUFLEN + 1];
            size_t crunched_len = gw_crunch(endptr, crunched, sizeof(crunched));
            gw_program_insert(g_state, line_num, crunched, crunched_len);
        } else {
            // Immediate execution mode
            uint8_t crunched[BUFLEN + 1];
            size_t crunched_len = gw_crunch(start, crunched, sizeof(crunched));
            
            g_state->ip = crunched;
            g_state->current_line = NULL;
            g_state->current_line_num = 0;
            
            g_state->execution_active = true;
            while (g_state->ip && *g_state->ip) {
                gw_exec_statement(g_state);
                if (g_state->jump_pending) {
                    g_state->jump_pending = false;
                    gw_run_program(g_state);
                    break;
                }
            }
            g_state->execution_active = false;
        }
    }
    
    if (g_state) {
        gw_mem_destroy(g_state->mem_sys);
        gw_plugin_cleanup(g_state->plugin_mgr);
        free(g_state->string_space);
        free(g_state);
    }
    
    gw_sdl2_cleanup();
    printf("Goodbye!\n");
    return 0;
}

#if defined(_WIN32) && !defined(NO_SDL2)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
#endif
