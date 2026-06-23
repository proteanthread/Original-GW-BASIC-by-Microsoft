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
 *    - Interface definitions for variable lookup and array space management.
 *    - Non-blocking keyboard/stdin event queues.
 * 
 * 4. WHAT TO DO IF SOMETHING BREAKS:
 *    - Check variable tables, default variable type states, and stack pointers.
 *    - Cross-reference token layouts with original detokenization specifications.
 *    - Run diagnostic verification script to identify isolated error line numbers.
 * -----------------------------------------------------------------------------
 */
#ifndef VARIABLES_H
#define VARIABLES_H

#include "gwbasic.h"

// Get or create variable by name. Checks default types map.
GW_Var *gw_var_get(GW_State *state, const char *name, bool create);

// Set variable values
void gw_var_set_num(GW_Var *var, double value);
double gw_var_get_num(GW_Var *var);
void gw_var_set_str(GW_State *state, GW_Var *var, const char *str, size_t len);

// Free all variables
void gw_vars_clear(GW_State *state);

// Arrays Management
GW_Array *gw_arr_get(GW_State *state, const char *name, bool create);
void *gw_arr_index_ptr(GW_Array *arr, int num_dims, int *indices);
GW_Array *gw_arr_create_dims(GW_State *state, const char *name, int num_dims, int *sizes);

#endif // VARIABLES_H
