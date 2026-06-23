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
 *    - Full, identical execution of classic GW-BASIC DOS11 game and graphics scripts.
 *    - Non-blocking keyboard/stdin event queues.
 * 
 * 4. WHAT TO DO IF SOMETHING BREAKS:
 *    - Check variable tables, default variable type states, and stack pointers.
 *    - Cross-reference token layouts with original detokenization specifications.
 *    - Run diagnostic verification script to identify isolated error line numbers.
 * -----------------------------------------------------------------------------
 */
#ifndef STRINGS_H
#define STRINGS_H

#include "gwbasic.h"

GW_String gw_str_create(const char *str, size_t len);
void gw_str_free(GW_String *s);

GW_String gw_str_concat(const GW_String *s1, const GW_String *s2);
GW_String gw_str_left(const GW_String *s, int len);
GW_String gw_str_right(const GW_String *s, int len);
GW_String gw_str_mid(const GW_String *s, int start, int len);
double gw_str_val(const GW_String *s);
GW_String gw_str_chr(int code);
GW_String gw_str_space(int count);
GW_String gw_str_string(int count, const GW_String *char_s);

#endif // STRINGS_H
