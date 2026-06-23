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
 *    - Interface definitions for logical devices and file control operations.
 *    - File control block (FCB) interface declarations and channel routing.
 * 
 * 4. WHAT TO DO IF SOMETHING BREAKS:
 *    - Check variable tables, default variable type states, and stack pointers.
 *    - Cross-reference token layouts with original detokenization specifications.
 *    - Run diagnostic verification script to identify isolated error line numbers.
 * -----------------------------------------------------------------------------
 */
#ifndef FILEIO_H
#define FILEIO_H

#include "gwbasic.h"

// OPEN statement
//   mode: 1 = Input, 2 = Output, 4 = Random, 8 = Append
int gw_file_open(GW_State *state, int channel, const char *path, int mode, int record_len);

// CLOSE statement
void gw_file_close(GW_State *state, int channel);

// Sequential writes/reads
void gw_file_print(GW_State *state, int channel, const char *str);
int gw_file_read_line(GW_State *state, int channel, char *buf, size_t max_len);

// Random I/O
void gw_file_field(GW_State *state, int channel, int offset, int field_len, GW_Var *var);
void gw_file_get(GW_State *state, int channel, int record_num);
void gw_file_put(GW_State *state, int channel, int record_num);

#endif // FILEIO_H
