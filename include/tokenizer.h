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
 *    - Interface definitions, token mappings, and constants for program serialization.
 *    - Token bytecode mapping tables, keyword hashes, and parser symbols.
 * 
 * 4. WHAT TO DO IF SOMETHING BREAKS:
 *    - Check variable tables, default variable type states, and stack pointers.
 *    - Cross-reference token layouts with original detokenization specifications.
 *    - Run diagnostic verification script to identify isolated error line numbers.
 * -----------------------------------------------------------------------------
 */
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>


// Token values for GW-BASIC keywords (typically values from 0x80 to 0xFF)
typedef enum {
    TOK_END = 0x80,
    TOK_FOR,
    TOK_NEXT,
    TOK_DATA,
    TOK_INPUT,
    TOK_DIM,
    TOK_READ,
    TOK_LET,
    TOK_GOTO,
    TOK_RUN,
    TOK_IF,
    TOK_RESTORE,
    TOK_GOSUB,
    TOK_RETURN,
    TOK_REM,
    TOK_STOP,
    TOK_PRINT,
    TOK_DEF,
    TOK_DEFINT,
    TOK_DEFSNG,
    TOK_DEFDBL,
    TOK_DEFSTR,
    TOK_POKE,
    TOK_PRINT_USING,
    TOK_WHILE,
    TOK_WEND,
    TOK_SCREEN,
    TOK_COLOR,
    TOK_LINE,
    TOK_CIRCLE,
    TOK_PAINT,
    TOK_PLAY,
    TOK_SOUND,
    TOK_BEEP,
    TOK_SEG,
    TOK_THEN,
    TOK_ELSE,
    TOK_TO,
    TOK_STEP,
    TOK_USR,
    TOK_CALL,
    TOK_NEW,
    TOK_LIST,
    TOK_SAVE,
    TOK_LOAD,
    TOK_CHAIN,
    TOK_MERGE,
    TOK_PEEK,
    TOK_SYSTEM,
    TOK_CLS,
    TOK_CLEAR,
    TOK_WIDTH,
    TOK_CONSOLE,
    TOK_FILES,
    TOK_KEY,
    TOK_LOCATE,
    TOK_ABS,
    TOK_INT,
    TOK_RND,
    TOK_SQR,
    TOK_CHR,
    TOK_STRING_FN,
    TOK_TAB,
    TOK_INKEY,
    TOK_ON,
    TOK_ERROR,
    TOK_RESUME,
    TOK_ERR,
    TOK_ERL,
    TOK_AND,
    TOK_OR,
    TOK_NOT,
    TOK_XOR,
    TOK_MID,
    TOK_LEFT,
    TOK_RIGHT,
    TOK_LEN,
    TOK_VAL,
    TOK_STR,
    TOK_ASC,
    TOK_INSTR,
    TOK_SGN,
    TOK_SIN,
    TOK_COS,
    TOK_TAN,
    TOK_ATN,
    TOK_EXP,
    TOK_LOG,
    TOK_POS,
    TOK_GET,
    TOK_PUT,
    TOK_DRAW,
    TOK_PSET,
    TOK_PRESET,
    TOK_MOD,
    TOK_CINT,
    TOK_CSNG,
    TOK_CDBL,
    TOK_FIX
} GW_Token;

// Crunch a text line into tokenized bytes
// Returns size of output, or 0 on error
size_t gw_crunch(const char *input_text, uint8_t *output_tokens, size_t max_len);

// List/detokenize tokenized bytes back into text
void gw_list(const uint8_t *tokens, size_t len, char *output_text, size_t max_len);

// Detokenize original GW-BASIC binary line to ASCII text representation
size_t gw_detokenize_binary(const uint8_t *bin, size_t bin_len, char *out, size_t max_len);

// Read a token-aligned binary line from file
bool gw_read_binary_line(FILE *f, uint8_t *temp, int max_len, int *out_len);

#endif // TOKENIZER_H



