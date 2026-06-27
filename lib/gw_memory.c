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
 *    - Simulated segmented RAM layout mapping for PEEK, POKE, DEF SEG, and VARPTR.
 *    - Emulation of x86 segmentation register bounds and DEF SEG offset routing.
 * 
 * 4. WHAT TO DO IF SOMETHING BREAKS:
 *    - Check variable tables, default variable type states, and stack pointers.
 *    - Cross-reference token layouts with original detokenization specifications.
 *    - Run diagnostic verification script to identify isolated error line numbers.
 * -----------------------------------------------------------------------------
 */
#include "gw_memory.h"
#include "gw_sdl2.h"
#include <stdlib.h>
#include <string.h>

#define MAX_HOOKS 64

typedef struct {
    uint32_t start;
    uint32_t end;
    GW_MemReadHook hook;
    void *ctx;
} GW_ReadHookEntry;

typedef struct {
    uint32_t start;
    uint32_t end;
    GW_MemWriteHook hook;
    void *ctx;
} GW_WriteHookEntry;

struct GW_Memory {
    uint8_t *buffer;
    size_t size;
    uint16_t def_seg;
    
    GW_ReadHookEntry read_hooks[MAX_HOOKS];
    int read_hook_count;
    
    GW_WriteHookEntry write_hooks[MAX_HOOKS];
    int write_hook_count;
};

static uint8_t bda_read_hook(uint32_t addr, void *ctx) {
    (void)ctx;
    switch (addr) {
        case 0x00410:
            return 0x24;
        case 0x00411:
            return 0x00;
        case 0x00417:
            return gw_sdl2_get_shift_flags();
        case 0x00418:
            return 0;
        case 0x00449: {
            int basic_mode = gw_sdl2_get_mode();
            int cols = gw_sdl2_get_columns();
            switch (basic_mode) {
                case 0: return (cols == 40) ? 1 : 3;
                case 1: return 4;
                case 2: return 6;
                case 7: return 13;
                case 8: return 14;
                case 9: return 16;
                case 10: return 15;
                case 11: return 17;
                case 12: return 18;
                case 13: return 19;
                default: return 3;
            }
        }
        case 0x0044A:
            return (uint8_t)gw_sdl2_get_columns();
        case 0x0044B:
            return 0;
        default:
            break;
    }
    return 0;
}

static uint8_t ticks_read_hook(uint32_t addr, void *ctx) {
    (void)ctx;
    uint32_t ticks = (uint32_t)(gw_sdl2_ticks() * 18.2 / 1000.0);
    int byte_idx = addr - 0x0046C;
    return (uint8_t)((ticks >> (byte_idx * 8)) & 0xFF);
}

static void vram_write_hook(uint32_t addr, uint8_t val, void *ctx) {
    (void)ctx;
    uint32_t offset = addr - 0xB8000;
    if (offset >= 4000) return;
    int cell_idx = offset / 2;
    int cols = gw_sdl2_get_columns();
    if (cols <= 0) cols = 80;
    int x = cell_idx % cols;
    int y = cell_idx / cols;
    if (offset % 2 == 0) {
        gw_sdl2_poke_char(x, y, (char)val);
    }
}

GW_Memory *gw_mem_create(size_t size) {
    GW_Memory *mem = (GW_Memory *)malloc(sizeof(GW_Memory));
    if (!mem) return NULL;
    
    mem->buffer = (uint8_t *)calloc(1, size);
    if (!mem->buffer) {
        free(mem);
        return NULL;
    }
    
    mem->size = size;
    mem->def_seg = 0;
    mem->read_hook_count = 0;
    mem->write_hook_count = 0;
    
    if (size >= 1048576) {
        memcpy(mem->buffer + 0xFFFF5, "04/24/89", 8);
        mem->buffer[0xFFFFE] = 0xFC;
    }
    
    gw_mem_register_read_hook(mem, 0x00410, 0x00411, bda_read_hook, NULL);
    gw_mem_register_read_hook(mem, 0x00417, 0x00418, bda_read_hook, NULL);
    gw_mem_register_read_hook(mem, 0x00449, 0x0044B, bda_read_hook, NULL);
    gw_mem_register_read_hook(mem, 0x0046C, 0x0046F, ticks_read_hook, NULL);
    gw_mem_register_write_hook(mem, 0xB8000, 0xB8F9F, vram_write_hook, NULL);
    
    return mem;
}

void gw_mem_destroy(GW_Memory *mem) {
    if (mem) {
        free(mem->buffer);
        free(mem);
    }
}

void gw_mem_def_seg(GW_Memory *mem, uint16_t segment) {
    if (mem) {
        mem->def_seg = segment;
    }
}

uint16_t gw_mem_get_seg(GW_Memory *mem) {
    if (!mem) return 0;
    return mem->def_seg;
}

uint32_t gw_mem_resolve(GW_Memory *mem, uint16_t offset) {
    if (!mem) return 0;
    return ((uint32_t)mem->def_seg * 16) + offset;
}

uint8_t gw_mem_peek(GW_Memory *mem, uint16_t offset) {
    if (!mem) return 0;
    
    uint32_t addr = gw_mem_resolve(mem, offset);
    
    // Check read hooks
    for (int i = 0; i < mem->read_hook_count; i++) {
        if (addr >= mem->read_hooks[i].start && addr <= mem->read_hooks[i].end) {
            return mem->read_hooks[i].hook(addr, mem->read_hooks[i].ctx);
        }
    }
    
    return mem->buffer[addr % mem->size];
}

void gw_mem_poke(GW_Memory *mem, uint16_t offset, uint8_t val) {
    if (!mem) return;
    
    uint32_t addr = gw_mem_resolve(mem, offset);
    
    // Check write hooks
    for (int i = 0; i < mem->write_hook_count; i++) {
        if (addr >= mem->write_hooks[i].start && addr <= mem->write_hooks[i].end) {
            mem->write_hooks[i].hook(addr, val, mem->write_hooks[i].ctx);
            return;
        }
    }
    
    mem->buffer[addr % mem->size] = val;
}

void gw_mem_register_read_hook(GW_Memory *mem, uint32_t start, uint32_t end, GW_MemReadHook hook, void *ctx) {
    if (!mem || mem->read_hook_count >= MAX_HOOKS) return;
    
    mem->read_hooks[mem->read_hook_count].start = start;
    mem->read_hooks[mem->read_hook_count].end = end;
    mem->read_hooks[mem->read_hook_count].hook = hook;
    mem->read_hooks[mem->read_hook_count].ctx = ctx;
    mem->read_hook_count++;
}

void gw_mem_register_write_hook(GW_Memory *mem, uint32_t start, uint32_t end, GW_MemWriteHook hook, void *ctx) {
    if (!mem || mem->write_hook_count >= MAX_HOOKS) return;
    
    mem->write_hooks[mem->write_hook_count].start = start;
    mem->write_hooks[mem->write_hook_count].end = end;
    mem->write_hooks[mem->write_hook_count].hook = hook;
    mem->write_hooks[mem->write_hook_count].ctx = ctx;
    mem->write_hook_count++;
}
