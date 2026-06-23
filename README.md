# Microsoft GW-BASIC Interpreter C17 Port

This repository contains a modern **C17 port** of Microsoft's original 1983 GW-BASIC interpreter, which was originally written in Intel 8086 assembly language. The goal of this project is to provide a highly accurate, compatible, and modern cross-platform executable that compiles with modern toolchains (like GCC, Clang, MSVC) and runs on modern operating systems (Windows, macOS, Linux).

### Key Architectural Highlights:
1. **Interpreter Core & AST/Token Loop**: Re-implements the original GW-BASIC interpreter mechanics, tokenizing ASCII BASIC source lines and executing statements via a parser design that matches classic behaviors.
2. **Emulated Segmented Memory**: Includes a simulated physical segmented memory architecture, supporting low-level operations such as `PEEK`, `POKE`, `DEF SEG`, and `VARPTR` to interact with simulated registers and RAM buffers.
3. **Microsoft Binary Format (MBF) Emulation**: Emulates MBF float storage and mathematical operations, ensuring that mathematical calculations and data files containing MBF float formats are handled and converted accurately.
4. **SDL2-Based Graphics & Sound Emulation**: Provides a dynamic hardware interface using Simple DirectMedia Layer (SDL2) to recreate legacy PCjr, Tandy, CGA, EGA, VGA, and Hercules screen modes (handling pixel plotting, video memory pages, screen modes, palette selection, sound playback, and real-time keyboard/controller inputs).
5. **Event-Trapping Engine**: Supports asynchronous and non-blocking event-trapping for `KEY`, `COM`, and `TIMER` traps, reproducing authentic DOS background-execution characteristics.
6. **Logical Device Channels**: Routes output dynamically through logical file/device channels (files, serial COM ports, printer LPT buffers, or console displays).
7. **Extension Plugin Interface**: Includes a modular plugin manager to register, query, and call custom statement and function handlers at runtime.


## Announcement blog
https://devblogs.microsoft.com/commandline/microsoft-open-sources-gw-basic/

## Information

This repo:

1. Is being released for historical reference/interest purposes, and reflects the state of the GW-BASIC interpreter source code as it was in 1983
1. Will not be modified - please do not submit PR's or request changes
1. Contains no build scripts, makefiles, or tools required to generate executable binaries, nor does it contain any pre-built binaries / executables

## License

All files within this repo are released under the [MIT (OSI) License]( https://en.wikipedia.org/wiki/MIT_License) as per the [LICENSE file](https://github.com/Microsoft/GW-BASIC/blob/master/LICENSE) stored in the root of this repo.

## Contributing

The source files in this repo are for historical reference and will remain read-only and unmodified in their original state. Please  **do not** send Pull Requests suggesting any modifications to the source files.  

Further contribution guidance can be found in the [Contributor's Guide](https://github.com/Microsoft/GW-BASIC/blob/master/CONTRIBUTING.md) stored in the root of this repo.

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).  For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
