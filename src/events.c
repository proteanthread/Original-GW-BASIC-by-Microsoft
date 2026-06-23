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
#include "events.h"
#include <stdio.h>

#ifndef NO_SDL2
#include <SDL.h>
#else
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif
#endif

void gw_event_register_key(GW_State *state, int key_idx, int32_t gosub_line) {
    if (!state || key_idx < 0 || key_idx >= NMKEYF) return;
    state->key_traps[key_idx].gosub_line = gosub_line;
    state->key_traps[key_idx].enabled = (gosub_line > 0);
    state->key_traps[key_idx].active = false;
    state->key_traps[key_idx].trapped = false;
}

void gw_event_register_timer(GW_State *state, int timer_idx, int32_t gosub_line) {
    if (!state || timer_idx < 0 || timer_idx >= 4) return;
    state->timer_traps[timer_idx].gosub_line = gosub_line;
    state->timer_traps[timer_idx].enabled = (gosub_line > 0);
    state->timer_traps[timer_idx].active = false;
    state->timer_traps[timer_idx].trapped = false;
}

static uint32_t g_last_ticks[4] = {0, 0, 0, 0};

static uint32_t get_ticks_ms(void) {
#ifndef NO_SDL2
    return SDL_GetTicks();
#else
#ifdef _WIN32
    return GetTickCount();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
#endif
}

void gw_event_poll(GW_State *state) {
    if (!state) return;
    
    uint32_t current_ticks = get_ticks_ms();
    
    // Poll Timer traps (e.g. Timer 1 checks every 1000ms, Timer 2 check every 2000ms etc.)
    for (int i = 0; i < 4; i++) {
        if (state->timer_traps[i].enabled) {
            uint32_t interval = (i + 1) * 1000;
            if (current_ticks - g_last_ticks[i] >= interval) {
                state->timer_traps[i].trapped = true;
                g_last_ticks[i] = current_ticks;
            }
        }
    }
    
    // Check if any trapped event can trigger a GOSUB branch
    for (int i = 0; i < 4; i++) {
        if (state->timer_traps[i].trapped && !state->timer_traps[i].active && !state->in_error_handler) {
            state->timer_traps[i].trapped = false;
            state->timer_traps[i].active = true;
            
            printf("[Event Trap: Timer %d Branching to %d]\n", i + 1, state->timer_traps[i].gosub_line);
            // In a fully recursive statement executor, we would trigger a branch here.
        }
    }
}
