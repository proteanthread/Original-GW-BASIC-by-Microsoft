@echo off
setlocal

echo ========================================================
echo GW-BASIC C17 Port -- Windows Build (MSVC)
echo ========================================================

:: Attempt to locate Visual Studio using vswhere
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found. Is Visual Studio installed?
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -property installationPath`) do (
    set "VS_PATH=%%i"
)

if "%VS_PATH%"=="" (
    echo [ERROR] Could not locate a Visual Studio installation.
    exit /b 1
)

set "VCVARS=%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat"

if not exist "%VCVARS%" (
    echo [ERROR] vcvarsall.bat not found at "%VCVARS%".
    exit /b 1
)

:: Set up the MSVC environment for x64 build
echo [INFO] Setting up Visual Studio Environment...
call "%VCVARS%" x64

echo [INFO] Compiling GW-BASIC and libraries...

:: Compiling with local SDL2 if available, or expecting it in system paths.
echo [INFO] Attempting to compile with SDL2 support...
cl /TC /std:c17 /W3 /O2 /Iinclude /Ilib /Isdl2\include /D_CRT_SECURE_NO_WARNINGS /Fe:gwbasic.exe src\main.c src\interp.c src\eval.c src\tokenizer.c src\variables.c src\strings.c src\fileio.c src\console.c src\events.c lib\gw_math_mbf.c lib\gw_memory.c lib\gw_plugin.c lib\gw_sdl2.c lib\gw_serial.c /link /SUBSYSTEM:CONSOLE /LIBPATH:sdl2\lib\x64 SDL2.lib SDL2main.lib Shell32.lib User32.lib Gdi32.lib Winmm.lib >nul 2>nul
if not errorlevel 1 goto compile_ok

echo [WARN] SDL2 compilation failed (SDL2 may not be installed/configured).
echo [INFO] Compiling in text-only/fallback mode (NO_SDL2)...
cl /TC /std:c17 /W3 /O2 /Iinclude /Ilib /D_CRT_SECURE_NO_WARNINGS /DNO_SDL2 /Fe:gwbasic.exe src\main.c src\interp.c src\eval.c src\tokenizer.c src\variables.c src\strings.c src\fileio.c src\console.c src\events.c lib\gw_math_mbf.c lib\gw_memory.c lib\gw_plugin.c lib\gw_sdl2.c lib\gw_serial.c /link /SUBSYSTEM:CONSOLE Shell32.lib User32.lib Gdi32.lib Winmm.lib
if not errorlevel 1 goto compile_ok

:compile_fail
echo [ERROR] Compilation failed.
exit /b 1

:compile_ok
:: Clean up intermediates
del /q *.obj 2>nul

echo ========================================================
echo Build complete! Executable: gwbasic.exe
echo ========================================================
endlocal
