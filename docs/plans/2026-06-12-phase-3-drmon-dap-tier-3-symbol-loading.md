# Phase 3 — drmon-dap Tier 3: Symbol File Loading

**Date:** 2026-06-12  
**Status:** COMPLETE — verifications 1–5 PASS  
**Repo plan:** `docs/plans/2026-06-12-phase-3-dap.md`  
**TODO entry:** DRMON — DEBUGGER BACKEND → Phase 3

---

## Context

Tiers 1–2 delivered a complete DAP adapter: attach, continue/pause/step, registers,
memory read, instruction breakpoints, and disassembly. Tier 3 adds symbol awareness —
resolving addresses to names in the disassembly view, evaluating symbol names in the
watch/REPL, and setting source-level breakpoints by file:line.

`sld.cpp` / `coff.cpp` both `#include "moninc.hpp"` directly, which pulls the entire
ncurses/TUI widget stack. The Tier 2 stub-preamble trick cannot break this — `moninc.hpp`
requires the whole object hierarchy. The fix is **fresh loaders** written in `linux/dap/`
with no TUI dependency at all.

---

## Key facts from source audit

- **Developer Resources binary `.sld`** (NOT WLA-DX text `.sld`):
  - 256-byte header, first 27 bytes = `"Source level debugging file"`
  - Records at offset `0x100`: type `0x01` = filename (bytes until `0xFF 0xFF`);
    type `0x02` = addr (3 bytes little-endian lo/hi/bank) + line (2 bytes little-endian)
  - No label→address data in `.sld`; that comes from COFF
  - Source: `sld.cpp::ParseSLDLine()` — confirmed byte order is `lo | (hi<<8) | (bank<<16)`

- **Sierra COFF** (magic `0x0150`, big-endian / 68k byte order):
  - `FILHDR` (20 bytes): `f_magic(2) f_nscns(2) f_timdat(4) f_symptr(4) f_nsyms(4) f_opthdr(2) f_flags(2)`
  - `SYMENT` (18 bytes): `name_or_offset(8) n_value(4) n_scnum(2) n_type(2) n_sclass(1) n_numaux(1)`
  - All multi-byte fields are big-endian; original code calls `swaphilo()` after `fread`
  - String table at `symptr + nsyms * 18`: 4-byte length (BE) + null-terminated strings;
    `n_offset` (when `n_zeroes == 0`) is measured from start of string table, so index = `n_offset - 4`
  - Skip: `C_FILE(103)`, `C_BLOCK(100)`, `C_FCN(101)`, `C_EOS(102)`, `C_TPDEF(13)`, `C_ALIAS(105)`;
    also `C_EXT` with `n_scnum == 0` (undefined), and names starting with `.`

---

## What changed

### New files

**`linux/dap/symbols.hpp`** — clean `SymbolTable` class, no TUI deps:
- `loadSld(path)` / `loadCoff(path)` — return bool (true = loaded ≥0 records)
- `add(addr, name)` / `addSrc(SrcLine)` — insert into sorted maps
- `findByAddr(addr)` — nearest preceding symbol, within 256 bytes
- `findByName(name)` — case-insensitive name lookup
- `addrForSrc(file, line)` — exact file:line → address
- `sourceFiles()` — all files with at least one line record
- Internal: `byAddr_` (sorted map), `byName_` (lowercase unordered_map), `srcMap_` (file→sorted pairs)

**`linux/dap/symbols.cpp`** — implementations:
- `.sld` loader: validates 256-byte header, seeks to `0x100`, reads type-1 and type-2 records
- COFF loader: reads big-endian FILHDR+SYMENT table; no section parsing needed (seeks directly to `symptr`)
- `findByAddr`: `upper_bound` then step back; returns `nullptr` if > 256 bytes away
- `findByName`: lowercases query, looks up `byName_`, cross-references `byAddr_`

### Modified files

**`linux/dap/main.cpp`** — added `--symbols <path>` argument (inline arg loop)

**`linux/dap/session.hpp`** — added `SymbolTable symtab_`, `std::vector<uint32_t> srcBptAddrs_`;
updated constructor signature to take `const char* symbolPath = nullptr`

**`linux/dap/session.cpp`** — 6 integration points:
1. Constructor: `symtab_.loadSld(path) || symtab_.loadCoff(path)` (try .sld first, fall back to COFF)
2. `initialize`: added `supportsLoadedSourcesRequest = true`
3. `disassemble`: if symtab non-empty and exact address match, sets `di.symbol`
4. `evaluate`: if symtab non-empty and no register/address match found, tries `findByName(expr)` —
   returns address as hex string with type `"symbol"`
5. `setBreakpoints`: full source breakpoint resolution — clears previous `srcBptAddrs_` list,
   calls `addrForSrc(file, line)`, marks `b.verified = true` with `instructionReference` if found;
   returns `verified = false` with helpful message if no symbol file or line not found
6. New `loadedSources` handler: returns source files from symtab

**`linux/dap/breakpoints.hpp/.cpp`** — added `add(addr)` and `remove(addr)` helpers to
`BreakpointTable` (complement to the existing `sync()`)

**`CMakeLists.txt`** — `linux/dap/symbols.cpp` added to `DRMON_DAP_SRC_COMMON`

### Test file

**`linux/dap/test_symbols.py`** — standalone Python3 test (runs on host, not in Docker):
- Creates a synthetic binary `.sld` (256-byte header + type-1/type-2 records)
- Creates a synthetic Sierra COFF (20-byte FILHDR + 18-byte SYMENT for `RESET@0x808000` + string table)
- Verifications 2–5 below

---

## Verification

1. ~~**Build**: `task build` succeeds with `symbols.cpp` added to both DAP targets.~~

   ```
   [5/10] Building CXX object CMakeFiles/drmon-dap-snes.dir/linux/dap/symbols.cpp.o
   [6/10] Building CXX object CMakeFiles/drmon-dap-gen.dir/linux/dap/symbols.cpp.o
   [9/10] Linking CXX executable drmon-dap-snes
   [10/10] Linking CXX executable drmon-dap-gen
   ```
   **PASS**

2. ~~**Evaluate symbol** (COFF): load a COFF file; send `evaluate` with a known symbol name;
   response `result` = the hex address.~~

   ```
   PASS  evaluate symbol (COFF)
   ```
   **PASS** — `evaluate "RESET"` → `{"result":"0x808000","type":"symbol","memoryReference":"0x808000"}`

3. ~~**Disassembly labels** (COFF): send `disassemble` at a known label address; response
   instruction has `symbol` field matching the label name.~~

   ```
   PASS  disassembly labels (COFF)
   ```
   **PASS** — `disassemble 0x808000 count=3` → first instruction `"symbol":"RESET"`

4. ~~**Source breakpoint smoke** (SLD): load a `.sld` with a known file:line→addr mapping;
   send `setBreakpoints` for that source file + line; response `breakpoints[0].verified == true`
   and `instructionReference` matches the address.~~

   ```
   PASS  source breakpoint smoke (SLD)
   ```
   **PASS** — `setBreakpoints test.asm:10` → `{"verified":true,"instructionReference":"0x808000"}`

5. ~~**No-symbols graceful**: `drmon-dap-snes` without `--symbols`; `evaluate` still works for
   register names; `setBreakpoints` returns `verified=false` with helpful message.~~

   ```
   PASS  no-symbols graceful
   ```
   **PASS** — `evaluate "PC"` → non-empty; `evaluate "UNKNOWN_SYM"` → `"<unknown expression>"`;
   `setBreakpoints` → `{"verified":false,"message":"No symbol file loaded (use --symbols)"}`

6. **VS Code source view** — set breakpoint in source view; execution halts at the correct address;
   VS Code highlights the source line. **✅ PASS — confirmed live in VS Code (2026-06-19).**

   Setup: extension `vscode-drmon/` loaded via `--extensionDevelopmentPath`; the `-g` ELF+DWARF companion
   `a16local.sfc.elf` passed as `--symbols`; MAME running `a16local.sfc`. A source breakpoint on
   **`a16local.c:17`** (the `for(;;)` loop) **fired**, the CPU **halted**, and VS Code **highlighted line
   17** with the **CALL STACK frame mapped to `a16local.c:17`** and **PC = PCL = `0x8074`** (line 17's
   DWARF address); the Registers scope shows the live `A = 0x1122` (the 16-bit add result):

   <img src="screenshots/dap-vscode-halt-line17.png" width="760">

   (Earlier states — attach + the verified breakpoint dot — are
   `screenshots/dap-vscode-attached.png` / `dap-vscode-bp-verified.png`.)

   Two real adapter bugs were found here and **fixed** (drdevtools `545723a`): (#5) `configurationDone`
   now issues the bridge "go" — on `attach` VS Code never sends a continue, so without it breakpoints
   never fired and pauses never surfaced; (#6) `stackTrace` now reverse-maps the stopped PC →
   `source`+`line` (it returned `line:0`/no source, so the editor had nothing to highlight), handing back
   the exact path the editor used. The same sequence is also a headless regression — `task test-dap` →
   `phasec` (attach → configurationDone → set bp, **no** explicit continue) — **8/8**:
   ```
     PASS PhaseC source bp (abs path):17 resolves via DWARF to 0x8074
     PASS PhaseC breakpoint fires (no explicit continue — attach flow)
     PASS PhaseC PC at/just past 0x8074 (DWARF-mapped source line)
     PASS PhaseC stackTrace maps PC -> source line 17
     PASS PhaseC stackTrace source.path == the editor's path
   ```

7. ~~**Regression**: `task smoke SYS=snes` passes.~~

   ```
   PASS: opened windows via Alt-keys (M-e M-k M-r M-n M-m M-w M-b M-s M-i M-o M-a M-y)
   + typed into Expression — no SIGSEGV
   ```
   **PASS**

---

## Scope notes

- `zardoz.cpp` calls an undefined `LoadSymbol` — skipped entirely.
- Modern WLA-DX text `.sld` and ca65 `.dbg` formats are a separate TODO item; this Tier
  implements Developer Resources binary `.sld` + Sierra COFF (what the existing TUI loaders
  handle).
- `supportsBreakpointLocationsRequest` / `sourceReference` disassembly-to-source navigation
  are future follow-ons beyond Tier 3 scope.
