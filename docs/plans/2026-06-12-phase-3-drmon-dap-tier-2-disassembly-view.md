# Phase 3 — drmon-dap Tier 2: Disassembly View

**Date:** 2026-06-12  
**Status:** Tier 2 COMPLETE — verifications 1–3 + 5 PASS  
**Repo plan:** `docs/plans/2026-06-12-phase-3-dap.md`  
**TODO entry:** DRMON — DEBUGGER BACKEND → Phase 3

---

## Context

Tier 1 shipped a working DAP adapter: attach, continue/pause/step, registers,
memory read, instruction breakpoints. The disassembly view is the remaining Tier 2
deliverable — it lets VS Code show the running instruction stream in its disassembly
pane, which is the primary interface for bare-metal debugging.

`dis816.cpp` (65816) and `dis68000.cpp` (68k) are the existing disassemblers. Both
compile cleanly in the TUI build. They cannot be directly added to the DAP target
because `dis816.cpp` includes `memory.hpp`, which references the TUI type `_object*`.
The fix is a **stub preamble** (a thin header that forward-declares `_object` and
defines a stub `_symbolList`) included before the disassembler source, plus a
**stubs TU** providing the Print* implementations that the disassemblers call through
`general.hpp` — avoiding `general.cpp`'s heavy TUI dependency chain entirely.

---

## Key facts from source audit

- `dis816.cpp` calls these external functions:
  `Print8Bits`, `Print16Bits`, `Print24Bits`, `PrintRaw24Bits`, `PrintString`,
  `PadString` (from `general.cpp`); `FindHexSymbol` / `_symbolList::Name()` (from
  `symbol.cpp` — only when `printCheckForSymbol` is true).
  `Print8BitsLabCheck` and `Print16BitsLabCheck` are **defined inside** `dis816.cpp`.

- `dis816.cpp` uses `procMode` (from `memory.cpp`) to pick immediate operand size
  for 65816 mode-5 addressing. Values: 0=A8/X8, 1=A16/X8, 2=A8/X16, 3=A16/X16
  (derived from P register M/X flags).

- `memory.hpp` declares `MemoryGetLineAddr(_object*, int)` — a pointer parameter
  that requires `_object` to be declared (but not defined) at the call site.

- `general.hpp` is clean — just function prototypes and two externs
  (`printCheckForSymbol`, `hexTable`). No TUI includes.

- `dis68000.cpp` includes only `<stdlib.h>`, `<stdio.h>`, `<string.h>`, and
  `dis68.hpp` (local). It calls the same Print* functions at link time.

- DAP protocol: `DisassembleRequest {memoryReference, instructionOffset?, instructionCount}`;
  `DisassembledInstruction {address, instruction, instructionBytes?}`.

- `cppdap` capability flag: `supportsDisassembleRequest` (currently `false` in
  `initialize` handler).

---

## What changes

### New files in `linux/dap/`

**`disasm_preamble.hpp`**  
Forward-declarations that allow dis816.cpp and dis68000.cpp to compile in the DAP
build without pulling in TUI headers:

```cpp
#pragma once
// Forward-declare _object so memory.hpp's MemoryGetLineAddr decl compiles
// (pointer-only use — complete type not required).
class _object;

// Minimal _symbolList for dis816.cpp's FindHexSymbol usage.
// Symbol lookup is disabled in the DAP build (printCheckForSymbol=false),
// so Name() is never called, but the type must be complete at the call site.
struct _symbolList {
    const char* Name() const { return ""; }
};
_symbolList* FindHexSymbol(unsigned long);
extern struct _symbolList symbolListBase;
```

**`disasm_stubs.cpp`**  
Defines all globals and Print* implementations the disassemblers call through,
without touching `general.cpp` or any TUI header:

```cpp
#include "disasm_preamble.hpp"
#include "global.hpp"          // UBYTE/UWORD/ULONG/FLAG, far→empty

// Globals
int   procMode           = 0;
FLAG  printCheckForSymbol = 0;     // symbol lookup always off in DAP
char  hexTable[16]       = { '0','1','2','3','4','5','6','7',
                              '8','9','A','B','C','D','E','F' };
struct _symbolList symbolListBase;
_symbolList* FindHexSymbol(ULONG) { return nullptr; }

// Print* implementations (extracted from general.cpp, TUI-free)
// Print4/8/16Bits, PrintRaw24Bits, Print24Bits, Print32Bits, PrintRaw32Bits,
// PrintString, PadString — all implemented here using hexTable only.
```

**`dis816_dap.cpp`**  
Wrapper that injects the preamble before dis816.cpp's own includes:

```cpp
#include "disasm_preamble.hpp"
#include "../../dis816.cpp"
```

**`dis68000_dap.cpp`**  
Same pattern for Genesis:

```cpp
#include "disasm_preamble.hpp"
#include "../../dis68000.cpp"
```

---

### Modified files

**`registers.hpp`** — add `flagsIndex` field to `RegTable`:

```cpp
struct RegTable {
    const char* const* names;
    const unsigned long* masks;
    int count;
    int pcIndex;
    int pbIndex;
    int flagsIndex;  // index of the flags/status register, or -1 if none
};
// kSnesRegs: flagsIndex=3 (FLAGS/P register)
// kGenRegs:  flagsIndex=-1 (procMode unused for 68k)
```

**`session.cpp`**:

1. Update `initialize` handler:
   ```cpp
   resp.supportsDisassembleRequest = true;
   ```

2. Add forward declaration (near top of file):
   ```cpp
   // Provided by the per-system disassembler wrapper (dis816_dap.cpp / dis68000_dap.cpp)
   extern int procMode;
   unsigned int Disassem(unsigned long addr, char* inBuff, char* outBuff, int disMode);
   ```

3. Add `disassemble` handler in `registerHandlers()`:
   ```cpp
   s->registerHandler([&](const dap::DisassembleRequest& req)
                       -> dap::DisassembleResponse {
       dap::DisassembleResponse resp;
       uint32_t base  = (uint32_t)strtoul(req.memoryReference.c_str(), nullptr, 0);
       int64_t  ioff  = req.instructionOffset.has_value() ? req.instructionOffset.value() : 0;
       int      count = (int)req.instructionCount;

       // Treat instructionOffset as a byte offset (Tier 2 approximation).
       // Negative values let VS Code show a few instructions before the current PC.
       uint32_t addr = (uint32_t)((int64_t)base + ioff);

       // Read enough bytes: count instructions × 4 bytes max + small margin
       uint32_t buflen = (uint32_t)(count * 4 + 16);
       std::vector<uint8_t> mem(buflen, 0);
       backend_.readMemory(addr, mem.data(), buflen);

       // Update procMode from current FLAGS (SNES: M/X bits → operand size)
       const RegTable& rt = backend_.regTable();
       if (rt.flagsIndex >= 0) {
           MameRegs r    = backend_.getRegisters();
           uint32_t flags = (uint32_t)r.v[rt.flagsIndex];
           int m = (flags >> 5) & 1;   // M=1 → 8-bit A immediate
           int x = (flags >> 4) & 1;   // X=1 → 8-bit X/Y immediate
           procMode = (m == 0 ? 1 : 0) | (x == 0 ? 2 : 0);
       }

       uint32_t cur = addr;
       size_t   idx = 0;
       while ((int)resp.instructions.size() < count && idx + 1 < mem.size()) {
           char obuf[128] = {};
           unsigned int len = Disassem(cur, (char*)&mem[idx], obuf, 0);
           if (len == 0) len = 1;

           dap::DisassembledInstruction di;
           di.address     = hexAddr(cur);
           di.instruction = obuf;
           // instructionBytes: space-separated hex pairs
           char hbuf[32] = {};  char* h = hbuf;
           for (unsigned i = 0; i < len && idx + i < mem.size(); i++)
               h += sprintf(h, "%02X ", mem[idx + i]);
           if (h > hbuf) { *(h-1) = 0; }  // trim trailing space
           di.instructionBytes = hbuf;
           resp.instructions.push_back(di);

           cur += len;
           idx += len;
       }
       return resp;
   });
   ```

**`CMakeLists.txt`** — restructure the DAP source list and function signature:

```cmake
# Split: common sources vs per-target disassembler wrapper
set(DRMON_DAP_SRC_COMMON
    linux/dap/main.cpp
    linux/dap/session.cpp
    linux/dap/backend.cpp
    linux/dap/breakpoints.cpp
    linux/dap/disasm_stubs.cpp       # new
)

function(drmon_dap_target TARGET SYSTEM_DEFINE DISASM_WRAPPER)
    add_executable(${TARGET} ${DRMON_DAP_SRC_COMMON} ${DISASM_WRAPPER})
    # Legacy disassembler source: suppress modern warnings, allow C-style casts
    set_source_files_properties(${DISASM_WRAPPER}
        PROPERTIES COMPILE_OPTIONS "-w;-fpermissive")
    target_link_libraries(${TARGET} PRIVATE cppdap Threads::Threads)
    target_compile_definitions(${TARGET} PRIVATE ${SYSTEM_DEFINE})
    target_compile_options(${TARGET} PRIVATE -std=c++17 -Wall -Wextra)
    target_include_directories(${TARGET} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/linux/dap
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/linux/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs
    )
endfunction()

drmon_dap_target(drmon-dap-snes DRMON_DAP_SNES linux/dap/dis816_dap.cpp)
drmon_dap_target(drmon-dap-gen  DRMON_DAP_GEN  linux/dap/dis68000_dap.cpp)
```

---

## Possible complications

- **`-w;-fpermissive` per-file**: `set_source_files_properties` overrides the
  target-level `-std=c++17 -Wall -Wextra` for the wrapper .cpp only. The stubs and
  session.cpp compile under normal strict flags.

- **Unaligned reads in dis816.cpp**: `*(unsigned int far *)inBuff` casts are UB on
  strict architectures. On x86-64 (the build target) they work; acceptable for Tier 2.

- **dis68000.cpp Print* declarations**: `dis68.hpp` may not include `general.hpp`.
  Including `disasm_preamble.hpp` first (which doesn't declare Print* functions) means
  dis68000.cpp might call undeclared functions — a warning in C++17, not an error.
  If it fails to compile, add `#include "general.hpp"` to the preamble (safe, since
  general.hpp has no TUI includes itself — only its implementation does).

- **Duplicate `procMode`**: `procMode` is defined in `disasm_stubs.cpp` and declared
  `extern` in `memory.hpp`. The TUI targets define it in `memory.cpp`. No conflict
  since the DAP and TUI binaries are separate. The `extern int procMode` in session.cpp
  links against the definition in `disasm_stubs.cpp`.

---

## Verification

1. ~~**Build**: `task build` produces both DAP binaries with no errors (link succeeds
   with dis816_dap.cpp / dis68000_dap.cpp included).~~

   ```
   [3/6] Building CXX object CMakeFiles/drmon-dap-snes.dir/linux/dap/dis816_dap.cpp.o
   [4/6] Building CXX object CMakeFiles/drmon-dap-gen.dir/linux/dap/dis68000_dap.cpp.o
   [5/6] Linking CXX executable drmon-dap-snes
   [6/6] Linking CXX executable drmon-dap-gen
   ```
   **PASS**

2. ~~**Disassemble smoke**: send a `disassemble` request over stdin — response should
   contain 5 `DisassembledInstruction` entries (zeros → `BRK $00`).~~

   ```
   {"body":{"instructions":[
     {"address":"0x808000","instruction":"BRK  $00","instructionBytes":"00 00"},
     {"address":"0x808002","instruction":"BRK  $00","instructionBytes":"00 00"},
     {"address":"0x808004","instruction":"BRK  $00","instructionBytes":"00 00"},
     {"address":"0x808006","instruction":"BRK  $00","instructionBytes":"00 00"},
     {"address":"0x808008","instruction":"BRK  $00","instructionBytes":"00 00"}
   ]},"command":"disassemble","request_seq":2,"seq":3,"success":true,"type":"response"}
   ```
   **PASS**

3. ~~**Capabilities**: `initialize` response now includes `"supportsDisassembleRequest":true`.~~

   ```
   {"body":{"supportsConfigurationDoneRequest":true,"supportsDisassembleRequest":true,
   "supportsInstructionBreakpoints":true,"supportsReadMemoryRequest":true},
   "command":"initialize","request_seq":1,"seq":2,"success":true,"type":"response"}
   ```
   **PASS**

4. **VS Code disassembly pane** (needs live MAME): attach, set a breakpoint,
   continue → pause; open the disassembly view (Debug toolbar → "Open Disassembly View")
   → correct 65816 mnemonics shown at the current PC. **(Needs live MAME.)**

5. ~~**Regression**: `task smoke SYS=snes` passes (TUI build untouched).~~

   ```
   PASS: opened windows via Alt-keys (M-e M-k M-r M-m M-w M-b M-s M-i M-o M-a M-y)
   + typed into Expression — no SIGSEGV
   ```
   **PASS**

---

## Tier 3 (still open after this)

- Symbol file loading (`.sld`/COFF via `sld.cpp`/`coff.cpp`) — those files include
  `moninc.hpp`; need a stripped stub preamble similar to this approach.
- Source breakpoints mapping symbol names to addresses.
