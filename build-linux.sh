#!/bin/bash
echo "=== Building GW-BASIC for Linux (WSL) ==="

# Compile CLI version
echo "[INFO] Compiling gwbasic-console..."
gcc -D_GNU_SOURCE -std=c17 -O2 -Iinclude -Ilib -DNO_SDL2 -o gwbasic-console \
  src/main.c src/interp.c src/eval.c src/tokenizer.c src/variables.c \
  src/strings.c src/fileio.c src/console.c src/events.c \
  lib/gw_math_mbf.c lib/gw_memory.c lib/gw_plugin.c lib/gw_sdl2.c lib/gw_serial.c \
  -lm

if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile gwbasic-console"
    exit 1
fi

# Compile GUI version
echo "[INFO] Compiling gwbasic..."
gcc -D_GNU_SOURCE -std=c17 -O2 -Iinclude -Ilib -o gwbasic \
  src/main.c src/interp.c src/eval.c src/tokenizer.c src/variables.c \
  src/strings.c src/fileio.c src/console.c src/events.c \
  lib/gw_math_mbf.c lib/gw_memory.c lib/gw_plugin.c lib/gw_sdl2.c lib/gw_serial.c \
  $(sdl2-config --cflags --libs) -lm

if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile gwbasic (GUI)"
    exit 1
fi

echo "=== Build Complete! ==="
