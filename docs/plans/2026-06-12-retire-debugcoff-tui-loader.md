# Retire the legacy DEBUGCOFF TUI symbol loader (`coff.cpp`)

**Status:** **decided 2026-06-12 — remove DEBUGCOFF; keep DEBUGZARDOZ pending investigation.**
Executed as its own pass, **separate from** the dead-`#ifdef`-arm cleanup
([plan](2026-06-12-drop-dead-dos-compiler-ifdef-arms.md)) — both remain uncommitted pending review.

## Context

`coff.cpp` / `coff.hpp` are the TUI's **Sierra COFF** symbol-table loader, one of the three
build-time `DEBUGTYPE` backends (`DEBUGDR` = native `.sld` is active; `DEBUGCOFF` and `DEBUGZARDOZ`
are the unselected alternatives — see the cleanup plan's "dormant but revivable" note). The cleanup
review asked whether `coff.cpp` is 64-bit-safe. It is not — and the deeper question surfaced: **is
Sierra COFF support even needed anymore?** Three findings make the answer "no":

1. **It's LP64-broken at runtime.** The loader `fread()`s file structs whose fields are declared
   `long` (`FILHDR`, `AOUTHDR`, `SCNHDR`, `SYMENT`, `RELOC`, `LINENO` in `coff.hpp`). COFF `long`
   fields are **4 bytes on disk**, but `long` is **8 bytes** on 64-bit Linux — so `sizeof(FILHDR)`,
   `fread(&sh, sizeof(SCNHDR), …)`, `section_seek += sizeof(SCNHDR)`, `sizeof(SYMENT)*num_symbols`,
   and `addr*sizeof(SYMENT)` are all wrong, and the struct field offsets don't line up with the file.
   Additional LP64 bugs: `*(long *)&name[4]` (`coff.hpp:306`) reads 8 bytes from a 9-byte buffer
   (OOB + wrong width); `swaphilo(long)` only byte-swaps the low 32 bits; the `SYMENT` union holds
   `char* _n_nptr[2]` (16 bytes on 64-bit). It *compiles* under `-DDEBUGCOFF` on the current toolchain
   (one pre-existing typo aside — `coff.cpp:592` uses `i` where it means `j`), but it would **misparse
   every COFF file at runtime**. "Revivable by flipping the flag" was optimistic; reviving it is a
   rewrite of the binary I/O.

2. **It's already superseded.** The Phase 3 DAP work shipped a **fresh, correct, LP64-safe** Sierra
   COFF loader — `SymbolTable::loadCoff()` in `linux/dap/symbols.cpp:136` (magic `0x0150`, byte-wise
   reads, no `fread`-into-struct). Any real need for COFF in the modern path is already met there.

3. **The format is obsolete.** Per the repo's own competitive analysis
   ([mesen2-bsnes-plus-vs-drmon](../investigations/2026-06-11-mesen2-bsnes-plus-vs-drmon.md)),
   drmon's parsers "speak 1994 (`.sld`, COFF, zardoz)," while **every active SNES toolchain emits
   ca65 `.dbg`, WLA-DX `.sym`, ELF, RGBDS, bass, or SDCC output** — *none* emit Sierra COFF (Sierra's
   in-house 1990s toolchain) or Zardoz C output. The forward path is already a TODO: *add ca65 `.dbg`
   + WLA-DX `.sym` importers* (table stakes), not maintaining a 1994 COFF parser.

## Recommendation

**Remove the TUI `DEBUGCOFF` backend.** It is unbuilt (the default is `DEBUGDR`), runtime-broken on
64-bit, superseded by the DAP loader, and for a format no current toolchain emits. Keep the DAP's
`loadCoff()` (modern, correct) so COFF is still readable where it matters.

## Decision (made 2026-06-12)

1. **DEBUGCOFF — REMOVE.** Unbuilt, LP64-broken, superseded by the DAP `loadCoff()`, for a format no
   current toolchain emits. Executed by this plan.
2. **DEBUGZARDOZ — KEEP, pending investigation.** Zardoz C (a 1990s 65816 C compiler) is equally
   obsolete and `zardoz.cpp` likely has the same LP64 issues, but it's preserved for now pending a
   look at whether any artifacts / historical value justify a modern re-loader. *Not touched by this
   plan.* (Open follow-up: audit `zardoz.cpp` for LP64 correctness and decide retire-vs-keep.)

## Scope of removal (if approved)

Pure deletion of an unbuilt backend — no behavior change to `snesmon`/`genmon` (they're `DEBUGDR`).

- **Delete files:** `coff.cpp`, `coff.hpp`; drop `coff.cpp` from `CMakeLists.txt` `DRMON_CXX_SRC`.
- **Strip `#ifdef DEBUGCOFF` arms** (keep each file's non-COFF path) in: `global.hpp` (the
  `SYMBOL_CASE_SENSITIVE` define), `symbol.cpp` (4 sites — load dispatch + the `_`-prefixed C-symbol
  lookup), `symbol.hpp` (2), `sld.cpp`, `sld.hpp`, `source.cpp`, `info.cpp`, `monmenu.cpp` (3 — incl.
  the "Load COFF…" File-menu entry + `FileMenuLoadCOFF`). Full site list: `grep -rn DEBUGCOFF`.
- **Check** `FileMenuLoadCOFF` and any `coffBase`/`coff`-typed references are all inside `#ifdef
  DEBUGCOFF` (they are — `coffBase` lives in `coff.hpp`); confirm no dangling refs after deletion.
- Leave `linux/dap/symbols.cpp` `loadCoff()` untouched (the modern path).

## Follow-on: orphaned symbol type/storage-class metadata (executed 2026-06-12)

Deleting `coff.hpp` left one cross-dependency — `symbol.hpp`'s `AddSymbolQuick(…, storageClass=C_LABEL)`
default — so `C_LABEL` was briefly re-homed into `symbol.hpp`. That exposed that the whole
`_symbolList` **type / storage-class machinery was orphaned** by the COFF removal, so it was ripped
out too: the `type`/`storageClass` params of `AddSymbolQuick`, the `Type()`/`StorageClass()`
getters+setters, the `ivType`/`ivStorageClass` members, and `#define C_LABEL`.

**Why it was dead:** `ivType` (COFF `n_type`, the C type) and `ivStorageClass` (COFF `n_sclass` —
`C_AUTO`/`C_ARG`/`C_REG`/`C_EXT`/`C_LABEL`…) were **only ever populated by the COFF loader** and
**only ever read by the COFF `Eval()`** (which used them to compute a local/arg's live value — read
`A6+off` for `C_ARG`, `SP+off` for `C_AUTO`, a register for `C_REG`, sized/sign-extended by `ivType`)
and the COFF storage-class display. On the shipped `DEBUGDR` (`.sld`) build these were **always the
defaults** (`type=0`, `C_LABEL`) and `Eval()` returned the raw address — i.e. inert. With the COFF
reader gone, they were write-only.

**Future note (don't resurrect from git — rebuild fresh):** the *capability* — typed, lexically-scoped
evaluation of C locals/args ("watch `score`", read the right stack slot as a signed 2-byte int, only
while in scope) — is genuinely valuable for real C source-level debugging and may return. But it
should be rebuilt on **modern formats** (ca65 `.dbg`, ELF **DWARF**), whose type/scope info is far
richer than COFF storage classes, and surfaced through the **DAP** front-end — *not* by reviving the
COFF `n_type`/`n_sclass` bytes on the TUI `_symbolList`. Tracked by the existing
[ca65 `.dbg` / WLA-DX `.sym` importer TODO](../../TODO.md) and the DWARF/source-level path.

## Verification

1. `task build` — `snesmon` + `genmon` link clean (DEBUGCOFF was never compiled, so this is a no-op
   for the produced binaries; the check is that no stray reference broke).
2. `gcc -E -P` before/after on a DEBUGCOFF-consumer (`symbol.cpp`, `monmenu.cpp`) — identical token
   stream (the removed arms were never compiled under `DEBUGDR`), proving zero behavior change.
3. `task smoke SYS=snes` / `SYS=gen` — both PASS.
4. Update the cleanup plan's "preserve" table: move DEBUGCOFF (and ZARDOZ, if approved) from
   *preserve* to *removed*, with this plan as the rationale. Add/refresh the TODO pointing at the
   ca65 `.dbg` / WLA-DX `.sym` / ELF importers as the real forward path.

## Non-goals

- Not modernizing `coff.cpp` for LP64 (rejected — the format is obsolete; effort belongs in modern
  importers instead).
- Not touching the DAP `loadCoff()`.
- Not part of the current dead-`#ifdef`-arm commit.
