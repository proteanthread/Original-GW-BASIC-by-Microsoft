# Microsoft GW-BASIC Interpreter C17 Port

This repository contains a modern, highly compatible **C17 port** of Microsoft's original 1983 GW-BASIC interpreter. Originally written entirely in Intel 8086 assembly language, this project systematically translates that legacy system into standard, portable C17. The primary objective is to deliver a fully functional, cross-platform interpreter capable of executing classic GW-BASIC scripts, games, and graphical/audio programs identically to the original MS-DOS environment, while running natively on modern operating systems (Windows, macOS, Linux).

## Project Scope & Objectives

The GW-BASIC C17 port project is designed to bridge the gap between historical computing and modern system environments by focusing on the following core domains:

1. **Syntactic & Operational Fidelity**: Re-creating the exact AST structures, tokenization processes, and statement execution pathways of the original interpreter. This guarantees that tokenized binary `.BAS` files and ASCII source files load and run with authentic parsing semantics.
2. **Segmented Memory Emulation**: Emulating the 8086 segmented memory architecture (up to 1MB of addressable space). This allows legacy low-level programming operations (`PEEK`, `POKE`, `DEF SEG`, and memory offset calculations via `VARPTR`) to behave identically, accessing a virtual RAM arena containing simulated hardware registers and buffers.
3. **Microsoft Binary Format (MBF) Mathematics**: Implementing accurate MBF single-precision (32-bit) and double-precision (64-bit) floating-point format conversions. Because original GW-BASIC stored floats in MBF rather than IEEE 754, this implementation is critical for interpreting binary float files and carrying out exact mathematical calculations without precision discrepancies.
4. **SDL2 Virtual Device Layer (VDev)**: Recreating CGA, EGA, VGA, Hercules, and Tandy graphics screens, color palettes, sound tone generation (`PLAY`, `SOUND`), and real-time input handling. It dynamically creates a hardware-independent canvas using Simple DirectMedia Layer (SDL2) when the BASIC script requests a graphics screen mode (> 0).
5. **Event-Trapping and I/O Routing**: Simulating background DOS interrupt traps for asynchronous serial communication (`COM`), keyboard keys (`KEY`), and elapsed intervals (`TIMER`), alongside logical file channel multiplexing.
6. **Extensible Plugin Subsystem**: Providing a modular C-interface to inject custom statement and function handlers at runtime, allowing users to extend the interpreter without altering its core architecture.
7. **BASIC++ Integration**: Architected to be fully embeddable as a static module (`GWBASIC`) within the broader **BASIC++** compiler/interpreter framework, facilitating a unified dialect mode (`DIALECT_GW_BASIC`) that supports Union, Mixed, and Strict modes.

---

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
